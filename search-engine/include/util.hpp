

#ifndef __YUFC_SERACH_ENGINE_UTIL_HPP__
#define __YUFC_SERACH_ENGINE_UTIL_HPP__

#include "cppjieba/Jieba.hpp"
#include "log.hpp"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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
        std::string ssep(1, sep);
        boost::split(*out, target, boost::is_any_of(ssep), boost::token_compress_on);
    }
};

// 这里要写相对于可执行文件的相对路径
const char* const DICT_PATH = "./include/cppjieba/dict/jieba.dict.utf8";
const char* const HMM_PATH = "./include/cppjieba/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "./include/cppjieba/dict/user.dict.utf8";
const char* const IDF_PATH = "./include/cppjieba/dict/idf.utf8";
const char* const STOP_WORD_PATH = "./include/cppjieba/dict/stop_words.utf8";
class jieba_util {
private:
    static cppjieba::Jieba jieba;
public:
    static void cut_string(const std::string& src, std::vector<std::string>* out) {
        jieba.CutForSearch(src, *out);
    }
};
cppjieba::Jieba jieba_util::jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH); // static 要在类外来定义
} // namespace ns_util

#endif