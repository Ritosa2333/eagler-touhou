// th12.dat (THA1) read in place: the same container as TH10/TH11, with
// TH12's own per-file cipher table (thtk thdat95.c th12_crypt_params).
#pragma once
#include "Platform.hpp"
#include <cstdio>
#include <mutex>
#include <string>
#include <unordered_map>

namespace th12host {
struct CryptParams {u8 key,step;u32 block,limit;};
// Source and destination must not overlap. Limits are in original whole blocks.
void resource_decrypt(const u8* source,u8* destination,u32 size,CryptParams params) noexcept;
void resource_encrypt(const u8* source,u8* destination,u32 size,CryptParams params) noexcept;
bool lzss_decode(const u8* input,u32 size,u8* output,u32 capacity,u32& written) noexcept;
std::vector<u8> lzss_literals(const std::vector<u8>& input);

class Archive {
public:
    struct Entry {std::string name;u32 offset=0,size=0,packed=0;};
    ~Archive();
    bool open(const std::string& path);
    void close();
    bool opened()const{return file!=nullptr||!directory.empty();}
    // Names are matched without case, like the original's lookup.
    const Entry* find(const std::string& name)const;
    // Safe to call from a loader thread while the game thread reads too.
    bool read(const std::string& name,std::vector<u8>& output);
    std::vector<Entry> entries;
    std::string error;
    // Builds a THA1 archive for the host tests (entries stored, or literal-only LZSS).
    static std::vector<u8> build_for_tests(const std::vector<std::pair<std::string,std::vector<u8>>>& files,bool compress=false);
private:
    std::string directory;
    FILE* file=nullptr;std::unordered_map<std::string,size_t> index;std::mutex mutex; // the file position and `error`
};
}
