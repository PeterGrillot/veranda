#!/usr/bin/bash
g++ -c Source/main.cpp Source/initializeText.cpp Source/ui.cpp Source/readJson.cpp
g++ main.o ui.o readJson.o initializeText.o -o veranda -ljsoncpp -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
rm main.o ui.o initializeText.o readJson.o
mkdir -p veranda-app
mkdir -p veranda-app/assets/
cp -r assets/* veranda-app/assets/
cp *.json veranda-app/
mv veranda veranda-app/
cp assets/RobotoCondensed-Regular.ttf veranda-app/

if [ "$1" == "build" ]; then
  zip -r -q veranda-app.zip veranda-app/
elif [ "$1" == "run" ]; then
  ./veranda-app/veranda
elif [ "$1" == "install" ]; then
  zip -r -q veranda-app.zip veranda-app/
  rm -rf ~/veranda-app/
  cp veranda-app.zip ~/
  unzip -qo ~/veranda-app.zip -d ~/
  rm veranda-app.zip
  rm ~/veranda-app.zip
fi