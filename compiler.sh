#!/usr/bin/bash
g++ -c arcade.cpp -o bin/arcade.o
g++ bin/arcade.o -o bin/arcade-app -ljsoncpp -lsfml-graphics -lsfml-window -lsfml-system
if [ "$1" == "run" ]; then
  bin/arcade-app
elif [ "$1" == "build" ]; then
  mkdir -p build
  cp assets/* build/
  cp data.json build/
  cp bin/arcade-app build/
else
  echo "compiled in /bin"
fi
echo "$1"