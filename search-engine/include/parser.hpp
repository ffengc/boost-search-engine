/*
 * Write by Yufc
 * See https://github.com/ffengc/boost-search-engine
 * please cite my project link: https://github.com/ffengc/boost-search-engine when you use this code
 */

#ifndef __YUFC_SERACH_ENGINE_PARSER_HPP__
#define __YUFC_SERACH_ENGINE_PARSER_HPP__

#include "log.hpp"
#include "util.hpp"
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <vector>

static const std::string src_path = "data/input";
static const std::string raw_file = "data/raw/raw.bin";
static const std::string url_head = "https://www.boost.org/doc/libs/1_85_0/doc/html";
static const char sep = '\3';
static const char file_sep = '\n';

class parser {
private:
    typedef struct __doc_info {
        std::string __title; // 文档标题
        std::string __content; // 文档内容
        std::string __url; // 文档url
    } doc_info_t;

public:
    void start() {
        // 1. 递归式的把每一个html文件名带路径，保存到 files_list 中，方便后期进行一个一个的文件读取
        std::vector<std::string> file_list;
        if (!enum_file(src_path, &file_list)) {
            LOG(FATAL) << "enum_file error" << std::endl;
            exit(1);
        }
        // 2. 按照file_list读取每个文件内容，并进行解析
        std::vector<doc_info_t> results;
        if (!parse_html(file_list, &results)) {
            LOG(FATAL) << "parse_html error" << std::endl;
            exit(2);
        }
        // 3. 把解析完毕的各个内容，写入到__raw_file对应的文件当中
        if (!save_html(results, raw_file)) {
            LOG(FATAL) << "save_html error" << std::endl;
            exit(3);
        }
        LOG(INFO) << "parse done" << std::endl;
    };

public:
    static bool enum_file(const std::string& src_path, std::vector<std::string>* file_list) {
        namespace fs = boost::filesystem;
        fs::path root_path(src_path);
        if (!fs::exists(root_path)) {
            LOG(ERROR) << "enum_file file not exists" << std::endl;
            return false;
        }
        fs::recursive_directory_iterator end; // 定义一个空的迭代器，用来判断递归结束
        for (fs::recursive_directory_iterator iter(root_path); iter != end; iter++) {
            if (!fs::is_regular_file(*iter)) // 如果不是普通文件
                continue;
            if (iter->path().extension() != ".html") // 如果后缀不是 html
                continue;
            //  当前的路径一定是一个html结束的普通网页文件
            // LOG(DEBUG) << iter->path().string() << std::endl;
            file_list->push_back(iter->path().string());
        }
        return true;
    }
    // for debug
    static void show(const doc_info_t& doc) {
        std::cout << "--------------" << std::endl;
        std::cout << doc.__title << std::endl;
        std::cout << doc.__content << std::endl;
        std::cout << doc.__url << std::endl;
        exit(1);
    }
    static bool parse_html(const std::vector<std::string>& file_list, std::vector<doc_info_t>* results) {
        for (const std::string& file : file_list) {
            // 1. 读取文件 read()
            std::string result;
            if (!ns_util::file_util::read_file(file, &result))
                continue;
            // 2. 解析指定的文件，提取title
            doc_info_t doc;
            if (!parse_title(result, &doc.__title))
                continue;
            // 3. 解析指定的文件，提取content
            if (!parse_content(result, &doc.__content))
                continue;
            // 4. 解析指定的文件路径，构建url
            if (!parse_url(file, &doc.__url))
                continue;
            // 走到这里一定是完成了解析任务，当前文档的相关结果都保存在了doc里面
            results->push_back(std::move(doc)); // 用move减少拷贝
            // show(doc);
        }
        return true;
    }
    static bool save_html(const std::vector<doc_info_t>& results, const std::string& raw_file) {
        std::ofstream out(raw_file, std::ios::out | std::ios::binary);
        if (!out.is_open()) {
            LOG(FATAL) << "open " << raw_file << "failed!" << std::endl;
            return false;
        }
        // 进行文件内容的写入
        std::size_t cnt = 0;
        for (auto& iter : results) {
            cnt++;
            std::string out_string;
            out_string += iter.__title;
            out_string += sep; // + '\3'
            out_string += iter.__content;
            out_string += sep;
            out_string += iter.__url;
            out_string += file_sep; // + '\n'
            out.write(out_string.c_str(), out_string.size());
            LOG(INFO) << "saving parse result: " << cnt << "\r";
            fflush(stdout);
        }
        std::cout << std::endl;
        out.close();
        return true;
    }

public:
    static bool parse_title(const std::string& file, std::string* title) {
        // 提取title
        std::size_t begin = file.find("<title>");
        if (begin == std::string::npos)
            return false;
        std::size_t end = file.find("</title>");
        if (begin == std::string::npos)
            return false;
        begin += std::string("<title>").size();
        if (begin > end) {
            return false;
        }
        *title = file.substr(begin, end - begin); // 提取title
        return true;
    }
    static bool parse_content(const std::string& file, std::string* content) {
        // 去标签, 基于一个简易的状态机去写
        enum status {
            LABLE,
            CONTENT
        };
        enum status s = LABLE;
        for (char c : file) {
            switch (s) {
            case LABLE:
                if (c == '>') // 此时标签已经被处理完毕了
                    s = CONTENT;
                break;
            case CONTENT:
                if (c == '<')
                    s = LABLE;
                else {
                    // 我们不想保留 \n
                    if (c == '\n')
                        c = ' ';
                    content->push_back(c);
                }
                break;
            default:
                break;
            }
        }
        return true;
    }
    static bool parse_url(const std::string& file_path, std::string* url) {
        std::string url_tail = file_path.substr(src_path.size());
        *url = url_head + url_tail;
        return true;
    }
};

#endif