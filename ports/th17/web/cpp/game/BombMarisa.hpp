#pragma once
#include "BombController.hpp"
namespace th17 {
class BombMarisa final:public BombController {
    bool start()override;bool frame(bool& finished)override;bool cancel();
public:
    using BombController::BombController;
    i32 hit(DamageSource&,const DamageQuery&)override;
};
}
