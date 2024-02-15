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
sudo apt install lua5.4 
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
~/codct$ git clone https://github.com/maximustyumen/codct . 
cd constructor_player/ cd src
make


## Сборка в Windows
### Для сборки порлностью самостоятельно - список зависимостей [тут](https://objexx.com/OpenSceneGraph.html , [так](https://habr.com/ru/articles/429816/) или [так](https://www.openscenegraph.com/index.php/documentation/platform-specifics/windows).
### Мы уже подготовили готовые библиотеки:
Все необходимые библиотеки находятся в каталоге
codct\constructor_player\DEPENDENCY

### Процесс установки
1. [Устанавливаем VisualStudio 2019 или VS2022](https://learn.microsoft.com/ru-ru/visualstudio/install/install-visual-studio?view=vs-2022).
2. Устанавливаем [CMAKE](https://cpp-python-nsu.inp.nsk.su/textbook/sec2/ch5).
3. Распаковать все архивы из codct\constructor_player\DEPENDENCY в свои каталоги
4. Последовательно для каждого каталога выполнить сборку проектов через CMake, открыть получившийся проект в VisualStudio и собрать в режиме Release
5. Открыть проект CODCT в VisualStudio и собрать конструктор (при необходимости настроить каталоги подключаемых заголовочных файлов и каталог библиотек на пути codct\constructor_player\DEPENDENCY).
   
## Сборка на Android 
Сборка на андроид производится в соответствии с официальной [документацией](https://www.openscenegraph.com/index.php/documentation/platform-specifics/android/43-building-openscenegraph-for-android-3-0-2) на OSG.
