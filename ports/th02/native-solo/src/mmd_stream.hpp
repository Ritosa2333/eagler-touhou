#pragma once
#include "mmd_preview_engine.hpp"
#include <algorithm>
#include <array>
namespace soew {
// TinySoundFont advances envelopes in render-call-dependent blocks. Canonicalize
// calls below the consumer boundary; never let device/output chunking alter them.
// Reconstruct/reset this adapter whenever Play starts a fresh song.
class MmdStream {
public:
    explicit MmdStream(th03::audio::MmdSoundfontEngine &engine):engine_(engine){}
    std::expected<void,th03::audio::MmdSoundfontError> render(std::span<th03::audio::PmdCanonicalStereoSample> output){
        std::size_t written=0;
        while(written<output.size()){
            if(cursor_==block_.size()){
                block_.fill({});const auto result=engine_.render_mix(block_);if(!result)return result;cursor_=0;
            }
            const auto n=std::min(output.size()-written,block_.size()-cursor_);
            std::copy_n(block_.begin()+cursor_,n,output.begin()+written);cursor_+=n;written+=n;
        }return {};
    }
    void reset(){cursor_=block_.size();block_.fill({});}
private:
    th03::audio::MmdSoundfontEngine &engine_;
    std::array<th03::audio::PmdCanonicalStereoSample,256> block_{};
    std::size_t cursor_=block_.size();
};
}
