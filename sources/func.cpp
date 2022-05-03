// Copyright 2021 Your Name <your_email>


//#include <stdexcept>
//#include "rocksdb.hpp"
#include <queue>
//#include <map>
#include <string>
#include <random>

//void print_db(RDB_Manager&& rdb){
//  std::queue<std::pair<std::string, std::map<std::string, std::string>>>q;
//  std::vector<std::string> res = rdb.getColumnFamilyNames();
//  for (auto& r : res) {
//    rdb.readColumn(r, q);
//  }
//  while (!q.empty()){
//    std::pair<std::string, std::map<std::string, std::string>> val = q.front();
//    q.pop();
//    for(auto& item : val.second){
//      std::cout << val.first + " : [" << item.first + " : " << item.second + "]" << std::endl;
//    }
//  }
//}


std::string create_random_str() {
  static const auto magic = static_cast<size_t>(time(nullptr));
  static std::mt19937 randomGenerator(magic);

  std::string res;
  for (size_t i = 0; i < 10; ++i)
    res += static_cast<char>(randomGenerator() % 26 + 97);

  return res;
}
