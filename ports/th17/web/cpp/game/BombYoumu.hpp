#pragma once
#include "BombController.hpp"
namespace th17 {
class BombYoumu final:public BombController {
    bool start()override;bool frame(bool& finished)override;
public:
    using BombController::BombController;
    std::array<u32,260> handles{};std::array<float,260> sides{},heights{};std::array<Timer,260> timers{};
    i32 emitted=0,interval=0;Timer spawn_age;
};
}
