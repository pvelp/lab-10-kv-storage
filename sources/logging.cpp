// Created by pvelp on 01.05.22.
#include "logging.hpp"
#include <iostream>

logs::trivial::severity_level level_form_str(const std::string& logLevel){
  if (logLevel.empty()){
    return logs::trivial::severity_level::error;
  } else if (logLevel == "warning"){
    return logs::trivial::severity_level::warning;
  } else if (logLevel == "info"){
    return logs::trivial::severity_level::info;
  } else if (logLevel == "error"){
    return logs::trivial::severity_level::error;
  } else {
    throw std::runtime_error{"invalid log level"};
  }
}

void set_logs(){
  auto trace_log = logs::add_file_log(
      keywords::file_name = "../logs_files/log_file_%N",
      keywords::rotation_size = 2*1024*1024,
      keywords::format = "[%TimeStamp%][%Severity%][%ThreadID%]: %Message%");
  trace_log->set_filter(logs::trivial::severity >= logs::trivial::error);
  auto info_log = boost::log::add_file_log(
      keywords::file_name = "../logs_files/info_file_%N",
      keywords::rotation_size = 2*1024*1024,
      keywords::format = "[%TimeStamp%][%Severity%][%ThreadID%]: %Message%");
  info_log->set_filter(logs::trivial::severity >= logs::trivial::info);
  auto console_log = logs::add_console_log(
      std::clog,
      keywords::format = "[%TimeStamp%][%Severity%][%ThreadID%]: %Message%");
  console_log->set_filter(logs::trivial::severity >= logs::trivial::warning);
  }