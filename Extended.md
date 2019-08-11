## <a name="setup-mobile"></a>Setting Up a Qt Project for iOS and Android

If this is your first time setting up a Qt project to be mobile compatible, you'll need to put in a little extra work.

First, make sure you have configured Qt Creator to build for iOS and Android. Check here for getting started documentation on [Android](https://doc.qt.io/qt-5/android-getting-started.html), and here for [iOS](https://doc.qt.io/qt-5/ios.html).

Once you are able to build projects for android and iOS, you're still not quite done! Mobile apps have a high-level file that defines permissions, capabilities, app meta-data, etc. In iOS this is a `Info.plist`, in android this an `AndroidManifest.xml`. Qt Creator will automatically generate this for a project if it a path to one isn't found in your `.pro` file. However, this autogenerates with default permissions, which are not enough for our current task. So in our case, we need to grab the files from the Qt `build` directory for that platform. For `iOS` it will be in the root of the build directory, for android it will be in an `Android` directory. Next, we should copy them into our `src` directory. For the `Info.plist` we've added it to `iOS/Info.plist`. For the `AndroidManifest.xml` we copied it into the `Android` directory, which is defined as the `ANDROID_SOURCE_DIR` in our `.pro`.

In step 3 of configuring the project for native sharing we'll come back to these files and add to them.

## <a name="mime-types"></a>A Few Words on MIME Types

MIME Types can turn out to be a bit of a balancing act. On one hand, you want to limit other apps sharing files that your app can't handle. Because of this, it helps to be as specific as possible. For example, if your app includes custom code for modifying images, you may want to specify _only_ allowing `.jpg`s or `.png`s since you may not have support for `.gif`.

However, you may run into some tough issues when attempting to be specific. Since other apps define the MIME type they are sending, and you are specifying which types to receive, you may run into issues when they don't match. A common example of this will be a `.json` file. Although in many cases it will be classified as `application/json` file, some applications may treat it as javascript, so they may send the MIME type as `application/javascript`.

The best way to deal with these constraints is to determine which apps you want to support sharing with, and test against _all_ those apps. Be ready to put in some extra work here when you're getting closer to production.

## <a name="further-steps"></a>Further Steps

* On an android device, each app's `FileProvider` must be unique. It is strongly recommend that you modify this path in the `AndroidManifest.xml`, and then update the resulting `AUTHORITY` string in the java `QShareUtils` object to reflect the unique path.
* Once you've gotten one sharing example working with a specific app, test out other apps that you may potentially want to share with. You may find that certain file types aren't sharing as expected. Look into [A Few Words on MIME Types](#mime-types) for help debugging this.
* You may want to refactor the java packages to better suit your application's existing packages. This will unfortunately requiring refactoring the `androidshareutils.cpp` file to your new package paths. Check [Corluma](https://github.com/timsee/Corluma) for an example of how the paths need to be refactored.
