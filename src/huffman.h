#pragma once
#include <cassert>
#include <cstdint>
#include <queue>
#include <vector>

namespace Huffman {
    class Tree {
    public:
        struct Node {
            uint32_t has_symb: 1;
            uint32_t symb: 31;
            uint32_t freq;
            uint32_t left;
            uint32_t right;

            bool is_leaf() const { return has_symb; }

            bool operator <(const Node &other) const {
                return freq < other.freq;
            }
        };

        uint32_t head_index;
        std::vector<Node> codes;

        Tree() : head_index(0), codes({}) {
        }

        struct IndexFreqPair {
            uint32_t index;
            uint32_t freq;

            bool operator <(const IndexFreqPair &other) const {
                return freq > other.freq;
            }
        };

        Node& head() {
            return codes[head_index];
        }

        static Tree buildTree(const std::span<uint32_t> freqs, const uint32_t offset) {
            Tree tree;
            std::priority_queue<IndexFreqPair> queue;
            size_t codes_i = 0;
            for (uint32_t i = 0; i < freqs.size(); ++i) {
                if (freqs[i] > 0) {
                    tree.codes.push_back(Node{1, i + offset, freqs[i], 0, 0});
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
#ifndef NDEBUG
        void print() {
            printNode(&codes[head_index]);
        }
#endif

    private:
#ifndef NDEBUG
        void printNode(const Node *node, const std::string &prefix = "", const bool is_left = true) {
            std::cout << prefix;
            std::cout << (is_left ? "├── " : "└── ");

            if (node->has_symb) {
                const char c = node->symb >= 32 && node->symb <= 126 ? static_cast<char>(node->symb) : '?';
                std::cout << "\033[1;32m['" << c << "' (" << node->freq << ")]\033[0m" << std::endl;
                return;
            }
            std::cout << "(" << node->freq << ")" << std::endl;

            const std::string new_prefix = prefix + (is_left ? "│   " : "    ");

            printNode(&codes[node->left], new_prefix, true);
            printNode(&codes[node->right], new_prefix, false);
        }
#endif
    };

    namespace Cannonical {
        // Trees with depth > 32 unsupported. WARNING: ORDER MATTERS!
        //std::pair<code length,symbol>
        using Codes = std::priority_queue<std::pair<uint32_t, uint32_t>,
            std::vector<std::pair<uint32_t, uint32_t> >,
            std::greater<>>;

        static void getLengths(const Tree &tree, const Tree::Node& node, Codes &codes, const uint32_t depth=0) {
            if (node.is_leaf()) {
                assert(depth < 32); // Check if tree depth (code length) less than 32.
                codes.emplace(depth, node.symb);
                return;
            }

            getLengths(tree, tree.codes[node.left], codes, depth + 1);
            getLengths(tree, tree.codes[node.right], codes, depth + 1);
        }

        // DANGER: bitcodes is sparce array and 0 is undefined state for non alphabet symbols and defined for alphabet symbols also (literally bitcode=0).
        static void getCodes(Codes &codes, std::span<uint32_t> bitcodes) {
            if (codes.empty()) return;
            auto [last_len, last_symb] = codes.top();
            codes.pop();

            uint32_t code = 0;
            bitcodes[last_symb] = code;

            while (!codes.empty()) {
                auto [cur_len, symb] = codes.top();
                codes.pop();
                code++;
                if (cur_len > last_len) {
                    code <<= cur_len - last_len;
                    last_len = cur_len;
                }
                bitcodes[symb] = code;
            }
        }
    }
}
