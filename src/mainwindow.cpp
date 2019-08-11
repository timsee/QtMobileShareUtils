/*!
 * MIT License
 *
 * Copyright (c) 2019 Tim Seemann
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "mainwindow.h"

#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // set app size for desktop builds
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    this->setGeometry(0, 0, 400, 600);
#endif

    mShareUtils = new ShareUtils(this);
    connect(mShareUtils, SIGNAL(fileUrlReceived(QString)), this, SLOT(receivedURL(QString)));

    mMainWidget = new QWidget(this);
    mMainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setCentralWidget(mMainWidget);

    mImageWidget = new QLabel(this);
    mImageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mSendButton = new QPushButton("Send", this);
    mSendButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(mSendButton, SIGNAL(clicked()), this, SLOT(sendButtonPressed()));

    mLayout = new QVBoxLayout(mMainWidget);
    mLayout->addWidget(mImageWidget, 10);
    mLayout->addWidget(mSendButton, 1);
}

void MainWindow::sendButtonPressed() {
#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
    // in Qt, resources are not stored absolute file paths, so in order to make
    // this example have a share-able image, we take an image from resources and
    // save it to disk. You can safely remove this additional copy if you can
    // already obtain the file path for whatever you're trying to share.
    QPixmap pixmap(":resources/lenna.png");
    auto savePath =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/lenna.png";
    pixmap.save(savePath);

    // a requestID tracks an intent with an identifier, useful if you have multiple intents being
    // sent at once.
    int requestID = 7;
    mShareUtils->sendFile(savePath, "ShareUtils", "image/png", requestID);
#else
    auto fileName =
        QFileDialog::getSaveFileName(this, tr("Save Image"), "lenna.png", tr("PNG (*.png)"));
    if (fileName.isEmpty()) {
        qDebug() << "WARNING: save file name empty";
        return;
    } else {
        QImage image(":resources/lenna.png");
        image.save(fileName);
    }
#endif
}

void MainWindow::receivedURL(QString url) {
    QFileInfo file(url);
    if (file.exists()) {
        // first check that we're receiving the type of file we expect. In this
        // app's case, we're expecting an image file.
        if (url.endsWith("png", Qt::CaseInsensitive) || url.endsWith("jpg", Qt::CaseInsensitive)
            || url.endsWith("jpeg", Qt::CaseInsensitive)) {
            qDebug() << " image found:" << url;
            // create a QImage
            mImageWidget->setPixmap(QPixmap(url));
            mImageWidget->setScaledContents(true);
        }
    } else {
        qDebug() << " File not found: " << url;
    }

#if defined(Q_OS_ANDROID)
    // android requires you to use a subdriectory within the AppDataLocation for
    // sending and receiving files. Because of this, when necessary we do a deep
    // copy of a file from the path provided to the required directory. Unless you
    // explicitly delete this copy, it will stick around, so its good practice to
    // delete all contents of this directory when the files are no longer needed.
    mShareUtils->clearTempDir();
#endif // Q_OS_ANDROID
}

void MainWindow::changeEvent(QEvent* event) {
    // for android, we explicitly check when the window becomes active if there
    // are any incoming intents.
    if (event->type() == QEvent::ActivationChange && this->isActiveWindow()) {
#if defined(Q_OS_ANDROID)
        mShareUtils->checkPendingIntents();
#endif // Q_OS_ANDROID
    }
}
