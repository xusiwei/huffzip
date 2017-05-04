#include "huffman.h"

#include <stdio.h>
#include <string.h>
#include <map>
#include <queue>
#include <algorithm>

HuffmanNode::HuffmanNode()
    : type_(LEAF)
    , left_(NULL)
    , right_(NULL)
    , parent_(NULL)
    , freq_(0)
    , value_(0)
{
}

void HuffmanNode::dump() const
{
    printf("HuffmanNode_%p: {type: %s, left:%p, right:%p, parent:%p, freq:%d, value:%c}.\n",
        this, (type_ == LEAF ? "LEAF" : "INNER"), left_, right_, parent_, freq_, value_);
}

HuffmanTree::HuffmanTree()
    : root_(NULL)
{
    leaves_.resize(NCHARS);
    for (int i = 0; i < NCHARS; i++) {
        leaves_[i].value_ = static_cast<uint8_t>(i);
    }
}

void HuffmanTree::feed(uint8_t byte)
{
    leaves_[byte].freq_++;
}

void HuffmanTree::build()
{
    std::multimap<uint32_t, HuffmanNode*> nodes;
    for (size_t i = 0; i < leaves_.size(); i++) {
        if (leaves_[i].freq_) {
            leaves_[i].dump();
            nodes.insert(std::make_pair(leaves_[i].freq_, &leaves_[i]));
        }
    }

    size_t count = 0;
    inners_.resize(leaves_.size() / 2 + 1);
    while (nodes.size() > 1) {
        HuffmanNode* a = nodes.begin()->second;
        nodes.erase(nodes.begin());
        HuffmanNode* b = nodes.begin()->second;
        nodes.erase(nodes.begin());

        HuffmanNode* c = &inners_[count++];
        c->freq_ = a->freq_ + b->freq_;
        c->type_ = HuffmanNode::INNER;
        c->left_ = a;
        c->right_ = b;
        a->parent_ = b->parent_ = c;

        c->dump();
        nodes.insert(std::make_pair(inners_.back().freq_, c));
    }
    root_ = nodes.begin()->second;

    std::vector<HuffmanNode*> stack;
    stack.push_back(root_);
    while (stack.size()) {
        HuffmanNode* p = stack.back();
        stack.pop_back();
        if (p->parent_) {
            p->code_ = p->parent_->code_;
            p->code_.push_back(p->parent_->left_ == p ? true : false);
        }
        if (p->left_) stack.push_back(p->left_);
        if (p->right_) stack.push_back(p->right_);
    }
}

void HuffmanTree::dump() const
{
    std::vector<HuffmanNode*> stack;
    if (!root_) return;

    printf("HuffmanTree::dump(%p)\n", root_);
    stack.push_back(root_);
    while (stack.size()) {
        HuffmanNode* p = stack.back();
        stack.pop_back();
        printf("%p:", p);
        if (HuffmanNode::LEAF == p->type_) {
            printf("LEAF| %c |", p->value_);
        } else {
            printf("    |   |");
        }
        printf("%6d | %s |\n", p->freq_, bv2string(p->code_).c_str());
        if (p->left_) stack.push_back(p->left_);
        if (p->right_) stack.push_back(p->right_);
    }
}

std::string bv2string(std::vector<bool>& bv)
{
    std::string result;
    for (size_t i = 0; i < bv.size(); i++) {
        result += (bv[i] ? '1' : '0');
    }
    return result;
}
