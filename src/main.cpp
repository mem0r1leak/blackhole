#include <fstream>
#include <iostream>

#include "bit-stream.h"

int main() {
    io::Writer writer(*std::cout.rdbuf());
    BitStream::Writer stream(writer);
    stream.writeBit(false);
    stream.writeBit(true);
    stream.writeBit(false);
    stream.writeBit(true);
    stream.flush();
    return 0;
}
