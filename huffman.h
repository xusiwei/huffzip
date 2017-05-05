#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include <map>
#include <vector>
#include <string>

struct HuffmanNode {
    enum { LEAF,
        INNER } type_;
    HuffmanNode* left_;
    HuffmanNode* right_;
    HuffmanNode* parent_;

    uint32_t freq_;
    uint8_t value_;
    std::vector<bool> code_;

    HuffmanNode();
    HuffmanNode(const HuffmanNode&) = default;
    void dump() const;
    std::string str() const;
};

class HuffmanTree
{
public:
    HuffmanTree();

    static const int NCHARS = 1 << (8 * sizeof(uint8_t));

    void feed(uint8_t byte);

    void build();

    void dump() const;

private:
    HuffmanNode* root_;
    std::vector<HuffmanNode> leaves_;
    std::vector<HuffmanNode> inners_;
    std::map<uint8_t, std::vector<bool> > dict_;
};

std::string bv2string(std::vector<bool>& bv);

#endif // HUFFMAN_H
