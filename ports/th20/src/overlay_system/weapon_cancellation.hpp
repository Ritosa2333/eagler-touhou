#pragma once
#include "../bullet_system/bullet.hpp"
#include <functional>
namespace th20::source::bullet {
int cancel_filtered_circle(Controller&,const sprite::Vec3&,float,std::function<int(const sprite::Vec3&)>); //47d240
}
