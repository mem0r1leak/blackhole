#pragma once
#include <cstdint>
#include <queue>
#include <unordered_map>
#include <vector>

#include "bit-stream.h"

// alphabet: We will make it uint32_t because no matter what it will be aligned to uint32_t but we need one bit to indicate if there is a symbol
// so |1 bit|      |31 bit|
//    has_symb      symbol
// frequency: uint32_t maybe for start

namespace Huffman {
    // Дерево ЛИШЕ для навчальних цілей
    class Tree {
    public:
        Tree() : head_index(0), codes({}) {
        }

        struct Node {
            uint32_t has_symb: 1;
            uint32_t symb: 31;
            uint32_t freq;
            uint32_t left;
            uint32_t right;

            bool operator <(const Node &other) const {
                return freq < other.freq;
            }
        };

        struct IndexFreqPair {
            uint32_t index;
            uint32_t freq;

            bool operator <(const IndexFreqPair &other) const {
                return freq > other.freq;
            }
        };

        // Max alphabet size is 255.
        void make_code_map(std::unordered_map<uint32_t, uint8_t[32]> &codes) {
            auto head = codes[head_index];
            uint8_t code[32];
            io::MemoryBuf buf(code, 32);
            io::Writer writer(buf);
            BitStream::Writer bitstream(writer);

            // TODO: implement
        }

        static Tree buildTree(const std::span<uint32_t> freqs, const uint32_t offset) {
            Tree tree;
            std::priority_queue<IndexFreqPair> queue;
            size_t codes_i = 0;
            for (uint32_t i = 0; i < freqs.size(); ++i) {
                if (freqs[i] > 0) {
                    tree.codes.push_back(Node{1, i+offset, freqs[i], 0, 0});
                    queue.emplace(codes_i++, freqs[i]);
                }
            }
            while (queue.size() > 1) {
                const auto [left_index, left_freq] = queue.top();
                queue.pop();
                const auto [right_index, right_freq] = queue.top();
                queue.pop();
                auto freq = left_freq + right_freq;
                tree.codes.push_back(Node{0, 0, freq, left_index, right_index});
                queue.emplace(tree.codes.size() - 1, freq);
            }
            if (!queue.empty())
                tree.head_index = queue.top().index;
            return tree;
        }

    private:
        uint32_t head_index;
        std::vector<Node> codes;
    };
}
