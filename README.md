# veranda

A really quick and dirty MAME front end built in C++ and SFML. You will need MAME set up. Also I built it on linux, so system command would need to be updated for OS.

## C++ Deps

```
jsoncpp
sfml-graphics
sfml-window
sfml-system
```

## To Run:

```bash
chmod +x ./compiler.sh
./compiler.sh
```

## Sort JSON

Node required, run `node jsonSort.js` to sort your json by title. You could ass year by editing `jsonSort >> ln 19` file. Should probably just do it in c++, but this is quick and easy.
