#pragma once
#include "AnmLayout.hpp"
namespace th08 {
enum class PlayerOptionKind:u32 {None,Yukari,Alice,RemiliaTeam,RemiliaSolo,YoumuTeam,YoumuSolo,Yuyuko};
struct PlayerOption {
    AnmVm animation;Vec3 position,center,velocity;
    i32 state=0,pose=0,index=0,reserved2d4=0;float angle=0,shot_angle=0;
    Timer timer;PlayerOptionKind update=PlayerOptionKind::None;u32 draw=0;
};
static_assert(sizeof(PlayerOption)==0x2f4&&offsetof(PlayerOption,state)==0x2c8&&offsetof(PlayerOption,timer)==0x2e0);
struct PlayerOptionContext {
    Vec3 player,history_last,enemy;Timer shooting_timer;i32 direction=0,bomb=0;u16 buttons=0;u8 focused=0,enemy_present=0;
};
struct PlayerOptionActions {
    virtual ~PlayerOptionActions()=default;
    virtual void animation(AnmVm&,i32 script)=0;
    virtual void effect(i32 type,const Vec3& position,u32 color)=0;
    virtual void draw(AnmVm&)=0;
};
class PlayerOptions {
    PlayerOptionContext& context;
    void approach_yukari(PlayerOption&,bool enemy);
    void turn_youmu(PlayerOption&);
public:
    explicit PlayerOptions(PlayerOptionContext& context):context(context){}
    PlayerOptionActions* actions=nullptr;
    static void initialize(PlayerOption* slots,u8 character);
    void update(PlayerOption&,PlayerOptionKind);
    void draw(PlayerOption&,const Vec2& offset);
};
}
