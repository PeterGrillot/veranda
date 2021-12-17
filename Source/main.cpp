#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <chrono>
#include <thread>
#include <string>
#include <sstream>
#include <libgen.h>
#include <unistd.h>
#include <linux/limits.h>

#include "../Headers/ui.h"
#include "../Headers/initializeText.h"
#include "../Headers/readJson.h"

using namespace std;

int main()
{
  buildUI();
}