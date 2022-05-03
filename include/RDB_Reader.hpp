//
// Created by pvelp on 02.05.22.
//

#ifndef LAB_10_RDB_READER_HPP
#define LAB_10_RDB_READER_HPP
#include "rocksdb.hpp"
#include <queue>

class RDB_Reader{
 public:
  explicit RDB_Reader(const std::string& filename);
  void readColumn(const std::string& column, std::queue<std::pair<std::string,
                                                                  std::map<std::string,
                                                                           std::string>>>& q);
//                  std::map<std::string,
//                           std::map<std::string, std::string>> &storage);

  std::vector<std::string> columnNames();
  ~RDB_Reader();
 private:
  RDB_Manager db;
};



#endif  // LAB_10_RDB_READER_HPP
