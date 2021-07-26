#!/usr/bin/bash
g++ -c arcade.cpp -o bin/arcade.o
g++ bin/arcade.o -o bin/arcade-app -ljsoncpp -lsfml-graphics -lsfml-window -lsfml-system
if [ "$1" == "run" ]; then
  bin/arcade-app
elif [ "$1" == "build" ]; then
  mkdir -p build/assets/
  cp assets/* build/assets/
  cp data.json build/
  cp bin/arcade-app build/
  cp RobotoCondensed-Regular.ttf build/
  zip -q veranda.zip build
  rm -r build
else
  echo "compiled in /bin"
fi
echo "$1"