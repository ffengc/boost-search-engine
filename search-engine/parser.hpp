

#ifndef __YUFC_SERACH_ENGINE_PARSER_HPP__
#define __YUFC_SERACH_ENGINE_PARSER_HPP__

#include <iostream>
#include <string>
#include <vector>
#include "log.hpp"

class parser {
private:
    const std::string __src_path = "data/input/";
    const std::string __raw_file = "data/raw/raw.bin";
private:
    typedef struct __doc_info {
        std::string __title;    // 文档标题
        std::string __content;  // 文档内容
        std::string __url;      // 文档url
    }doc_info_t;
public:
    void start() {
        // 1. 递归式的把每一个html文件名带路径，保存到 files_list 中，方便后期进行一个一个的文件读取
        std::vector<std::string> file_list;
        if(!enum_file(__src_path, &file_list)) {
            LOG(FATAL) << "enum_file error" << std::endl;
            exit(1);
        }
        //2. 按照file_list读取每个文件内容，并进行解析
        std::vector<doc_info_t> results;
        if(!parse_html(file_list, &results)) {
            LOG(FATAL) << "parse_html error" << std::endl;
            exit(2);
        }
        // 3. 把解析完毕的各个内容，写入到__raw_file对应的文件当中
        if(!save_html(results, __raw_file)) {
            LOG(FATAL) << "save_html error" << std::endl;
            exit(3);
        }
    };
public:
    static bool enum_file(const std::string& src_path, std::vector<std::string>* file_list) {}
    static bool parse_html(const std::vector<std::string>& file_list,  std::vector<doc_info_t>* results) {}
    static bool save_html(const std::vector<doc_info_t>& result, const std::string& raw_file) {}
};

#endif