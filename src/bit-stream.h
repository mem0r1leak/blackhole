#pragma once
#include <cassert>
#include <cstdint>

#include "io.h"

class BitStream {
    // Краще збільшити буфер до 8 байт для збільшення продуктивності. Поки як є.
    using Buffer = uint64_t;
    static constexpr uint8_t buf_bitsize = sizeof(Buffer) * 8;

public:
    // Автоматичний flush через деструктор НЕ ВІДБУВАЄТЬСЯ.
    class Writer {
        Buffer buf;
        uint8_t bits_written;
        io::Writer &writer;

    public:
        explicit Writer(io::Writer &writer) : buf(0), bits_written(0), writer(writer) {
        }

        // Пише біти починаючи з найстаршого біта.
        void writeBits(const uint32_t bits, const uint8_t amount) {
            assert(amount <= buf_bitsize);
            auto free_bits = buf_bitsize - bits_written;
            if (const auto carry = amount - free_bits; carry > 0) [[unlikely]] {
                // Якщо не все влазить.
                const auto payload = bits & ((1 << (amount - carry)) - 1) << carry;
                buf |= payload >> carry;
                bits_written += amount;
                flush();
                free_bits = buf_bitsize;
                buf |= (static_cast<Buffer>(bits) & (1 << carry) - 1) << (free_bits - carry);
                bits_written += amount;
            } else [[likely]] {
                // Якщо все влазить.
                buf |= static_cast<Buffer>(bits) << (free_bits - amount);
                bits_written += amount;
            }
        }

        void flush() {
            writer.writeInt(buf);
            buf = 0;
            bits_written = 0;
        }

        // Пише все побайтово щоб не було багато зайвих нулів.
        void endflush() {
            for (size_t byte_index = sizeof(Buffer)-1; byte_index > 0; byte_index--) {
                uint8_t byte = buf >> byte_index * 8 & 0xff;
                if ( byte != 0)
                    writer.writeInt(byte);
            }
            buf = 0;
            bits_written = 0;
        }
    };

    class Reader {
        Buffer buf;
        uint8_t bits_read;
        io::Reader &reader;

    public:
        explicit Reader(io::Reader &reader) : buf(0), bits_read(0), reader(reader) {
        }

        // Читає біти починаючи з найстаршого
        bool readBit() {
            if (bits_read == 0) [[unlikely]] {
                reader.readInt(buf);
            }
            const auto data = buf >> (buf_bitsize - bits_read - 1) & 1;
            bits_read++;
            if (bits_read == buf_bitsize) {
                [[unlikely]]
                        bits_read = 0;
            }
            return data;
        }
    };
};
