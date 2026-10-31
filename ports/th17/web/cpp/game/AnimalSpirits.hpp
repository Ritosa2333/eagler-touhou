#pragma once
#include "AnmManager.hpp"
#include <list>
namespace th17 {
struct AnimalSpiritState {
    u32 animation=0;i32 kind=0;Vec3 position{},velocity{};
    Timer age{0,0,0,0,0},outside{0,0,0,0,0},cycle{0,0,0,0,0};u32 flags=0;
};
static_assert(sizeof(AnimalSpiritState)==96);
struct AnimalSpiritHost {
    virtual ~AnimalSpiritHost()=default;
    virtual bool sound(i32)=0;virtual bool trophy(i32)=0;virtual bool collect_spirit(i32)=0;
};
class AnimalSpirits {
    struct Entry {u32 id;AnimalSpiritState state;};
    AnmManager& animations;Rng& random;AnimalSpiritHost& host;i32 resource;std::list<Entry> entries;u32 generation=0;
    bool adjust_direction(AnimalSpiritState&,float angle);
    bool advance(AnimalSpiritState&,const Vec3& player,float rate,bool& finished);
public:
    std::string error;
    AnimalSpirits(AnmManager& a,Rng& r,AnimalSpiritHost& h,i32 bank):animations(a),random(r),host(h),resource(bank){}
    ~AnimalSpirits();
    u32 spawn(i32 kind,const Vec3&,float angle,bool practice=false);
    bool update(const Vec3& player,float rate);
    bool clear();
    u32 count()const noexcept{return entries.size();}
    AnimalSpiritState* find(u32 id)noexcept{for(auto& e:entries)if(e.id==id)return &e.state;return nullptr;}
    u32 order(u32 index)const noexcept{for(const auto& e:entries)if(!index--)return e.id;return 0;}
};
}
