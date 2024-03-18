https://github.com/maximustyumen/codct/tree/main?tab=readme-ov-file#%D1%81%D0%B1%D0%BE%D1%80%D0%BA%D0%B0-%D0%BD%D0%B0-android

��������� ����������� �� ������
https://github.com/OGStudio/openscenegraph-cross-platform-guide/tree/master/1.8.SampleUnderAndroid

������������������ ������ (https://github.com/OGStudio/openscenegraph-cross-platform-guide/tree/master):

���������� Android Studio (https://developer.android.com/studio)

���������� 32-bit ���������� ��� ������ �������: sudo apt-get install libc6:i386 libncurses5:i386 libstdc++6:i386 lib32z1 libbz2-1.0:i386

��������� Android Studio �� ���������� ������������ (cd ~) ��� ������� �������: ./android-studio/bin/studio.sh

�������� ����������� ����� ��������� (standard installation options)

��������� SDK Manager ��� ������ ������������ ���� "Configure -> SDK Manager" � ������� �������������� ���������� (���������� CMake, LLDB, NDK)

�������� "Import an Android code sample"

�������� 'Hello GL2' ��� �������

��������� �������� ������ 'Hello GL2' �������. ���� ������ "Android Virtual Device" ���� - �������� ����������� ���������� (�������� Nexus One 3.7" with 480x800)

��������� ����������� ������� ��������, �������� armeabi-v7a ��� ARMv7

��������� ������, �� ������ ������� ������� ����������� �� ������� AVD ����� GLES2 ������

�������� �������� ������ �������.

� ������ ������� ������� ������������� C++ �������� ���������� ��� �������.

�������� ������ ������

� ������ ������� ����������� C++11, ���������� (exceptions � RTTI)

��������� ������, �� ������ ������� ����� �� ������

��������� � ������� OpenSceneGraph ��� ���������� ���: cd ~ cd AndroidStudioProjects git clone https://github.com/openscenegraph/OpenSceneGraph

��������� � ������� �������� ��� ���������� ���: cd ~ cd AndroidStudioProjects git clone https://github.com/ogstudio/openscenegraph-cross-platform-guide-application

��������� Android part � ����� ������: cd ~ cd AndroidStudioProjects cp -R openscenegraph-cross-platform-guide-application/android/app your_project

��������� ��������� ����������� ���������� osgNativeLib

�������� ����������� � ���� CMakeLists: include(CMakeLists-osgNativeLib.txt)

��������� �������� ������ ������� � ��� ������ �� ����� ��������� ����������� ����������

������ �������� ����� ������ � ������������� ������� �������� �� ������: https://github.com/OGStudio/openscenegraph-cross-platform-guide/tree/master