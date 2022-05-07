// Copyright 2022 pvelp

#include "Writer.hpp"

#include <iostream>

#include "kv_storage.hpp"
Writer::Writer(const int& num_workers) : writers(num_workers) {}

std::string Writer::hash256(std::string& key, std::string& value) {
  return writers.enqueue([&]
                         (std::string& key_, std::string& value_){
                           BOOST_LOG_TRIVIAL(info) << "hashing thread"
                                                   << std::this_thread::get_id();
//                           std::cout << std::this_thread::get_id() << std::endl;
                           return picosha2::hash256_hex_string(key_ + value_);
                               }, key, value).get();
}
