#pragma once
#include "IceBirthRules.hpp"
namespace th128::ice {
bool canFreezeBullet(std::uint32_t flags,int policy,int age,bool overridePolicy=false) noexcept;
std::optional<bool> contacts(Position ice,float radius,int lifetime,int delay,Position point,float pointRadius) noexcept;
}
