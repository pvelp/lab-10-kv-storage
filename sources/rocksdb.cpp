//
// Created by pvelp on 01.05.22.
//
#include "rocksdb.hpp"
#include <thread>
#include <utility>

std::vector<std::string> RDB_Manager::getColumnFamilyNames
    (const std::string &filename, bool create) {
  std::vector<std::string> res;
  rocksdb::Status s = rocksdb::DB::ListColumnFamilies(rocksdb::Options(),
                                                      filename,
                                                      &res);
  if(!s.ok() && !(s.IsIOError() && create)){
    throw std::runtime_error("Failed to retrieve column from db");
  } else if (!s.ok()){
    res.push_back(rocksdb::kDefaultColumnFamilyName);
  }
  return res;
}

RDB_Manager::RDB_Manager(std::string fname, bool readOnly, bool create)
    : filename(std::move(fname)), _readOnly(readOnly){
  static const rocksdb::CompressionType prefferedCompression =
      rocksdb::kNoCompression;

  rocksdb::Status s{};
  rocksdb::Options options{};
  options.IncreaseParallelism(
      static_cast<int>(std::thread::hardware_concurrency()));
  options.OptimizeLevelStyleCompaction();
  options.OptimizeForSmallDb();
  options.compression = prefferedCompression;
  options.compression_per_level = std::vector<rocksdb::CompressionType>();
  options.create_if_missing = create;

  columnFamilyNames = getColumnFamilyNames(filename, create);
  for (auto& columnFamilyName : columnFamilyNames) {
    rocksdb::ColumnFamilyDescriptor descriptor{};
    descriptor.name = columnFamilyName;
    descriptor.options.OptimizeLevelStyleCompaction();
    descriptor.options.OptimizeForSmallDb();
    descriptor.options.compression = prefferedCompression;
    descriptor.options.compression_per_level =
        std::vector<rocksdb::CompressionType>();
    columnFamilyDescriptors.push_back(std::move(descriptor));
  }
  if (readOnly){
    s = rocksdb::DB::OpenForReadOnly(options,
                                     filename,
                                     columnFamilyDescriptors,
                                     &columnFamilyHandles,
                                     &db);
  } else {
    s = rocksdb::DB::Open(options,
                          filename,
                          columnFamilyDescriptors,
                          &columnFamilyHandles,
                          &db);
  }
  if (!s.ok()){
    throw std::runtime_error("Failed to open db");
  }
}

RDB_Manager::RDB_Manager(RDB_Manager &&rdb) noexcept
    : db(rdb.db),
      filename(rdb.filename),
      columnFamilyNames(std::move(rdb.columnFamilyNames)),
      columnFamilyDescriptors(std::move(rdb.columnFamilyDescriptors)),
      columnFamilyHandles(std::move(rdb.columnFamilyHandles)),
      _readOnly(rdb._readOnly) {}

RDB_Manager::~RDB_Manager() {
  for (auto* handle : columnFamilyHandles){
    db->DestroyColumnFamilyHandle(handle);
  }
  rocksdb::Status s = db->Close();
//  if (!s.ok()){
//    throw std::runtime_error("Failed to close db");
//  }
  delete db;
}

const std::vector<std::string>& RDB_Manager::getColumnFamilyNames() const {
  return columnFamilyNames;
}

void RDB_Manager::addNode(const std::string &key, const std::string &value) {
  if (_readOnly){
    throw std::runtime_error("Tried to modify read-only db");
  }
  rocksdb::Status s = db->Put(rocksdb::WriteOptions(), key, value);
  if (!s.ok()){
    throw std::runtime_error("Failed to add node with errpr: "
         + s.ToString());
  }
}

void RDB_Manager::addNode(const std::string &column,
                          const std::string &key,
                          const std::string &value) {
  if(_readOnly){
    throw std::runtime_error("Tried to modify read-only db");
  }
  rocksdb::ColumnFamilyHandle* handle = findHandle(column);
  if (handle == nullptr){
    rocksdb::ColumnFamilyDescriptor descriptor{};
    descriptor.name = column;
    descriptor.options.OptimizeLevelStyleCompaction();
    descriptor.options.OptimizeForSmallDb();
    descriptor.options.compression = rocksdb::kNoCompression;
    descriptor.options.compression_per_level =
      std::vector<rocksdb::CompressionType>();
    db->CreateColumnFamily(descriptor.options,
                           descriptor.name,
                           &handle);
    columnFamilyNames.push_back(column);
    columnFamilyDescriptors.push_back(std::move(descriptor));
    columnFamilyHandles.push_back(handle);
  }
  rocksdb::Status s = db->Put(rocksdb::WriteOptions(),
                              handle,
                              key,
                              value);
  if (!s.ok()){
    throw std::runtime_error("Failed to add node with errpr: "
                             + s.ToString());
  }
}

void RDB_Manager::addNode(const Node& node) {
  addNode(node.column, node.key, node.value);
}

std::string RDB_Manager::getNode(const std::string &key) {
  std::string res{};
  rocksdb::Status s = db->Get(rocksdb::ReadOptions(), key, &res);
  if (!s.ok()){
    throw std::runtime_error("Failed to get node with error: "
                             + s.ToString());
  }
  return res;
}

std::string RDB_Manager::getNode(const std::string &column,
                                 const std::string &key) {
  std::string res{};
  rocksdb::ColumnFamilyHandle* handle = findHandle(column);
  if (handle == nullptr){
    throw std::runtime_error("Column not found");
  }
  rocksdb::Status s = db->Get(rocksdb::ReadOptions(), handle, key, &res);
  if (!s.ok()){
    throw std::runtime_error("Failed to get node with error: "
                             + s.ToString());
  }
  return res;
}

void RDB_Manager::removeNode(const std::string &key) {
  if (_readOnly){
    throw std::runtime_error("Tried to modify read-only db");
    return ;
  }
  rocksdb::Status s = db->Delete(rocksdb::WriteOptions(), key);
  if (!s.ok()){
    throw std::runtime_error("Failed to delete node with error: "
                             + s.ToString());
  }
}

void RDB_Manager::removeNode(const std::string &column, const std::string &key) {
  if (_readOnly){
    throw std::runtime_error("Tried to modify read-only db");
    return ;
  }
  rocksdb::ColumnFamilyHandle* handle = findHandle(column);
  if (handle == nullptr){
    throw std::runtime_error("Column not found");
    return ;
  }
  rocksdb::Status s = db->Delete(rocksdb::WriteOptions(), handle, key);
  if (!s.ok()){
    throw std::runtime_error("Failed to delete node with error: "
                             + s.ToString());
  }
}

rocksdb::Iterator* RDB_Manager::createIterator(const std::string &column) {
  rocksdb::ColumnFamilyHandle* handle = findHandle(column);
  rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions(), handle);
  rocksdb::Status s = it->status();
  if (!s.ok()){
    throw std::runtime_error("Failed to get iterator for database \"" +
                             filename + "\"");
  }
  return it;
}

rocksdb::ColumnFamilyHandle* RDB_Manager::findHandle(const std::string &columnName) {
  for (size_t i = 0; i < columnFamilyDescriptors.size(); ++i){
    if (columnFamilyDescriptors[i].name == columnName){
      return columnFamilyHandles[i];
    }
  }
  return nullptr;
}