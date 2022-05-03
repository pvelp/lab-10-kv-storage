#include <boost/program_options.hpp>
#include <iostream>
#include <thread>
#include <string>
#include <random>
#include "rocksdb.hpp"
#include "RDB_Reader.hpp"


void print_db()


std::string create_random_str() {
  static const auto magic = static_cast<size_t>(time(nullptr));
  static std::mt19937 randomGenerator(magic);

  std::string res;
  for (size_t i = 0; i < 10; ++i)
    res += static_cast<char>(randomGenerator() % 26 + 97);

  return res;
}


void generate_db(){
  std::string column_names[] = { "ex1", "some2", "col3" };
  std::string dbname = "../basedb";

  RDB_Manager rdbManager(dbname, false, true);

  for (size_t j = 0; j < 10; ++j) {
    rdbManager.addNode(create_random_str(), create_random_str());
  }

  for (const auto& column_name : column_names) {
    for (size_t j = 0; j < 10; ++j) {
      rdbManager.addNode(column_name, create_random_str(), create_random_str());
    }
  }
  rdbManager.~RDB_Manager();
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

  RDB_Reader rdbReader("../basedb");
  std::vector<std::string> res = rdbReader.columnNames();
  for (auto& r : res) {
    rdbReader.readColumn(r, q);
  }
//  }
//  while (!q.empty()){
//    std::pair<std::string, std::map<std::string, std::string>> val = q.front();
//    q.pop();
//    for(auto& item : val.second){
//      std::cout << val.first + " : [" << item.first + " : " << item.second + "]" << std::endl;
//    }
//  }

//  RDB_Manager rdbManager(output, false, true);
//  while (!q.empty()){
//    std::pair<std::string, std::map<std::string, std::string>> val = q.front();
//    q.pop();
//    for(auto& item : val.second){
////      std::cout << val.first + " : [" << item.first + " : " << item.second + "]" << std::endl;
//     rdbManager.addNode(val.first, item.first, item.second);
//    }
//  }


//  generate_db();
//  std::cout << log_level << " " << thread_count << " " << output << std::endl;
  return 0;
}