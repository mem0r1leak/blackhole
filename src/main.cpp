#include <filesystem>
#include <fstream>
#include <iostream>

#include "bit-stream.h"
#include "huffman.h"
#include "io.h"

int main() {
    constexpr std::string_view filename = "../assets/testdata";

    uint32_t freqs[255]={};
    const std::fstream file(filename.data(), std::ios::in | std::ios::binary);
    io::Reader reader(*file.rdbuf());
    std::vector<uint8_t> raw;
    raw.resize(std::filesystem::file_size(filename));
    reader.readSpan(std::span(raw));
    uint8_t min=raw.front(), max=raw.front();
    for (const auto& i : raw) {
        if (i < min) min = i;
        if (i > max) max = i;
        freqs[i]++;
    }
    auto tree = Huffman::Tree::buildTree(std::span(freqs).subspan(min), min);
    auto code_lengths = Huffman::Cannonical::Codes{};
    Huffman::Cannonical::getLengths(tree, tree.head(), code_lengths);
    Huffman::Cannonical::Code codes[255]={};
    Huffman::Cannonical::getCodes(code_lengths, std::span(codes));
    auto stdout_writer = io::Writer(*std::cout.rdbuf());
    BitStream::Writer bitwriter(stdout_writer);
    for (uint8_t c : raw) {
        bitwriter.writeBits(codes[c].code, codes[c].len);
    }
    bitwriter.endflush();
    return 0;
}
