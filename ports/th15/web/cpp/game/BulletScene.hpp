#pragma once
#include "BulletManager.hpp"
#include "BulletVisual.hpp"
#include "AnmManager.hpp"
#include <memory>
namespace th15 {
struct BulletSceneHost:PlayerDamageHost {
    virtual const PlayerCollision& player_collision()const noexcept=0;
    virtual void sound(i32 id)=0;
    virtual void cancellation_effect(i32 script,const Vec3& position,const Vec3& velocity)=0;
    virtual void graze_spark(const Vec3& position)=0;
    virtual void graze()=0;
    virtual void graze_resonance(float value)=0;
    virtual void item(i32 type,const Vec3& position,float angle,float speed)=0;
    virtual bool spawn_enemy(const EnemySpawnRequest&){return false;}
    virtual bool create_laser(const MovingLaserRequest&){return false;}
    virtual bool create_laser(const StationaryLaserRequest&){return false;}
};
// One owner connects the fixed-size bullet pool to its real ANM instances,
// collision processing and game services. Slots and VM addresses stay stable.
class BulletScene final:public BulletManagerHost {
    struct Slot;
    BulletSceneHost& host;Rng& game_random;Rng& visual_random;
    AnmManager* cancellation_owner=nullptr;i32 cancellation_bank=-1;
    std::vector<std::unique_ptr<Slot>> slots;
    BulletFrameHost& bullet(u32 slot)override;
    void prepare(u32 slot,BulletFrameContext& context)override;
    void play(i32 id)override;
    const std::string* failure()const noexcept override{return &error;}
public:
    BulletManager manager;AnmResource* resource=nullptr;AnmEnvironment* environment=nullptr;AnmObjectHost* animation_objects=nullptr;
    std::array<u32,BulletManager::capacity> cancellation_animations{};
    std::string error;
    BulletScene(BulletSceneHost&,Rng& game_random,Rng& visual_random);
    ~BulletScene();
    void own_cancellation_animations(AnmManager& a,i32 bank)noexcept{cancellation_owner=&a;cancellation_bank=bank;}
    void reset();bool update();
    bool emit(const BulletShooter& shooter,float minimum_distance_squared);
    bool draw(AnmRenderer&);
    bool cancel_circle(const Vec3&,float radius,i32 reward,bool honor_protection=false);
    bool cancel_rectangle(const Vec3&,const Vec2&,float angle,i32 reward);
    bool cancel_all(i32 reward);
    bool cancel_slot(u32 slot,i32 reward);
    BulletVisual* visual(u32 slot)noexcept;
};
}
