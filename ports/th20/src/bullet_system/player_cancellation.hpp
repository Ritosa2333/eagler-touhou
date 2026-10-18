#pragma once
#include "bullet.hpp"
namespace th20::source::bullet {
// 47d100: this cancellation deliberately includes field_18-protected bullets,
// preserves their kind flags, and increments the counter after each cancellation.
void cancel_near_circle(Controller&,const sprite::Vec3&,float radius,int drop_mode);
}
