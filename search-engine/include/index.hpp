

#ifndef __YUFC_SERACH_ENGINE_INDEX_HPP__
#define __YUFC_SERACH_ENGINE_INDEX_HPP__

#include "log.hpp"
#include "util.hpp"
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace ns_index {
typedef struct doc_info {
    std::string __title; // 文档标题
    std::string __content; // 文档内容
    std::string __url; // 文档url
    uint64_t __doc_id; // 文档的id
} doc_info_t;
struct inverted_elem {
    uint64_t __doc_id;
    std::string __word;
    int __weight;
};
typedef std::vector<inverted_elem> inverted_list_t; // 倒排拉链
static const char sep = '\3';
static const int title_co_rate = 10; // 计算title和内容出现词的权重
static const int content_co_rate = 1;
class index {
private:
    // 正排索引的数据结构使用数组，数组的下标天然就是文档的id
    std::vector<doc_info_t> __forward_index; // 正排索引
    // 倒排索引一定是一个关键字和一组（个）inverted_elem对应
    std::unordered_map<std::string, inverted_list_t> __inverted_index; // 倒排索引
public:
    index() { }
    ~index() { }

public:
    // 根据doc_id找到文档内容
    doc_info_t* get_forward_index(const uint64_t& doc_id) {
        if (doc_id >= __forward_index.size()) {
            LOG(ERROR) << "get_forward_index: doc_id out of range" << std::endl;
            return nullptr;
        }
        return &__forward_index[doc_id];
    }
    // 根据关键字，获得倒排拉链
    inverted_list_t* get_inverted_list(const std::string& word) {
        auto iter = __inverted_index.find(word);
        if (iter == __inverted_index.end()) {
            // 找不到
            LOG(ERROR) << "get_inverted_list: cannot find word in hashmap: " << word << std::endl;
            return nullptr;
        }
        return &(iter->second);
    }

public:
    // 根据去标签格式化之后的文档，构建正排和倒排索引
    bool build_index(const std::string& input) {
        // input: raw.bin
        // 1. 读取bin文件，按行读取
        std::ifstream in(input, std::ios::in | std::ios::binary);
        if (!in.is_open()) {
            LOG(ERROR) << "path file: " << input << " open error" << std::endl;
            return false;
        }
        std::string line;
        std::size_t cnt = 0; // 表示第几行
        while (std::getline(in, line)) {
            // 建立正排索引
            doc_info* doc = __build_forward_index(line);
            if (doc == nullptr) {
                LOG(WARNING) << "build_forward_index error in line:" << cnt << std::endl;
                continue;
            }
            // 建立倒排索引
            __build_inverted_index(*doc);
            cnt++;
        }

        return true;
    }

private:
    doc_info* __build_forward_index(const std::string& line) {
        // 1. 解析line, 字符串切分
        std::vector<std::string> results; // 最终
        ns_util::string_util::cut_string(line, &results, sep);
        if (results.size() != 3)
            // 切分出错了
            return nullptr;
        // 2. 字符串进行填充到docinfo
        doc_info doc;
        doc.__title = results[0];
        doc.__content = results[1];
        doc.__title = results[2];
        doc.__doc_id = __forward_index.size();
        // 3. 插入到正排索引的vector中
        __forward_index.push_back(std::move(doc));
        return &__forward_index.back();
    }
    bool __build_inverted_index(const doc_info& doc) {
        struct word_count {
            int title_count;
            int content_count;
            word_count()
                : title_count(0)
                , content_count(0) { }
        };
        std::unordered_map<std::string, word_count> word_map; // 用来暂存词频的映射表
        // 标题分词
        std::vector<std::string> title_words;
        ns_util::jieba_util::cut_string(doc.__title, &title_words);
        for (auto& s : title_words) {
            boost::to_lower(s);
            word_map[s].title_count++;
        }
        // 内容分词
        std::vector<std::string> content_words;
        ns_util::jieba_util::cut_string(doc.__content, &content_words);
        for (auto& s : content_words) {
            boost::to_lower(s);
            word_map[s].content_count++;
        }
        // 构建倒排拉链
        for (auto& word_pair : word_map) {
            inverted_elem item;
            item.__doc_id = doc.__doc_id;
            item.__word = word_pair.first;
            item.__weight = title_co_rate * (word_pair.second.title_count) + content_co_rate * (word_pair.second.content_count); // 相关性
            // 把东西放进去
            inverted_list_t& inverted_list = __inverted_index[word_pair.first];
            inverted_list.push_back(std::move(item));
        }
    }
};
} // namespace ns_index

#endif