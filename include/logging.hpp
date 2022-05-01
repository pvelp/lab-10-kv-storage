// Created by pvelp on 01.05.22.
#ifndef LAB_10_LOGGING_HPP
#define LAB_10_LOGGING_HPP
#include <iostream>
#include <boost/log/exceptions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>

namespace logs = boost::log;
namespace keywords = boost::log::keywords;

void set_logs(size_t val);

#endif  // LAB_10_LOGGING_HPP
