#include "huffman.h"

#include <stdio.h>

int main(int argc, char* argv[])
{
    std::string str = "ABBCCC";
    if (argc > 1) str = argv[1];

    HuffmanTree tree;
    for (auto c : str)
        tree.feed(c);

    tree.build();
    tree.dump();
    return 0;
}
