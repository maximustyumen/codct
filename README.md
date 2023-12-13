# codct
Универсальная платформа для синтеза образовательных VR приложений

Сборка в Lunux'е

1. Ставим cmake
2. Распаковываем OSG
3. cmake -i ./    
4. Устанавливаем и собираем freetype2-devel
5. Устанавливаем и собираем giflib-devel 
6. Устанавливаем и собираем zlib-devel
7. Устанавливаем и собираем libpng-devel
8. Устанавливаем и собираем libjpeg-devel 
9. Устанавливаем и собираем mesa-devel
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

Сборка в Windows
1.Устанавливаем VisualStudio 2019 или VS2022

2.Устанавливаем CMAKE

3.Собираем OSG (https://habr.com/ru/articles/429816/) или (https://www.openscenegraph.com/index.php/documentation/platform-specifics/windows)

  Список зависимостей: (https://objexx.com/OpenSceneGraph.html)
  
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
4. Открыть проект MAXplayer в VisualStudio и собрать конструктор

