#include "FighterPattern.hpp"
namespace th075 {
static i16 signed_word(ByteReader& f){const u16 bits=f.word();i16 result;std::memcpy(&result,&bits,2);return result;}
static void rectangle(ByteReader& f,Rectangle& r){for(auto& v:r.bits)v=f.dword();}
bool read_image_frame(ByteReader& f,ImageFrame& v){
    v.sprite=f.word();v.originX=signed_word(f);v.originY=signed_word(f);v.duration=f.word();const u8 type=f.byte();
    if(type>2)return false;v.basicVisual=type==0;v.hasVisual=type==2;
    if(v.hasVisual){
        // The first word overlaps the following alpha byte in the original
        // visual owner. Only its low byte survives the subsequent byte read.
        v.visual.mode=u8(f.word());v.visual.alpha=f.byte();v.visual.red=f.byte();v.visual.green=f.byte();v.visual.blue=f.byte();
        const float scale=float(signed_word(f)*0.01f);v.visual.scale.fill(scale);
        for(auto& value:v.visual.projection)value=signed_word(f);
        for(auto& value:v.visual.rotation)value=float(signed_word(f));
    }
    return f.valid;
}
static bool frame(ByteReader& f,PatternFrame& v){
    if(!read_image_frame(f,v))return false;
    v.baseDamage=signed_word(f);v.proration=float(signed_word(f)*0.001f);
    for(auto& value:v.combat)value=signed_word(f);
    for(auto& value:v.motion)value=float(signed_word(f)*0.01f);
    for(auto& value:v.events)value=signed_word(f);
    for(auto& value:v.flags)value=f.dword();rectangle(f,v.baseBounds);
    for(auto* group:{&v.bodyBounds,&v.attackBounds}){const u32 count=f.dword();if(count>4096)return false;group->resize(count);for(auto& r:*group)rectangle(f,r);}
    return f.valid;
}
bool FighterPattern::load(const void* source,std::size_t bytes){
    sequences.clear();error.clear();ByteReader file(source,bytes);const u32 slots=file.dword(),count=file.dword();
    if(!file.valid||slots>65536||count>65536){error="Invalid fighter pattern table";return false;}
    std::vector<PatternSequence> parsed(slots);
    for(u32 i=0;i<count;i++){
        const u32 id=file.dword();if(id>=slots){error="Pattern sequence outside table";return false;}
        auto& sequence=parsed[id];sequence.present=true;sequence.mode=file.word();sequence.next=file.word();const u32 frames=file.dword();
        if(!file.valid||frames>16384){error="Invalid pattern frame count";return false;}sequence.frames.clear();sequence.frames.resize(frames);
        for(auto& value:sequence.frames)if(!frame(file,value)){error="Invalid pattern frame at "+std::to_string(file.offset);return false;}
    }
    if(!file.valid||file.offset!=file.size){error="Unexpected pattern trailing bytes";return false;}
    sequences=std::move(parsed);return true;
}
}



