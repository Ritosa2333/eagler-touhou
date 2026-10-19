#pragma once
#include "BulletVisuals.hpp"
#include "LaserManager.hpp"
#include "AttackTransfer.hpp"
namespace th09 {
struct BulletDrawServices {
    PlayfieldGeometry geometry;
    virtual ~BulletDrawServices()=default;
    virtual void begin_field()=0;
    virtual void draw_animation(AnmVm&)=0;
    virtual void upper_effects()=0;
};
class BulletDraw {
public:
    static void draw(BulletManager&,BulletVisuals&,LaserManager&,BulletDrawServices&);
};
}
