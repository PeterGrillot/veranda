#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>

using namespace std;

Json::Value readJson(string path)
{
  // JSON
  ifstream ifs(path);
  Json::Reader reader;
  Json::Value obj;
  reader.parse(ifs, obj);
  return  obj;
}