#-------------------------------------------------
#
# Project created by QtCreator 2019-07-25T21:15:44
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = ShareUtils
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

# check for proper version of Qt
message("DEBUG: Qt Version: $$QT_MAJOR_VERSION _ $$QT_MINOR_VERSION arch: $$QT_ARCH " )
equals (QT_MAJOR_VERSION, 5)  {
  !greaterThan(QT_MINOR_VERSION, 12) {
    error(ERROR: Qt5 is installed, but it is not a recent enough version. This project uses QT5.13 or later)
  }
}
!equals(QT_MAJOR_VERSION, 5) {
    error(ERROR: Qt5 is not installed. This project uses QT5.13 or later)
}

#--------
# Android and iOS setup
#--------

android {
   OTHER_FILES += android/AndroidManifest.xml
   ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
}

ios {
   QMAKE_INFO_PLIST = ios/Info.plist
}


#--------
# Sources
#--------

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

RESOURCES  = resources.qrc

#--------
# Native Share Sources
#--------

# shareutils.hpp contains all the C++ code needed for calling the native shares on iOS and android
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


