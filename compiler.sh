#!/usr/bin/bash
g++ -c main.cpp ui.cpp readJson.cpp
g++ main.o ui.o readJson.o -o veranda -ljsoncpp -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
rm main.o ui.o readJson.o
mkdir -p veranda-app
mkdir -p veranda-app/assets/
cp -r assets/* veranda-app/assets/
cp *.json veranda-app/
mv veranda veranda-app/
cp RobotoCondensed-Regular.ttf veranda-app/
zip -r -q veranda-app.zip veranda-app/

if [ "$1" == "run" ]; then
  ./veranda-app/veranda
fi