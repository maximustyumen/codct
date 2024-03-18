# codct
# Универсальная платформа для синтеза образовательных VR приложений

## Перечень направлений прикладного использования
1. Разработка интерактивных  VR учебных программ для школ (ЭОР).
2. Разработка интерактивных  VR школьниками в рамках изучения информатики в школе.
3. Разработка интерактивных  VR учащимися НПО/СПО в рамках изучения информатики.
4. Разработка игровых приложений.

## Для запуска и работы программного продукта минимальные системные требования - следующие:
+ Процессор Intel i5;
+ Оперативная память 16 Гб;
+ Видеокарта минимум GeForce 1660 (разъем DisplayPort необходим, если шлем подключается напрямую к ПК. Если шлем подключается через WiFi, то необходимо его подключение к роутеру стандарта WiFi6)
+ Жесткий диск 10Гб (свободного места);
+ Цветной монитор Full HD (1920x1080 пикселей);
+ Сетевая карта, клавиатура, мышь, акустическая система;
+ Интернет-соединение;
+ Операционная система (64-разрядная): Win 10. Проверены запуски на Linux (Ubuntu, AstraLinux, AltLinux и т.д.);
+ Шлем VR линейки HTC Vive Elite, Pro и выше;
+ Базовые станции для трекинга датчиков.

## Сборка в Lunux'е (Ubuntu 20.04.1 x86_64)
Прежде всего обновляем базу репозиториев:
```
sudo apt-get update
```
sudo apt install gcc

sudo apt -y install openscenegraph

sudo apt install libopenscenegraph-dev

sudo apt install git

sudo apt install make

sudo apt install lua5.3 

sudo apt install libsdl2-2.0-0

sudo apt install libsdl1.2-dev

sudo apt install g++ 

sudo apt install curl

sudo apt -y install libcurl4-openssl-dev

sudo apt -y install libopenvr-dev

sudo apt install libopenal-dev

sudo apt install libalut-dev

sudo apt install liblua5.3-dev

sudo apt install libsdl2-dev

sudo apt install libvorbis-dev

sudo apt install freeglut3-dev

sudo apt install libsdl1.2-dev

mkdir codct

cd codct

~/codct$ git clone https://github.com/maximustyumen/codct

cd constructor_player/ cd src

make

Видео процесса сборки и запуска приложения: https://youtu.be/rG9VwT4sems?si=SHkW4gwkiaTJ-aYi


## Сборка в Windows
### Для сборки порлностью самостоятельно - список зависимостей [тут](https://objexx.com/OpenSceneGraph.html , [так](https://habr.com/ru/articles/429816/) или [так](https://www.openscenegraph.com/index.php/documentation/platform-specifics/windows).
### Мы уже подготовили готовые библиотеки:
Все необходимые библиотеки находятся в каталоге
codct\constructor_player\DEPENDENCY

