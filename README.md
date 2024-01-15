# codct
# Универсальная платформа для синтеза образовательных VR приложений

## Перечень направлений прикладного использования
1. Разработка интерактивных  VR учебных программ для школ (ЭОР)
2. Разработка интерактивных  VR школьниками в рамках изучения информатики в школе
3. Разработка интерактивных  VR учащимися НПО/СПО в рамках изучения информатики
4. Разработка игровых приложений

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
3. cmake -i ./    
4. Устанавливаем и собираем freetype2-devel
```
sudo apt-get install -y libfreetype-dev
```
5. Устанавливаем и собираем giflib-devel
```
sudo apt-get install -y libgif-dev
```
6. Устанавливаем и собираем zlib-devel
```
sudo apt-get install -y zlib1g
sudo apt-get install -y zlib1g-dev
```
7. Устанавливаем и собираем libpng-devel
```
sudo apt install libpng-dev
```
8. Устанавливаем и собираем libjpeg-devel
```
sudo apt install libjpeg-dev
```
9. Устанавливаем и собираем mesa-devel
```
sudo add-apt-repository ppa:oibaf/graphics-drivers
sudo apt update
sudo apt upgrade
```
10. Устанавливаем и собираем make, su , make install
11. Устанавливаем и собираем Распаковываем Certi
12. Устанавливаем и собираем bison, flex, m4
13. cmake -i ./
14. make, su, make install
15. define RTI_USES_STD_FSTREAM.....
16. Устанавливаем и собираем  freealut, openal, libvorbis , libvorbis-devel
17. Распаковываем Bullet
19. make, su, make install
20. Распаковываем Lua
21. readline-devel
22. make linux
23. su, make install
24. Распаковываем Cal3D
26. configure, make, su, make install
27. Распаковываем OsgCal
29. cmake -i ./
30. make, su, make install
31. SDL, SDL-devel (из интернет-репозитария)
32. freealut
33. скопировать содержимое Lib в usr/lib и сделать exe выполняемыми и все

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
1. Устанавливаем VisualStudio 2019 или VS2022
2. Устанавливаем CMAKE
3. Собираем OSG [так](https://habr.com/ru/articles/429816/) или [так](https://www.openscenegraph.com/index.php/documentation/platform-specifics/windows)
4. Открыть проект MAXplayer в VisualStudio и собрать конструктор
   
## Сборка на Android 
Сборка на андроид производится в соответствии с официальной [документацией]
(https://www.openscenegraph.com/index.php/documentation/platform-specifics/android/43-building-openscenegraph-for-android-3-0-2) на OSG