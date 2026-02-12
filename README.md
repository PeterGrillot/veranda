# Veranda

### Game Launcher

Veranda is a lightweight MAME front end built in C++ using SFML. It provides a simple, fast interface for browsing and launching your MAME games but with some tweaking, it can launch anything! Designed for Linux, it relies on a local MAME or emulator setup and may require further adjustments for other operating systems.

## Arcade

![Arcade](./assets/screenshots/arcade.png?raw=true "Arcade")

# Console

![console](./assets/screenshots/console.png?raw=true "console")

## Developement

### C++ Dependancies

```bash
sudo apt install libsfml-dev libjsoncpp-dev
```

Veranda uses the following libraries:

- jsoncpp
- sfml-graphics
- sfml-window
- sfml-system

## To Run:

```bash
# Compile
chmod +x ./scripts/compiler.sh
./scripts/compiler.sh
# Run
./veranda-app/veranda
```

## Sorting

You can sort the JSON by changing the compile script. Also you can run ince compiled in the veranda-app folder.

```bash
./jsonSort console title
./jsonSort mame title
# How to
./jsonSort {json} {key}
```

If you want to sort by something other than title, just change the key.
