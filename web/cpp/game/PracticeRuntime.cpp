// Source-level adapter for thprac TH10 1.00a. Upstream patches: MIT,
// Copyright (c) 2022 Ack; see THPRAC-LICENSE.txt and PracticePatches.inc.
#include "PracticeRuntime.hpp"
#include "PracticeSections.hpp"
#include "GameEconomy.hpp"
#include "Stage.hpp"
#include "EnemyManager.hpp"
#include "EclProgram.hpp"
#include "Player.hpp"
#include "../platform/World.hpp"
#include "../platform/GameState.hpp"
#include <algorithm>
#include <cstring>
#include <utility>
namespace th10 {
namespace {
using std::pair;
// Canonical th10 STD instruction (thprac_th10.cpp). Length and opcode share
// the word at +4; arguments begin at +8.
struct Th10Std {i32 time;std::int16_t ins,length;i32 param1,param2;};
// 12-word camera/fog status used by the generated ST4 chapter code. The field
// order is exactly the aggregate initialiser order in PracticePatches.inc.
struct StdStatus {
    u32 fogChgTime,fogCol,fogStart,fogEnd;
    u32 camPosChgTime,camPosX,camPosY,camPosZ;
    u32 camDirChgTime,camDirX,camDirY,camDirZ;
};
// Upstream ECLHelper::SetFile uses a 0x99999 byte window into loaded_files[0].
constexpr u32 kEclBytes=0x99999;
// All addresses in generated code are offsets into owned game scripts.
// Bounds failures mark the writer invalid instead of touching foreign memory.
class ScriptWriter {
    u8* data;u32 size;u32 position=0;
public:
    bool valid=true;
    ScriptWriter(u8* bytes,u32 length):data(bytes),size(length){}
    void SetPos(u32 offset){position=offset;}
    template<class T> ScriptWriter& operator<<(T value){if(!data||position>size||sizeof(T)>size-position){valid=false;return *this;}std::memcpy(data+position,&value,sizeof(T));position+=sizeof(T);return *this;}
    template<class K,class T> ScriptWriter& operator<<(std::pair<K,T> value){SetPos(u32(value.first));return *this<<value.second;}
    template<class T> ScriptWriter& operator>>(T& value){value=read<T>(position);position+=sizeof(T);return *this;}
    template<class T> T read(u32 offset){T value{};if(!data||offset>size||sizeof(T)>size-offset)valid=false;else std::memcpy(&value,data+offset,sizeof(T));return value;}
};
using ECLHelper=ScriptWriter;
u8* ecl_data(browser::World& world){auto* enemies=world.actors.enemies;return enemies&&enemies->program?enemies->program->loaded_files[0]:nullptr;}
u8* stage_data(Stage* stage){return stage?reinterpret_cast<u8*>(stage->file):nullptr;}
class PracticePatcher {
    Stage* stage;GameEconomy& game;const PracticeConfig& thPracParam;
    ScriptWriter ecl,stdfile;bool valid=true;
    StdStatus st4_status{};
    enum {ECL_INS_TIME=0,ECL_INS_OPCODE=4,ECL_INS_ARG1=12,ECL_INS_ARG2=16};
    // thprac_th10.cpp:881. Uses the ECL time/opcode word layout.
    void ECLJump(ECLHelper& target,unsigned start,unsigned dest,int at_frame,int ecl_time=0){
        target.SetPos(start);
        target << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << unsigned(dest-start) << at_frame;
    }
    // thprac_th10.cpp:914. Zero the 16-bit operand at pos+4.
    template<class... Args> void ECLVoid(ECLHelper& target,size_t pos,Args... rest){
        target.SetPos(u32(pos+4));
        target << std::int16_t(0);
        if constexpr(sizeof...(Args)!=0)ECLVoid(target,rest...);
    }
    // thprac_th10.cpp:923. Clamp every instruction time to 60 while walking the
    // stage 6 shared ECL, then skip the boss intro.
    void ECLSt6Boss(ECLHelper& target){
        i32 ecl_time;std::int16_t ecl_ins;std::int16_t ecl_length;u32 p=0x68c;
        target << pair{0x754,(std::int16_t)0};
        ECLJump(target,0x10100,0x10120,0);
        ECLJump(target,0xfb0c,0xfb64,3669);
        target.SetPos(p);
        while(target.valid){
            ecl_time=target.read<i32>(p);ecl_ins=target.read<std::int16_t>(p+4);ecl_length=target.read<std::int16_t>(p+6);(void)ecl_ins;
            if(!target.valid||ecl_time==0x484c4345)break;
            if(ecl_time>60)ecl_time=60;
            target.SetPos(p);target << ecl_time;
            if(ecl_length<=0)break;
            p+=u32(ecl_length);target.SetPos(p);
        }
    }
    // thprac_th10.cpp:949.
    void ECLSt7MidBoss(ECLHelper& target){
        target << pair{0x18983,(std::int8_t)0x37}
            << pair{0x10bb4,60} << pair{0x10bc4,60} << pair{0x10bd8,60}
            << pair{0x10bec,60} << pair{0x10c04,60} << pair{0x10c18,60}
            << pair{0x10c28,60} << pair{0x10c48,60} << pair{0x10c5c,60}
            << pair{0x10c70,60} << pair{0x10c88,60} << pair{0x10c9c,60}
            << pair{0x10cb4,60} << pair{0x10cc8,60} << pair{0x10cdc,60}
            << pair{0x10afc,(std::int16_t)0};
        ECLJump(target,0x171a8,0x171cc,61);
    }
    // thprac_th10.cpp:960.
    void ECLSt7Boss(ECLHelper& target){
        target << pair{0x18983,(std::int8_t)0x42} << pair{0x18984,(std::int8_t)0x6f}
            << pair{0x18985,(std::int8_t)0x73} << pair{0x18986,(std::int8_t)0x73};
        ECLJump(target,0x18118,0x1813c,1);
        target << pair{0x1816c,0} << pair{0x18170,(std::int16_t)0};
    }
    // thprac_th10.cpp:829-867. Re-derive the ST4 intro camera/fog interpolator
    // timers once the injected STD instructions are reached. The original uses
    // an executable hook; the port applies the same write set directly.
    void std_st4_set_status(){
        if(!stage)return;
        if(stage->target_interpolation.duration&&st4_status.camDirChgTime){
            stage->target_interpolation.timer.previous=i32(st4_status.camDirChgTime)-1;
            stage->target_interpolation.timer.current=i32(st4_status.camDirChgTime);
            stage->target_interpolation.timer.fractional=float(st4_status.camDirChgTime);
        }
        if(stage->position_interpolation.duration&&st4_status.camPosChgTime){
            stage->position_interpolation.timer.previous=i32(st4_status.camPosChgTime)-1;
            stage->position_interpolation.timer.current=i32(st4_status.camPosChgTime);
            stage->position_interpolation.timer.fractional=float(st4_status.camPosChgTime);
        }
        if(stage->fog_interpolation.duration&&st4_status.fogChgTime){
            stage->fog_interpolation.timer.previous=i32(st4_status.fogChgTime)-1;
            stage->fog_interpolation.timer.current=i32(st4_status.fogChgTime);
            stage->fog_interpolation.timer.fractional=float(st4_status.fogChgTime);
        }
        st4_status=StdStatus{};
    }
    // thprac_th10.cpp:868. Inject the camera/fog block into STD 0x600 and jump.
    void STDSt4Jump(u32 pos,u32 time,StdStatus& status){
        stdfile.SetPos(0x600);
        stdfile << 0 << 0x00140002 << status.camPosX << status.camPosY << status.camPosZ
            << 0 << 0x00140004 << status.camDirX << status.camDirY << status.camDirZ
            << 0 << 0x00140008 << status.fogCol << status.fogStart << status.fogEnd
            << 0 << 0x00100001 << pos << time;
        st4_status=status;
        std_st4_set_status();
    }
    // thprac_th10.cpp:709. Rewrite the ST4 stage ANM timing words in place.
    void THStage4ANM(std::int16_t time_delta){
        u8* buffer=stage&&stage->animation_file?stage->animation_file->loaded:nullptr;
        const std::int16_t d1=std::int16_t(8300-time_delta),d2=std::int16_t(8600-time_delta),d3=std::int16_t(15000-time_delta);
        const auto word=[&](u32 pos,std::int16_t value){if(buffer)std::memcpy(buffer+pos,&value,2);};
        word(0xf4,d1);word(0x110,d2);word(0x118,d2);
        word(0x164,0);word(0x178,d1);word(0x194,d2);word(0x19c,d2);
        word(0x1e8,0);word(0x1fc,d1);word(0x218,d2);word(0x220,d2);
        word(0x1002e4,0);word(0x1002f8,d1);word(0x100314,d2);word(0x10031c,d3);word(0x10032c,d3);
    }
    // thprac_th10.cpp:743. Shift ST4 STD times from 0x5e0 onward.
    void THStage4STD(i32 time_delta){
        u32 p=0x5e0;
        while(stdfile.valid){
            const i32 time=stdfile.read<i32>(p);
            const std::int16_t ins=stdfile.read<std::int16_t>(p+4);
            const std::int16_t length=stdfile.read<std::int16_t>(p+6);
            if(!stdfile.valid||time==-1)break;
            if(ins==1){const i32 jmp=stdfile.read<i32>(p+12);stdfile.SetPos(p+12);stdfile << (jmp-time_delta>=0?jmp-time_delta:0);}
            stdfile.SetPos(p);stdfile << (time-time_delta>=0?time-time_delta:0);
            if(length<=0)break;
            p+=u32(length);
        }
    }
    // thprac_th10.cpp:766. Zero stage 6 boss ANM timing words.
    void THStage6ANM(){
        if(thPracParam.mode!=1||thPracParam.section<TH10_ST6_BOSS1||thPracParam.section>TH10_ST6_BOSS9)return;
        u8* buffer=stage&&stage->animation_file?stage->animation_file->loaded:nullptr;
        if(!buffer)return;
        const std::int16_t zero=0;
        const u32 offsets[]{
            0x0801d4,0x0801e8,0x080224,0x080238,
            0x0c0348,0x0c0364,0x0c03b4,0x0c03d0,0x0c0420,0x0c043c,
            0x0c048c,0x0c04a8,0x0c04f8,0x0c0514,0x0c0564,0x0c0580,
            0x0c05d0,0x0c05ec,
            0x01047f8,0x0104804,0x010484c,0x0104858};
        for(const u32 position:offsets)std::memcpy(buffer+position,&zero,2);
    }
    // thprac_th10.cpp:800. Shift stage 6 boss STD times by 3487 and enable the
    // three boss phase flags.
    void THStage6STD(){
        if(thPracParam.mode!=1||thPracParam.section<TH10_ST6_BOSS1||thPracParam.section>TH10_ST6_BOSS9)return;
        u32 p=0x994;
        while(stdfile.valid){
            const i32 time=stdfile.read<i32>(p);
            const std::int16_t ins=stdfile.read<std::int16_t>(p+4);
            const std::int16_t length=stdfile.read<std::int16_t>(p+6);
            if(!stdfile.valid||time==-1)break;
            if(ins==1){const i32 jmp=stdfile.read<i32>(p+12);stdfile.SetPos(p+12);stdfile << (jmp-3487>=0?jmp-3487:0);}
            stdfile.SetPos(p);stdfile << (time-3487>=0?time-3487:0);
            if(length<=0)break;
            p+=u32(length);
        }
        stdfile.SetPos(0xb68);stdfile << 1;
        stdfile.SetPos(0xb84);stdfile << 1;
        stdfile.SetPos(0xba0);stdfile << 1;
    }
#include "PracticePatches.inc"
    // thprac_th10.cpp:1734.
    void THSectionPatch(){
        const auto section=thPracParam.section;
        if(section>=10000&&section<20000){
            const int warp_stage=(section-10000)/100;
            const int portion_id=(section-10000)%100;
            THStageWarp(ecl,warp_stage,portion_id);
        }else{
            THPatch(ecl,section);
        }
        THStage6ANM();
        THStage6STD();
    }
public:
    PracticePatcher(browser::World& world,browser::GameState& state):
        stage(world.backgrounds.current),
        game(state.game),
        thPracParam(state.practice.run),
        ecl(ecl_data(world),kEclBytes),
        stdfile(stage_data(stage),stage?stage->source_size:0){
        if(!ecl_data(world)||!stage||!stage->file)valid=false;
    }
    bool apply(){
        // Upstream th10_patch_main only invokes the section patch in Custom
        // mode; apply_practice already gates that.
        if(thPracParam.mode==1)THSectionPatch();
        return valid&&ecl.valid&&stdfile.valid;
    }
};
bool cheat_active(const PracticeState& p,u32 bit){return p.enabled&&!p.replay&&(p.cheats&bit)!=0;}
}
// thprac_th10.cpp:2314 th10_logo. The upstream hook jumps over the stage-entry
// logo ANM creation (GuiResources::activate's stgXXlogo.anm scripts 0/1) for a
// custom practice start at a section, except a stage-warp portion 1, which is a
// normal stage start.
bool practice_skip_stage_logo(const PracticeState& p){
    const auto& c=p.run;
    if(!(p.active&&c.mode==1&&c.section))return false;
    const i32 section=c.section;
    return section<=10000||section>=20000||section%100!=1;
}
// thprac_th10.cpp:1752 THBGMTest. Non-zero only for a custom practice spell/
// midboss section whose generated th_sections_bgm entry selects the boss theme;
// stage warps and midboss sections keep the stage theme.
bool practice_boss_bgm(const PracticeState& p){
    const auto& c=p.run;
    if(!(p.active&&c.mode==1))return false;
    if(c.section<=0||c.section>=10000)return false;
    if(u32(c.section)>=sizeof(practice_sections)/sizeof(*practice_sections))return false;
    return practice_sections[c.section].bgm!=0;
}
bool practice_invincible(const PracticeState& p){return cheat_active(p,1u);}
bool practice_infinite_lives(const PracticeState& p){return cheat_active(p,2u);}
bool practice_infinite_power(const PracticeState& p){return cheat_active(p,4u);}
bool practice_time_lock(const PracticeState& p){return cheat_active(p,8u);}
bool practice_auto_bomb(const PracticeState& p){return cheat_active(p,16u);}
bool practice_no_faith_loss(const PracticeState& p){return cheat_active(p,32u);}
bool apply_practice(browser::World& world,browser::GameState& state){
    auto& practice=state.practice;
    if(!practice.active||practice.run.mode!=1)return true;
    const auto& p=practice.run;
    if(!p.valid())return false;
    // Practice data belongs to its selected stage. If the game advances after
    // completing it, continue normally instead of applying stale script edits.
    // p.stage is zero-based; GameEconomy::stage is one-based (Extra is 7).
    if(u32(p.stage+1)!=state.game.stage){practice.active=false;return true;}
    auto& game=state.game;
    // thprac_th10.cpp:2331-2361, one-shot writes.
    game.score=i32(p.score/10);
    game.lives=p.life;
    game.power=static_cast<std::int16_t>(p.power);
    game.reserved_power=0;
    game.item_value=p.faith/10;
    if(p.faith_bar){
        game.faith_timer.previous=p.faith_bar-1;
        game.faith_timer.current=p.faith_bar;
        game.faith_timer.fractional=static_cast<float>(p.faith_bar);
    }
    if(game.difficulty==4){
        if(p.score>=100000000)game.rank=2;
        else if(p.score>=30000000)game.rank=1;
    }else{
        if(p.score>=150000000)game.rank=4;
        else if(p.score>=80000000)game.rank=3;
        else if(p.score>=40000000)game.rank=2;
        else if(p.score>=20000000)game.rank=1;
    }
    practice.real_bullet_sprite=p.real_bullet_sprite!=0;
    PracticePatcher patcher(world,state);
    if(!patcher.apply())return false;
    return true;
}
void update_practice(browser::World& world,browser::GameState& state){
    auto& practice=state.practice;
    if(!practice.enabled||practice.replay||!practice.cheats)return;
    practice.assisted=true;
    // F5 auto-bomb (upstream PATCH_HK(0x425C13, "c6")): press bomb while the
    // deathbomb window is open. Player state 4 is the pre-death window; state 2
    // is the bomb. The original patch forces the branch that consumes the bomb.
    if(practice_auto_bomb(practice)){
        auto* player=world.actors.player;
        if(player&&player->state==4&&player->state_timer.current<8)
            world.input.player_profiles[0].input.current|=2;
    }
}
}
