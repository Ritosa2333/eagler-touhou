#pragma once
#include "PlayerFrame.hpp"
#include "ScreenDeformation.hpp"
#include "BombReimuTh14.hpp"
namespace th11 {
struct BombWorld {
    virtual ~BombWorld()=default;
    virtual bool bomb_sound(i32,float,bool)=0;
    virtual bool bomb_stop_sound(i32)=0;
    virtual bool bomb_cancel(Vec3,float,u32,bool)=0;
    virtual i32& bomb_count()=0;
    virtual bool bomb_background_color(u32)=0;
    virtual bool bomb_refund_power()=0;
    virtual bool bomb_cancel_beam(Vec3,bool)=0;
    virtual bool th14_bomb_freeze(bool){return false;}
    virtual bool th14_bomb_cancel(Vec3,float,u32){return false;}
    virtual bool th14_bomb_dialogue()const{return false;}
    virtual bool th14_bomb_circle(Vec3,float,float,i32,i32){return false;}
    virtual bool th14_bomb_item(i32,Vec3,float,float){return false;}
    virtual bool th14_bomb_rectangle(Vec3,Vec2,float,i32,i32){return false;}
    virtual bool th14_bomb_cancel_rectangle(Vec3,Vec2,float,u32){return false;}
    virtual bool th14_reimu_cancel_rectangle(Vec3 p,Vec2 size,float angle,u32 rewards){return th14_bomb_cancel_rectangle(p,size,angle,rewards);}
    virtual bool th14_bomb_shake(i32,i32,i32,i32){return false;}
    virtual bool th14_bomb_shake_short(i32,i32,i32){return false;}
    virtual EnemyLink* th14_bomb_enemies()const{return nullptr;}
};
struct BombState {
    Timer elapsed,secondary;
    i32 active=0;u32 animation=0,auxiliary=0;
    Vec3 position{};float radius=0,growth=0;i32 started_during_spell=0;
    i32 th14_mode=0;
};
// Original 406510/406620. Unsupported character branches fail explicitly until
// their original geometry/damage callbacks have also been reconstructed.
class BombController {
public:
    BombState state;
    i32 selection=0,last_error=0;
#if PORT_TITLE == 14
    std::array<Th14ReimuOrb,8> th14_orbs{};
    bool th14_orbs_allocated=false;
    std::array<u32,4> th14_reimu_areas{{256,256,256,256}};
    std::array<Vec3,10> th14_reimu_positions{};
    std::array<float,10> th14_reimu_angles{};
    FloatInterpolator th14_reimu_shift{};
    bool th14_reimu_allocated=false;
    void invalidate_target(Enemy* e)noexcept{for(auto& o:th14_orbs)if(o.target==e)o.target=nullptr;}
#endif
    // What the ANM manager reported the last time a Bomb animation could not be created.
    i32 create_script=-1,create_opcode=0;u32 create_depth=0,create_scripts=0;
    BombController(AnmManager&,AnmResource&,PlayerFrame&,BombWorld&,u16 file=7,AnmResource* text=nullptr);
    ~BombController();
    bool supported()const noexcept{return selection>=0&&selection<6&&((selection!=2&&selection!=3)||text_resource);}
    const ScreenDeformation* distortion()const noexcept{return mesh.get();}
    i32 start();
    bool update();
    bool damage(const Vec3&,const Vec2&,i32&);
private:
    AnmManager& animations;AnmResource& resource;PlayerFrame& player;BombWorld& world;u16 file_id;
    AnmResource* text_resource;std::unique_ptr<ScreenDeformation> mesh;
    bool fail_spell();bool sound(i32,bool=false);bool create(u32&,i32,bool);
    bool body(i32);void invincible(i32);void interrupt(u32,i16);
    AnmVm* child(i32);Vec3 game_position(const AnmVm&)const;
    Vec3 th14_raw_position(const AnmVm&)const;
    static int ring_noise(AnmVm&);
    bool create_distortion();void update_distortion();
    i32 tick();
#if PORT_TITLE == 14
    bool th14_orb_explode(Th14ReimuOrb&);
#endif
};
}
