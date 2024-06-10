Rem Note: the following requires the Android NDK which should be included in this repository and MinGW!
Rem Note: On windows problems can occur if the path of the NDK contains whitespaces...!!!
set ANDROID_NDK=%cd%/NDK

if not exist build_android mkdir build_android
cd build_android
del * /S /Q

cmake ^
-G "MinGW Makefiles"  ^
-DANDROID_BUILD=True ^
-DCMAKE_SYSTEM_NAME=Android ^
-DCMAKE_TOOLCHAIN_FILE="%ANDROID_NDK%/build/cmake/android.toolchain.cmake" ^
-DCMAKE_MAKE_PROGRAM="%ANDROID_NDK%/prebuilt/windows-x86_64/bin/make.exe" ^
-DANDROID_NDK="%ANDROID_NDK%" ^
-DANDROID_PLATFORM=android-16 ^
-DANDROID_STL=c++_static ^
-DCMAKE_BUILD_TYPE=Release ^
-DANDROID_ABI=arm64-v8a ^
..

cmake ^
--build . --target RecordingPlugin

cd ..

Rem -DANDROID_ABI=arm64-v8a
Rem -DANDROID_ABI=armeabi-v7a