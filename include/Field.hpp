// Copyright 2022 pvelp

#ifndef STORAGE_FIELD_HPP
#define STORAGE_FIELD_HPP
#include <string>
#include <utility>
using string = std::string;
struct Field{
  Field(string  value_, string  key_) : value(std::move(value_)),
                                      key(std::move(key_)){}
  string value;
  string key;
};
#endif  // STORAGE_FIELD_HPP
