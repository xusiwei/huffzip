#ifndef HUFFZIP_HUFFMAN_H
#define HUFFZIP_HUFFMAN_H

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

    void reset();

    void dump() const;

    bool getEncodeDict(std::map<uint8_t, std::vector<bool> >* dict);

    bool getDecodeDict(std::map<std::vector<bool>, uint8_t>* dict);

private:
    HuffmanNode* root_;
    std::vector<HuffmanNode> leaves_;
    std::vector<HuffmanNode> inners_;
};

std::string bv2string(std::vector<bool>& bv);

#endif // HUFFZIP_HUFFMAN_H
