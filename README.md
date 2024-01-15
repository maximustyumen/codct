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

## Сборка в Lunux'е (Ubuntu версия старше 18.04)
Прежде всего обновляем базу репозиториев:
```
sudo apt-get update
```
Далее, по порядку:
1. Ставим cmake
```
sudo apt-get install cmake
```
2. Распаковываем OSG
3. Устанавливаем и собираем freetype2-devel
```
sudo apt-get install -y libfreetype-dev
```
4. Устанавливаем и собираем giflib-devel
```
sudo apt-get install -y libgif-dev
```
5. Устанавливаем и собираем zlib-devel
```
sudo apt-get install -y zlib1g
sudo apt-get install -y zlib1g-dev
```
6. Устанавливаем и собираем libpng-devel
```
sudo apt-get install libpng-dev
```
7. Устанавливаем и собираем libjpeg-devel
```
sudo apt-get install libjpeg-dev
```
8. Устанавливаем и собираем mesa-devel
```
sudo add-apt-repository ppa:oibaf/graphics-drivers
sudo apt-get update
sudo apt-get upgrade
```
9. Устанавливаем и собираем распаковываем Certi
```
скачайте [пакет](http://download.savannah.nongnu.org/releases/certi/)
tar zxvf certi-3.4.0-Source.tar.gz
mkdir build_certi
cd build_certi
cmake
make
make install
make package
```
10. Устанавливаем и собираем bison, flex, m4
```
sudo apt-get install flex bison
sudo apt-get install libxml2-dev
sudo apt-get install libx11-dev xbitmaps
```
11. define RTI_USES_STD_FSTREAM.
12. Устанавливаем и собираем  freealut, openal, libvorbis, libvorbis-devel
```
sudo apt-get install -y libalut-dev

git clone git@github.com:kcat/openal-soft.git
cd openal-soft 
cd build
cmake ..
make

sudo apt-get install -y libvorbis-dev
```
13. Распаковываем Bullet
```
sudo snap install bullet
```
14. Распаковываем Lua
```
sudo apt-get install lua5.3
```
15. readline-devel
```
sudo apt-get install libreadline6 libreadline6-dev
```
16. Распаковываем Cal3D
```
sudo apt-get install libcal3d12-dev
```
17. Распаковываем OsgCal
```
sudo apt-get install osgcal
```
18. SDL, SDL-devel
```
sudo apt-get install libsdl2-dev libsdl2-2.0-0 -y;
```

## Сборка в Windows
### [Список](https://objexx.com/OpenSceneGraph.html) зависимостей
OpenSceneGraph 3.6.5 (VC2022 2023/01 Build)
curl 7.87.0
FBX 2020.3.2
ffmpeg 4.4.1
freetype 2.12.0
GDAL 3.0.6
giflib 5.2.1
glut 3.7
libjpeg 9e
libpng 1.6.39
libtiff 4.5.0
minizip git
zlib 1.2.13
### Процесс установки
1. [Устанавливаем VisualStudio 2019 или VS2022](https://learn.microsoft.com/ru-ru/visualstudio/install/install-visual-studio?view=vs-2022).
2. Устанавливаем [CMAKE](https://cpp-python-nsu.inp.nsk.su/textbook/sec2/ch5).
3. Собираем OSG [так](https://habr.com/ru/articles/429816/) или [так](https://www.openscenegraph.com/index.php/documentation/platform-specifics/windows).
4. Открыть проект в VisualStudio и собрать конструктор.
   
## Сборка на Android 
Сборка на андроид производится в соответствии с официальной [документацией](https://www.openscenegraph.com/index.php/documentation/platform-specifics/android/43-building-openscenegraph-for-android-3-0-2) на OSG.