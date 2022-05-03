//
// Created by pvelp on 02.05.22.
//
#include <RDB_Reader.hpp>

RDB_Reader::RDB_Reader(const std::string &filename) : db(filename, false){}

std::vector<std::string>RDB_Reader::columnNames(){
  return db.getColumnFamilyNames();
}

void RDB_Reader::readColumn(const std::string &column,std::queue<std::pair<std::string,
                                                                            std::map<std::string,
                                                                                     std::string>>> &q){
//                            std::map<std::string,
//                                     std::map<std::string,
//                                              std::string>> &storage) {
  Node node;
  node.column = column;

  rocksdb::Iterator* it = db.createIterator(column);
  for (it->SeekToFirst(); it->Valid(); it->Next()){
    node.key = it->key().ToString();
    node.value = it->value().ToString();
    std::map<std::string, std::string> kv {{node.key, node.value}};
    std::pair<std::string, std::map<std::string, std::string>> p{column, kv};
    q.push(p);
//    storage.insert({column, kv});
//    db.removeNode(node.column, node.key);

    //TODO тут закидывать в очередь прочтенные данные
  }
  delete it;
}

RDB_Reader::~RDB_Reader() {
  db.~RDB_Manager();
}