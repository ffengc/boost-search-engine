/*
 * Write by Yufc
 * See https://github.com/ffengc/boost-search-engine
 * please cite my project link: https://github.com/ffengc/boost-search-engine when you use this code
 */

#include "../include/searcher.hpp"

const std::string input = "./data/raw/raw.bin";

int main() {
    // for test
    ns_searcher::searcher * ser = new ns_searcher::searcher();
    ser->init_searcher(input);
    std::string query;
    std::string json_string;
    while(true) {
        std::cout << "input Query# ";
        std::getline(std::cin, query);
        ser->search(query, &json_string);
        std::cout << json_string << std::endl;
    }
    return 0;
}