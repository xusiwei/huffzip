//
// Created by xu on 17-5-5.
//

#ifndef HUFFZIP_COMPRESSOR_H
#define HUFFZIP_COMPRESSOR_H

#include "huffman.h"

#include <stdio.h>

class Compressor
{
public:
    Compressor();

    ~Compressor();

    int64_t compress(const char* ifn, const char* ofn);

private:
    void reset();

    std::string get_meta_info();

private:
    FILE* fin_;
    FILE* fout_;
    FILE* fmeta_;
    HuffmanTree tree_;
    std::map<uint8_t, std::vector<bool> > dict_;
};

#endif //HUFFZIP_COMPRESSOR_H
