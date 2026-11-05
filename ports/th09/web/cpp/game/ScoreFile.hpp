#pragma once
#include "Lzss.hpp"
#include "Rng.hpp"
namespace th09 {
// Native TH09 score.dat container. Chapters remain intact for their managers.
class ScoreFile {
public:
    bool decode(const u8* input,u32 size);
    bool assign(const u8* input,u32 size);
    std::vector<u8> encode(Rng& rng);
    const std::vector<u8>& data() const noexcept {return bytes;}
private:
    bool valid() const;
    Lzss codec;
    std::vector<u8> bytes;
};
}
