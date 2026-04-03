#pragma once
#include <cstdint>

#include "io.h"

class BitStream {
    // Краще збільшити буфер до 8 байт для збільшення продуктивності. Поки як є.
    using Buffer = uint8_t;
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
        void writeBit(const bool bit) {
            buf |= static_cast<Buffer>(bit) << (buf_bitsize - bits_written - 1);
            bits_written++;
            if (bits_written == buf_bitsize) [[unlikely]] {
                flush();
            }
        }

        // Примусово пише буфер в Writer. Після чого скидає стан: buf=0, bits_written=0
        void flush() {
            writer.writeInt(buf);
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
