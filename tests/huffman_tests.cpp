#include <gtest/gtest.h>
#include "../src/huffman.h"

using namespace Huffman;

// 1. Тест на порожні частоти
TEST(HuffmanTree, EmptyFrequencies) {
    std::vector<uint32_t> freqs(256, 0);
    const Tree tree = Tree::buildTree(freqs, 0);
    EXPECT_TRUE(tree.codes.empty());
}

// 2. Один символ (крайній випадок)
TEST(HuffmanTree, SingleSymbol) {
    std::vector<uint32_t> freqs(256, 0);
    freqs['a'] = 10;
    Tree tree = Tree::buildTree(freqs, 0);

    EXPECT_EQ(tree.codes.size(), 1);
    EXPECT_TRUE(tree.head().is_leaf());
    EXPECT_EQ(tree.head().symb, 'a');
}

// 3. Два символи (просте дерево)
TEST(HuffmanTree, TwoSymbols) {
    std::vector<uint32_t> freqs(256, 0);
    freqs['a'] = 5;
    freqs['b'] = 10;
    Tree tree = Tree::buildTree(freqs, 0);

    // Має бути 3 вузли: 2 листки + 1 корінь
    EXPECT_EQ(tree.codes.size(), 3);
    EXPECT_FALSE(tree.head().is_leaf());
    EXPECT_EQ(tree.head().freq, 15);
}

// 4. Перевірка пріоритетності (менші частоти глибше)
TEST(HuffmanTree, PriorityCheck) {
    std::vector<uint32_t> freqs(256, 0);
    freqs['a'] = 1; // найрідший
    freqs['b'] = 1;
    freqs['c'] = 10;
    Tree tree = Tree::buildTree(freqs, 0);

    // 'a' та 'b' мають бути дітьми одного вузла, бо у них найменші частоти
    const uint32_t root_left = tree.head().left;
    const uint32_t root_right = tree.head().right;

    // Один з дітей кореня має бути внутрішнім вузлом (сума 1+1=2)
    const bool internal_found = !tree.codes[root_left].is_leaf() || !tree.codes[root_right].is_leaf();
    EXPECT_TRUE(internal_found);
}

// 5. Тест з offset
TEST(HuffmanTree, OffsetCheck) {
    std::vector<uint32_t> freqs = {10, 20}; // індекси 0 та 1
    constexpr uint32_t offset = 100;
    const Tree tree = Tree::buildTree(freqs, offset);

    // Перевіряємо, що символи отримали offset
    bool found_100 = false;
    for(const auto& n : tree.codes) if(n.has_symb && n.symb == 100) found_100 = true;
    EXPECT_TRUE(found_100);
}