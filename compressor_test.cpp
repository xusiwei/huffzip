//
// Created by xu on 17-5-5.
//

#include <inttypes.h>

#include "compressor.h"

int main(int argc, char* argv[])
{
    std::string in = __FILE__;
    std::string out = in + ".hzip";
    if (argc > 1) in = argv[1];
    if (argc > 2) out = argv[2];

    Compressor compressor;
    printf("write count: %" PRId64, compressor.compress(in.c_str(), out.c_str()));
    return 0;
}
