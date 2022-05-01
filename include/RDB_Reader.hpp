//
// Created by pvelp on 02.05.22.
//

#ifndef LAB_10_RDB_READER_HPP
#define LAB_10_RDB_READER_HPP
#include "rocksdb.hpp"

class RDB_Reader{
 public:
  explicit RDB_Reader(const std::string& filename);
  void readColumn(const std::string& column);

 private:
  RDB_Manager db;
};



#endif  // LAB_10_RDB_READER_HPP
