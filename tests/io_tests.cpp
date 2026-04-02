#include <gtest/gtest.h>
#include <sstream>
#include "../src/io.h"

using namespace io;

// Допоміжна функція для перегляду байтів у буфері (для відладки)
std::vector<uint8_t> get_bytes(const std::stringbuf& buf) {
    std::string s = buf.str();
    return {s.begin(), s.end()};
}

// Тест запису та читання цілих чисел з різним порядком байтів
TEST(IoTest, IntEndianness) {
    std::stringbuf buffer;
    Writer writer(buffer);
    Reader reader(buffer);

    constexpr uint32_t original = 0x12345678;

    // Пишемо як Big Endian
    writer.writeInt<std::endian::big>(original);

    // Перевіряємо сирі байти: у Big Endian перший байт має бути 0x12
    const auto bytes = get_bytes(buffer);
    EXPECT_EQ(bytes[0], 0x12);
    EXPECT_EQ(bytes[3], 0x78);

    // Читаємо назад як Big Endian
    uint32_t restored;
    ASSERT_TRUE(reader.readInt<std::endian::big>(restored));
    EXPECT_EQ(original, restored);
}

// Тест варіативного запису/читання (writeValues / readValues)
TEST(IoTest, MultipleValues) {
    std::stringbuf buffer;
    Writer writer(buffer);
    Reader reader(buffer);

    uint16_t a = 0xAA;
    uint32_t b = 0xBBBB;
    float c = 3.14f;

    writer.writeValues<std::endian::little>(a, b, c);

    uint16_t ra;
    uint32_t rb;
    float rc;

    ASSERT_TRUE(reader.readValues<std::endian::little>(ra, rb, rc));
    EXPECT_EQ(a, ra);
    EXPECT_EQ(b, rb);
    EXPECT_NEAR(c, rc, 0.0001f);
}

// Тест роботи з Optional
TEST(IoTest, OptionalRead) {
    std::stringbuf buffer;
    Writer writer(buffer);
    Reader reader(buffer);

    writer.writeInt<std::endian::big>(uint16_t(42));

    const auto res = reader.readInt<std::endian::big, uint16_t>();
    ASSERT_TRUE(res.has_value());
    EXPECT_EQ(res.value(), 42);

    // Спроба прочитати далі, коли даних немає
    const auto failed_res = reader.readInt<std::endian::big, uint16_t>();
    EXPECT_FALSE(failed_res.has_value());
}

// Тест для Span та Endianness (масовий byteswap)
TEST(IoTest, SpanEndianness) {
    std::stringbuf buffer;
    Writer writer(buffer);
    Reader reader(buffer);

    std::vector<uint32_t> data = {1, 2, 3, 4};
    // Пишемо нативно
    writer.writeSpan(std::span(data));

    // Читаємо як "не нативно" (має відбутися byteswap)
    std::vector<uint32_t> read_data(4);
    constexpr std::endian other = std::endian::native == std::endian::little
                        ? std::endian::big
                        : std::endian::little;

    reader.readSpan<other>(std::span(read_data));

    // Перше число 1 (0x00000001) після swap має стати 0x01000000
    EXPECT_EQ(read_data[0], std::byteswap(static_cast<uint32_t>(1)));
}

// Тест для Array
TEST(IoTest, ArrayTest) {
    std::stringbuf buffer;
    Writer writer(buffer);
    Reader reader(buffer);

    constexpr std::array<uint16_t, 3> src = {10, 20, 30};
    writer.writeArray<std::endian::big, std::array<uint16_t, 3>>(src);

    std::array<uint16_t, 3> dst;
    const auto read_size = reader.readArray<std::endian::big>(dst);

    EXPECT_EQ(read_size, sizeof(uint16_t) * 3);
    EXPECT_EQ(src, dst);
}