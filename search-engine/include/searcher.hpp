
#ifndef __YUFC_SERACH_ENGINE_SEARCHER_HPP__
#define __YUFC_SERACH_ENGINE_SEARCHER_HPP__

#include "index.hpp"
#include "util.hpp"
#include <jsoncpp/json/json.h>

namespace ns_searcher {
class searcher {
private:
    ns_index::index* __index; // 供系统进行查找的索引
public:
    searcher() = default;
    ~searcher() = default;

public:
    void init_searcher(const std::string& input) {
        // 1. 获取或者创建index对象
        __index = ns_index::index::get_instance();
        // 2. 根据index对象建立索引
        __index->build_index(input);
    }
    void search(const std::string& query, std::string* json_string) {
        // query: 搜索关键字
        // json_string: 返回用户浏览器的搜索结果
        // 1. 对query分词
        std::vector<std::string> query_words;
        ns_util::jieba_util::cut_string(query, &query_words);
        // 2. 触发：就是根据分词的各个词，进行index查找
        // 注意大小写，这里需要忽略大小写
        ns_index::inverted_list_t inverted_list_all; // 内部是 inverted_elem
        for (std::string word : query_words) {
            boost::to_lower(word);
            // 必须先查倒排
            ns_index::inverted_list_t* inverted_list = __index->get_inverted_list(word); // 找倒排
            if (nullptr == inverted_list)
                // 没有倒排就一定没有正排
                continue;
            inverted_list_all.insert(inverted_list_all.end(), inverted_list->begin(), inverted_list->end()); // 批量化插入
            // 这里的 inverted_list_all 可能会有遗留问题，可能文档名是有重复的
        }
        // 3. 合并排序：汇总查找结果，按照相关性进行降序排序
        std::sort(inverted_list_all.begin(), inverted_list_all.end(), [](const ns_index::inverted_elem& e1, const ns_index::inverted_elem& e2) {
            return e1.__weight > e2.__weight;
        });
        // 4. 构建：根据查找出来的结果构建json串，jsoncpp
        Json::Value root;
        for (auto& item : inverted_list_all) {
            ns_index::doc_info* doc = __index->get_forward_index(item.__doc_id);
            if (nullptr == doc)
                continue;
            // 序列化
            Json::Value elem;
            elem["title"] = doc->__title;
            // doc->__content 是去标签的全部结果，不是我们想要的，我们想要的只有一部分。TODO
            elem["desc"] = doc->__content;
            elem["url"] = doc->__url;
            root.append(elem); // 按顺序append到root中了
        }
        Json::StyledWriter writer;
        *json_string = writer.write(root); // 序列化！
    }
};
} // namespace ns_searcher

#endif