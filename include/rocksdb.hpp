// Created by pvelp on 01.05.22.

#ifndef LAB_10_ROCKSDB_HPP
#define LAB_10_ROCKSDB_HPP
#include <string>
#include <iostream>
#include <rocksdb/db.h>
#include "Node.hpp"
#include <queue>
#include <mutex>

class RDB_Manager{
 public:
  RDB_Manager(std::string fname, bool readOnly, bool create = false);
  RDB_Manager(const RDB_Manager& value) = delete;
  RDB_Manager(RDB_Manager &&db) noexcept;
  RDB_Manager& operator=(const RDB_Manager& val) = delete;
  RDB_Manager& operator=(RDB_Manager&& val) = delete;
  ~RDB_Manager();

  static std::vector<std::string> getColumnFamilyNames(
      const std::string& filename, bool create);
  [[nodiscard]] const std::vector<std::string>& getColumnFamilyNames()const;
  void addNode(const std::string& key, const std::string& value);
  void addNode(const std::string& column,
               const std::string& key,
               const std::string& value);
  void addNode(const Node& node);
  std::string getNode(const std::string& key);
  std::string getNode(const std::string& column,
                      const std::string& key);

  void removeNode(const std::string& key);
  void removeNode(const std::string& column,
                  const std::string& key);

  rocksdb::Iterator* createIterator(const std::string& column);

  void readColumn(const std::string &column,
                  std::queue<std::pair<std::string,
                                       std::map<std::string,
                                                std::string>>> &q);

  void print_db();

 private:
  rocksdb::ColumnFamilyHandle* findHandle(const std::string& columnName);

  rocksdb::DB* db{nullptr};
  std::string filename;

  std::vector<std::string> columnFamilyNames;
  std::vector<rocksdb::ColumnFamilyDescriptor> columnFamilyDescriptors;
  std::vector<rocksdb::ColumnFamilyHandle*> columnFamilyHandles;

  bool _readOnly;

};

void generate_db();


#endif  // LAB_10_ROCKSDB_HPP
