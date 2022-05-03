#include <boost/program_options.hpp>
#include <iostream>
#include <thread>
#include <string>
#include <random>
#include "rocksdb.hpp"
#include "RDB_Reader.hpp"
#include "ThreadPool.hpp"
#include "../third-party/PicoSHA2/picosha2.h"


std::string hash(const std::string& value) {
  return picosha2::hash256_hex_string(value);
}



namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  po::options_description desc("Options");
  desc.add_options()("help", "Usage: \n./demo [options]"
                     " <path/to/input/storage.db>\n")
                    ("log-level,l", po::value<std::string>(),
                        "Log level (default: error)")
                    ("thread-count,t", po::value<size_t>(),
                        "Count of threads (default: count of logical core)")
                    ("output,o", po::value<std::string>(),
                        "Path to output db file (default: <path/to/input/dbcs-storage.db>");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
//  po::notify(vm);

  if (vm.count("help")){
    std::cout << desc << std::endl;
    return 0;
  }

  std::string log_level;
  if (vm.count("log-level")){
    if (!vm.at("log-level").as<std::string>().empty()){
      log_level = vm.at("log-level").as<std::string>();
    }
  } else {
    log_level = "error";
  }

  size_t thread_count = std::thread::hardware_concurrency();
  if (vm.count("thread-count")) {
    if (vm.at("thread-count").as<size_t>() >
        std::thread::hardware_concurrency()) {
      std::cerr << "Maximum allowed number of concurrently is " +
                       std::to_string(thread_count)
                << std::endl;
    } else {
      thread_count = vm.at("thread-count").as<size_t>();
    }
  }

  std::string output;
  if (vm.count("output")){
    if (!vm.at("output").as<std::string>().empty()){
      output = "../"+vm.at("output").as<std::string>();
    }
  } else {
    output = "../outdb";
  }

  std::queue<std::pair<std::string, std::map<std::string, std::string>>>q;
  ThreadPool pool(thread_count);
//  std::mutex read_mutex;

  RDB_Manager rdbReader("../basedb", false, false);
  rdbReader.print_db();
  std::cout << "--------------------../basedb--------------------" << std::endl;
  std::vector<std::string> res = rdbReader.getColumnFamilyNames();
  for (auto& r : res) {
//    pool.enqueue([&rdbReader, &r, &q] {rdbReader.readColumn(r, q);});
//    rdbReader.readColumn(r, q);
  }

  RDB_Manager rdbManager(output, false, true);
  while (!q.empty()){
    std::pair<std::string, std::map<std::string, std::string>> val = q.front();
    q.pop();
    for(auto& item : val.second){
     rdbManager.addNode(val.first, item.first, hash(item.second));
    }
  }
  rdbManager.print_db();
  std::cout << "--------------------../outdb--------------------" << std::endl;

//  rdbReader.~RDB_Manager();
//  rdbManager.~RDB_Manager();

//  generate_db();
  return 0;
}