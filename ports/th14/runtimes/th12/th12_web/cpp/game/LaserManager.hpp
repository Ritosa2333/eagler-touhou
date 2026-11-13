#pragma once
#include "LaserState.hpp"
namespace th11 {
class LaserManager final:public LaserWorld {
public:
    static constexpr u32 capacity=kPortTitle==16?512:256;
    LaserManager(AnmResource&,AnmEnvironment&,LaserWorld&,u16 file_id=6);
    ~LaserManager();
    LaserManager(const LaserManager&)=delete;LaserManager& operator=(const LaserManager&)=delete;
    LaserState* first()const noexcept{return sentinel.next;}
    LaserState* last()const noexcept{return tail==&sentinel?nullptr:tail;}
    u32 active_count=0;i32 next_id=0x10000,last_error=0;Vec3 last_center{},last_size{};
    i32 spawn(const LaserLineParameters&);
    i32 spawn(const LaserInfiniteParameters&);
#if PORT_TITLE == 16
    i32 spawn_th16_line(const u8* request);
    bool reset_th16();
    bool th16_line_appearance(Th16LaserLine&,i32)override;
    i32 th16_line_cut(Th16LaserLine&,Vec3,Vec2,float,u32,bool)override;
    bool th16_line_graze(Vec3 p)override{return world.th16_line_graze(p);}
    bool th16_line_reward(Vec3 p,u32 rewards)override{return world.th16_line_reward(p,rewards);}
    bool th16_line_cut_effect(Vec3 p,i32 script)override{return world.th16_line_cut_effect(p,script);}
#endif
#if PORT_TITLE == 14
    i32 spawn_th14_line(const u8* request);
    i32 spawn_th14_infinite(const u8* request);
    i32 spawn_th14_shell(const u8* request);
    i32 spawn_th14_curve(const u8* request,const std::vector<Th14CurveNode>* inherited=nullptr);
    bool reset_th14();
    bool freeze_th14(bool);
#endif
    bool update(bool paused=false,bool frozen=false);
    bool draw(AnmRenderer&,bool paused=false);
    bool update_warning(float angular_delta);
    LaserState* find(i32 id)const noexcept;
    bool cancel_id(i32 id);
    // The type-dispatched virtual cancel. TH12's 0x262 calls the object's vtable slot directly with
    // (0,0) and clears the id itself, while cancel_id's loop passes (true,false); keeping the object
    // form separate is what lets both be transcribed without one borrowing the other's flags.
    bool cancel_object(LaserState& b,bool reward,bool skip);
    void mark_all()noexcept;
    // Linking an object into the list, the operation the original's spawn function does at 0x41bb40 after it
    // allocates. It is public because spawn needs the appearance resources to finish and the command tests only
    // need an object in the list with an id, which is what the original side fabricates as well.
    void append(LaserState&)noexcept;
    void clear()noexcept;
    bool cancel_all(bool reward,bool skip_protected);
    i32 cancel_rectangle(Vec3 center,Vec3 size,bool reward);
#if PORT_TITLE == 14
    i32 cancel_rectangle_th14(Vec3,Vec2,float,u32,bool skip_infinite=false);
#endif
    i32 cancel_circle(Vec3 center,float radius,u32 rewards,bool skip_protected);
    bool sound(i32 id,float x,bool positional)override{return world.sound(id,x,positional);}
    bool change_appearance(LaserLine&,i32)override;
    bool spawn_line(const LaserLineParameters&)override;
    bool cancel_effect(Vec3 p,i32 script)override{return world.cancel_effect(p,script);}
    bool cancel_reward(Vec3 p)override{return world.cancel_reward(p);}
    bool cancel_shot(Vec3 p,float angle)override{return world.cancel_shot(p,angle);}
    i32 collision(Vec3 p,float angle,float width,float length)override{return world.collision(p,angle,width,length);}
    i32 warning_collision(Vec3 p,float angle,float width,float length)override{return world.warning_collision(p,angle,width,length);}
    bool cut(LaserLine&,Vec3,Vec3,bool,bool)override;
    bool cut_infinite(LaserInfinite&,Vec3,Vec3,bool,bool)override;
    bool graze_effect(Vec3 p)override{return world.graze_effect(p);}
    bool graze_reward()override{return world.graze_reward();}
    bool graze_register()override{return world.graze_register();}
    bool boss_position(Vec3& p)override{return world.boss_position(p);}
#if PORT_TITLE == 14
    bool th14_curve_reward(Vec3 p,u32 rewards)override{return world.th14_curve_reward(p,rewards);}
    bool th14_curve_graze(Vec3 p)override{return world.th14_curve_graze(p);}
    i32 th14_line_cut(Th14LaserLine&,Vec3,Vec2,float,u32,bool)override;
    i32 th14_infinite_cut(Th14LaserInfinite&,Vec3,Vec2,float,u32,bool)override;
    i32 th14_curve_cut(Th14LaserCurve&,Vec3,Vec2,float,u32,bool)override;
    bool th14_line_spawn(const u8* request)override{spawn_th14_line(request);return !last_error;}
#endif
private:
    LaserState sentinel{};LaserState* tail=&sentinel;
    AnmResource& resource;AnmEnvironment& animations;LaserWorld& world;u16 file_id;
    bool updating=false;
    void sync()noexcept{rate=world.rate;player=world.player;}
    void release(LaserState&)noexcept;
    i32 issue_id()noexcept;
};
}
