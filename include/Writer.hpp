// Copyright 2022 pvelp

#ifndef INCLUDE_WRITER_HPP_
#define INCLUDE_WRITER_HPP_
#include "ThreadPool.hpp"
#include "../third-party/PicoSHA2/picosha2.h"
#include <string>
class Writer{
 public:
  explicit Writer(const int& num_workers);

  std::string hash256(std::string& key, std::string& value);

 private:
  ThreadPool writers;
};
#endif  // INCLUDE_WRITER_HPP_
