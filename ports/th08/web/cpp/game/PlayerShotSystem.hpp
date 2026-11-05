#pragma once
#include "PlayerShots.hpp"
#include "ShotFiring.hpp"
#include "AnmLibrary.hpp"
#include "AnmRenderer.hpp"
namespace th08 {
struct PlayerShotRequest {i32 kind,value;Vec3 position;i32 mode;};
class PlayerShotSystem:private PlayerShotActions,private ShotFiringActions {
    AnmLibrary& animations;Rng& rng;AnmExecutor executor;
    std::unique_ptr<PlayerShotsState> state=std::make_unique<PlayerShotsState>();
    PlayerShots shots{*state,rng};ShotResource resources[2];bool loaded[2]{},animation_loaded=false,failed=false;
    std::vector<PlayerShotRequest> requests;AnmRenderer* renderer=nullptr;
    void animation(AnmVm& vm,i32 script)override{failed|=!animations.start(5,script,vm,executor);}
    bool step_animation(AnmVm& vm)override{const bool result=executor.execute(vm);failed|=executor.invalid;return result;}
    void sound(i32 index,float x)override{requests.push_back({1,index,{x,0,0},1});}
    void damage_region(const Vec3& position,const Vec2& size,i32 strength,i32 lifetime,bool laser)override{auto& area=state->regions.rectangle(true,{position.x,position.y},size.x,size.y,strength,lifetime);area.suppress_effect=laser;}
    void effect(i32 type,const Vec3& position)override{requests.push_back({2,type,position,1});}
    void item(i32 type,const Vec3& position,i32 mode)override{requests.push_back({3,type,position,mode});}
    void draw(AnmVm& vm,bool impact)override{if(renderer){if(impact)renderer->draw_player_bullet(vm);else renderer->draw_2d(vm);}else failed=true;}
    void fire(i32 frame)override;
public:
    PlayerShotSystem(AnmLibrary& animations,Rng& rng):animations(animations),rng(rng),executor(rng){shots.actions=this;state->shooting_timer.set(-1);}
    FrameTiming timing;ShotFiringInputs firing;i32 power=0;bool bomb_ready=false;
    bool load_shots(bool focused,const u8* data,u32 size);
    bool load_animation(const u8* data,u32 size);
    void reset();
    bool ready()const noexcept{return loaded[0]&&loaded[1]&&animation_loaded;}
    bool update();
    bool emit(i32 frame);
    i32 damage(const Vec3& position,const Vec3& size,i32& time_items,i32* bomb_hit);
    bool draw(AnmRenderer& renderer,bool impact,const Vec2& offset);
    bool invalid()const noexcept{return failed;}
    PlayerShotsState& status()noexcept{return *state;}
    const ShotProfile& profile(bool focused)const noexcept{return resources[focused].settings();}
    const std::vector<PlayerShotRequest>& pending_requests()const noexcept{return requests;}
    void clear_requests(){requests.clear();}
};
}
