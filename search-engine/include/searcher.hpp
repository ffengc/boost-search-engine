
#ifndef __YUFC_SERACH_ENGINE_SEARCHER_HPP__
#define __YUFC_SERACH_ENGINE_SEARCHER_HPP__

#include "index.hpp"
#include "util.hpp"
#include <jsoncpp/json/json.h>

namespace ns_searcher {

struct inverted_elem_print {
    uint64_t __doc_id;
    int __weight;
    std::vector<std::string> __words;
    inverted_elem_print()
        : __doc_id(0)
        , __weight(0) { }
};

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
        LOG(INFO) << "get words index success" << std::endl;
        // 2. 根据index对象建立索引
        if (!__index->build_index(input)) {
            LOG(FATAL) << "build words index failed" << std::endl;
            exit(1);
        }
        LOG(INFO) << "build words index success" << std::endl;
    }
    void search(const std::string& query, std::string* json_string) {
        // query: 搜索关键字
        // json_string: 返回用户浏览器的搜索结果
        // 1. 对query分词
        std::vector<std::string> query_words;
        ns_util::jieba_util::cut_string(query, &query_words);
        // 2. 触发：就是根据分词的各个词，进行index查找
        // 注意大小写，这里需要忽略大小写
        // ns_index::inverted_list_t inverted_list_all; // 内部是 inverted_elem
        std::vector<inverted_elem_print> inverted_list_all;
        std::unordered_map<uint64_t, inverted_elem_print> token_map;
        for (std::string word : query_words) {
            boost::to_lower(word);
            // 必须先查倒排
            ns_index::inverted_list_t* inverted_list = __index->get_inverted_list(word); // 找倒排
            if (nullptr == inverted_list)
                // 没有倒排就一定没有正排
                continue;
            // 去重
            for (const auto& elem : *inverted_list) {
                auto &item = token_map[elem.__doc_id]; // 如果存在，直接获取，如果不在，直接新建, 注意，这里的&不要漏掉，如果拷贝一个临时变量就麻烦了
                // item一定是doc_id相同的print节点
                item.__doc_id = elem.__doc_id;
                item.__weight += elem.__weight;
                item.__words.push_back(elem.__word);
            }
        }
        for (const auto& item : token_map)
            inverted_list_all.push_back(item.second);
        // inverted_list_all.insert(inverted_list_all.end(), inverted_list->begin(), inverted_list->end()); // 批量化插入
        // 这里的 inverted_list_all 可能会有遗留问题，可能文档名是有重复的
        // 3. 合并排序：汇总查找结果，按照相关性进行降序排序
        // std::sort(inverted_list_all.begin(), inverted_list_all.end(), [](const ns_index::inverted_elem& e1, const ns_index::inverted_elem& e2) {
        //     return e1.__weight > e2.__weight;
        // });
        std::sort(inverted_list_all.begin(), inverted_list_all.end(),
            [](const inverted_elem_print& e1, const inverted_elem_print& e2) { return e1.__weight > e2.__weight; });
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
            // elem["desc"] = get_desc(doc->__content, item.__word);
            elem["desc"] = get_desc(doc->__content, item.__words[0]); // 改了之后，有多个词，所以随便选一个构成摘要就行了
            elem["url"] = doc->__url;
            root.append(elem); // 按顺序append到root中了
        }
        Json::FastWriter writer;
        *json_string = writer.write(root); // 序列化！
    }
    std::string get_desc(const std::string& html_content, const std::string& word) {
        // 找到word在html_content中的首次出现，然后往前找n个字节，往后找m个字节，截取出这部分内容
        const int prev_step = 150;
        const int next_step = 180;
        // 1. 找到首次出现
        auto iter = std::search(html_content.begin(), html_content.end(), word.begin(), word.end(), [](int x, int y) { return std::tolower(x) == std::tolower(y); });
        if (iter == html_content.end())
            return "null: iter == html_content.end()"; // 这种情况是不可能存在的, 因为文本里一定有关键字word
        std::size_t pos = std::distance(html_content.begin(), iter);
        // 2. 获取start，end
        int start = 0; // 不能用size_t防止减成负数
        int end = html_content.size() - 1;
        if (pos > start + prev_step)
            start = pos - prev_step;
        if ((int)pos < (int)(end - next_step))
            end = pos + next_step;
        // 3. 截取字串
        if (start >= end)
            return "null: start >= end"; // 不可能情况
        return html_content.substr(start, end - start) + "  ...";
    }
};
} // namespace ns_searcher

#endif
