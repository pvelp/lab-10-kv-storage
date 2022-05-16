// Copyright 2022 pvelp

#include <kv_storage.hpp>

Storage::Storage(int& num_workers)
    : writer(num_workers/2),
      reader(num_workers - num_workers/2){}

void Storage::check_status(const rocksdb::Status& status,
                           const string& message) {
  if (!status.ok())
    throw std::runtime_error(message + status.ToString());
}
void Storage::delete_db(rocksdb::DB* db,
    const std::vector<rocksdb::ColumnFamilyHandle*>& handles_cf,
    const std::vector<rocksdb::Iterator*>& iterators_cf) {
  rocksdb::Status status;
  for (auto& iterator : iterators_cf) {
    delete iterator;
  }
  for (auto& handle : handles_cf) {
    status = db->DestroyColumnFamilyHandle(handle);
    Storage::check_status(status, "BD::Destroy_cf fail fail ");
  }
  status = db->Close();
  Storage::check_status(status, "BD::Close fail fail ");
  delete db;
}
void Storage::delete_db(
    rocksdb::DB* db,
    const std::vector<rocksdb::ColumnFamilyHandle*>& handles_cf) {
  rocksdb::Status status;
  for (auto& handle : handles_cf) {
    status = db->DestroyColumnFamilyHandle(handle);
    Storage::check_status(status, "BD::Destroy_cf fail fail ");
  }
  status = db->Close();
  Storage::check_status(status, "BD::Close fail fail ");
  delete db;
}
void Storage::create_iterators(
    rocksdb::DB* db,
    const std::vector<rocksdb::ColumnFamilyHandle*>& handles_cf,
    std::vector<rocksdb::Iterator*>& iterators_cf) {
  iterators_cf.reserve(handles_cf.size());
  for (auto & handle : handles_cf){
    iterators_cf.emplace_back(db->NewIterator(rocksdb::ReadOptions(),
                                              handle));
  }
}
void Storage::create_descriptors(
    const std::vector<string>& names_cf,
    std::vector<rocksdb::ColumnFamilyDescriptor>& descriptors_cf) {
  descriptors_cf.reserve(names_cf.size());
  for (auto& name : names_cf) {
    descriptors_cf.emplace_back(name, rocksdb::ColumnFamilyOptions());
  }
}
void Storage::check_iterators(
    const std::vector<rocksdb::Iterator*>& iterators_cf,
    const std::vector<rocksdb::ColumnFamilyHandle*>& handles_cf,
    std::queue<Field>& fields, std::vector<int>& nums_in_columns) {
  int counter = 0;
  for (size_t i = 1; i < iterators_cf.size(); ++i){
    for (iterators_cf[i]->SeekToFirst();
        iterators_cf[i]->Valid();
        iterators_cf[i]->Next(), ++counter){
      BOOST_LOG_TRIVIAL(trace) << "Read:" << iterators_cf[i]->key().ToString()
                               << " : " << iterators_cf[i]->value().ToString()
                               << " in " << handles_cf[i]->GetName() <<endl;
      fields.emplace(iterators_cf[i]->value().ToString(),
                     iterators_cf[i]->key().ToString());
    }
    nums_in_columns.push_back(counter);
    counter = 0;
  }
}

