#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <sstream>
#include "../src/bit-stream.h"

// Допоміжний клас-мокер для io::Writer, якщо він записує в std::stringbuf або вектор
// Припускаємо, що твоя структура проектів дозволяє використовувати stringbuf

class BitStreamTest : public ::testing::Test {
protected:
    std::stringbuf buffer;
};

// 1. Перевірка базового запису шматочками (без переповнення байта)
TEST_F(BitStreamTest, WriteShortChunks) {
    io::Writer io_writer(buffer);
    BitStream::Writer bit_writer(io_writer);

    // Записуємо 0b10 (2 біти) та 0b111 (3 біти) та 0b0 (1 біт)
    // Разом 6 бітів: 101110.. (0b10111000 = 0xB8)
    bit_writer.writeBits(0b10, 2);
    bit_writer.writeBits(0b111, 3);
    bit_writer.writeBits(0b0, 1);
    bit_writer.endflush();

    const std::string res = buffer.str();
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(static_cast<uint8_t>(res[0]), 0xB8);
}

// 2. Перевірка Carry (Перенос бітів у наступний байт)
TEST_F(BitStreamTest, WriteWithCarryOver) {
    io::Writer io_writer(buffer);
    BitStream::Writer bit_writer(io_writer);

    // Заповнюємо 7 бітів: 1111111.
    bit_writer.writeBits(0x7F, 7);

    // Додаємо 3 біти: 010
    // Перший байт має стати: 1111111 + 0 = 11111110 (0xFE)
    // Другий байт має розпочатися з: 10...... (0x80)
    bit_writer.writeBits(0b010, 3);
    bit_writer.endflush();

    std::string res = buffer.str();
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(static_cast<uint8_t>(res[0]), 0xFE);
    EXPECT_EQ(static_cast<uint8_t>(res[1]), 0x80);
}

// 3. Запис повного 32-бітного числа (декілька байтів за один виклик)
TEST_F(BitStreamTest, WriteFullUint32) {
    io::Writer io_writer(buffer);
    BitStream::Writer bit_writer(io_writer);

    // Пакуємо 0xDEADBEEF (32 біти)
    bit_writer.writeBits(0xDEADBEEF, 32);
    bit_writer.endflush();

    const std::string res = buffer.str();
    ASSERT_EQ(res.size(), 4);
    EXPECT_EQ(static_cast<uint8_t>(res[0]), 0xDE);
    EXPECT_EQ(static_cast<uint8_t>(res[1]), 0xAD);
    EXPECT_EQ(static_cast<uint8_t>(res[2]), 0xBE);
    EXPECT_EQ(static_cast<uint8_t>(res[3]), 0xEF);
}

// 4. Тест на "рвані" довжини (імітація реальних кодів Хаффмана)
TEST_F(BitStreamTest, HuffmanCodeSimulation) {
    io::Writer io_writer(buffer);
    BitStream::Writer bit_writer(io_writer);

    // Імітуємо послідовність кодів Хаффмана
    bit_writer.writeBits(0b1, 1);    // '1'
    bit_writer.writeBits(0b00, 2);   // '00'
    bit_writer.writeBits(0b1111, 4); // '1111'
    bit_writer.writeBits(0b0, 1);    // '0'
    // Разом 8 біт: 10011110 (0x9E)

    bit_writer.writeBits(0b101010, 6); // 6 біт наступного байта (0b10101000 = 0xA8)
    bit_writer.endflush();

    std::string res = buffer.str();
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(static_cast<uint8_t>(res[0]), 0x9E);
    EXPECT_EQ(static_cast<uint8_t>(res[1]), 0xA8);
}

// 5. Тест на межові значення Carry (коли залишається рівно 0 вільного місця)
TEST_F(BitStreamTest, ExactByteFill) {
    io::Writer io_writer(buffer);
    BitStream::Writer bit_writer(io_writer);

    bit_writer.writeBits(0xAA, 8); // 10101010
    bit_writer.writeBits(0x55, 8); // 01010101
    bit_writer.endflush();

    std::string res = buffer.str();
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(static_cast<uint8_t>(res[0]), 0xAA);
    EXPECT_EQ(static_cast<uint8_t>(res[1]), 0x55);
}

// 6. Тест на велику кількість 1-бітних записів (Stress Test)
TEST_F(BitStreamTest, ManySingleBits) {
    io::Writer io_writer(buffer);
    BitStream::Writer bit_writer(io_writer);

    for (int i = 0; i < 16; ++i) {
        bit_writer.writeBits(i % 2, 1); // 01010101...
    }
    bit_writer.endflush();

    std::string res = buffer.str();
    ASSERT_EQ(res.size(), 2);
    EXPECT_EQ(static_cast<uint8_t>(res[0]), 0x55); // 01010101
    EXPECT_EQ(static_cast<uint8_t>(res[1]), 0x55);
}

// 7. Тест на очищення буфера (Padding)
TEST_F(BitStreamTest, FlushPadding) {
    io::Writer io_writer(buffer);
    BitStream::Writer bit_writer(io_writer);

    bit_writer.writeBits(0b1, 1);
    bit_writer.endflush(); // Має бути 10000000 (0x80)

    std::string res = buffer.str();
    EXPECT_EQ(static_cast<uint8_t>(res[0]), 0x80);
}