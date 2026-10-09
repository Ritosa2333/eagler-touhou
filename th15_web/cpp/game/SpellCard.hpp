#pragma once
#include "PlayerLife.hpp"
#include "SpellPresentationClock.hpp"
#include <array>
#include <string>
namespace th15 {
struct SpellStartRequest {i32 id=0,duration=0,requested_bonus=0;std::string name;bool survival=false;};
struct SpellBackground {i32 resource=-1,script=-1;bool independent=false;i32 overlay_resource=-1,overlay_script=-1;};
struct SpellVisualResources {i32 ascii=-1,name=-1,effect=-1;std::array<SpellBackground,3> backgrounds{};bool secondary=false;};
struct SpellStartContext {i32 stage=0,difficulty=0,character=0,bomb_state=0;bool replay=false;SpellVisualResources visuals;};
struct SpellFrameContext {float rate=1,player_y=0;i32 bomb_state=0;bool lock_time=false;};
// Game presentation and records are explicit services. The native graphics
// layout and address-based dispatch never enter the spell's game logic.
struct SpellCardHost {
    virtual ~SpellCardHost()=default;
    virtual bool background_visible(bool)=0;
    virtual bool prepare_hud(bool)=0;
    virtual u32 create_visual(i32 resource,i32 script)=0;
    virtual bool interrupt(u32 handle,i32 label)=0;
    virtual bool retire(u32& handle)=0;
    virtual bool text(u32 handle,const std::string&)=0;
    virtual bool child_integer(u32 handle,i32 script,i32 slot,i32 value)=0;
    virtual bool position(u32 handle,const Vec3&)=0;
    virtual bool boss_position(Vec3&)=0;
    virtual bool history_begin(i32,const std::string&)=0;
    virtual bool history_capture(i32)=0;
    virtual bool result(i32 bonus,bool failed)=0;
    virtual bool sound(i32)=0;
};
class SpellCard {
    PlayerSpellStatus& status;SpellCardHost& host;i32& score;
    bool check(bool,const char*);bool banners(i32);
public:
    SpellPresentationClock clock;Timer age;i32 identifier=0,maximum_bonus=0,duration=0,active_frames=0;
    std::string name,error;Vec3 anchor{};std::array<u32,5> handles{};bool replay=false;
    SpellCard(PlayerSpellStatus& state,i32& score,SpellCardHost& services):status(state),host(services),score(score){age.set(0);}
    bool begin(const SpellStartRequest&,const SpellStartContext&);
    bool update(const SpellFrameContext&);
    bool finish();bool abort();
    PlayerSpellStatus& state()noexcept{return status;}
};
}
