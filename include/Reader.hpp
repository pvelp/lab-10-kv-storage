// Copyright 2022 pvelp

#ifndef STORAGE_READER_HPP
#define STORAGE_READER_HPP
#include <rocksdb/db.h>

#include "Field.hpp"
#include "ThreadPool.hpp"

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
#endif  // STORAGE_READER_HPP
