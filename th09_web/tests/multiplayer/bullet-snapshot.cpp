#include "../../cpp/game/BulletManager.hpp"
#include <eagler/netplay/RollbackJournal.hpp>
#include <eagler/netplay/SparsePoolCapture.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace th09;
#define CHECK(x) do { if(!(x)){std::fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);std::abort();} } while(0)

struct Actions final:BulletFrameActions {
    bool prepare(Bullet& b,u32,i32,i32,u32)override{b.cull_width=b.cull_height=16;b.hitbox={4,4,0};return true;}
    bool advance_animation(Bullet&,BulletAnimation)override{return true;}
    i32 probe_attacks(Bullet&)override{return 0;}
    i32 collide_attacks(Bullet&)override{return 0;}
    void collide_player(Bullet&)override{}
    void play_sound(i32,i32)override{}
    void play_positioned_sound(i32,float)override{}
    void change_type(Bullet&,i32,i32)override{}
    bool emit_children(const BulletEmission&)override{return true;}
};
struct Checkpoint final:PoolCheckpoint {
    Netplay::RollbackJournal journal;
    Netplay::SparsePoolCapture<BulletManager::update_count+1> slots;
    BulletManager& owner;
    explicit Checkpoint(BulletManager& b):owner(b){CHECK(journal.Reset({1,2*1024*1024,2048,true,true}));}
    void Save(){
        journal.DiscardBefore(1);CHECK(journal.BeginFrame(0));
        CHECK(slots.Capture(owner.pool.data(),[](const Bullet& b){return b.state!=0;},[this](void* p,std::size_t n){return journal.Touch(p,n);}));
        CHECK(journal.Touch(&owner.total,sizeof(owner.total)));
        CHECK(journal.Touch(&owner.first_count,sizeof(owner.first_count)));
        CHECK(journal.Touch(&owner.second_count,sizeof(owner.second_count)));
        CHECK(journal.Touch(&owner.cancel_frames,sizeof(owner.cancel_frames)));
        CHECK(journal.Touch(&owner.frame,sizeof(owner.frame)));
        CHECK(journal.Touch(&owner.lifetime,sizeof(owner.lifetime)));
        CHECK(journal.Touch(&owner.draw_heads,sizeof(owner.draw_heads)));
        owner.checkpoint=this;
    }
    void Restore(){owner.checkpoint=nullptr;CHECK(journal.EndFrame());CHECK(journal.UndoTo(0));}
    void BeforeBullet(BulletManager& b,std::size_t i)override{
        CHECK(&b==&owner&&i<b.pool.size());
        CHECK(slots.TouchSlot(b.pool.data(),&b.pool[i],[this](void* p,std::size_t n){return journal.Touch(p,n);}));
    }
    void BeforeEnemy(EclVm&)override{CHECK(false);}
    void BeforeEffect(EffectManager&,std::size_t)override{CHECK(false);}
    void BeforeAttack(AttackQueue&,std::size_t)override{CHECK(false);}
    void BeforeBulletVisual(BulletVisuals&,std::size_t)override{CHECK(false);}
    void BeforeBulletAnimation(BulletVisuals&,std::size_t,std::size_t)override{CHECK(false);}
    void BeforeBytes(void*,std::size_t)override{CHECK(false);}
};
int main(){
    BulletManager manager;Actions actions;FrameTiming timing;Rng random{917,0,0};
    BulletEmission emission;emission.count=emission.layers=1;emission.pattern=8;emission.speed=1.5f;emission.ending_speed=.5f;emission.angle=2.f;emission.spread=-2.f;emission.position={0,220,0};
    const Vec3 player{0,400,0};Checkpoint checkpoint(manager);
    const auto spawn=[&](bool second){auto* b=manager.create(emission,0,0,0,second,timing,random,player,actions);CHECK(b);return b;};
    const auto bytes=[&]{std::vector<unsigned char> data(manager.pool.size()*sizeof(Bullet));std::memcpy(data.data(),manager.pool.data(),data.size());return data;};
    unsigned tests=0;
    for(unsigned trial=0;trial<300;++trial){
        manager.reset_pool();
        // Real spawn and retire leave meaningful, nonzero bytes in cold slots.
        for(unsigned i=0;i<BulletManager::first_capacity;++i)spawn(false);
        for(unsigned i=0;i<BulletManager::second_capacity;++i)spawn(true);
        for(unsigned i=0;i<manager.pool.size();++i)if(manager.pool[i].state!=6&&(i*17+trial)%10<trial%11)manager.pool[i].remove();
        const auto original=bytes();const auto initialRandom=random;
        const auto simulate=[&]{
            for(unsigned tick=0;tick<8;++tick){
                for(unsigned n=0;n<7;++n)spawn((tick+n)%2);
                CHECK(manager.update_bullets(timing,player,0,0,actions));manager.finish_frame(timing,0,0);
                // Every active slot was captured on entry or before its spawn.
                for(auto& b:manager.pool)if(b.state&&b.state!=6&&b.lifetime.current%7==0)b.remove();
            }
        };
        checkpoint.Save();simulate();const auto expected=bytes();const auto expectedRandom=random;
        checkpoint.Restore();CHECK(bytes()==original);random=initialRandom;
        simulate();CHECK(bytes()==expected);CHECK(random.seed==expectedRandom.seed&&random.calls==expectedRandom.calls);++tests;
        const auto beforeReset=bytes();checkpoint.Save();
        // Repeat reset/reuse in one checkpoint. The shared run/slot helper must
        // not overwrite first-write bytes or submit partially overlapping runs.
        for(unsigned reset=0;reset<3;++reset){
            manager.reset_pool();
            for(unsigned i=0;i<BulletManager::first_capacity;++i)spawn(false);
            for(unsigned i=0;i<BulletManager::second_capacity;++i)spawn(true);
            const auto calls=random.calls;
            CHECK(spawn(false)==&manager.pool[0]);CHECK(spawn(true)==&manager.pool[BulletManager::second_begin]);
            CHECK(random.calls==calls);
            CHECK(manager.pool[BulletManager::first_capacity].state==6&&manager.pool[BulletManager::update_count].state==6);
        }
        checkpoint.Restore();CHECK(bytes()==beforeReset);++tests;
    }
    std::printf("Bullet sparse runs: %u exact full-pool restore/replay/reset/reuse/overflow cases PASS\n",tests);
}
