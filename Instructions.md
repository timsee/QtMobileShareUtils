## Instructions

### 1. Run the existing sample application.
Use the application in this repo as a sanity test. Open the Qt project and build it for mobile. Test that it allows you to share files to and from the device you are running on. If you are unable to build, check out the docs in the [Pre-reqs](README.md#Prereqs).

The sample app was designed to show its functionality with basic image types (`.jpg` and `.png`). When it loads, it will load with a blank page with a single `Send` button on the bottom. To test sending, click the button. It will provide an option send the `lenna.png` image to any apps that can load images.

Testing sending files can get a bit more complicated, since there's multiple apps you can send from. In my testing, I tested sending files from `Dropbox` in iOS and android. Then on android, I also tested using the `Photos` and the `Files` app. For iOS, I tested with the `Files` app. Extra steps are needed for specifically accessing the `Photos` app, so using the `Photos` app is out of scope for this sample code.

### 2. Add an info.plist and AndroidManifest file to your build.
For most mobile Qt projects, this step is likely already done and you can skip it. To make sure you qualify for skipping, check your `.pro` file and verify that you have code that allows you to use a custom `Info.plist` for iOS and a custom `AndroidManifest.xml` for Android. This will allow us to set the app permissions to enable sharing.

To check if you have this code, look in the `.pro` for something along the lines of:
```
android {
   OTHER_FILES += android/AndroidManifest.xml
   ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
}

ios {
   QMAKE_INFO_PLIST = ios/Info.plist
}
```
If this does not already exist, [check here](Extended.md/#setup-mobile) for a quick guide for setting up your application for mobile builds.

### 3. Add the `shareutils` code to your application's codebase.
- Copy the `shareutils` directory from the sample application to your codebase
- Copy the Java files from the sample application into your project's `ANDROID_PACKAGE_SOURCE_DIR`.
- Copy this block from samples `.pro` and append it to the bottom of your `.pro`:

```
# shareutils.hpp contains all the C++ code needed for calling the native shares on iOS and Android
HEADERS += shareutils/shareutils.hpp

ios {
    # Objective-C++ files are needed for code that mixes objC and C++
    OBJECTIVE_SOURCES += shareutils/iosshareutils.mm

    # Headers for objC classes must be separate from a C++ header.
    HEADERS += shareutils/docviewcontroller.hpp
}
android {
    # used for JNI calls
    QT += androidextras

    # the source file that contains the JNI and the android share implementation
    SOURCES += shareutils/androidshareutils.cpp

    # this contains the java code for and parsing and sending android intents, and
    # the android activity required to read incoming intents.
    OTHER_FILES += android/src/org/shareluma/utils/QShareUtils.java \
        android/src/org/shareluma/utils/QSharePathResolver.java \
        android/src/org/shareluma/activity/QShareActivity.java
}

```
This is a good time to test compilation on desktop. Since it hasn't been added to your application yet it won't do anything, but it should compile for all environments.

### 4. Update the info.plist or AndroidManifest to include the proper permissions for sharing.

#### iOS
Add the ability for the app for received files from another application. Do so by adding this block to your `Info.plist`:
```
<key>CFBundleDocumentTypes</key>		
<array>		
	<dict>		
		<key>CFBundleTypeName</key>
		<string>public.png</string>
		<key>CFBundleTypeRole</key>
		<string>Viewer</string>
		<key>LSHandlerRank</key>
		<string>Alternate</string>
		<key>LSItemContentTypes</key>
		<array>
			<string>public.image</string>
		</array>		
	</dict>		
</array>
```
In this case, the `CFBundleDocumentyType` is set up for generic image types. For your application, you may need to look up the `LSItemContentType` you intend to support.  


#### Android
This is a bit more complicated than the iOS updates. Android sends and receives files across apps with [Intents](https://developer.android.com/guide/components/intents-filters). First, add to your main activity in the `AndroidManifest.xml` the permission to send and receive intents:
```
<manifest ...>
	<application ...>
		<activity...>
		    <!-- Beginning of Intent Filters for Sending and Viewing Files -->
		    <intent-filter>		
	                <action android:name="android.intent.action.SEND"/>		
	                <category android:name="android.intent.category.DEFAULT"/>		
	                <data android:mimeType="image/*"/>		
	            </intent-filter>		
	            <intent-filter>		
	                <action android:name="android.intent.action.VIEW"/>		
	                <category android:name="android.intent.category.DEFAULT"/>		
	                <data android:mimeType="image/*"/>		
	            </intent-filter>		
		    <!-- End of Intent Filters for Sending and Viewing Files -->
		</activity>
	</application>
</manifest>
```

Next, update the main activity from `QtActivity` to the ShareUtils Activity. This allows us to add code that handles intents:
```
<manifest ...>
	<application ...>
		<activity
			android:name="org.shareluma.activity.QShareActivity"
		...>
		</activity>
	</application>
</manifest>
```
Finally, add a FileProvider for accessing files:
```
<manifest ...>
	<application ...>
		<activity...>
		</activity>
   		<provider		
	            android:name="android.support.v4.content.FileProvider"		
	            android:authorities="org.shareluma.fileprovider"		
	            android:exported="false"		
	            android:grantUriPermissions="true">		
	            <meta-data		
	              android:name="android.support.FILE_PROVIDER_PATHS"		
	              android:resource="@xml/filepaths"/>		
	        </provider>
	</application>
</manifest>
```
Great! Now we just need to add some extra files to the Android build directory. First, `filepaths.xml` to your `ANDROID_PACKAGE_SOURCE_DIR/res/xml` directory. This defines the directory that FileProvider will save files to and send files from.

Next, add the `build.gradle` to the root of the `ANDROID_PACKAGE_SOURCE_DIR`. This will link the `android.support.v4.content` library to the Qt application. This is a fairly common library so its likely on your machine already, but if you don't typically do Android development, you may need to load into Android Studio and download the library.


### 5. Integrate the class.
Use the sample code for guidance, the general gist is that you'll need to add a `ShareUtils` object to a QObject in your application.

You should construct the class like this:
```
    mShareUtils = new ShareUtils(this);
    connect(mShareUtils, SIGNAL(fileUrlReceived(QString)), this, SLOT(receivedURL(QString)));
```

You'll also need to listen to event changes in android only. This is the only surefire way to catch incoming intents:
```
void MainWindow::changeEvent(QEvent* event) {
    // for android, we explicitly check when the window becomes active if there
    // are any incoming intents.
    if (event->type() == QEvent::ActivationChange && this->isActiveWindow()) {
#if defined(Q_OS_ANDROID)
        mShareUtils->checkPendingIntents();
#endif // Q_OS_ANDROID
    }
}

```

In the .cpp file, connect `fileUrlReceived` signal from the `ShareUtils` to a corresponding slot. When this slot is received, it will provide an absolute path to the file you are trying to receive. There is an additional edge case in Android: files sent from Intents are received and saved in a temporary directory. Because of this, it is best practice to utilize the file, save it elsewhere if you want to access it in the future, and then delete the temporary files.

```
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
```

To send a file, use this block of code. You must have an absolute path to the file on disk in order for sharing to be successful, so you cannot use relative paths or use features that require relative paths, such as sharing from Qt resources directly.

```
// a requestID tracks an intent with an identifier, useful if you have multiple intents being
// sent at once. A 7 here is random and just used for example purposes
int requestID = 7;
mShareUtils->sendFile(savePath, "ShareUtils", "image/png", requestID);
```

Once all this is in and your code is compiling, run the same checks that you ran in step 1 from within your codebase.

### Next Steps

Great! You've integrated iOS and Android sharing into your application. Here are some [further steps](Extended.md#further-steps).
