#pragma once
#include "BombController.hpp"
namespace th17 {
struct BombOrb {
    u32 handle=0;MotionState motion;bool active=false;Timer age;Vec3 last_velocity{};
    i32 target=0,index=0,damage_id=0,retired=0;float turn_speed=0;
};
class BombReimu final:public BombController {
    bool start()override;bool frame(bool& finished)override;
    bool create_orbs(u32 first,bool clockwise,i32 amount);
    bool update_orb(BombOrb&);bool explode(BombOrb&);bool clear_orbs();bool cancel();
public:
    using BombController::BombController;
    std::unique_ptr<std::array<BombOrb,24>> orbs;
    bool stop()override;
};
}
