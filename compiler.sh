#!/usr/bin/bash
g++ -c main.cpp readJson.cpp
g++ main.o readJson.o -o arcade-app -ljsoncpp -lsfml-graphics -lsfml-window -lsfml-system
rm main.o readJson.o
mkdir -p build
mkdir -p build/assets/
cp assets/* build/assets/
cp data.json build/
cp arcade-app build/
cp RobotoCondensed-Regular.ttf build/
zip -r veranda-app.zip build/
rm arcade-app

if [ "$1" == "run" ]; then
  ./build/arcade-app
fi