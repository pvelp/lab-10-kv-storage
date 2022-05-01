//
// Created by pvelp on 02.05.22.
//
#include <RDB_Reader.hpp>

RDB_Reader::RDB_Reader(const std::string &filename) : db(filename, true){}

void RDB_Reader::readColumn(const std::string &column) {
  Node node;
  node.column = column;

  rocksdb::Iterator* it = db.createIterator(column);
  for (it->SeekToFirst(); it->Valid(); it->Next()){
    node.key = it->key().ToString();
    node.value = it->value().ToString();

    //TODO тут закидывать в очередь прочтенные данные
  }
  delete it;
}