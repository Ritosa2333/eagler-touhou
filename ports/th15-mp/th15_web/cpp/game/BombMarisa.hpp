#pragma once
#include "BombController.hpp"
#include "DamageSources.hpp"
namespace th15 {
class BombMarisa final:public BombController {
    DamageSources& damage;bool start()override;bool frame(bool&)override;bool create(u32&,i32);bool interrupt(u32,i32);bool cancel();
public:
    u32 beam=0,aura=0;
    BombMarisa(BombContext& c,DamageSources& d):BombController(c),damage(d){}
};
}
