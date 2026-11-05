#include "Replay.hpp"
#include "SpellCatalog.hpp"
#include "PracticeReplay.hpp"
#include "ResourceCrypt.hpp"
#include "Lzss.hpp"
namespace th17 {
namespace {
u32 dword(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
u16 word(const u8* p){u16 v;std::memcpy(&v,p,2);return v;}
}
bool Replay::open(const u8* source,u32 size){
    bytes.clear();stages={};touches={};count=selected=cursor=touch_cursor=0;input={};historical_web=false;web_rules=0;practice_patch={};native_practice={};practice_changes.clear();practice_cursor=0;failure.clear();
    auto fail=[&](const char* reason){failure=reason;return false;};
    if(!source||size<36||dword(source)!=0x72373174||word(source+4)!=2)return fail("Not a TH17 version 2 replay");
    const u32 packed=dword(source+28),unpacked=dword(source+32);
    if(!packed||packed>size-36||packed>64*1024*1024||unpacked<0xa0||unpacked>64*1024*1024)return fail("Invalid replay sizes");
    std::vector<u8> first(packed),second(packed),decoded(unpacked);
    resource_crypt(source+36,first.data(),packed,{0x5c,0xe1,0x400,packed},false);
    resource_crypt(first.data(),second.data(),packed,{0x7d,0x3a,0x100,packed},false);
    Lzss codec;u32 written=0;
    if(!codec.decode(second.data(),packed,decoded.data(),unpacked,written)||written!=unpacked)return fail("Replay decompression failed");
    const u32 n=dword(decoded.data()+0x84);
    if(!n||n>7||dword(decoded.data()+0x88)>2||dword(decoded.data()+0x8c)>2||dword(decoded.data()+0x90)>4)return fail("Invalid replay selection");
    u32 offset=0xa0;std::array<ReplayStage,8> parsed{};
    for(u32 i=0;i<n;i++){
        if(offset>unpacked||unpacked-offset<0x158)return fail("Truncated replay stage");
        const u8* h=decoded.data()+offset;
        const u32 number=word(h),frames=dword(h+4),payload=dword(h+8);
        if(number<1||number>7||parsed[number].number)return fail("Invalid replay stage number");
        if(payload>unpacked-offset-0x158||frames>payload/6)return fail("Truncated replay input");
        const u32 fps=frames>1?1+(frames-2)/30:0;
        if(payload-frames*6<fps)return fail("Truncated replay frame rates");
        parsed[number]={u16(number),word(h+2),offset,frames,payload};offset+=0x158+payload;
    }
    if(offset!=unpacked)return fail("Unexpected decoded replay tail");
    // Match the delivered ports: continuous targets are a USER block, not
    // guessed keyboard directions in the original encrypted input stream.
    PracticePatch imported_practice;
    PracticeSettings native_settings;
    const i32 rank=signed_bits(dword(decoded.data()+0x90));
    const i32 spell=(decoded[10]&2)?signed_bits(dword(decoded.data()+0x9c)):-1;
    std::vector<PracticeToolChange> imported_tools;
    bool found_touch=false;std::array<std::vector<ReplayTouch>,8> imported;
    for(u32 tail=0x24+packed;size-tail>=12;){
        if(dword(source+tail)!=0x52455355)break;
        const u32 length=dword(source+tail+4);if(length<12||length>size-tail)return fail("Invalid replay USER block");
        if(dword(source+tail+8)==0x17){
            if(found_touch||length<32||dword(source+tail+12)!=0x54373154||dword(source+tail+16)!=1)return fail("Invalid replay touch header");found_touch=true;
            const u32 entries=dword(source+tail+20);if(entries>(length-32)/16||entries*16!=length-32)return fail("Invalid replay touch length");
            for(u32 i=0;i<entries;i++){const auto* p=source+tail+32+i*16;const u32 stage=word(p),frame=dword(p+4);PlayerTouch touch;touch.mode=word(p+2);std::memcpy(&touch.x,p+8,4);std::memcpy(&touch.y,p+12,4);
                if(stage<1||stage>7||!parsed[stage].number||frame>=parsed[stage].frames||!touch.mode||!touch.valid()||(!imported[stage].empty()&&frame<=imported[stage].back().frame))return fail("Invalid replay touch entry");
                imported[stage].push_back({frame,touch});
            }
        }else if(dword(source+tail+8)==0x18){
            if(web_rules||length!=24||dword(source+tail+12)!=0x52373154||dword(source+tail+16)!=1||dword(source+tail+20)!=2)return fail("Unsupported web replay rules revision");
            web_rules=dword(source+tail+20);
        }else if(dword(source+tail+8)==0x43415250){
            i32 stage=1;
            while(stage<8&&!parsed[stage].number)++stage;
            if(imported_practice||n!=1||!read_native_practice(source+tail+12,length-12,stage,rank,spell,native_settings))
                return fail("Invalid native TH17 thprac replay metadata");
            imported_practice=native_settings.patch;
        }else if(dword(source+tail+8)==0x19){
            if(imported_practice||length!=68||dword(source+tail+12)!=0x50373154||dword(source+tail+16)!=1)
                return fail("Invalid TH17 practice replay header");
            const auto* p=source+tail+20;
            imported_practice.stage=signed_bits(dword(p));
            imported_practice.section=signed_bits(dword(p+4));
            imported_practice.phase=signed_bits(dword(p+8));
            const u32 flags=dword(p+12);
            if(flags>3)return fail("Invalid TH17 practice replay flags");
            imported_practice.dialogue=flags&1;
            imported_practice.keep_drops=flags&2;
            for(u32 i=0;i<3;++i)imported_practice.keiki_offsets[i]=signed_bits(dword(p+16+4*i));
            for(u32 i=0;i<5;++i)imported_practice.spirits[i]=signed_bits(dword(p+28+4*i));
            if(!imported_practice.valid(rank,spell)||n!=1||!parsed[imported_practice.stage].number||
               (spell<0?!(decoded[10]&1):spell>=SpellCatalog::count))
                return fail("Invalid TH17 practice replay selection");
        }else if(dword(source+tail+8)==0x1a){
            if(!imported_practice||native_settings.patch||!imported_tools.empty()||length<28||(length-20)%8||
               dword(source+tail+12)!=0x43373154||dword(source+tail+16)!=1)
                return fail("Invalid TH17 practice tools stream");
            for(u32 at=20;at<length;at+=8){
                const u32 frame=dword(source+tail+at),tools=dword(source+tail+at+4);
                if((tools&~practice_tool_mask)||frame>=parsed[imported_practice.stage].frames||
                   (!imported_tools.empty()&&frame<=imported_tools.back().frame))
                    return fail("Invalid TH17 practice tools entry");
                imported_tools.push_back({frame,tools});
            }
        }tail+=length;
    }
    // The first web release had no rules marker. Only the complete decoded
    // fingerprint of the user-confirmed historical sample is recognized;
    // filenames, zero metadata and keyboard/touch guesses are insufficient.
    if(!web_rules&&decoded.size()==632081){u64 identity=14695981039346656037ull;for(const auto value:decoded)identity=(identity^value)*1099511628211ull;historical_web=identity==0x1ba464935e1f8f79ull;}
    bytes.swap(decoded);stages=parsed;practice_patch=imported_practice;native_practice=native_settings;practice_changes.swap(imported_tools);touches.swap(imported);count=n;return true;
}
const ReplayStage* Replay::stage(u32 number)const{return number<stages.size()&&stages[number].number?&stages[number]:nullptr;}
const u8* Replay::header(u32 number)const{auto* s=stage(number);return s?bytes.data()+s->offset:nullptr;}
bool Replay::initial_state(u32 number,ReplayStageState& state)const{const auto* p=header(number);if(!p)return false;std::memcpy(&state,p,sizeof(state));return true;}
u32 Replay::character()const{return bytes.empty()?0:dword(bytes.data()+0x88);}
u32 Replay::animal()const{return bytes.empty()?0:dword(bytes.data()+0x8c);}
u32 Replay::difficulty()const{return bytes.empty()?0:dword(bytes.data()+0x90);}
u32 Replay::mode_flags()const{return bytes.empty()?0:bytes[10];}
bool Replay::select(u32 number){if(!stage(number))return false;selected=number;cursor=touch_cursor=practice_cursor=0;input={};return true;}
ReplayInput Replay::tick(bool active){
    if(!active)return input;
    const u16 previous=input.held;const u8 fps=input.fps;const u32 tools=input.practice_tools;input={};input.fps=fps;input.practice_tools=tools;
    const auto* s=stage(selected);
    if(s&&cursor<s->frames){
        const auto* p=bytes.data()+s->offset+0x158+cursor*6;
        if(word(p)==65535&&word(p+2)==65535&&word(p+4)==65535){input.end=true;selected=0;return input;}
        const auto& stream=touches[selected];if(touch_cursor<stream.size()&&stream[touch_cursor].frame==cursor)input.touch=stream[touch_cursor++].motion;
        if(practice_cursor<practice_changes.size()&&practice_changes[practice_cursor].frame==cursor)
            input.practice_tools=practice_changes[practice_cursor++].tools;
        input.held=word(p);
        // Original 0x44f2f0 invokes 0x41b550 after loading the three words.
        // Edges used by the game are recomputed from the previous held state.
        input.pressed=(input.held^previous)&input.held;
        input.released=(input.held^previous)&u16(~input.held);
        const u32 index=cursor?1+(cursor-1)/30:0;
        const u32 off=s->offset+0x158+s->frames*6+index;
        input.fps=off<bytes.size()?bytes[off]:0;
    }
    ++cursor;
    return input;
}
}
