#include "BulletScene.hpp"
#include "BulletArea.hpp"
namespace th15 {
struct BulletScene::Slot final:BulletFrameHost,BulletContactHost {
    BulletScene& scene;u32 index;BulletState& state;BulletVisual visual;bool cancelling=false;
    Slot(BulletScene& owner,u32 slot,BulletState& bullet):scene(owner),index(slot),state(bullet),visual(bullet,owner.visual_random){}
    void play(i32 id)override{scene.host.sound(id);}
    void hit()override{scene.host.hit();}
    bool interrupt(i32 id)override{return visual.interrupt(id);}
    bool bind_appearance(i32 script,i32 overlay)override{
        visual.resource=scene.resource;visual.body.environment=scene.environment;visual.body.object_host=scene.animation_objects;
        const bool result=visual.bind(script,overlay);if(!result)scene.error=visual.error;return result;
    }
    bool spawn_animation()override{return visual.spawn();}
    bool hit_animation()override{return visual.hit();}
    bool overlay_interrupt(i32 id)override{return visual.interrupt_overlay(id);}
    bool emit_children(const BulletShooter& shooter)override{return scene.manager.emit(shooter,scene.manager.minimum_distance_squared);}
    bool spawn_enemy(const EnemySpawnRequest& request)override{return scene.host.spawn_enemy(request);}
    bool create_laser(const MovingLaserRequest& request)override{return scene.host.create_laser(request);}
    bool create_laser(const StationaryLaserRequest& request)override{return scene.host.create_laser(request);}
    bool cancel_bullet(i32 kind)override{cancelling=true;const bool result=state.cancel(kind,scene.manager.context.rate,scene.game_random,scene.manager.cancellation_rewards,*this,*this,scene.error);cancelling=false;return result&&scene.error.empty();}
    i32 contact(BulletState& bullet,bool graze_only)override{return i32(bullet.collide(scene.host.player_collision(),graze_only,scene.manager.context.rate,scene.game_random,scene.visual_random,*this,scene.error));}
    bool sprite_dimensions(Vec2& out)const noexcept override{const auto* sprite=visual.body.sprite;if(!sprite)return false;out={sprite->width,sprite->height};return true;}
    bool animation_finished(bool overlay)override{const i32 result=visual.tick(overlay);if(result<0)scene.error=visual.error;return result>0;}
    void cancellation_effect(i32 script,const Vec3& position,const Vec3& velocity)override{
        if(!scene.cancellation_owner){scene.host.cancellation_effect(script,position,velocity);return;}
        auto& owner=*scene.cancellation_owner;owner.rate=scene.manager.context.rate;
        const u32 handle=owner.create(scene.cancellation_bank,script,-1,0,position);
        if(!handle){scene.error=owner.error;return;}
        if(cancelling)scene.cancellation_animations[index]=handle;
        else if(auto* vm=owner.registry.find(handle))vm->interpolators.position.begin(30,6,{0,0,0},{velocity.x,velocity.y,velocity.z});
    }
    void graze_spark(const Vec3& position)override{scene.host.graze_spark(position);}
    void graze()override{scene.host.graze();}
    void graze_resonance(float value)override{scene.host.graze_resonance(value);}
    void item(i32 type,const Vec3& position,float angle,float speed)override{scene.host.item(type,position,angle,speed);}
    void recycle()override{scene.manager.recycle(index);}
};
BulletScene::BulletScene(BulletSceneHost& services,Rng& game,Rng& visual):host(services),game_random(game),visual_random(visual),manager(*this){
    manager.random=&game_random;slots.reserve(BulletManager::capacity);for(u32 i=0;i<BulletManager::capacity;i++)slots.push_back(std::make_unique<Slot>(*this,i,*manager.state(i)));
}
BulletScene::~BulletScene()=default;
BulletFrameHost& BulletScene::bullet(u32 index){auto& slot=*slots[index];slot.visual.rate=manager.context.rate;return slot;}
void BulletScene::prepare(u32 index,BulletFrameContext& context){const auto& vm=slots[index]->visual.body;context.sprite_available=vm.sprite!=nullptr;
    // Native 0x419390 reads the selected resource sprite dimensions, even when
    // the animation changes its visual size. Recycling against the latter
    // removes shrinking/cancelling bullets one or more frames too early.
    context.sprite_size=vm.sprite?Vec2{vm.sprite->width,vm.sprite->height}:Vec2{};}
void BulletScene::play(i32 id){host.sound(id);}
BulletVisual* BulletScene::visual(u32 index)noexcept{return index<slots.size()?&slots[index]->visual:nullptr;}
void BulletScene::reset(){manager.reset();cancellation_animations.fill(0);error.clear();for(auto& slot:slots){if(!slot->visual.clear_children()){error=slot->visual.error;return;}slot->visual.body=AnmVm{};slot->visual.overlay=AnmVm{};slot->visual.error.clear();}}
bool BulletScene::emit(const BulletShooter& shooter,float minimum){error.clear();const bool result=manager.emit(shooter,minimum);if(!result&&error.empty())error=manager.error;return result&&error.empty();}
bool BulletScene::update(){error.clear();const bool result=manager.update();if(!result&&error.empty())error=manager.error;return result&&error.empty();}
bool BulletScene::cancel_circle(const Vec3& p,float radius,i32 reward,bool honor){
    error.clear();for(u32 index=manager.first_active();index!=BulletManager::none;){const u32 next=manager.next_active(index);auto& slot=*slots[index];if(bullet_circle_area(slot.state,p,radius,honor)){slot.visual.rate=manager.context.rate;if(!slot.cancel_bullet(reward))return false;}index=next;}return error.empty();
}
bool BulletScene::cancel_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward){
    error.clear();for(auto& entry:slots){auto& slot=*entry;if(bullet_rectangle_area(slot.state,p,size,angle)){slot.visual.rate=manager.context.rate;if(!slot.cancel_bullet(reward))return false;}}return error.empty();
}
bool BulletScene::cancel_all(i32 reward){
    // Unlike area cancellation, the original scans storage order and also
    // processes bullets already playing their cancellation animation.
    error.clear();for(auto& entry:slots){auto& slot=*entry;if(slot.state.phase!=0&&slot.state.phase!=3){slot.visual.rate=manager.context.rate;if(!slot.cancel_bullet(reward))return false;}}return error.empty();
}
bool BulletScene::cancel_slot(u32 index,i32 reward){error.clear();if(index>=slots.size()){error="Bullet cancellation slot outside pool";return false;}auto& slot=*slots[index];slot.visual.rate=manager.context.rate;return slot.cancel_bullet(reward)&&error.empty();}
}
