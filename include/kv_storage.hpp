// Copyright 2022 pvelp

#ifndef INCLUDE_KV_STORAGE_HPP_
#define INCLUDE_KV_STORAGE_HPP_
#include <rocksdb/db.h>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/program_options.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "Field.hpp"
#include "Reader.hpp"
#include "Writer.hpp"

namespace po = boost::program_options;
using string = std::string;
using std::cout;
using std::endl;

const int spases_for_table[4] = {76, 74, 42, 33};

class Storage{
 public:
  explicit Storage(int& num_workers);

  static void start(const string& path_to_db,
                    int num_workers, const string& path_to_out_db);

  static void check_status(const rocksdb::Status& status,
                           const string& message);

  static void create_descriptors(
      const std::vector<string>& names_cf,
      std::vector<rocksdb::ColumnFamilyDescriptor>& descriptors_cf);

  static void create_iterators(
      rocksdb::DB* db,
      const std::vector<rocksdb::ColumnFamilyHandle*>& handles_cf,
      std::vector<rocksdb::Iterator*>& iterators_cf);

  static void delete_db(
      rocksdb::DB* db,
      const std::vector<rocksdb::ColumnFamilyHandle*>& handles_cf,
      const std::vector<rocksdb::Iterator*>& iterators_cf);

  static void delete_db(
      rocksdb::DB* db,
      const std::vector<rocksdb::ColumnFamilyHandle*>& handles_cf);

  static void check_iterators(
      const std::vector<rocksdb::Iterator*>& iterators_cf,
      const std::vector<rocksdb::ColumnFamilyHandle*>& handles_cf,
      std::queue<Field>& fields, std::vector<int>& nums_in_columns);

  static std::stringstream print_table(
      const std::vector<Field>& out_fields,
      const std::vector<rocksdb::ColumnFamilyHandle*>& handles_cf_out,
      const std::vector<int>& nums_in_columns);

  static void hashing(Storage* storage, std::queue<Field>& fields,
                      std::queue<Field>& hashs, std::vector<Field>& out_fields);

  static void init(const boost::log::trivial::severity_level& sev_lvl);

  static boost::log::trivial::severity_level choose_sev_lvl(
      const string& sev_lvl_str);

  static void str_of_separator(std::stringstream& ss);

  Writer& use_writer();

  Reader& use_reader();

 private:
  Writer writer;
  Reader reader;
};
#endif // INCLUDE_KV_STORAGE_HPP_
