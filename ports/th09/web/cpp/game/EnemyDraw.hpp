#pragma once
#include "EnemyManager.hpp"
#include "AttackTransfer.hpp"
namespace th09 {
struct EnemyDrawServices {
    PlayfieldGeometry geometry;
    virtual ~EnemyDrawServices()=default;
    virtual void draw_animation(AnmVm&)=0;
    virtual void draw_strip(AnmVm&,const EnemyTrailVertex*,u32 count)=0;
};
class EnemyDraw {
    static void trail(EclVm&,EnemyDrawServices&);
public:
    static void enemy(EclVm&,EnemyDrawServices&);
    static void layers(EnemyManager& m,u32 first,u32 last,EnemyDrawServices& s){for(u32 n=first;n<last&&n<m.draw_lists.size();++n)for(auto* e:m.draw_lists[n])enemy(*e,s);}
};
}
