#pragma once
#include "SpellStatus.hpp"
#include "SpellRecords.hpp"
#include "EnemySceneCommands.hpp"
#include "SpellCaptureClock.hpp"
namespace th17 {
struct SpellBackground {i32 resource=-1,script=-1;bool independent=false;i32 overlay_resource=-1,overlay_script=-1;};
struct SpellVisualResources {i32 ascii=-1,name=-1,effect=-1;std::vector<SpellBackground> backgrounds;};
struct SpellStartContext {i32 stage=1,difficulty=1,character=0,animal=0;bool replay=false,practice=false;SpellVisualResources visuals;};
struct SpellFrameContext {float rate=1,player_y=0;i32 bomb_state=0;};
struct SpellCardHost {
    virtual ~SpellCardHost()=default;
    virtual bool background_visible(bool)=0;virtual bool prepare_hud(bool)=0;
    virtual u32 create_visual(i32 resource,i32 script)=0;
    virtual bool interrupt(u32 handle,i32 label)=0;virtual bool retire(u32& handle)=0;
    virtual bool text(u32 handle,const std::string&)=0;
    virtual bool child_integer(u32 handle,i32 script,i32 slot,i32 value)=0;
    virtual bool position(u32 handle,const Vec3&)=0;virtual bool boss_position(Vec3&)=0;
    virtual bool result(i32 bonus,bool failed)=0;virtual bool sound(i32,bool queued)=0;
    virtual bool trophy(i32)=0;
};
class SpellCard {
    PlayerSpellStatus& status;SpellCardHost& host;SpellRecords& records;i32& score;u32& roaring_flags;
    bool check(bool,const char*);bool banners(i32);
public:
    Timer age;i32 maximum_bonus=0,duration=0,active_frames=0,loadout=0;
    SpellCaptureClock result_clock;
    std::string name,error;Vec3 anchor{};std::array<u32,5> handles{};bool replay=false,practice=false;
    SpellCard(PlayerSpellStatus& state,i32& score,SpellRecords& records,u32& roaring,SpellCardHost& services):status(state),host(services),records(records),score(score),roaring_flags(roaring){age.set(0);}
    bool begin(const SpellStartRequest&,const SpellStartContext&);bool update(const SpellFrameContext&);bool finish();
    PlayerSpellStatus& state()noexcept{return status;}
};
}
