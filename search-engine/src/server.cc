/*
 * Write by Yufc
 * See https://github.com/ffengc/boost-search-engine
 */

#include "../include/httplib.h"
#include "../include/searcher.hpp"

const std::string root_path = "./wwwroot"; // 资源根目录
const std::string resource_path = "./data/raw/raw.bin";
const std::string listen_ip = "0.0.0.0";
const int listen_port = 8081;

void build_server() {
    // 定义searcher
    ns_searcher::searcher ser;
    ser.init_searcher(resource_path);

    httplib::Server svr;
    svr.set_base_dir(root_path.c_str());
    svr.Get("/s", [&ser](const httplib::Request& req, httplib::Response& rsp) {
        // 设置用户输入的关键字的参数名为 word
        if (!req.has_param("word")) {
            rsp.set_content("none key word, please enter your param", "text/plain; charset=utf-8");
            return;
        }
        // 有关键字
        std::string word = req.get_param_value("word");
        std::string json_string; // 这个是搜索结果
        LOG(INFO) << "user search: " << word << std::endl;
        ser.search(word, &json_string);
        rsp.set_content(json_string, "application/json; charset=utf-8");
    });
    LOG(INFO) << "server listening, ip: " << listen_ip << " port: " << listen_port << std::endl;
    svr.listen(listen_ip, listen_port);
}

int main() {
    build_server();
    return 0;
}