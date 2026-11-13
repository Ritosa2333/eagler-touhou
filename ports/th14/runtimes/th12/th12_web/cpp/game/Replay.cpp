#include "Replay.hpp"
#include "PortTitle.hpp"
#include "ShotFiles.hpp"
#include "ResourceCrypt.hpp"
#include "Lzss.hpp"
#include "GameEconomy.hpp"
#include "Rng.hpp"
#if PORT_TITLE == 14
#include "ReplayRngRestoreTh14.hpp"
#endif
#include "PlayerMotion.hpp"
#include <algorithm>
#include <cmath>
#include <string>

namespace th11 {
namespace {
u32 dword(const u8* p){return u32(p[0])|u32(p[1])<<8|u32(p[2])<<16|u32(p[3])<<24;}
u32 word(const u8* p){return u32(p[0])|u32(p[1])<<8;}
}
void ReplayStageState::read(const u8* p){
    seed=word(p+2);x=signed_bits(dword(p+replay_state_x));y=signed_bits(dword(p+replay_state_y));
    scalars=replay_state_scalars;
    if(!scalars)return;
#if PORT_TITLE == 14
    std::memcpy(th14_globals.data(),p+0x14,0x70);
    const auto value=[&](u32 n){return signed_bits(th14_globals[n]);};
    score=value(2);continues=value(4);rank=value(5);graze=value(6);th14_practice_mode=value(7);
    points=value(9);power=value(12);lives=value(15);fragments=value(16);extends=value(17);
    weapon=signed_bits(dword(p+0x84));
    for(u32 i=0;i<20;++i)spell_times[i]=dword(p+0x88+i*4);
    th14_stage_flags=dword(p+0xd8);
    return;
#endif
    score=signed_bits(dword(p+12));power=i16(word(p+16));
    points=signed_bits(dword(p+20)*100);lives=i16(word(p+24));fragments=i16(word(p+26));
    rank=signed_bits(dword(p+28));
    // The entry position is at +0x30/+0x34, not +0x20/+0x24. The stage-one recording stores y = 51200 there, and
    // 51200/128 = 400 - exactly the height a new stage starts the player at, with x = 0 being that same start's x,
    // while +0x20/+0x24 hold zeroes. Reading the zeroes put the replay's player at the top of the screen, in the
    // enemy spawn and bullet field, so the recording's own dodging no longer worked and the run was a different game
    // from its first frame. The fields around it keep their offsets: only the position is proven by that arithmetic.

    continues=signed_bits(dword(p+32));extends=signed_bits(dword(p+36));focused=signed_bits(dword(p+56));
    graze=signed_bits(dword(p+60));weapon=signed_bits(dword(p+64));
    for(u32 i=0;i<20;++i)spell_times[i]=dword(p+60+i*4);
}
void ReplayStageState::restore(GameEconomy& e,Rng& rng,bool first,Rng* visual)const{
#if PORT_TITLE == 14
    th14_restore_replay_rng(rng,visual,seed,first);
    // Initial ordinary/attract creation restores only gameplay RNG at
    // 454a41..454a47. Existing-controller stage entry restores both seeds at
    // 45610b..456129. Both paths reset only the gameplay call counter.
    restore_economy_th14(e);return;
#endif
    rng.set_seed(u16(seed));rng.set_call_count(0);
    (void)visual;
    // The title's own restore writes the entry pair and the float copy of it; the economy it
    // leaves as the stage start built it (th13.exe 0x047850..0x0478bd).
    if(!scalars)return;e.score_units=score;e.power=std::min(power,e.max_power);
    e.point_value=points;e.lives=lives;e.rank=rank;
    // Initial playback setup 4356e0 restores these. The next-stage path
    // 436f30 deliberately retains the live fragment and graze counters.
    if(first){e.life_fragments=fragments;e.graze=graze;}
}
#if PORT_TITLE == 14
void ReplayStageState::restore_economy_th14(GameEconomy& e)const{
    th14_project_stage_globals(*this,e);
}
#endif
void ReplayStageState::restore_position(PlayerMotionState& s)const{
    s.x=x;s.y=y;
#if PORT_TITLE == 14
    // 45455a converts each integer to float before multiplying by 1/128.
    s.position.x=float(x)*.0078125f;s.position.y=float(y)*.0078125f;
#else
    s.position.x=float(double(x)*.0078125);s.position.y=float(double(y)*.0078125);
#endif
    s.focused=focused;s.weapon_mode=weapon;
}
bool Replay::open(const u8* data,u32 size){
    bytes.clear();stages={};touches={};count=selected=cursor=touch_cursor=0;input={};failure.clear();metadata_only=false;
#if PORT_TITLE == 14
    callback_cursor=0;
#endif
    auto fail=[&](const char* why){failure=why;return false;};
    // The replay magic and version are the title's, and every demo a title ships is the reading:
    // TH12 writes "t12r" (0x72323174) with version 4 over all four of its demos, TH13 "t13r"
    // (0x72333174) with version 2 over all four, TH14 the same pair "t13r"/2 over all three of
    // its own (the writer kept the older constant, so `kPortTitle==13` alone did not cover it),
    // TH16 and TH17 "t16r"/"t17r" (0x72363174, 0x72373174) with version 2 over their seven
    // demos together, and TH18 "t18r" (0x72383174) with version 6 over its four. Every title
    // puts the packed and unpacked sizes at +0x1c and +0x20, so only the pair moves.
    // tools/th-replay-table.py prints the same table from a directory.
    // TH11, the build the harness and its inherited fixtures are, keeps its own pair: the four
    // demos at `reference/assets/demo0..3.rpy` are "t11r" (0x72313174) with the same version
    // word 4 (.uhfl11.state, `py -3 tools/tmp/th11-reference.py`).
    constexpr u32 replay_magic=kPortTitle==18?0x72383174u:kPortTitle==17?0x72373174u
        :kPortTitle==16?0x72363174u:(kPortTitle==13||kPortTitle==14)?0x72333174u
        :kPortTitle==12?0x72323174u:0x72313174u;
    constexpr u32 replay_version=kPortTitle==18?6u
        :(kPortTitle==13||kPortTitle==14||kPortTitle==16||kPortTitle==17)?2u:4u;
    if(!data||size<0x24||dword(data)!=replay_magic||word(data+4)!=replay_version)return fail("not a TH12 replay");
    const u32 packed=dword(data+0x1c),unpacked=dword(data+0x20);
    // Bounds are checked before allocations or any stage/input pointer use.
    if(!packed||packed>size-0x24||packed>64*1024*1024||unpacked<0x70||unpacked>64*1024*1024)return fail("invalid replay sizes");
    std::vector<u8> first(packed),second(packed),decoded(unpacked);
    // TH12's replay cipher keys are NOT TH11's. Read out of th12.exe's replay
    // loader at 0x43c424..0x43c44a:
    //   0x43c42d push 0xe1   0x43c428 push 0x800   0x43c434 mov al,0x5e
    //   0x43c444 push 0x3a   0x43c442 push 0x40    0x43c448 mov al,0x7d
    // so the passes are key 0x5e over 0x800-byte blocks and key 0x7d over
    // 0x40-byte blocks; TH11 used 0xaa and 0x3d/0x7a. With these values all four
    // attract demos in th12.dat decompress to exactly their declared length
    // (see tools/th12-replay-keys.mjs).
    //
    // TH13's keys are its own, and both the read and the confirmation are mechanical now:
    // `tools/th-replay-keys-exe.py` - which is checked against TH12's known pair first -
    // finds 0x48863 mov al,0x7d / push 0x100 / push 0x3a and 0x48877 mov al,0x5c /
    // push 0xe1 / push 0x100 in th13.exe, and `tools/th-replay-keys.mjs` demands that
    // every demo the title ships decode to precisely the length its header declares:
    // all four of TH13's do with 0x5c/0xe1/0x400 then 0x7d/0x3a/0x100 (the LZSS
    // parameterisation is unchanged at 13/4/8192), and the result opens with the
    // replay's own "ZUN" signature. With TH12's pair the same demos stop at 11494 of
    // 20808 bytes, which is what the boot reported. The exe shows both a 0x100 and a
    // 0x400 push beside each `mov`, and the decode is what picks between them: 0x400 for
    // the first pass, 0x100 for the second.
    // Every demo of TH14, TH16, TH17 and TH18 decodes to exactly the length its header declares
    // with TH13's pair and with no other candidate: the port's own chain - two resource_crypt
    // passes then Lzss::decode - mirrored over all fourteen of their demos, after the mirror was
    // validated on all eight of TH12's and TH13's (tools/tmp/replay-keys-search.py). TH12's pair
    // stops TH16's third demo at 17169 of 33376, which is the boot's own number above. So the
    // pair belongs to the generation, and only TH12 is out of it.
    // TH11's pair is the one TH12's row above names as TH11's, and the fixture's four demos
    // are the reading: 0xaa/0xe1/0x800 then 0x3d/0x7a/0x40 writes exactly the declared length
    // on all four and opens with "ZUN", and the mirrored 0xaa/0x3a/0x800 + 0x3d/0xe1/0x40
    // split does not (tools/tmp/th11-pair.py).
    constexpr u8 replay_key1=kPortTitle==11?0xaa:(kPortTitle==12?0x5e:0x5c);
    constexpr u8 replay_step1=0xe1;
    constexpr u32 replay_block1=kPortTitle==11?0x800:(kPortTitle==12?0x800:0x400);
    constexpr u8 replay_key2=kPortTitle==11?0x3d:0x7d;
    constexpr u8 replay_step2=kPortTitle==11?0x7a:0x3a;
    constexpr u32 replay_block2=kPortTitle==11?0x40:(kPortTitle==12?0x40:0x100);
    if(!resource_crypt(data+0x24,first.data(),packed,{replay_key1,replay_step1,replay_block1,packed},false)||
       !resource_crypt(first.data(),second.data(),packed,{replay_key2,replay_step2,replay_block2,packed},false))return fail("replay decrypt failed");
    Lzss codec;u32 written=0;
    if(!codec.decode(second.data(),packed,decoded.data(),unpacked,written)||written!=unpacked)
        return fail(("replay decompression failed: wrote "+std::to_string(written)+" of "+std::to_string(unpacked)).c_str());
    // The four words sit where the title puts them (`replay_selection`), and the character bound
    // is the title's own count of characters (`player_character_max`): the six titles' attract
    // demos reach exactly 2/3/2/3/2/3, which is how the offsets were confirmed a second time.
    const u32 selection=replay_selection;
    const u32 n=dword(decoded.data()+selection);
    if(!n||n>7||dword(decoded.data()+selection+4)>player_character_max||
       dword(decoded.data()+selection+8)>player_subtype_max||dword(decoded.data()+selection+12)>4)
        return fail("invalid replay selection");
    u32 offset=kReplayFileHeader;std::array<ReplayStage,8> parsed{};
    for(u32 i=0;i<n;++i){
        if(offset>unpacked||unpacked-offset<kReplayStageHeader)return fail("truncated replay stage");
        const auto* p=decoded.data()+offset;const u32 st=word(p),frames=dword(p+4),payload=dword(p+8);
        if(st<1||st>7||parsed[st].number)return fail("invalid replay stage number");
        if(payload>unpacked-offset-kReplayStageHeader||frames>payload/6)return fail("truncated replay inputs");
        // Native reads one FPS byte per 30 ticks. Payload may contain one
        // extra byte when the final all-ffff marker was appended on save.
        const u32 fps=frames>1?1+(frames-2)/30:0;
        if(payload-frames*6<fps)return fail("truncated replay frame rates");
        parsed[st]={st,offset,frames,payload};offset+=kReplayStageHeader+payload;
    }
    if(offset!=unpacked)return fail("unexpected replay data after stages");
    // Optional browser continuous-motion data lives in a USER block, outside
    // the original encrypted payload. Ordinary original files need no block.
    for(u32 tail=0x24+packed;size-tail>=12;){
        if(dword(data+tail)!=0x52455355)break;
        const u32 length=dword(data+tail+4);if(length<12||length>size-tail)return fail("invalid replay USER block");
        if(dword(data+tail+8)==0x11){
            if(length<32||dword(data+tail+12)!=0x54313154||dword(data+tail+16)!=1)return fail("invalid replay touch header");
            const u32 entries=dword(data+tail+20);if(entries>(length-32)/16||entries*16!=length-32)return fail("invalid replay touch length");
            for(u32 i=0;i<entries;++i){const u8* p=data+tail+32+i*16;const u32 st=word(p),mode=word(p+2),frame=dword(p+4);float x,y;std::memcpy(&x,p+8,4);std::memcpy(&y,p+12,4);
                if(st<1||st>7||!parsed[st].number||frame>=parsed[st].frames||mode<1||mode>2||!std::isfinite(x)||!std::isfinite(y)||(!touches[st].empty()&&frame<=touches[st].back().frame))return fail("invalid replay touch entry");
                touches[st].push_back({frame,i32(mode),x,y});
            }
        }
        tail+=length;
    }
    bytes.swap(decoded);stages=parsed;count=n;return true;
}
const ReplayStage* Replay::stage(u32 number)const{return number<stages.size()&&stages[number].number?&stages[number]:nullptr;}
const u8* Replay::header(u32 number)const {auto* s=stage(number);return s?bytes.data()+s->offset:nullptr;}
u32 Replay::character()const{return bytes.empty()?0:dword(bytes.data()+replay_selection+4);}
u32 Replay::subtype()const{return bytes.empty()?0:dword(bytes.data()+replay_selection+8);}
u32 Replay::difficulty()const{return bytes.empty()?0:dword(bytes.data()+replay_selection+12);}
bool Replay::uses_touch()const{for(const auto& stream:touches)if(!stream.empty())return true;return false;}
void Replay::retain_metadata(){
    if(metadata_only||bytes.empty())return;
    std::vector<u8> headers(bytes.begin(),bytes.begin()+kReplayFileHeader);
    for(auto& stage:stages)if(stage.number){const u32 offset=u32(headers.size());headers.insert(headers.end(),bytes.begin()+stage.offset,bytes.begin()+stage.offset+kReplayStageHeader);stage.offset=offset;}
    bytes.swap(headers);touches={};selected=cursor=touch_cursor=0;input={};metadata_only=true;
}
bool Replay::select(u32 number){if(metadata_only||!stage(number))return false;selected=number;cursor=touch_cursor=0;
#if PORT_TITLE == 14
    // Native455ed0 enables the Replay callbacks, resets both stage pointers,
    // and456040 clears +210 before the incoming stage's first Replay12 tick.
    callback_cursor=0;
#endif
    input={};return true;}
ReplayInput Replay::tick(bool active){
    if(!active)return input;
    input.previous=input.held;input.end_marker=false;
    input.touch_mode=0;input.touch_x=input.touch_y=0;
    const auto* s=stage(selected);
    if(s&&cursor<s->frames){
        const auto* p=bytes.data()+s->offset+kReplayStageHeader+cursor*6;
        input.held=word(p);input.pressed=word(p+2);input.released=word(p+4);
        input.end_marker=input.held==0xffff&&input.pressed==0xffff&&input.released==0xffff;
#if PORT_TITLE == 14
        //45521b requests the replay-end menu before the FPS read and cursor
        // increments. Repeated terminal callbacks must retain that marker
        // and the last recorded rate, including an empty terminal replay.
        if(input.end_marker)return input;
#endif
        // TH14 4552a9..4552c4 advances the FPS pointer AFTER ticks0,30,60,... .
        const u32 fps_index=cursor?1+(cursor-1)/30:0;
        const u32 fps_size=s->payload_size-s->frames*6;
        input.fps=fps_size?bytes[s->offset+kReplayStageHeader+s->frames*6+(fps_index<fps_size?fps_index:fps_size-1)]:0;
    }else{input.held=input.pressed=input.released=0;}
    if(selected<touches.size()&&touch_cursor<touches[selected].size()){const auto& t=touches[selected][touch_cursor];if(t.frame==cursor){input.touch_mode=t.mode;input.touch_x=t.x;input.touch_y=t.y;++touch_cursor;}}
    ++cursor;
#if PORT_TITLE == 14
    //4552fb/455326 advance this global counter even after payload exhaustion.
    // The terminal marker returns above, without advancing either counter.
    ++callback_cursor;
#endif
    return input;
}
}
