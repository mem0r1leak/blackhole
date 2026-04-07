#include <filesystem>
#include <fstream>

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
    return 0;
}
