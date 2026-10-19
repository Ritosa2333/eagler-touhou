#include "EffectPattern.hpp"
namespace th075 {
bool EffectPattern::load(const void* source,std::size_t bytes){
    sequences.clear();error.clear();ByteReader file(source,bytes);
    const u32 slots=file.dword(),count=file.dword();
    if(!file.valid||slots>65536||count>65536){error="Invalid effect pattern table";return false;}
    std::vector<EffectSequence> parsed(slots);
    for(u32 i=0;i<count;i++){
        const u32 id=file.dword(),frames=file.dword();
        if(!file.valid||id>=slots||frames>16384){error="Invalid effect sequence";return false;}
        auto& sequence=parsed[id];sequence.present=true;sequence.frames.resize(frames);
        for(auto& frame:sequence.frames)if(!read_image_frame(file,frame)){error="Invalid effect frame";return false;}
    }
    if(!file.valid||file.offset!=file.size){error="Unexpected effect trailing bytes";return false;}
    sequences=std::move(parsed);return true;
}
}
