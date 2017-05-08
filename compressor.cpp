//
// Created by xu on 17-5-5.
//

#include <climits>
#include <bitset>
#include <assert.h>
#include <sstream>

#include "compressor.h"

Compressor::Compressor()
    : fin_(NULL)
    , fout_(NULL)
    , fmeta_(NULL)
{
}

Compressor::~Compressor()
{
    reset();
}

// binary string to file stream
int64_t bins2fs(FILE* stream, std::vector<bool> bins, bool last = false)
{
    if (last) {
        // the low half of the last byte is the number of padding bits.
        //
        //  appendix   |   binstr + padding    |  nop  | padcount
        // byte sides: |               |               |
        //      0       x x x x x x x x 0 0 0 0 0 1 0 0    4
        //      1       x x x x x x x x x 0 0 0 0 0 1 1    3
        //      2       x x x x x x x x x x 0 0 0 0 1 0    2
        //      3       x x x x x x x x x x x 0 0 0 0 1    1
        //      4       x x x x x x x x x x x x 0 0 0 0    0
        //      5       x x x x x 0 0 0 0 0 0 0 0 1 1 1    7
        //      6       x x x x x x 0 0 0 0 0 0 0 1 1 0    6
        //      7       x x x x x x x 0 0 0 0 0 0 1 0 1    5
        size_t appendix = bins.size() % 8;
        size_t padcount = appendix <= 4 ? 4 - appendix : 12 - appendix;

        // append padding bits
        bins.insert(bins.end(), padcount, false);

        // convert and append number of padding bits to binary string
        for (size_t i = 0; i < 4; i++) {
            bins.insert(bins.end(), (padcount & (1 << (3 - i))) != 0);
        }
    }
    assert(bins.size() % 8 == 0);

    int64_t count = 0;
    for (size_t i = 0; i < bins.size(); i += 8) {
        uint8_t byte = 0;
        for (size_t j = 0; j < 8; j++) { // get a byte
            byte |= bins[i + j] ? (1 << (7 - j)) : 0;
        }
        count += fwrite(&byte, sizeof(byte), 1, stream);
    }
    return count;
}

int64_t Compressor::compress(const char* ifn, const char* ofn)
{
    reset();
    fin_ = fopen(ifn, "rb");
    fout_ = fopen(ofn, "wb+");

    std::string meta_fn = ifn;
    meta_fn += ".meta";
    fmeta_ = fopen(meta_fn.c_str(), "wb+");

    size_t nbytes = 0;
    const int PAGE = 4096;
    std::vector<uint8_t> buffer(PAGE);

    uint64_t length = 0;
    while ((nbytes = fread(&buffer[0], sizeof(buffer[0]), buffer.size(), fin_)) > 0) {
        length += nbytes;
        for (size_t i = 0; i < nbytes; i++) {
            tree_.feed(buffer[i]);
        }
    }

    tree_.build();
    tree_.dump();
    tree_.getEncodeDict(&dict_);

    // write meta info
    std::string meta = get_meta_info();
    fwrite(&meta[0], sizeof(meta[0]), meta.size(), fmeta_);

    // compressing
    fseek(fin_, 0, SEEK_SET);
    std::vector<bool> bins_buffer; // binary string buffer

    uint64_t read_count = 0, write_count = 0;
    clock_t last_report_clock = clock();
    while ((nbytes = fread(&buffer[0], sizeof(buffer[0]), buffer.size(), fin_)) > 0) {
        read_count += nbytes;

        // report progress
        if (clock() - last_report_clock > 1 * CLOCKS_PER_SEC) {
            int percent = read_count * 100 / length;
            printf("%d%%\n", percent);
            last_report_clock = clock();
        }
        for (size_t i = 0; i < nbytes; i++) {
            std::vector<bool> code = dict_[buffer[i]];
            bins_buffer.insert(bins_buffer.end(), code.begin(), code.end());
        }
        if (bins_buffer.size() > PAGE) {
            std::vector<bool> remain(bins_buffer.begin() + PAGE, bins_buffer.end());
            bins_buffer.resize(PAGE);
            write_count += bins2fs(fout_, bins_buffer);
            bins_buffer = remain;
        }
    }
    if (bins_buffer.size()) {
        write_count += bins2fs(fout_, bins_buffer, true);
    }

    return write_count;
}

void Compressor::reset()
{
    if (fin_) {
        fclose(fin_);
    }
    if (fout_) {
        fclose(fout_);
    }
    if (fmeta_) {
        fclose(fmeta_);
    }
    tree_.reset();
    dict_.clear();
}

std::string Compressor::get_meta_info()
{
    std::stringstream ss;
    ss << "size: " << dict_.size() << "\n";
    for (auto& kv : dict_) {
        ss << (int)kv.first << ": " << bv2string(kv.second) << "\n";
    }
    return ss.str();
}
