#pragma once
#include <algorithm>
#include <optional>
#include <ostream>
#include <streambuf>

namespace io {
    struct MemoryBuf : std::streambuf {
        MemoryBuf(uint8_t* base, const size_t size) {
            const auto p = reinterpret_cast<char*>(base);
            setg(p, p, p + size);
        }

        // Повертає кількість байт що лишились
        std::streamsize showmanyc() override {
            return egptr() - gptr();
        }
    };

    template<typename T>
    struct is_std_array : std::false_type {
    };

    template<typename T, std::size_t N>
    struct is_std_array<std::array<T, N> > : std::true_type {
    };

    template<typename T>
    inline constexpr bool is_std_array_v = is_std_array<T>::value;

    using stream_size = std::streamsize;

    class Writer {
        std::streambuf &stream;

    public:
        explicit Writer(std::streambuf &stream) : stream(stream) {
        }

        template<std::endian E = std::endian::native, typename T> requires std::is_arithmetic_v<T>
        stream_size writeInt(const T &value) {
            if constexpr (E != std::endian::native) {
                const auto swapped_value = std::byteswap(value);
                return stream.sputn(reinterpret_cast<const char *>(&swapped_value), sizeof(T));
            } else {
                return stream.sputn(reinterpret_cast<const char *>(&value), sizeof(T));
            }
        }

        bool writeInt(const uint8_t &value) {
            return stream.sputc(static_cast<char>(value)) != std::streambuf::traits_type::eof();
        }

        template<std::endian E = std::endian::native, typename... Args>
        stream_size writeValues(Args &&... args) {
            stream_size total = 0;
            total = (writeInt<E>(std::forward<Args>(args)) + ... + 0);
            return total;
        }

        template<typename T>
        stream_size writeRaw(const T *data, const stream_size size) {
            return stream.sputn(reinterpret_cast<const char *>(data), size);
        }

        template<typename T>
        stream_size writeSpan(const std::span<T> value) {
            return stream.sputn(reinterpret_cast<const char *>(value.data()),
                                static_cast<stream_size>(value.size()) * sizeof(T));
        }

        template<std::endian E = std::endian::native, typename T> requires is_std_array_v<T>
        stream_size writeArray(const T &arr) {
            stream_size written = 0;
            for (const auto &item: arr) {
                written += writeInt<E>(item);
            }
            return written;
        }
    };

    class Reader {
        std::streambuf &stream;

    public:
        explicit Reader(std::streambuf &stream) : stream(stream) {
        }

        template<std::endian E = std::endian::native, typename T> requires std::is_arithmetic_v<T>
        bool readInt(T &value) {
            if (stream.sgetn(reinterpret_cast<char *>(&value), sizeof(T)) != sizeof(T)) {
                return false;
            }
            if constexpr (E != std::endian::native) {
                value = std::byteswap(value);
            }
            return true;
        }

        bool readInt(uint8_t &value) {
            const int tmp = stream.sbumpc();
            // fuck c++
            // stupid shit
            if (tmp == std::streambuf::traits_type::eof()) [[unlikely]] {
                return false;
            }
            value = static_cast<uint8_t>(tmp);
            return true;
        }

        template<std::endian E = std::endian::native, typename T> requires std::is_arithmetic_v<T>
        std::optional<T> readInt() {
            if (T value; readInt<E>(value)) {
                return value;
            }
            return std::nullopt;
        }

        template<std::endian E = std::endian::native, typename... Args>
        bool readValues(Args &... args) {
            return (readInt<E>(args) && ...);
        }

        template<typename T>
        stream_size readRaw(T *data, const stream_size size) {
            return stream.sgetn(static_cast<char *>(data), size);
        }

        template<std::endian E = std::endian::native, typename T> requires std::is_arithmetic_v<T>
        stream_size readSpan(std::span<T> value) {
            const stream_size bytes_read = stream.sgetn(
                reinterpret_cast<char *>(value.data()),
                static_cast<stream_size>(value.size_bytes())
            );

            if constexpr (E != std::endian::native) {
                const size_t elements_read = static_cast<size_t>(bytes_read) / sizeof(T);
                for (size_t i = 0; i < elements_read; ++i) {
                    value[i] = std::byteswap(value[i]);
                }
            }
            return bytes_read;
        }

        template<std::endian E = std::endian::native, typename T>
            requires is_std_array_v<T>
        stream_size readArray(T &arr) {
            using ElementT = T::value_type;
            return readSpan<E, ElementT>(std::span{arr});
        }
    };
}
