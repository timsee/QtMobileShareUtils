
### ShareUtils

The goal of this project is to reduce the number of steps required in order to utilize native share interfaces on iOS and Android with Qt. In order to do this you will need to:
- Update the app's permission to enable sharing files.
- Add some Java/Objective-C code for handling the native interface.
- Wrap this code in a Qt object.
- Integrate this Qt object into your application.

Due to the cross-language nature of this task, creating a single drop-in library to handle all cases would be difficult. Therefore, I've broken down the task into as few steps as possible.

Most of the original legwork of figuring out how to share on iOS and Android came from this extremely useful series of [blog posts (Sharing Files on Android or iOS from your Qt App](https://blog.qt.io/blog/2017/12/01/sharing-files-android-ios-qt-app/) by Ekkehard Gentz. If you are interested in _how_ the sharing works, this is a great place to look for more information.

### Glossary

* [Getting Started Guide](Instructions.md)
* [Credits](Credits.md)

### <a name="pre-reqs"></a>Pre-reqs

This project assumes that you have already successfully built a Qt application for iOS and Android. If this hasn't been done yet, check here for getting started documentation on [Android](https://doc.qt.io/qt-5/android-getting-started.html), and here for [iOS](https://doc.qt.io/qt-5/ios.html).
