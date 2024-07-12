

#ifndef __YUFC_SERACH_ENGINE_UTIL_HPP__
#define __YUFC_SERACH_ENGINE_UTIL_HPP__

#include "log.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

namespace ns_util {
class file_util {
public:
    static bool read_file(const std::string file_path, std::string* out) {
        std::ifstream in(file_path, std::ios::in);
        if (!in.is_open()) {
            LOG(WARNING) << "ns_util::file_util::read_file read file error" << std::endl;
            return false;
        }
        std::string line;
        while (std::getline(in, line))
            *out += line;
        in.close();
        return true;
    }
};
class string_util {
public:
    static void cut_string(const std::string& target, std::vector<std::string>* out, char sep) {
        // boost split
        boost::split(*out, target, boost::is_any_of(sep), boost::token_compress_on);
    }
};
} // namespace ns_util

#endif