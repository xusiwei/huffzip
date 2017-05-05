#include "huffman.h"

#include <stdio.h>
#include <string.h>
#include <map>
#include <queue>
#include <sstream>
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

std::string HuffmanNode::str() const
{
    std::stringstream ss;
    ss << this
       << ":{type: " << (LEAF == type_ ? "LEAF" : "INNER")
       << ", left: " << left_
       << ", right: " << right_
       << ", parent: " << parent_
       << ", freq: " << freq_
       << ", value: " << (int)value_ << "}";
    return ss.str();
}

void HuffmanNode::dump() const
{
    printf("%s\n", str().c_str());
}

HuffmanTree::HuffmanTree()
{
    reset();
}

void HuffmanTree::reset()
{
    root_ = NULL;
    leaves_.resize(NCHARS);
    for (int i = 0; i < NCHARS; i++) {
        leaves_[i].value_ = static_cast<uint8_t>(i);
    }
    inners_.resize(0);
    inners_.reserve(NCHARS);
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
            nodes.insert(std::make_pair(leaves_[i].freq_, &leaves_[i]));
        }
    }

    size_t count = 0;
    size_t leaves_count = nodes.size();
    while (nodes.size() > 1) {
        HuffmanNode* a = nodes.begin()->second;
        nodes.erase(nodes.begin());
        HuffmanNode* b = nodes.begin()->second;
        nodes.erase(nodes.begin());

        inners_.resize(count + 1);
        HuffmanNode* c = &inners_[count++];
        c->freq_ = a->freq_ + b->freq_;
        c->type_ = HuffmanNode::INNER;
        c->left_ = a;
        c->right_ = b;
        a->parent_ = b->parent_ = c;

        nodes.insert(std::make_pair(c->freq_, c));
    }
    root_ = nodes.begin()->second;

    if (1 == leaves_count) {
        inners_.resize(1);
        HuffmanNode* a = root_;
        root_ = &inners_[0];
        root_->freq_ = a->freq_;
        root_->type_ = HuffmanNode::INNER;
        root_->left_ = a;
        root_->right_ = NULL;
        a->parent_ = root_;
    }

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

    std::map<HuffmanNode*, int> depth;
    depth[root_] = 0;

    stack.push_back(root_);
    while (stack.size()) {
        HuffmanNode* p = stack.back();
        stack.pop_back();

        std::string prefix(depth[p] * 4, ' ');
        printf("%s%s\n", prefix.c_str(), p->str().c_str());
        if (p->left_) {
            stack.push_back(p->left_);
            depth[p->left_] = depth[p] + 1;
        }
        if (p->right_) {
            stack.push_back(p->right_);
            depth[p->right_] = depth[p] + 1;
        }
    }
}

bool HuffmanTree::getEncodeDict(std::map<uint8_t, std::vector<bool> >* dict)
{
    if (!dict) return false;

    dict->clear();
    for (size_t i = 0; i < NCHARS; i++) {
        if (leaves_[i].freq_) {
            dict->insert(std::make_pair(leaves_[i].value_, leaves_[i].code_));
        }
    }
    return true;
}

bool HuffmanTree::getDecodeDict(std::map<std::vector<bool>, uint8_t>* dict)
{
    if (!dict) return false;

    dict->clear();
    for (size_t i = 0; i < NCHARS; i++) {
        if (leaves_[i].freq_) {
            dict->insert(std::make_pair(leaves_[i].code_, leaves_[i].value_));
        }
    }
    return true;
}

std::string bv2string(std::vector<bool>& bv)
{
    std::string result;
    for (size_t i = 0; i < bv.size(); i++) {
        result += (bv[i] ? '1' : '0');
    }
    return result;
}
