#!/usr/bin/bash
g++ -c main.cpp readJson.cpp
g++ main.o readJson.o -o veranda -ljsoncpp -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
rm main.o readJson.o
mkdir -p veranda-app
mkdir -p veranda-app/assets/
cp assets/* veranda-app/assets/
cp data.json veranda-app/
mv veranda veranda-app/
cp RobotoCondensed-Regular.ttf veranda-app/
zip -r veranda-app.zip veranda-app/

if [ "$1" == "run" ]; then
  ./veranda-app/veranda
fi