// Focused lookup contract test. Real 32-bit game layouts and registry code.
// The integration lane executes the unchanged PlayerShooting.cpp source;
// motion/timer are explicit no-op test doubles, and extended arithmetic traps.
#include "PlayerShooting.hpp"
#include "PlayerShotLookup.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>
#include <algorithm>
using namespace th10;
namespace {
unsigned assertions=0, scenarios=0;
const char* phase="startup";
[[noreturn]] void fail(const char* expression,int line){std::fprintf(stderr,"lookup test [%s]: line %d: %s\n",phase,line,expression);std::abort();}
#define CHECK(x) do{++assertions;if(!(x))fail(#x,__LINE__);}while(false)
u64 digest=14695981039346656037ull;
void hash(u32 n){for(unsigned i=0;i<4;++i){digest^=u8(n>>(i*8));digest*=1099511628211ull;}}
u32 bits(float v){u32 n;std::memcpy(&n,&v,sizeof(n));return n;}
u32 collide(unsigned i){return (0xff800000u+i)*244002641u;}
struct Fixture {
    Player player{};
    std::array<PlayerShotDefinition,129> definitions{};
    std::array<AnmVm,800> vms{};
    AnmRegistry registry{};
    unsigned used=0;
    Fixture(){for(unsigned i=0;i<128;++i){auto& s=player.shots[i];s.state=1;s.definition=&definitions[i];s.animation=i*2+1;s.secondary_animation=i*2+2;s.motion.position={float(i+1),float(i+2),float(i+3)};s.motion.angle=.5f;s.timer.current=0;}player.option_count=4;}
    void clear_registry(){registry={};used=0;}
    AnmVm* append(u32 id,bool ui=false){CHECK(used<vms.size());auto& vm=vms[used++];vm={};vm.id=id;vm.registry_node.initialize(&vm);vm.child_node.initialize(&vm);vm.flags=0x8000000;auto*& head=ui?registry.ui_head:registry.world_head;auto*& tail=ui?registry.ui_tail:registry.world_tail;if(tail){tail->next=&vm.registry_node;vm.registry_node.previous=tail;}else head=&vm.registry_node;tail=&vm.registry_node;return &vm;}
    void full_registry(){for(unsigned i=0;i<256;++i)append(i+1,i%3==0);}
    void check(PlayerShotLookup& lookup,u32 id){CHECK(lookup.find(id,registry)==registry.find(id));}
    void check_all(PlayerShotLookup& lookup){for(auto& shot:player.shots){check(lookup,shot.animation);check(lookup,shot.secondary_animation);}check(lookup,0);check(lookup,0xdeadbeef);}
};
void basic_tests(){
    phase="empty lists and before prepare";auto f=std::make_unique<Fixture>();PlayerShotLookup lookup;f->check_all(lookup);lookup.prepare(f->player.shots,f->player.shots+128,f->registry);f->check_all(lookup);++scenarios;
    phase="all handles zero, zero-id registry VM";f=std::make_unique<Fixture>();for(auto& s:f->player.shots)s.animation=s.secondary_animation=0;f->append(0);PlayerShotLookup zero;zero.prepare(f->player.shots,f->player.shots+128,f->registry);f->check_all(zero);CHECK(zero.find(0,f->registry)==nullptr);++scenarios;
    phase="full capacity distinct and duplicate precedence";f=std::make_unique<Fixture>();f->append(0);auto* world_first=f->append(1);f->append(1);auto* ui_first=f->append(2,true);f->append(2,true);f->append(1,true);for(unsigned i=3;i<=256;++i)f->append(i,i%2);auto* unrequested=f->append(7000);PlayerShotLookup full;full.prepare(f->player.shots,f->player.shots+128,f->registry);f->check_all(full);CHECK(full.find(1,f->registry)==world_first);CHECK(full.find(2,f->registry)==ui_first);CHECK(full.find(7000,f->registry)==unrequested);++scenarios;
    phase="same bucket, 256 distinct, wrap-around and misses";f=std::make_unique<Fixture>();for(unsigned i=0;i<128;++i){auto& s=f->player.shots[i];s.animation=collide(i*2);s.secondary_animation=collide(i*2+1);CHECK((s.animation*2654435761u)>>23==511);CHECK((s.secondary_animation*2654435761u)>>23==511);}for(unsigned i=0;i<256;++i)f->append(collide(i+300),i%2);for(unsigned i=0;i<256;++i)if(i%7)f->append(collide(i),i%3==0);auto* duplicate=f->append(collide(3));f->append(collide(3),true);PlayerShotLookup collisions;collisions.prepare(f->player.shots,f->player.shots+128,f->registry);f->check_all(collisions);CHECK(collisions.find(collide(3),f->registry)==duplicate);for(unsigned i=300;i<556;++i)f->check(collisions,collide(i));++scenarios;
    phase="inactive null-definition gaps and count threshold";for(unsigned count=1;count<=8;++count){f=std::make_unique<Fixture>();for(auto& s:f->player.shots){s.state=0;s.definition=nullptr;}for(unsigned i=0;i<count;++i){auto& s=f->player.shots[i*15+1];s.state=i%2?2:1;s.definition=&f->definitions[i];}f->full_registry();PlayerShotLookup gaps;for(auto& s:f->player.shots)if(s.state){gaps.prepare(&s,f->player.shots+128,f->registry);f->check(gaps,s.animation);f->check(gaps,s.secondary_animation);}++scenarios;}
}
void callback_direct_tests(){
    phase="all callback boundaries, mutation before/inside/after callback";
    for(unsigned boundary=0;boundary<128;++boundary){auto f=std::make_unique<Fixture>();f->definitions[boundary].on_update=1;f->full_registry();if(boundary+2<128){f->player.shots[boundary+2].state=0;f->player.shots[boundary+2].definition=nullptr;}PlayerShotLookup lookup;unsigned callbacks=0;
        for(unsigned i=0;i<128;++i){auto& s=f->player.shots[i];if(!s.state)continue;lookup.prepare(&s,f->player.shots+128,f->registry);if(s.definition->on_update){++callbacks;
                // This query was resolved before the boundary if there was a batch.
                const u32 old_id=s.animation;
                // Repoint list heads and change IDs in place: any stale scratch
                // pointer resolves to a different or now absent VM.
                for(unsigned j=0;j<f->used;++j)f->vms[j].id=0;
                f->registry={};f->used=300;
                for(unsigned j=i;j<128;++j){auto& later=f->player.shots[j];later.animation=10000+callbacks*1000+j*2;later.secondary_animation=10001+callbacks*1000+j*2;f->append(later.animation,j%2);if(j%5)f->append(later.secondary_animation,j%3==0);}
                CHECK(lookup.find(old_id,f->registry)==nullptr);f->check_all(lookup);
                if(i==boundary){if(i+1<128)f->player.shots[i+1].state=0;if(i+2<128){auto& later=f->player.shots[i+2];later.state=1;later.definition=&f->definitions[i+2];}if(i+3<128)f->definitions[i+3].on_update=2;if(i>0){f->player.shots[i-1].state=1;f->definitions[i-1].on_update=3;}}
                // Current shot's callback can replace its own definition.
                f->definitions[128].on_update=0;s.definition=&f->definitions[128];f->check_all(lookup);
            }f->check(lookup,s.animation);f->check(lookup,s.secondary_animation);}
        CHECK(callbacks==(boundary+3<128?2u:1u));++scenarios;
    }
}
void callback_density_tests(){
    phase="adjacent/dense callbacks and successive list reorder";
    for(unsigned stride:{1u,2u,3u,5u,17u}){auto f=std::make_unique<Fixture>();f->full_registry();for(unsigned i=0;i<128;++i)f->definitions[i].on_update=i%stride==0;PlayerShotLookup lookup;unsigned generation=0;
        for(unsigned i=0;i<128;++i){auto& s=f->player.shots[i];lookup.prepare(&s,f->player.shots+128,f->registry);if(s.definition->on_update){++generation;const u32 old=s.animation;for(unsigned j=0;j<f->used;++j)f->vms[j].id=0;f->registry={};f->used=300;
                for(unsigned k=128;k>i;){const unsigned j=--k;auto& later=f->player.shots[j];later.animation=200000+generation*300+j*2;later.secondary_animation=later.animation+1;f->append(later.animation,(j+generation)%2);f->append(later.secondary_animation,(j+generation)%3==0);}
                CHECK(lookup.find(old,f->registry)==nullptr);f->check(lookup,s.animation);f->check(lookup,f->player.shots[127].secondary_animation);
            }f->check(lookup,s.animation);f->check(lookup,s.secondary_animation);}
        CHECK(generation==(127/stride)+1);++scenarios;
    }
}
struct IntegrationEnvironment final:PlayerShootingEnvironment {
    Fixture& f;unsigned boundary;unsigned callbacks=0;unsigned mode;std::array<bool,128> processed{};
    IntegrationEnvironment(Fixture& fixture,AnmManager& owner,unsigned b,unsigned m):f(fixture),boundary(b),mode(m){manager=&owner;manager->registry=f.registry;dialogue_active=false;enemy_manager_present=true;}
    void initialize_shot(Player&,PlayerShot&,i32)override{CHECK(false);}
    void play_shot_sound(i32,float)override{CHECK(false);}
    void update_shot(Player& p,PlayerShot& s)override{
        const unsigned slot=&s-p.shots;CHECK(slot<128);++callbacks;hash(slot);hash(callbacks);
        // Replace every old list node and mutate the old VM IDs, with the
        // current callback slot and all future handles remapped as well.
        f.registry=manager->registry;for(unsigned j=0;j<f.used;++j)f.vms[j].id=0;f.registry={};f.used=300;
        for(unsigned j=slot;j<128;++j){auto& later=p.shots[j];later.animation=10000+callbacks*1000+j*2;later.secondary_animation=later.animation+1;auto* first=f.append(later.animation,j%2);first->position={-1,-1,-1};if(j%5){auto* second=f.append(later.secondary_animation,j%3==0);second->position={-2,-2,-2};}if(j%13==0)f.append(later.animation,j%2);}
        if(slot==boundary){
            if(slot+1<128)p.shots[slot+1].state=0;
            if(slot+2<128){auto& later=p.shots[slot+2];later.state=1;later.definition=&f.definitions[slot+2];}
            if(slot+3<128)f.definitions[slot+3].on_update=2;
            if(slot+4<128){p.shots[slot+4].animation=0;p.shots[slot+4].secondary_animation=0;}
            if(slot+5<128)p.shots[slot+5].animation=0xdeadbeef;
            if(slot+6<128)p.shots[slot+6].secondary_animation=0xbadcafe;
            if(slot+7<128){p.shots[slot+7].definition=&f.definitions[128];f.definitions[128].type=3;f.definitions[128].option=1;p.shots[slot+7].collided=0;p.shots[slot+7].first_collision=1;}
            if(slot>0){p.shots[slot-1].state=1;f.definitions[slot-1].on_update=3;}
            if(mode==1)s.state=0; // Original still completes this iteration.
            if(mode==2)s.animation=0xdeadbeef;
            if(mode==3)s.secondary_animation=0xbadcafe;
        }
        f.definitions[slot].on_update=0;manager->registry=f.registry;
    }
};
void integration_tests(){
    phase="actual update_shots callback mutation trace";
    auto manager=std::make_unique<AnmManager>();
    for(unsigned mode=0;mode<4;++mode)for(unsigned boundary=0;boundary<128;++boundary){auto f=std::make_unique<Fixture>();f->definitions[boundary].on_update=1;f->full_registry();if(boundary+2<128){f->player.shots[boundary+2].state=0;f->player.shots[boundary+2].definition=nullptr;}
        IntegrationEnvironment env(*f,*manager,boundary,mode);CHECK(f->player.update_shots(env)==0);CHECK(env.callbacks==(boundary+3<128?2u:1u));
        for(const auto& s:f->player.shots){hash(u32(s.state));hash(s.animation);hash(s.secondary_animation);hash(s.collided);hash(s.first_collision);hash(s.timer.current);hash(u32(s.definition? s.definition-f->definitions.data():999));}
        for(unsigned i=0;i<f->used;++i){const auto& vm=f->vms[i];hash(vm.id);hash(vm.flags);hash(bits(vm.position.x));hash(bits(vm.position.y));hash(bits(vm.position.z));hash(bits(vm.rotation.z));hash(u32(vm.pending_interrupt));}
        for(auto* head:{manager->registry.world_head,manager->registry.ui_head}){for(auto* node=head;node;node=node->next)hash(u32(node->value-f->vms.data()));hash(0xffffffff);}
        ++scenarios;
    }
}
}
// Test doubles isolate the lookup schedule; numeric simulation has its own gates.
namespace th10 {
void Movement::update_velocity()noexcept{}
void Movement::update()noexcept{}
i32 Timer::tick()noexcept{return current;}
bool single_precision_nearest()noexcept{return true;}
Extended Extended::from_float(float)noexcept{fail("unexpected extended arithmetic",__LINE__);}
float Extended::to_float()const noexcept{fail("unexpected extended arithmetic",__LINE__);}
Extended operator+(Extended,Extended)noexcept{fail("unexpected extended arithmetic",__LINE__);}
Extended operator-(Extended,Extended)noexcept{fail("unexpected extended arithmetic",__LINE__);}
Extended operator*(Extended,Extended)noexcept{fail("unexpected extended arithmetic",__LINE__);}
Extended operator/(Extended,Extended)noexcept{fail("unexpected extended arithmetic",__LINE__);}
bool operator<(Extended,Extended)noexcept{fail("unexpected extended arithmetic",__LINE__);}
bool operator==(Extended,Extended)noexcept{fail("unexpected extended arithmetic",__LINE__);}
}
int main(){basic_tests();callback_direct_tests();callback_density_tests();integration_tests();std::printf("PASS scenarios=%u assertions=%u trace=%016llx sizeof_lookup=%zu\n",scenarios,assertions,static_cast<unsigned long long>(digest),sizeof(PlayerShotLookup));}
