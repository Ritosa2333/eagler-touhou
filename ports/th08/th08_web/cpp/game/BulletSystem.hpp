#pragma once
#include "EclVm.hpp"
#include "BulletUpdate.hpp"
#include "LaserRuntime.hpp"
#include "BulletCancel.hpp"
#include "BulletDrawing.hpp"
#include "PlayerSimulation.hpp"
#include "ItemSystem.hpp"
#include "EffectSystem.hpp"
namespace th08 {
struct BulletSystemAudio {
    virtual ~BulletSystemAudio()=default;
    virtual void sound(i32 index,float position,bool panned)=0;
};
// One manager owns the original item -> bullet -> laser update order and
// item -> laser -> layered bullet -> effect drawing order. The enemy emitter,
// native callbacks and player collisions all reference this same pool.
class BulletSystem:public BulletEmissionActions,public LaserEmissionActions,private BulletCreationActions,private BulletUpdateActions,private LaserActions,private BulletCancelActions,private BulletDrawingActions {
    BulletManagerState& state;EclGlobals& globals;PlayerSimulation& player;ItemSystem& inventory;EffectSystem& effect_system;AnmRenderer& renderer;BulletSystemAudio& audio;
    BulletCreation creation;BulletUpdate updater;LaserRuntime lasers;BulletDrawing drawing;
    Vec2 arcade{32,16};bool failed=false,ready=false,presentation_prepared=false;
    void synchronize();void publish_collision();
    void sound(i32 index,float position,bool panned)override{audio.sound(index,position,panned);}
    void reemit(BulletEmission& emission)override{emit(emission);}
    i32 collision(i32 kind,BulletState&)override;
    void collision(const Vec2&,const Vec2&,const Vec3&,float,bool)override;
    i32 barrier(BulletState& bullet)override{return collision(0,bullet);}
    void item(const Vec3& p,i32 kind)override{drop(p,kind,1);}
    void drop(const Vec3& p,i32 kind,i32 mode)override{inventory.spawn(p,kind,mode);failed|=inventory.invalid();}
    void tint(u32 color)override{renderer.mix_color=color;renderer.mix_enabled=true;}
    void clear_tint()override{renderer.mix_color=0x80808080;renderer.mix_enabled=false;}
    void items()override{failed|=!inventory.draw(arcade);}
    void effects()override{effect_system.draw_alternative();}
    void draw(AnmVm& vm)override{renderer.draw_2d(vm);}
public:
    BulletSystem(BulletManagerState&,EclGlobals&,Rng&,PlayerSimulation&,ItemSystem&,EffectSystem&,AnmRenderer&,BulletSystemAudio&);
    ~BulletSystem(){if(globals.bullet_actions==this)globals.bullet_actions=nullptr;if(globals.laser_actions==this)globals.laser_actions=nullptr;}
    bool initialize(AnmLoaded&,Rng&);
    void emit(BulletEmission&)override;
    LaserState* laser(BulletEmission&)override;
    void clear(i32 mode)override;
    bool update();
    void snapshot_presentation(){if(ready){drawing.snapshot();presentation_prepared=true;}}
    bool draw(const Vec2& origin={32,16});
    bool invalid()const{return failed;}
};
}
