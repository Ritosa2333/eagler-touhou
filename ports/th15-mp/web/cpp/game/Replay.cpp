#include "Replay.hpp"
#include "ResourceCrypt.hpp"
#include "Lzss.hpp"
namespace th15 {
namespace {u32 dword(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}u16 word(const u8* p){u16 v;std::memcpy(&v,p,2);return v;}}
bool Replay::open(const u8* data,u32 size){
    bytes.clear();stages={};touches={};count=selected=cursor=touch_cursor=0;input={};failure.clear();
    auto fail=[&](const char* reason){failure=reason;return false;};
    if(!data||size<0x24||dword(data)!=0x72353174||word(data+4)!=3)return fail("Not a TH15 version 3 replay");
    const u32 packed=dword(data+0x1c),unpacked=dword(data+0x20);
    if(!packed||packed>size-0x24||packed>64*1024*1024||size>128*1024*1024||unpacked<0xa4||unpacked>64*1024*1024)return fail("Invalid replay sizes");
    std::vector<u8> first(packed),second(packed),decoded(unpacked);
    if(!resource_crypt(data+0x24,first.data(),packed,{0x5c,0xe1,0x400,packed},false)||!resource_crypt(first.data(),second.data(),packed,{0x7d,0x3a,0x100,packed},false))return fail("Replay decryption failed");
    Lzss codec;u32 written=0;if(!codec.decode(second.data(),packed,decoded.data(),unpacked,written)||written!=unpacked)return fail("Replay decompression failed");
    const u32 n=dword(decoded.data()+0x88);if(!n||n>7||dword(decoded.data()+0x8c)>3||dword(decoded.data()+0x90)!=0||dword(decoded.data()+0x94)>4)return fail("Invalid replay selection");
    u32 offset=0xa4;std::array<ReplayStage,8> parsed{};
    for(u32 i=0;i<n;i++){
        if(offset>unpacked||unpacked-offset<0x238)return fail("Truncated replay stage");
        const u8* h=decoded.data()+offset;const u32 number=word(h),frames=dword(h+4),payload=dword(h+8);
        if(number<1||number>7||parsed[number].number)return fail("Invalid replay stage number");
        if(payload>unpacked-offset-0x238||frames>payload/6)return fail("Truncated replay input");
        const u32 fps=frames>1?1+(frames-2)/30:0;if(payload-frames*6<fps)return fail("Truncated replay frame rates");
        parsed[number]={u16(number),word(h+2),offset,frames,payload};offset+=0x238+payload;
    }
    if(offset!=unpacked)return fail("Unexpected decoded replay tail");
    // Match the delivered ports: continuous targets are a USER block, not
    // guessed keyboard directions in the original encrypted input stream.
    bool found_touch=false;std::array<std::vector<ReplayTouch>,8> imported;
    for(u32 tail=0x24+packed;size-tail>=12;){
        if(dword(data+tail)!=0x52455355)break;
        const u32 length=dword(data+tail+4);if(length<12||length>size-tail)return fail("Invalid replay USER block");
        if(dword(data+tail+8)==0x15){
            if(found_touch||length<32||dword(data+tail+12)!=0x54353154||dword(data+tail+16)!=1)return fail("Invalid replay touch header");found_touch=true;
            const u32 entries=dword(data+tail+20);if(entries>(length-32)/16||entries*16!=length-32)return fail("Invalid replay touch length");
            for(u32 i=0;i<entries;i++){const auto* p=data+tail+32+i*16;const u32 stage=word(p),frame=dword(p+4);PlayerTouch touch;touch.mode=word(p+2);std::memcpy(&touch.x,p+8,4);std::memcpy(&touch.y,p+12,4);
                if(stage<1||stage>7||!parsed[stage].number||frame>=parsed[stage].frames||!touch.mode||!touch.valid()||(!imported[stage].empty()&&frame<=imported[stage].back().frame))return fail("Invalid replay touch entry");
                imported[stage].push_back({frame,touch});
            }
        }tail+=length;
    }
    bytes.swap(decoded);stages=parsed;touches.swap(imported);count=n;return true;
}
const ReplayStage* Replay::stage(u32 number)const{return number<stages.size()&&stages[number].number?&stages[number]:nullptr;}
const u8* Replay::header(u32 number)const{auto* s=stage(number);return s?bytes.data()+s->offset:nullptr;}
u32 Replay::character()const{return bytes.empty()?0:dword(bytes.data()+0x8c);}
u32 Replay::difficulty()const{return bytes.empty()?0:dword(bytes.data()+0x94);}
u32 Replay::mode_flags()const{return bytes.empty()?0:bytes[10];}
bool Replay::select(u32 number){if(!stage(number))return false;selected=number;cursor=touch_cursor=0;input={};return true;}
ReplayInput Replay::tick(bool active){
    if(!active)return input;const u8 fps=input.fps;input={};input.fps=fps;const auto* s=stage(selected);
    if(s&&cursor<s->frames){
        const auto* p=bytes.data()+s->offset+0x238+cursor*6;input.held=word(p);input.pressed=word(p+2);input.released=word(p+4);
        input.end=input.held==65535&&input.pressed==65535&&input.released==65535;
        if(input.end){input.held=input.pressed=input.released=0;selected=0;return input;}
        const auto& stream=touches[selected];if(touch_cursor<stream.size()&&stream[touch_cursor].frame==cursor)input.touch=stream[touch_cursor++].motion;
        // 0x45c150 advances the FPS cursor after ticks 0,30,60,... .
        // Near a partial final interval it reads the byte after the FPS stream.
        // Preserve an adjacent stage byte; use zero beyond the allocated payload.
        const u32 index=cursor?1+(cursor-1)/30:0,offset=s->offset+0x238+s->frames*6+index;
        input.fps=offset<bytes.size()?bytes[offset]:0;
    }
    ++cursor;return input;
}
}
