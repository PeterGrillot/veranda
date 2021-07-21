#!/usr/bin/bash
g++ -c arcade.cpp -o bin/arcade.o
g++ bin/arcade.o -o bin/arcade-app -ljsoncpp -lsfml-graphics -lsfml-window -lsfml-system
if [ "$1" == "run" ]; then
  bin/arcade-app
else
  echo "compiled in /bin"
fi
echo "$1"