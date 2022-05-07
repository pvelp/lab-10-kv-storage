// Copyright 2022 pvelp

#ifndef STORAGE_WRITER_HPP
#define STORAGE_WRITER_HPP
#include "ThreadPool.hpp"
#include "../third-party/PicoSHA2/picosha2.h"

class Writer{
 public:
  explicit Writer(const int& num_workers);

  std::string hash256(std::string& key, std::string& value);

 private:
  ThreadPool writers;
};
#endif  // STORAGE_WRITER_HPP
