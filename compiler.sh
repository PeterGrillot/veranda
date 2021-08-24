#!/usr/bin/bash
g++ -c main.cpp readJson.cpp
g++ main.o readJson.o -o bin/arcade-app -ljsoncpp -lsfml-graphics -lsfml-window -lsfml-system
rm main.o readJson.o
if [ "$1" == "run" ]; then
  bin/arcade-app
elif [ "$1" == "build" ]; then
  mkdir -p veranda-app
  mkdir -p veranda-app/assets/
  cp assets/* veranda-app/assets/
  cp data.json veranda-app/
  cp bin/arcade-app veranda-app/
  cp RobotoCondensed-Regular.ttf veranda-app/
  zip -r veranda-app.zip veranda-app/
  rm -r veranda-app
else
  echo "compiled in /bin"
fi
echo "$1"