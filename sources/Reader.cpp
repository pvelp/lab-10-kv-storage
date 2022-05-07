// Copyright 2022 pvelp
#include "Reader.hpp"

#include "kv_storage.hpp"
Reader::Reader(const int& num_workers) : readers(num_workers) {}
void Reader::read(const Field& field_) {
  readers.enqueue([&](const Field& field){
    ++temp_nums_in_column;
    BOOST_LOG_TRIVIAL(info) << "write thread"
                            << std::this_thread::get_id();
    db_out->Put(rocksdb::WriteOptions(), handles_cf_out.front(),
                field.key, field.value);
    if (nums_in_columns.front() == temp_nums_in_column){
      handles_cf_out.erase(handles_cf_out.begin());
      nums_in_columns.erase(nums_in_columns.begin());
      temp_nums_in_column = 0;
    }
  }, field_);
}
void Reader::setting(
    rocksdb::DB* db_out_,
    const std::vector<rocksdb::ColumnFamilyHandle*>& handles_cf_out_,
    const std::vector<int>& nums_in_columns_) {
  db_out = db_out_;
  handles_cf_out = handles_cf_out_;
  nums_in_columns = nums_in_columns_;
}
