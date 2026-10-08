#pragma once
#include "GameTypes.hpp"
namespace th12 {
inline constexpr int FriendlyCapacity = 256;
extern std::array<Projectile*, FriendlyCapacity> friendlySlots;
void resetFriendlyPool();
int reserveFriendlySlot();
void attachFriendlySlot(Projectile&);
void refreshFriendlyPointers();
} // namespace th12