void Storage::hashing(Storage* storage, std::queue<Field>& fields,
                      std::queue<Field>& hashs, std::vector<Field>& out_fields){
  while (!fields.empty() || !hashs.empty()){
    if (!fields.empty()){
      hashs.emplace(storage->use_writer().
                        hash256(fields.front().key, fields.front().value),
                    fields.front().key);
      fields.pop();
    }
    if (!hashs.empty()){
      storage->use_reader().read(hashs.front());
      out_fields.push_back(hashs.front());
      hashs.pop();
    }
  }
}
void Storage::str_of_separator(std::stringstream& ss) {
      ss << std :: right << std::setfill('-')  << "|"
     << std::setw(spases_for_table[0]);
      ss << "|\n" << std::setfill(' ') << std::left;
}
std::stringstream Storage::print_table(
    const std::vector<Field>& out_fields,
    const std::vector<rocksdb::ColumnFamilyHandle*>& handles_cf_out,
    const std::vector<int>& nums_in_columns) {
  std::stringstream ss;
  int num = 0;
  for (size_t i = 0; i < handles_cf_out.size(); ++i){
    str_of_separator(ss);
    ss << "|" << std::right << std::setw(spases_for_table[2]) <<
        handles_cf_out[i]->GetName() << std::setw(spases_for_table[3]) <<
        " |" << endl << std::left;
    for (int j = 0; j < nums_in_columns[i]; ++j, ++num){
      str_of_separator(ss);
      ss << std::setw(spases_for_table[1]) << "| " + out_fields[num].key +
                                                  " : " + out_fields[num].value
         << " |" << endl;
    }
    str_of_separator(ss);
    ss << endl << endl;
  }
  return ss;
}
void Storage::start(const string& path_to_db,
                    int num_workers, const string& path_to_out_db) {
  try{
    Storage storage(num_workers);
    std::vector<string> names_cf;
    rocksdb::Status status = rocksdb::DB::ListColumnFamilies(
        rocksdb::DBOptions(),
        path_to_db,
        &names_cf);
    Storage::check_status(status, "BD::List_cf fail ");
    BOOST_LOG_TRIVIAL(debug) << "Read ListColumnFamilies\n";

    std::vector<rocksdb::ColumnFamilyDescriptor> descriptors_cf;
    Storage::create_descriptors(names_cf, descriptors_cf);
    BOOST_LOG_TRIVIAL(debug) << "Create descriptors\n";

    rocksdb::DB* db;
    std::vector<rocksdb::ColumnFamilyHandle*> handles_cf;
    status = rocksdb::DB::Open(rocksdb::DBOptions(), path_to_db,
                               descriptors_cf, &handles_cf, &db);
    Storage::check_status(status, "BD::Open fail ");
    BOOST_LOG_TRIVIAL(info) << "DB opened\n";

    std::vector<rocksdb::Iterator*> iterators_cf;
    Storage::create_iterators(db, handles_cf, iterators_cf);
    BOOST_LOG_TRIVIAL(debug) << "DB create iterators\n";

    std::vector<int> nums_in_columns;
    std::queue<Field> fields;
    Storage::check_iterators(iterators_cf, handles_cf, fields, nums_in_columns);
    BOOST_LOG_TRIVIAL(debug) << "DB checking data in iterators\n";

    Storage::delete_db(db, handles_cf, iterators_cf);
    BOOST_LOG_TRIVIAL(info) << "DB closed\n";


    BOOST_LOG_TRIVIAL(debug) << "Hashing begin...\n";
    rocksdb::DB* db_out;
    rocksdb::Options options;
    options.create_if_missing = true;
    options.error_if_exists = true;
    status = rocksdb::DB::Open(options, path_to_out_db, &db_out);
    Storage::check_status(status, "BD::Open fail ");
    BOOST_LOG_TRIVIAL(info) << "DB_out opened\n";

    names_cf.erase(names_cf.begin());
    std::vector<rocksdb::ColumnFamilyHandle*> handles_cf_out;
    status = db_out->CreateColumnFamilies(rocksdb::ColumnFamilyOptions(),
                                          names_cf, &handles_cf_out);
    Storage::check_status(status, "BD::Create_cf fail ");
    BOOST_LOG_TRIVIAL(debug) << "DB_out Create_cf\n";

    storage.use_reader().setting(db_out, handles_cf_out, nums_in_columns);
    std::queue<Field> hashs;
    std::vector<Field> out_fields;
    Storage::hashing(&storage, fields, hashs, out_fields);
    BOOST_LOG_TRIVIAL(info) << "End hashing and writing to db_out" << endl;

    BOOST_LOG_TRIVIAL(info)<< "Result table:\n\n" <<
        Storage::print_table(out_fields, handles_cf_out, nums_in_columns).str();

    Storage::delete_db(db_out, handles_cf_out);
    BOOST_LOG_TRIVIAL(info) << "DB_out closed\n";
  } catch (std::exception& e) {
    BOOST_LOG_TRIVIAL(error) << e.what();
  }
}

void Storage::init(const boost::log::trivial::severity_level& sev_lvl) {
  boost::log::add_common_attributes();

  boost::log::core::get()->set_filter(boost::log::trivial::severity >= sev_lvl);

  boost::log::add_console_log(std::clog,
                              boost::log::keywords::format =
                                  "[%Severity%] %TimeStamp%: %Message%");

  boost::log::add_file_log
      (
          boost::log::keywords::file_name = "../sample_%N.log",
          boost::log::keywords::rotation_size = 30 * 1024 * 1024,
          boost::log::keywords::time_based_rotation =
              boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
          boost::log::keywords::format = "[%Severity%][%TimeStamp%]: "
              "%Message%");
}

boost::log::trivial::severity_level Storage::choose_sev_lvl(
    const string& sev_lvl_str) {
  if (sev_lvl_str == "trace")
    return boost::log::trivial::severity_level::trace;
  else if (sev_lvl_str == "debug")
    return boost::log::trivial::severity_level::debug;
  else if (sev_lvl_str == "info")
    return boost::log::trivial::severity_level::info;
  else if (sev_lvl_str == "warning")
    return boost::log::trivial::severity_level::warning;
  else
    return boost::log::trivial::severity_level::error;
}
Reader& Storage::use_reader() { return reader; }

Writer& Storage::use_writer() { return writer; }
