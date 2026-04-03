#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "../src/bit-stream.h"

TEST(BitStreamTest, BasicWriteRead) {
    std::stringbuf buffer;

    {
        io::Writer io_writer(buffer);
        BitStream::Writer bit_writer(io_writer);
        // Записуємо 1011 (4 біти)
        bit_writer.writeBit(true);
        bit_writer.writeBit(false);
        bit_writer.writeBit(true);
        bit_writer.writeBit(true);
        bit_writer.flush(); // Має записати 10110000 (0xB0)
    }

    io::Reader io_reader(buffer);
    BitStream::Reader bit_reader(io_reader);

    EXPECT_EQ(bit_reader.readBit(), true);
    EXPECT_EQ(bit_reader.readBit(), false);
    EXPECT_EQ(bit_reader.readBit(), true);
    EXPECT_EQ(bit_reader.readBit(), true);
}

TEST(BitStreamTest, MultiByteOverflow) {
    std::stringbuf buffer;
    io::Writer io_writer(buffer);

    {
        BitStream::Writer bit_writer(io_writer);
        // Записуємо 17 бітів (2 повних байти + 1 біт)
        for (int i = 0; i < 17; ++i) {
            bit_writer.writeBit(true);
        }
        bit_writer.flush();
    }

    io::Reader io_reader(buffer);
    BitStream::Reader bit_reader(io_reader);
    for (int i = 0; i < 17; ++i) {
        EXPECT_TRUE(bit_reader.readBit()) << "Error at bit " << i;
    }
}

TEST(BitStreamTest, PatternTest) {
    std::stringbuf buffer;
    io::Writer io_writer(buffer);
    std::vector<bool> pattern = {1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1};

    {
        BitStream::Writer bit_writer(io_writer);
        for (const bool b : pattern) bit_writer.writeBit(b);
        bit_writer.flush();
    }

    io::Reader io_reader(buffer);
    BitStream::Reader bit_reader(io_reader);
    for (bool b : pattern) {
        EXPECT_EQ(bit_reader.readBit(), b);
    }
}

TEST(BitStreamTest, PaddingCheck) {
    std::stringbuf buffer;
    io::Writer io_writer(buffer);

    {
        BitStream::Writer bit_writer(io_writer);
        bit_writer.writeBit(true); // Записуємо лише 1 біт
        bit_writer.flush();        // Має записати 10000000 (0x80)
    }

    io::Reader io_reader(buffer);
    uint8_t raw_byte = 0;
    io_reader.readInt(raw_byte);
    EXPECT_EQ(raw_byte, 0x80); // 128
}