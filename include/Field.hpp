// Copyright 2022 pvelp

#ifndef INCLUDE_FIELD_HPP_
#define INCLUDE_FIELD_HPP_
#include <string>
#include <utility>
using string = std::string;
struct Field{
  Field(string  value_, string  key_) : value(std::move(value_)),
                                      key(std::move(key_)){}
  string value;
  string key;
};
#endif  // INCLUDE_FIELD_HPP_
