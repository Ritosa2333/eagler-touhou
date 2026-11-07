#include "BulletScene.hpp"
#include "BulletArea.hpp"
#if TH17_MULTIPLAYER
#include "../multiplayer/FrameImage.hpp"
#include "../multiplayer/AnmCheckpoint.hpp"
#include <bitset>
#endif
namespace th17 {
struct BulletScene::Slot final:BulletFrameHost,BulletContactHost {
    BulletScene& scene;u32 index;BulletState& state;BulletVisual visual;bool cancelling=false;
#if TH17_MULTIPLAYER
    struct VisualFields {
        AnmResource* resource;
        AnmManager* manager;
        u32 overlay_handle;
        float rate;
        std::string error;
        bool cancelling;

        explicit VisualFields(const Slot& slot):resource(slot.visual.resource),
            manager(slot.visual.manager),overlay_handle(slot.visual.overlay_handle),rate(slot.visual.rate),
            error(slot.visual.error),cancelling(slot.cancelling){}
        void restore(Slot& slot)const{
            auto& v=slot.visual;
            v.resource=resource;v.manager=manager;v.overlay_handle=overlay_handle;
            v.rate=rate;v.error=error;slot.cancelling=cancelling;slot.visual_touched=true;
        }
    };
    struct VisualImage {
        AnmVm body;
        VisualFields fields;
        explicit VisualImage(const Slot& slot):body(slot.visual.body),fields(slot){}
        void restore(Slot& slot)const{slot.visual.body=body;fields.restore(slot);}
    };
    std::weak_ptr<const VisualImage> idle_image;
    multiplayer::AnmCheckpoint::Cache rollback_curves;
    bool visual_touched=false;
#endif
    void touch_visual(){
#if TH17_MULTIPLAYER
        idle_image.reset();
        visual_touched=true;
#endif
    }
    Slot(BulletScene& owner,u32 slot,BulletState& bullet):scene(owner),index(slot),state(bullet),visual(bullet,owner.visual_random){}
    void play(i32 id)override{scene.host.sound(id);}
    void hit()override{scene.host.hit();}
    bool interrupt(i32 id)override{return visual.interrupt(id);}
    bool bind_appearance(i32 script,i32 overlay)override{
        visual.resource=scene.resource;visual.manager=scene.cancellation_owner;visual.body.environment=scene.environment;visual.body.object_host=scene.animation_objects;
        const bool result=visual.bind(script,overlay);if(!result)scene.error=visual.error;return result;
    }
    bool spawn_animation()override{return visual.spawn();}
    bool hit_animation()override{return visual.hit();}
    bool overlay_interrupt(i32 id)override{return visual.interrupt_overlay(id);}
    bool emit_children(const BulletShooter& shooter)override{return scene.manager.emit(shooter,scene.manager.minimum_distance_squared);}
    bool spawn_enemy(const EnemySpawnRequest& request)override{return scene.host.spawn_enemy(request);}
    bool create_laser(const MovingLaserRequest& request)override{return scene.host.create_laser(request);}
    bool create_laser(const StationaryLaserRequest& request)override{return scene.host.create_laser(request);}
    bool cancel_bullet(i32 kind)override{touch_visual();cancelling=true;const bool result=state.cancel(kind,scene.manager.context.rate,scene.game_random,scene.manager.cancellation_rewards,*this,*this,scene.error);cancelling=false;return result&&scene.error.empty();}
    i32 contact(BulletState& bullet,bool graze_only)override{
        if(scene.host.contact_players()==1)
            return i32(bullet.collide(scene.host.player_collision(),graze_only,scene.manager.context.rate,scene.game_random,scene.visual_random,*this,scene.error));
        auto result=PlayerContact::none;
        for(u32 seat=0;seat<scene.host.contact_players();++seat){
            const auto* player=scene.host.select_contact(seat);
            if(!player)continue;
            const auto contact=bullet.collide(*player,graze_only,scene.manager.context.rate,scene.game_random,scene.visual_random,*this,scene.error);
            if(contact==PlayerContact::hit){result=contact;break;}
            if(contact==PlayerContact::graze)result=contact;
            if(!scene.error.empty())break;
        }
        scene.host.finish_contacts();
        return i32(result);
    }
    bool sprite_dimensions(Vec2& out)const noexcept override{const auto* sprite=visual.body.sprite;if(!sprite)return false;out={sprite->width,sprite->height};return true;}
    bool animation_finished(bool overlay)override{const i32 result=visual.tick(overlay);if(result<0)scene.error=visual.error;return result>0;}
    void cancellation_effect(i32 script,const Vec3& position,const Vec3& velocity)override{
        if(!scene.cancellation_owner){scene.host.cancellation_effect(script,position,velocity);return;}
        auto& owner=*scene.cancellation_owner;owner.rate=scene.manager.context.rate;
        const u32 handle=owner.create(scene.cancellation_bank,script,-1,0,position);
        if(!handle){scene.error=owner.error;return;}
        if(cancelling)scene.cancellation_animations[index]=handle;
        else if(auto* vm=owner.registry.find(handle)){vm->interpolators.invalidate();vm->interpolators.position.begin(30,6,{0,0,0},{velocity.x,velocity.y,velocity.z});}
    }
    void graze_spark(const Vec3& position)override{scene.host.graze_spark(position);}
    void graze()override{scene.manager.graze_history[0]=wrapping_add(scene.manager.graze_history[0],1);scene.host.graze();}
    void graze_resonance(float value)override{scene.host.graze_resonance(value);}
    void item(i32 type,const Vec3& position,float angle,float speed)override{scene.host.item(type,position,angle,speed);}
    void recycle()override{scene.manager.recycle(index);}
};
BulletScene::BulletScene(BulletSceneHost& services,Rng& game,Rng& visual):host(services),game_random(game),visual_random(visual),manager(*this){
    manager.random=&game_random;slots.reserve(BulletManager::capacity);for(u32 i=0;i<BulletManager::capacity;i++)slots.push_back(std::make_unique<Slot>(*this,i,*manager.state(i)));
}
BulletScene::~BulletScene()=default;
BulletFrameHost& BulletScene::bullet(u32 index){auto& slot=*slots[index];slot.touch_visual();slot.visual.rate=manager.context.rate;return slot;}
void BulletScene::prepare(u32 index,BulletFrameContext& context){const auto& vm=slots[index]->visual.body;context.sprite_available=vm.sprite!=nullptr;
    // Native 0x414ae0 reads the selected resource sprite dimensions, even when
    // the animation changes its visual size. Recycling against the latter
    // removes shrinking/cancelling bullets one or more frames too early.
    context.sprite_size=vm.sprite?Vec2{vm.sprite->width,vm.sprite->height}:Vec2{};}
void BulletScene::play(i32 id){host.sound(id);}
BulletVisual* BulletScene::visual(u32 index)noexcept{
    if(index>=slots.size())return nullptr;
    auto& slot=*slots[index];slot.touch_visual();return &slot.visual;
}
void BulletScene::reset(){manager.reset();cancellation_animations.fill(0);error.clear();for(auto& slot:slots){slot->touch_visual();if(!slot->visual.clear_children()){error=slot->visual.error;return;}slot->visual.body=AnmVm{};slot->visual.error.clear();}}
bool BulletScene::emit(const BulletShooter& shooter,float minimum){error.clear();const bool result=manager.emit(shooter,minimum);if(!result&&error.empty())error=manager.error;return result&&error.empty();}
bool BulletScene::update(){error.clear();const bool result=manager.update();if(!result&&error.empty())error=manager.error;return result&&error.empty();}
bool BulletScene::cancel_circle(const Vec3& p,float radius,i32 reward,bool honor){
    error.clear();for(u32 index=manager.first_active();index!=BulletManager::none;){const u32 next=manager.next_active(index);auto& slot=*slots[index];if(bullet_circle_area(slot.state,p,radius,honor)){slot.visual.rate=manager.context.rate;if(!slot.cancel_bullet(reward))return false;}index=next;}return error.empty();
}
bool BulletScene::cancel_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward){
    error.clear();if(manager.first_active()==BulletManager::none)return true;const BulletRectangleArea area(p,size,angle);for(u32 i=manager.next_occupied(0);i!=BulletManager::none;i=manager.next_occupied(i+1)){auto& slot=*slots[i];if(area.contains(slot.state)){slot.visual.rate=manager.context.rate;if(!slot.cancel_bullet(reward))return false;}}return error.empty();
}
bool BulletScene::cancel_all(i32 reward){
    // Unlike area cancellation, the original scans storage order and also
    // processes bullets already playing their cancellation animation.
    error.clear();for(u32 i=manager.next_occupied(0);i!=BulletManager::none;i=manager.next_occupied(i+1)){auto& slot=*slots[i];if(slot.state.phase!=0&&slot.state.phase!=3){slot.visual.rate=manager.context.rate;if(!slot.cancel_bullet(0))return false;}}return error.empty();
}
bool BulletScene::cancel_slot(u32 index,i32 reward){error.clear();if(index>=slots.size()){error="Bullet cancellation slot outside pool";return false;}auto& slot=*slots[index];slot.visual.rate=manager.context.rate;return slot.cancel_bullet(reward)&&error.empty();}
#if TH17_MULTIPLAYER
void BulletScene::save_rollback(multiplayer::FrameImage& image){
    manager.save_rollback(image);
    image.save(cancellation_animations,error,drawing_disabled);
    struct Active {
        Slot* slot;
        Slot::VisualFields fields;
        multiplayer::AnmCheckpoint body;
        Active(Slot& s,multiplayer::AnmCheckpoint::Curves& curves):
            slot(&s),fields(s),body(s.visual.body,s.rollback_curves,curves){}
    };
    struct Idle {Slot* slot;std::shared_ptr<const Slot::VisualImage> image;};
    std::vector<Active> active;
    std::vector<Idle> idle;
    multiplayer::AnmCheckpoint::Curves curves;
    size_t changing=0;
    std::bitset<BulletManager::capacity> bound,shared;
    for(u32 i=0;i<slots.size();++i){
        auto& slot=*slots[i];auto& v=slot.visual;
        if(!v.resource)continue;
        bound.set(i);
        // Embedded parents can be observed or updated through the ANM graph.
        // Keep those full checkpoints even when their bullet slot is idle.
        if(!manager.active(i)&&!v.body.creation_parent&&!v.body.rotation_parent&&
           (!v.body.object_host||v.body.object_host==v.manager)&&(!v.manager||!v.manager->registry.borrowed_root(v.body)))
            shared.set(i);
        else changing+=multiplayer::AnmCheckpoint::changing(v.body);
    }
    active.reserve(bound.count()-shared.count());
    idle.reserve(shared.count());
    curves.changing.reserve(changing);
    for(u32 i=0;i<slots.size();++i){
        if(!bound.test(i))continue;
        auto& slot=*slots[i];
        if(!shared.test(i)){
            slot.idle_image.reset();
            active.emplace_back(slot,curves);
        }else{
            auto cached=slot.idle_image.lock();
            if(!cached){
                // Separate allocation lets the last frame release the VM
                // storage even while the slot retains a weak lookup.
                cached=std::shared_ptr<const Slot::VisualImage>(new Slot::VisualImage(slot));
                slot.idle_image=cached;
            }
            idle.push_back({&slot,std::move(cached)});
        }
    }
#if TH17_ROOM_BENCHMARK
    image.payload[image.BulletVisual]=u32(active.capacity()*sizeof(Active)+idle.capacity()*sizeof(Idle)+curves.changing.capacity()*sizeof(AnmInterpolators));
    image.unused_capacity+=u32((active.capacity()-active.size())*sizeof(Active));
#endif
    image.restore_with([this,bound,active=std::move(active),idle=std::move(idle),curves=std::move(curves)]{
        for(const auto& row:active){
            row.slot->idle_image.reset();
            row.body.restore(row.slot->visual.body,row.slot->rollback_curves,curves);
            row.fields.restore(*row.slot);
        }
        for(const auto& row:idle){
            if(row.slot->idle_image.lock()!=row.image)row.image->restore(*row.slot);
            row.slot->idle_image=row.image;
        }
        for(u32 i=0;i<slots.size();++i){
            if(bound.test(i)||!slots[i]->visual_touched)continue;
            auto& s=*slots[i];auto& v=s.visual;
            s.cancelling=false;v.body={};v.resource=nullptr;v.manager=nullptr;v.overlay_handle=0;v.rate=1;v.error.clear();
            s.idle_image.reset();s.rollback_curves.reset();s.visual_touched=false;
        }
        return true;
    });
}
#endif
}