### Процесс установки
1. [Устанавливаем VisualStudio 2019 или VS2022](https://learn.microsoft.com/ru-ru/visualstudio/install/install-visual-studio?view=vs-2022).
2. Устанавливаем [CMAKE](https://cpp-python-nsu.inp.nsk.su/textbook/sec2/ch5).
4. Распаковать все архивы из codct\constructor_player\DEPENDENCY в свои каталоги
5. Для каталога codct\OpenSceneGraph выполнить сборку проекта через CMake, открыть получившийся проект в VisualStudio и собрать в режиме Release
6. Открыть проект CODCT в VisualStudio и собрать конструктор (при необходимости настроить каталоги подключаемых заголовочных файлов и каталог библиотек на пути codct\constructor_player\DEPENDENCY и codct\OpenSceneGraph).
   
## Сборка на Android 
Сборка на андроид производится в соответствии с официальной [документацией](https://www.openscenegraph.com/index.php/documentation/platform-specifics/android/43-building-openscenegraph-for-android-3-0-2) на OSG.
https://www.openscenegraph.com/index.php/documentation/platform-specifics/android/178-building-openscenegraph-for-android-3-4
### Видео процесса сборки приложений на основе OSG в Android
https://rutube.ru/video/b2933dc78abc4145deded37294f41dd8/
### Пошаговое руководство по сборке 
https://github.com/OGStudio/openscenegraph-cross-platform-guide/tree/master/1.8.SampleUnderAndroid

Последовательность сборки (https://github.com/OGStudio/openscenegraph-cross-platform-guide/tree/master):

1. Установить Android Studio (https://developer.android.com/studio)
   
2. Установите 32-bit библиотеки при помощи команды: sudo apt-get install libc6:i386 libncurses5:i386 libstdc++6:i386 lib32z1 libbz2-1.0:i386

3. Запустите Android Studio из директории пользователя (cd ~) при пормощи команды: ./android-studio/bin/studio.sh

4. Выберите стандартные опции установки (standard installation options)

5. Запустите SDK Manager при помощи контекстного меню "Configure -> SDK Manager" и укажите дополнительные компоненты (необходимы CMake, LLDB, NDK)

6. Выберите "Import an Android code sample"

7. Напишите 'Hello GL2' для импорта

8. Выполните тестовый запуск 'Hello GL2' примера. Если список "Android Virtual Device" пуст - выберите эмулируемое устройство (например Nexus One 3.7" with 480x800)
   
9. Загрузите необходимую систему эмуляции, например armeabi-v7a для ARMv7

10. Запустите пример, вы должны увидеть зеленый треугольник на дисплее AVD через GLES2 рендер

11. Выберите создание нового проекта.

12. В опциях проекта укажите использование C++ нативной библиотеки для проекта.

13. Выберите пустой проект

14. В опциях укажите подключение C++11, исклчюений (exceptions и RTTI)

15. Запустите проект, Вы должны увидеть текст на экране

16. Перейдите в каталог OpenSceneGraph или клонируйте его: cd ~  cd AndroidStudioProjects git clone https://github.com/openscenegraph/OpenSceneGraph

17. Перейдите в каталог примеров или клонируйте его: cd ~  cd AndroidStudioProjects  git clone https://github.com/ogstudio/openscenegraph-cross-platform-guide-application

18. Копируйте Android part в новый проект: cd ~    cd AndroidStudioProjects   cp -R openscenegraph-cross-platform-guide-application/android/app your_project

19. Выполните постоение статической библиотеки osgNativeLib

20. Включите зависимость в файл CMakeLists:  include(CMakeLists-osgNativeLib.txt)

21. Выполните итоговую сборку проекта и его запуск на ранее выбранном эмулируемом устройстве

22. Полный перечень шагов сборки с изображениями экранов доступен по адресу: https://github.com/OGStudio/openscenegraph-cross-platform-guide/tree/master


## примеры  новых вариантов использования кода с инструкциями по использованию библиотеки
### Варианты использования части платформы -стандарта OPC UA отдельно доступны на странице проекта (есть примеры использования для c++, c#, Unity, Unreal) -  https://github.com/maximum2000/open62541_Unity 

Поддержка архитектуры OPC Unified Architecture — спецификации, определяющей передачу данных в промышленных сетях и взаимодействие устройств в них. Поскольку тренажеры очень часто имитируют место оператора (SCADA-системы), а там в свою очередь OPC UA является де-факто "стандартом".....

Порядок сборки:

Установка cmake, установка VisualStudio (рекомендуется 2022)

Конфигурация проекта open62541 при помощи cmake

Сборка open62541

Сборка DLLtoUnity (не забыть поменять путь до xml в коде)

Скопировать файлы DLL в папку plugins в проекте Unity

Запустить (сервер) в командной строке

Запустить Unity, открыть сцену, проверить работу

### Варианты использования части платформы - полнофункциональная поддержка архитектуры имитационных систем IEEE 1516-2010 / HLA Evolved  (есть примеры использования для c++, c#, Unity, Unreal) - https://github.com/maximum2000/OpenRTI_Unity

Состав:

DLLtoUnity - собственно DLL для связи Unity<->OpenRTI

OpenRTI - библиотека OpenRTI (клон https://github.com/onox/OpenRTI.git)

UnityTest - тестовый проект Unity (2021.3.8)

Порядок сборки:

Установка cmake, установка VisualStudio (рекомендуется 2022)

Конфигурация проекта OpenRTI при помощи cmake

Сборка OpenRTI

Сборка DLLtoUnity (не забыть поменять путь до xml в коде)

Скопировать файлы DLL в папку plugins в проекте Unity

Запустить файл rtinode.exe (сервер) в командной строке

Запустить Unity, открыть сцену, проверить работу

Поскольку для создания образовательных приложений могут использоваться инструменты, основанные на различных языках программирования, мы активно развиваем имплементацию стандарта IEEE 1516-2010 для таких инструментов. Наш репозитарий может быть полезен для использования стандарта IEEE 1516-2010 в средах разработки на основе C# (в т.ч. в проекты Unity), т.к. открытые и коммерческие версии реализации данного стандарта включают только C++ / JAVA.

Проект позволяет приложениям на Unity реализовать полнофункциональную поддержку архитектуры имитационных систем IEEE 1516-2010 / HLA Evolved – сразу всех вариантов стандарта – HLA 1.3, IEEE1516 и IEEE1516E. Это означает 100% совместимость со всеми распределенными тренажерными. Список совместимых программных инфраструктур для поддержки группы международных стандартов семейства IEEE 1516-2010::

RRTI (“Russian RTI”)

pRTI 1.3, 1516 and 1516e from Pitch Technologies.

MAK RTI 1.3, 1516 and 1516e from Mak Technologies

RTI NG Pro 1.3, 1516 and 1516e from Raytheon

OpenSource RTI Portico

OpenSource RTI CERTI

OpenSource RTI Open-RTI


### Варианты использования части платформы - примеры работы с xAPI  - https://github.com/maximum2000/xAPI_cpp

xAPI — это спецификация программ в сфере дистанционного обучения, которая позволяет обучающим системам общаться между собой путём отслеживания и записи учебных занятий всех видов. Информация об учебной деятельности сохраняется в специальную базу — хранилище учебных записей (англ. learning record store, LRS).

Подробнее: https://habr.com/ru/articles/508624/  и https://habr.com/ru/articles/551374/

