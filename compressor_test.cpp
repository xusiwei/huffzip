//
// Created by xu on 17-5-5.
//

#include <inttypes.h>

#include "compressor.h"

int main(int argc, char* argv[])
{
    std::string in = "zero.bin";
    std::string out = in + ".hzip";
    std::string meta;
    if (argc > 1) in = argv[1];
    if (argc > 2) out = argv[2];

    Compressor compressor;
    printf("write count: %" PRId64, compressor.compress(in, out, &meta));
    return 0;
}
