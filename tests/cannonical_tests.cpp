#include <gtest/gtest.h>
#include "../src/huffman.h"

using namespace Huffman;

TEST(HuffmanCannonical, GetLengthsSimple) {
    std::vector<uint32_t> freqs(256, 0);
    freqs['a'] = 10; freqs['b'] = 10; freqs['c'] = 10; freqs['d'] = 10;
    Tree tree = Tree::buildTree(freqs, 0);

    Cannonical::Codes q;
    Cannonical::getLengths(tree, tree.head(), q);

    EXPECT_EQ(q.size(), 4);
    while(!q.empty()) {
        auto [len, symb] = q.top();
        EXPECT_EQ(len, 2); // В ідеальному дереві на 4 елементи всі мають довжину 2
        q.pop();
    }
}

// 7. Довжини для виродженого дерева (лінійне)
TEST(HuffmanCannonical, GetLengthsDegenerate) {
    std::vector<uint32_t> freqs(256, 0);
    freqs['a'] = 1; freqs['b'] = 2; freqs['c'] = 4;
    Tree tree = Tree::buildTree(freqs, 0);

    Cannonical::Codes q;
    Cannonical::getLengths(tree, tree.head(), q);

    // Мають бути довжини: 'c'=1, 'b'=2, 'a'=2 (або близько того)
    std::map<uint32_t, uint32_t> res;
    while(!q.empty()) {
        res[q.top().second] = q.top().first;
        q.pop();
    }
    EXPECT_EQ(res['c'], 1);
    EXPECT_EQ(res['a'], 2);
}

// 8. Канонічні коди: однакова довжина
TEST(HuffmanCannonical, GetCodesSameLength) {
    Cannonical::Codes q;
    q.emplace(3, 'a');
    q.emplace(3, 'b');
    q.emplace(3, 'c');

    Cannonical::Code bitcodes[255];
    Cannonical::getCodes(q, bitcodes);

    // Мають бути послідовні числа: 0, 1, 2
    EXPECT_EQ(bitcodes['a'].code, 0);
    EXPECT_EQ(bitcodes['b'].code, 1);
    EXPECT_EQ(bitcodes['c'].code, 2);
}

// 9. Канонічні коди: перехід довжини (зсув)
TEST(HuffmanCannonical, GetCodesShift) {
    Cannonical::Codes q;
    q.emplace(1, 'a'); // Має бути 0
    q.emplace(3, 'b'); // (0+1) << (3-1) = 4 (100)

    Cannonical::Code bitcodes[255];
    Cannonical::getCodes(q, bitcodes);

    EXPECT_EQ(bitcodes['a'].code, 0);
    EXPECT_EQ(bitcodes['b'].code, 4); // 100 в бінарці
}

TEST(HuffmanCannonical, GetCodesComplex) {
    Cannonical::Codes q;
    q.emplace(1, 'e');
    q.emplace(2, 't');
    q.emplace(3, 'a');
    q.emplace(3, 'o');

    Cannonical::Code bitcodes[255];
    Cannonical::getCodes(q, bitcodes);

    // e: 0 (1 біт)
    // t: (0+1) << 1 = 2 (10)
    // a: (2+1) << 1 = 6 (110)
    // o: (6+1) = 7 (111)
    EXPECT_EQ(bitcodes['e'].code, 0);
    EXPECT_EQ(bitcodes['t'].code, 2);
    EXPECT_EQ(bitcodes['a'].code, 6);
    EXPECT_EQ(bitcodes['o'].code, 7);
}
