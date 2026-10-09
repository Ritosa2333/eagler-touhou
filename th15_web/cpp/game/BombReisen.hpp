#pragma once
#include "BombController.hpp"
namespace th15 {
class BombReisen final:public BombController {
    bool start()override;bool frame(bool&)override;
    bool create(u32& handle,i32 script);bool interrupt(u32 handle,i32 label);
    void hitbox(float size)noexcept;
public:
    bool practice_shields(i32 count);
    u32 barrier=0,aura=0;i32 charges=0;
    explicit BombReisen(BombContext& c):BombController(c){}
};
}
