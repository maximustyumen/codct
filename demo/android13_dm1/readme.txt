https://github.com/maximustyumen/codct/tree/main?tab=readme-ov-file#%D1%81%D0%B1%D0%BE%D1%80%D0%BA%D0%B0-%D0%BD%D0%B0-android

Пошаговое руководство по сборке
https://github.com/OGStudio/openscenegraph-cross-platform-guide/tree/master/1.8.SampleUnderAndroid

Последовательность сборки (https://github.com/OGStudio/openscenegraph-cross-platform-guide/tree/master):

Установить Android Studio (https://developer.android.com/studio)

Установите 32-bit библиотеки при помощи команды: sudo apt-get install libc6:i386 libncurses5:i386 libstdc++6:i386 lib32z1 libbz2-1.0:i386

Запустите Android Studio из директории пользователя (cd ~) при пормощи команды: ./android-studio/bin/studio.sh

Выберите стандартные опции установки (standard installation options)

Запустите SDK Manager при помощи контекстного меню "Configure -> SDK Manager" и укажите дополнительные компоненты (необходимы CMake, LLDB, NDK)

Выберите "Import an Android code sample"

Напишите 'Hello GL2' для импорта

Выполните тестовый запуск 'Hello GL2' примера. Если список "Android Virtual Device" пуст - выберите эмулируемое устройство (например Nexus One 3.7" with 480x800)

Загрузите необходимую систему эмуляции, например armeabi-v7a для ARMv7

Запустите пример, вы должны увидеть зеленый треугольник на дисплее AVD через GLES2 рендер

Выберите создание нового проекта.

В опциях проекта укажите использование C++ нативной библиотеки для проекта.

Выберите пустой проект

В опциях укажите подключение C++11, исклчюений (exceptions и RTTI)

Запустите проект, Вы должны увидеть текст на экране

Перейдите в каталог OpenSceneGraph или клонируйте его: cd ~ cd AndroidStudioProjects git clone https://github.com/openscenegraph/OpenSceneGraph

Перейдите в каталог примеров или клонируйте его: cd ~ cd AndroidStudioProjects git clone https://github.com/ogstudio/openscenegraph-cross-platform-guide-application

Копируйте Android part в новый проект: cd ~ cd AndroidStudioProjects cp -R openscenegraph-cross-platform-guide-application/android/app your_project

Выполните постоение статической библиотеки osgNativeLib

Включите зависимость в файл CMakeLists: include(CMakeLists-osgNativeLib.txt)

Выполните итоговую сборку проекта и его запуск на ранее выбранном эмулируемом устройстве

Полный перечень шагов сборки с изображениями экранов доступен по адресу: https://github.com/OGStudio/openscenegraph-cross-platform-guide/tree/master