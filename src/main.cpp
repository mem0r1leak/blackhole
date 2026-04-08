#include <filesystem>
#include <fstream>
#include <iostream>

#include "huffman.h"
#include "io.h"

int main() {
    const std::ifstream input("../assets/testdata.txt");
    io::Reader reader(*input.rdbuf());
    uint32_t freqs[256] = {};
    std::vector<uint8_t> raw_data;
    raw_data.resize(std::filesystem::file_size("../assets/testdata.txt"));
    reader.readSpan(std::span(raw_data));
    uint8_t min = raw_data.front();
    uint8_t max = raw_data.front();
    for (const auto& c : raw_data) {
        if (c < min) min = c;
        if (c > max) max = c;
        freqs[c]++;
    }
    auto tree = Huffman::Tree::buildTree(std::span(freqs+min, max-min+1), min);
    tree.printTree();
    auto map = std::unordered_map<uint32_t, std::string>{};
    tree.makeCodeMap(map);
    std::cout << "Code map: " << std::endl;
    for (const auto&[fst, snd] : map) {
        std::cout << static_cast<char>(fst) << ": " << snd << std::endl;
    }
    std::cout << "Frequencies: " << std::endl;
    for (size_t i = 0; i < 256; i++) {
        if (freqs[i] == 0) continue;
        std::cout << static_cast<char>(i) << ": " << freqs[i] << std::endl;
    }
    double avg_len = 0;
    for (size_t i = 0; i < 256; i++) {
        if (freqs[i] == 0) continue;
        avg_len += static_cast<double>(freqs[i])/44 * static_cast<double>(map[i].size());
    }
    std::cout << avg_len << std::endl;
    return 0;
}
