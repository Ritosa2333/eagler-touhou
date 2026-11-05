#include "BulletShooter.hpp"
namespace th15 {
void BulletShooter::reset(){*this=BulletShooter{};speed=2;count=rows=1;flags=0x23;shoot_sound=0x15;transform_sound=0x26;}
}
