

#ifndef __YUFC_SERACH_ENGINE_UTIL_HPP__
#define __YUFC_SERACH_ENGINE_UTIL_HPP__

#include "log.hpp"
#include <fstream>
#include <iostream>
#include <string>

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
} // namespace ns_util

#endif