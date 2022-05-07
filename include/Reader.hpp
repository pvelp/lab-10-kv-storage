// Copyright 2022 pvelp

#ifndef INCLUDE_READER_HPP_
#define INCLUDE_READER_HPP_
#include <rocksdb/db.h>

#include "Field.hpp"
#include "ThreadPool.hpp"
#include <vector>
class Reader{
 public:
  explicit Reader(const int& num_workers);

  void read(const Field& field);

  void setting(rocksdb::DB* db_out,
               const std::vector<rocksdb::ColumnFamilyHandle*>& handles_cf_out,
               const std::vector<int>& nums_in_columns);

 private:
  ThreadPool readers;
  rocksdb::DB* db_out;
  std::vector<rocksdb::ColumnFamilyHandle*> handles_cf_out;
  std::vector<int> nums_in_columns;
  int temp_nums_in_column = 0;
};
#endif  // INCLUDE_READER_HPP_
