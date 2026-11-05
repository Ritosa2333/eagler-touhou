#pragma once
#include "PlayerFrame.hpp"
#include "ScreenDeformation.hpp"
#ifdef TH11_MULTIPLAYER
#include "ScreenDeformationPool.hpp"
#endif
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
    virtual void bomb_invincibility(i32){}
};
struct BombState {
    Timer elapsed,secondary;
    i32 active=0;u32 animation=0,auxiliary=0;
    Vec3 position{};float radius=0,growth=0;i32 started_during_spell=0;
};
// Original 406510/406620. Unsupported character branches fail explicitly until
// their original geometry/damage callbacks have also been reconstructed.
class BombController {
public:
    BombState state;
    i32 selection=0,last_error=0;
    BombController(AnmManager&,AnmResource&,PlayerFrame&,BombWorld&,u16 file=7,AnmResource* text=nullptr);
    ~BombController();
    bool supported()const noexcept{return selection>=0&&selection<6&&((selection!=2&&selection!=3)||text_resource);}
    const ScreenDeformation* distortion()const noexcept{
#ifdef TH11_MULTIPLAYER
        return mesh;
#else
        return mesh.get();
#endif
    }
    i32 start();
    bool update();
    bool damage(const Vec3&,const Vec2&,i32&);
#ifdef TH11_MULTIPLAYER
    void multiplayer_clear();
    bool BeginRollbackFrame(Netplay::RollbackJournal&,DeformationRollbackFrame&);
    bool RollbackFrameActive()const noexcept{return deformation_pool.RollbackFrameActive();}
    void EndRollbackFrame()noexcept{deformation_pool.EndRollbackFrame();}
    bool RestoreRollbackFrame(const DeformationRollbackFrame& frame,u32 restored_frame){return deformation_pool.RestoreRollbackFrame(frame,restored_frame);}
#endif
private:
    friend struct WorldRollbackTestAccess;
    AnmManager& animations;AnmResource& resource;PlayerFrame& player;BombWorld& world;u16 file_id;
    AnmResource* text_resource;
#ifdef TH11_MULTIPLAYER
    ScreenDeformationPool deformation_pool;
    ScreenDeformation* mesh=nullptr;
#else
    std::unique_ptr<ScreenDeformation> mesh;
#endif
    void clear_distortion();
    bool initialize_distortion(u32 width,u32 height,bool second_target=false);
    bool fail_spell();bool sound(i32,bool=false);bool create(u32&,i32,bool);
    bool body(i32);void invincible(i32);void interrupt(u32,i16);
    AnmVm* child(i32);Vec3 game_position(const AnmVm&)const;
    static int ring_noise(AnmVm&);
    bool create_distortion();void update_distortion();
    i32 tick();
};
}
