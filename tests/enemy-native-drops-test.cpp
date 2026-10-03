#include "../th10_web/cpp/game/Enemy.hpp"
#include "../th10_web/cpp/game/EnemyDrops.hpp"
#include <array>
#include <cassert>
#include <vector>

using namespace th10;
struct Drop {Vec3 position;i32 kind,color;float angle,speed;};
struct Probe final:ItemDropEnvironment {
    Rng rng{};std::vector<Drop> items;
    Probe(){drop_rng=&rng;rng.seed=1234;}
    void spawn_item(const Vec3& position,i32 kind,i32 color,float angle,float speed)override{
        items.push_back({position,kind,color,angle,speed});
    }
};
int main(){
    arithmetic_mode(Precision::Single,Rounding::NearestEven);
    // Execute the real native drop owner in ordinary and multiplayer builds.
    // The environment intentionally owns no player multiplier: a scripted
    // count is released once, independent of roster size and spectators.
    EnemyDrops native{};native.kind=7;native.spread={24,16};
    unsigned scripted=0;
    for(unsigned kind=0;kind<11;++kind){native.counts[kind]=i32(kind%3);scripted+=kind%3;}
    native.counts[11]=123; // Native unused twelfth counter is cleared, not emitted.
    auto replay=native;Probe first,second;const Vec3 origin{10,120,0};
    native.release(origin,first);replay.release(origin,second);
    assert(first.items.size()==scripted+1&&second.items.size()==first.items.size());
    std::array<unsigned,12> counts{};
    for(unsigned i=0;i<first.items.size();++i){
        const auto& a=first.items[i];const auto& b=second.items[i];++counts[unsigned(a.kind)];
        assert(a.kind==b.kind&&a.color==b.color&&a.angle==b.angle&&a.speed==b.speed);
        assert(a.position.x==b.position.x&&a.position.y==b.position.y&&a.position.z==b.position.z);
    }
    for(unsigned kind=1;kind<=11;++kind)assert(counts[kind]==(kind-1)%3+(kind==7?1u:0u));
    assert(native.kind==0);for(auto count:native.counts)assert(count==0);
    const auto before=first.items.size();native.release(origin,first);
    assert(first.items.size()==before); // Repeated release cannot duplicate items.
    assert(first.rng.calls==2+scripted*4+2); // Native RNG, including empty scatter.
}
