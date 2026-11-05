#include "DynamicState.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <typeinfo>

using namespace th09;
using namespace th09::multiplayer;
#define CHECK(x) do { if (!(x)) { std::fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); std::abort(); } } while (0)

void enemy_ownership(){
    EclWorldState world;EclPlayfieldState field,other;EclProgram program;
    EnemyState checkpoint;
    EclVm e;
    CHECK(checkpoint.Save(e));e.bind_context();CHECK(checkpoint.Restore(e));CHECK(e.values.locals==nullptr);
    // A cold enemy's animation tail padding must survive activation/reuse.
    std::memset(&e.animation,0,sizeof(e.animation));CHECK(checkpoint.Save(e));
    std::memset(&e.animation,0x96,sizeof(e.animation));CHECK(checkpoint.Restore(e));
    const auto* animationBytes=reinterpret_cast<const unsigned char*>(&e.animation);
    for(std::size_t i=0;i<sizeof(e.animation);++i)CHECK(animationBytes[i]==0);
    for(unsigned cycle=0;cycle<128;++cycle){
        e=EclVm{};e.values.world=&world;e.values.field=&field;e.values.opponent=&other;e.program=&program;
        e.behavior_flags=0x803;e.scratch_depth=7;e.generations[2]=cycle;
        e.asynchronous[2]=std::make_unique<EclContext>();
        e.primary.locals.integers[0]=42;e.asynchronous[2]->locals.integers[0]=1000+cycle;
        e.asynchronous[2]->stack[15].locals.floats[7]=float(cycle)+.25f;
        e.active=e.asynchronous[2].get();e.active_slot=2;e.bind_context();
        e.trail.history[95].position={float(cycle),2,3};e.animation.layers[2].pendingInterrupt=91;
        CHECK(checkpoint.Save(e));const auto bytes=checkpoint.Bytes();CHECK(bytes>sizeof(EclVm));
        e=EclVm{};e.asynchronous[0]=std::make_unique<EclContext>();
        CHECK(checkpoint.Restore(e));
        CHECK(e.values.world==&world&&e.values.field==&field&&e.values.opponent==&other&&e.program==&program);
        CHECK(e.active==e.asynchronous[2].get()&&e.active_slot==2&&e.values.locals==&e.asynchronous[2]->locals);
        CHECK(!e.asynchronous[0]&&e.generations[2]==cycle&&e.behavior_flags==0x803&&e.scratch_depth==7);
        CHECK(e.asynchronous[2]->locals.integers[0]==int(1000+cycle));
        CHECK(e.asynchronous[2]->stack[15].locals.floats[7]==float(cycle)+.25f);
        CHECK(e.trail.history[95].position.x==float(cycle)&&e.animation.layers[2].pendingInterrupt==91);
        e.asynchronous[2]->locals.integers[0]=-1;
        CHECK(checkpoint.Restore(e));CHECK(e.values.locals->integers[0]==int(1000+cycle));
    }
    EclContext executing;e.active=&executing;CHECK(!checkpoint.Save(e));CHECK(!checkpoint.Restore(e));
    e.active=nullptr;e.values.locals=&executing.locals;CHECK(!checkpoint.Save(e));
    std::puts("ECL owning copy, context rebinding, delete/reuse, transient-pointer rejection PASS");
}
void effect_ownership(){
    EffectState checkpoint;EffectActor e;
    CHECK(!checkpoint.Restore(e));
    for(unsigned cycle=0;cycle<128;++cycle){
        e=EffectActor{};e.active=1;e.kind=EffectKind::reisen_burst;e.position={float(cycle),19,2};
        e.slot=7;e.flags2=29;e.upper_layer=1;e.time.current=33;
        e.animation=std::make_unique<AnmVm>();e.animation->pendingInterrupt=17;
        e.burst=std::make_unique<EffectBurst>();e.burst->jitter[3][32]=float(cycle);
        e.colors.resize(33);e.colors[32].color=0xff00ff00;e.texture.resize(19);e.texture[18].position.x=37;
        checkpoint.Save(e);const auto bytes=checkpoint.Bytes();CHECK(bytes>sizeof(e)+sizeof(AnmVm)+sizeof(EffectBurst));
        e=EffectActor{};e.animation=std::make_unique<AnmVm>();
        checkpoint.Restore(e);CHECK(e.animation&&e.animation->pendingInterrupt==17);
        CHECK(e.burst&&e.burst->jitter[3][32]==float(cycle));CHECK(e.colors.size()==33&&e.colors[32].color==0xff00ff00);
        CHECK(e.texture.size()==19&&e.texture[18].position.x==37);CHECK(e.position.x==float(cycle));
        CHECK(e.kind==EffectKind::reisen_burst&&e.slot==7&&e.flags2==29&&e.upper_layer==1&&e.time.current==33);
        e.burst->jitter[3][32]=-10;checkpoint.Restore(e);CHECK(e.burst->jitter[3][32]==float(cycle));
    }
    e=EffectActor{};checkpoint.Save(e);e.burst=std::make_unique<EffectBurst>();e.colors.resize(3);
    checkpoint.Restore(e);CHECK(!e.burst&&!e.animation&&e.colors.empty());
    std::puts("Effect owning copy, animation/burst deletion, vector reuse, empty restore PASS");
}
template<class T,class Set,class Verify>void attack(Set set,Verify verify){
    AttackStateSnapshot checkpoint;AttackActor a,parent;
    a.active=1;a.source_side=1;a.layer=2;a.angle=.375f;a.animations.resize(5);a.animations[4].pendingInterrupt=7;
    auto state=std::make_unique<T>();state->phase=3;set(*state,parent);a.state=std::move(state);
    CHECK(checkpoint.Save(a));a=AttackActor{};a.state=std::make_unique<TravelAttackState>();
    CHECK(checkpoint.Restore(a));CHECK(a.state&&typeid(*a.state)==typeid(T));
    CHECK(a.active==1&&a.source_side==1&&a.layer==2&&a.angle==.375f&&a.animations.size()==5&&a.animations[4].pendingInterrupt==7);
    const auto& restored=static_cast<const T&>(*a.state);CHECK(restored.phase==3);verify(restored,parent);
    auto* retained=a.state.get();
    for(unsigned i=0;i<128;++i){static_cast<T&>(*a.state).phase=-1;CHECK(checkpoint.Restore(a));CHECK(a.state.get()==retained);CHECK(static_cast<T&>(*a.state).phase==3);verify(static_cast<const T&>(*a.state),parent);}
    a.state.reset();CHECK(checkpoint.Restore(a));CHECK(typeid(*a.state)==typeid(T));
    Vec3 transient;a.extra_position=&transient;CHECK(!checkpoint.Save(a));CHECK(!checkpoint.Restore(a));
}
void attacks(){
    attack<TravelAttackState>([](auto& s,auto&){s.emissions=37;},[](const auto& s,auto&){CHECK(s.emissions==37);});
    attack<CirnoAttackState>([](auto& s,auto&){s.acceleration.x=37;},[](const auto& s,auto&){CHECK(s.acceleration.x==37);});
    attack<TewiAttackState>([](auto& s,auto&){s.horizontal_acceleration=37;},[](const auto& s,auto&){CHECK(s.horizontal_acceleration==37);});
    attack<AyaAttackState>([](auto& s,auto&){s.variant=37;},[](const auto& s,auto&){CHECK(s.variant==37);});
    attack<SakuyaAttackState>([](auto& s,auto& p){s.parent=&p;s.history[31].y=37;},[](const auto& s,auto& p){CHECK(s.parent==&p&&s.history[31].y==37);});
    attack<MedicineAttackState>([](auto& s,auto&){s.directions[15]=37;},[](const auto& s,auto&){CHECK(s.directions[15]==37);});
    attack<MystiaAttackState>([](auto& s,auto&){s.angular_velocity=37;},[](const auto& s,auto&){CHECK(s.angular_velocity==37);});
    attack<ReisenAttackState>([](auto& s,auto&){s.jitter[3][32]=37;},[](const auto& s,auto&){CHECK(s.jitter[3][32]==37);});
    attack<FieldAttackState>([](auto& s,auto&){s.radial_velocity[32]=37;},[](const auto& s,auto&){CHECK(s.radial_velocity[32]==37);});
    std::puts("All 9 polymorphic attack state types, child-parent link, delete/reuse PASS");
}
int main(){enemy_ownership();effect_ownership();attacks();return 0;}
