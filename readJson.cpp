#include <iostream>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/json.h>
#include <fstream>

using namespace std;

Json::Value readJson()
{
  // JSON
  ifstream ifs("data.json");
  Json::Reader reader;
  Json::Value obj;
  reader.parse(ifs, obj);
  return  obj;
}