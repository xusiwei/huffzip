#include "huffman.h"

#include <stdio.h>

int main(int argc, char* argv[])
{
    std::string str = "ABBCCC";
    if (argc > 1) str = argv[1];

    HuffmanTree tree;
    for (size_t i = 0; i < str.length(); i++) {
        tree.feed(str[i]);
    }

    tree.build();
    tree.dump();
    return 0;
}
