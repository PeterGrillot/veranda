#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <memory>
#include <jsoncpp/json/json.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <file-base> [sort_key]\n";
    std::cerr << "Example: " << argv[0] << " console title\n";
    return 1;
  }

  std::string fileBase = argv[1];
  std::string sortKey = (argc >= 3) ? argv[2] : "title";
  std::string path = "./" + fileBase + ".json";

  std::ifstream ifs(path);
  if (!ifs) {
    std::cerr << "Error opening file: " << path << "\n";
    return 2;
  }

  Json::CharReaderBuilder rbuilder;
  Json::Value root;
  std::string errs;
  bool ok = Json::parseFromStream(rbuilder, ifs, &root, &errs);
  if (!ok) {
    std::cerr << "Failed to parse JSON: " << errs << "\n";
    return 3;
  }

  if (!root.isObject()) {
    std::cerr << "Unexpected JSON root (expected object)\n";
    return 4;
  }

  const Json::Value library = root["library"];
  if (!library || !library.isArray()) {
    std::cerr << "No 'library' array found in JSON\n";
    return 5;
  }

  std::vector<Json::Value> items;
  items.reserve(library.size());
  for (const auto& v : library) items.push_back(v);

  std::sort(items.begin(), items.end(), [&](const Json::Value& a, const Json::Value& b) {
    Json::Value va = a.get(sortKey, Json::Value(""));
    Json::Value vb = b.get(sortKey, Json::Value(""));

    std::string sa = va.isString() ? va.asString() : va.toStyledString();
    std::string sb = vb.isString() ? vb.asString() : vb.toStyledString();

    return sa < sb;
    });

  Json::Value outRoot;
  if (root.isMember("cmd")) outRoot["cmd"] = root["cmd"];
  Json::Value outLibrary(Json::arrayValue);
  for (const auto& it : items) outLibrary.append(it);
  outRoot["library"] = outLibrary;

  std::ofstream ofs(path);
  if (!ofs) {
    std::cerr << "Error opening file for writing: " << path << "\n";
    return 6;
  }

  Json::StreamWriterBuilder wbuilder;
  wbuilder["commentStyle"] = "None";
  wbuilder["indentation"] = "  ";
  std::unique_ptr<Json::StreamWriter> writer(wbuilder.newStreamWriter());
  writer->write(outRoot, &ofs);
  ofs << std::endl;

  std::cout << "Complete! Sorted " << path << " by key '" << sortKey << "'.\n";
  return 0;
}
