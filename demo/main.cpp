#include <kv_storage.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  po::options_description desc("Options");
  desc.add_options()("help", "Usage: \n./demo [options]"
                     " <path/to/input/storage.db>\n")
      ("log-level,l", po::value<std::string>(),

       "Log level (default: error)")
          ("thread-count,t", po::value<int>(),
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
    log_level = "info";
  }

  int thread_count = std::thread::hardware_concurrency();
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
  try
  {
    Storage::init(Storage::choose_sev_lvl(log_level));
    BOOST_LOG_TRIVIAL(info) << "Start program\n";
    Storage::start("../testdb", thread_count, output);
    } catch (const po::error &ex)
    {
      BOOST_LOG_TRIVIAL(error) << ex.what() << '\n';
    }
}