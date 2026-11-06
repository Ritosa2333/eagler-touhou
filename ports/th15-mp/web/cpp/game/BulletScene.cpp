#include "BulletScene.hpp"
#include <algorithm>
#include "BulletArea.hpp"
#include "MultiplayerVisualPolicy.hpp"
namespace th15 {
struct BulletScene::Slot final:BulletFrameHost,BulletContactHost {
    BulletScene& scene;u32 index;BulletState& state;BulletVisual visual;bool cancelling=false;
    Slot(BulletScene& owner,u32 slot,BulletState& bullet):scene(owner),index(slot),state(bullet),visual(bullet,owner.visual_random){}
    void play(i32 id)override{scene.host.sound(id);}
    void hit()override{scene.host.hit();}
    bool interrupt(i32 id)override{return visual.interrupt(id);}
    bool bind_appearance(i32 script,i32 overlay)override{
        scene.manager.changed(index);
        graze_state.initialized=false;graze_state.views={};
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
    bool cancel_bullet(i32 kind)override{scene.manager.changed(index);cancelling=true;const bool result=state.cancel(kind,scene.manager.context.rate,scene.game_random,scene.manager.cancellation_rewards,*this,*this,scene.error);cancelling=false;return result&&scene.error.empty();}
    struct GrazeState {
        u64 version=0;u32 active=0,color=0;bool initialized=false,contacted=false;
        std::array<Timer,3> flash{},duration{};std::array<i32,3> interval{{3,3,3}};
        std::array<MultiplayerGrazeVisual,3> views{};
    } graze_state;
    static_assert(std::is_trivially_copyable_v<GrazeState>);
    bool changed_graze(u32 mask,u32 color,bool contacted,bool first){
        const u32 key=mask?color:0;
        if(!first&&!graze_state.contacted&&!contacted&&graze_state.active==mask&&graze_state.color==key)return false;
        graze_state.version=++scene.manager.snapshot_serial;graze_state.active=mask;graze_state.color=key;graze_state.contacted=contacted;graze_state.initialized=true;return true;
    }
    i32 contact(BulletState& bullet,bool graze_only)override{
        const unsigned count=scene.contact_players?scene.contact_players:scene.host.player_count();
        if(count==1)return i32(bullet.collide(scene.host.player_collision(),graze_only,scene.manager.context.rate,scene.game_random,scene.visual_random,*this,scene.error));
        const bool first=!graze_state.initialized;const u32 color=bullet.graze_color;
        if(first)for(unsigned s=0;s<count;++s){graze_state.flash[s]=bullet.graze_flash;graze_state.duration[s]=bullet.graze_duration;graze_state.interval[s]=bullet.graze_interval;}
        const auto candidates=scene.host.contact_candidates(bullet);
        if(!candidates.possible){
            unsigned last=candidates.last;
            if(changed_graze(candidates.active,color,false,first)){
                graze_state.views={};for(unsigned s=0;s<count;++s)if(candidates.active&(1u<<s))graze_state.views[s]={0,color,{}};
            }
            if(last>=count&&candidates.active){last=count;for(unsigned s=0;s<count;++s)if(candidates.active&(1u<<s))last=s;}
            if(last<count){bullet.visual_flags&=0xfff9ffffu;bullet.visual_jitter={};bullet.graze_flash=graze_state.flash[last];bullet.graze_duration=graze_state.duration[last];bullet.graze_interval=graze_state.interval[last];}
            scene.host.select_player(0);return 0;
        }
        graze_state.views={};MultiplayerGrazeVisual glow;bool hit_player=false;unsigned last_idle=count;u32 active=0;
        i32 result=0;for(unsigned s=0;s<count;++s){const auto* player=scene.host.contact_player(s);if(!player)continue;active|=1u<<s;
            if(bullet.contact(*player,graze_only)==PlayerContact::none){
                bullet.visual_flags&=0xfff9ffffu;bullet.visual_jitter={};graze_state.views[s]={0,bullet.graze_color,{}};last_idle=s;continue;
            }
            last_idle=count;
            bullet.graze_flash=graze_state.flash[s];bullet.graze_duration=graze_state.duration[s];bullet.graze_interval=graze_state.interval[s];
            const auto hit=bullet.collide(*player,graze_only,scene.manager.context.rate,scene.game_random,scene.visual_random,*this,scene.error);
            graze_state.flash[s]=bullet.graze_flash;graze_state.duration[s]=bullet.graze_duration;graze_state.interval[s]=bullet.graze_interval;
            graze_state.views[s]={bullet.visual_flags&0x60000u,bullet.graze_color,bullet.visual_jitter};glow.observe(bullet.visual_flags,bullet.graze_color,bullet.visual_jitter);
            result=std::max(result,i32(hit));if(hit==PlayerContact::hit){hit_player=true;break;}
        }
        if(last_idle<count){bullet.graze_flash=graze_state.flash[last_idle];bullet.graze_duration=graze_state.duration[last_idle];bullet.graze_interval=graze_state.interval[last_idle];}
        if(!hit_player)glow.apply(bullet.visual_flags,bullet.graze_color,bullet.visual_jitter);
        changed_graze(active,color,result!=0,first);scene.host.select_player(0);return result;
    }
    bool sprite_dimensions(Vec2& out)const noexcept override{const auto* sprite=visual.body.sprite;if(!sprite)return false;out={sprite->width,sprite->height};return true;}
    bool animation_finished(bool overlay)override{const i32 result=visual.tick(overlay,!scene.manager.snapshot_exposed[index]);if(result<0)scene.error=visual.error;return result>0;}
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
    manager.random=&game_random;slots.reserve(BulletManager::capacity);for(u32 i=0;i<BulletManager::capacity;i++)slots.push_back(std::make_unique<Slot>(*this,i,manager.slots[i].state));
}
BulletScene::~BulletScene()=default;
BulletFrameHost& BulletScene::bullet(u32 index){auto& slot=*slots[index];slot.visual.rate=manager.context.rate;return slot;}
void BulletScene::prepare(u32 index,BulletFrameContext& context){const auto& vm=slots[index]->visual.body;context.sprite_available=vm.sprite!=nullptr;
    const auto& state=slots[index]->state;
    const bool pending=state.transform_index>=0&&state.transform_index<18&&state.transforms[state.transform_index].type;
    if(manager.multiplayer_target&&(pending||(state.transform_flags&16)))context.player=manager.multiplayer_target(state.motion.position);
    // Native 0x419390 reads the selected resource sprite dimensions, even when
    // the animation changes its visual size. Recycling against the latter
    // removes shrinking/cancelling bullets one or more frames too early.
    context.sprite_size=vm.sprite?Vec2{vm.sprite->width,vm.sprite->height}:Vec2{};}
void BulletScene::play(i32 id){host.sound(id);}
BulletVisual* BulletScene::owned_visual(u32 index)noexcept{return index<slots.size()?&slots[index]->visual:nullptr;}
const BulletVisual* BulletScene::visual(u32 index)const noexcept{return index<slots.size()?&slots[index]->visual:nullptr;}
BulletVisual* BulletScene::visual(u32 index)noexcept{
    if(index>=slots.size())return nullptr;
    // A caller can hold this mutable owner across captures, including while its
    // slot is cold. Internal drawing and chapter writes use their tracked path.
    manager.snapshot_exposed.set(index);return owned_visual(index);
}
const MultiplayerGrazeVisual* BulletScene::viewpoint_graze(u32 index)const noexcept{return host.player_count()>1&&index<slots.size()?&slots[index]->graze_state.views[std::min(host.local_player(),2u)]:nullptr;}
void BulletScene::reset(){manager.reset();cancellation_animations.fill(0);error.clear();for(auto& slot:slots){if(!slot->visual.clear_children()){error=slot->visual.error;return;}slot->visual.body=AnmVm{};slot->visual.overlay=AnmVm{};slot->visual.error.clear();}}
bool BulletScene::emit(const BulletShooter& shooter,float minimum){error.clear();const bool result=manager.emit(shooter,minimum);if(!result&&error.empty())error=manager.error;return result&&error.empty();}
bool BulletScene::update(){
    error.clear();
    struct Contacts {BulletScene& scene;explicit Contacts(BulletScene& s):scene(s){scene.contact_players=scene.host.player_count();scene.host.contact_frame(scene.contact_players);}~Contacts(){scene.host.contact_frame(0);scene.contact_players=0;}} contacts(*this);
    const bool result=manager.update();if(!result&&error.empty())error=manager.error;return result&&error.empty();
}
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

#include "../multiplayer/BulletFrameStateImpl.hpp"
