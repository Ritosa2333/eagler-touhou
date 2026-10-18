#include "basic_weapons.hpp"
#include "bar_weapon.hpp"
#include "ring_weapon.hpp"
#include "shield_weapon.hpp"
#include "cloud_weapon.hpp"
#include "yellow_weapon.hpp"
#include <cstring>
#include <new>
namespace th20::source::overlay {

namespace {
template<class T>Weapon* allocate(){void* memory=::operator new(sizeof(T),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(T));try{return new(memory)T;}catch(...){::operator delete(memory);throw;}}
using Factory=Weapon*(*)();
const Factory factories[]{
 allocate<FocusBoostWeapon<0>>,allocate<FlagBoostWeapon<0>>,allocate<OrbitWeapon<0>>,allocate<BarWeapon<0>>,allocate<ShieldWeapon<0>>,allocate<YellowWeapon<0>>,allocate<RingWeapon<0>>,allocate<CloudWeapon<0>>,allocate<FocusBoostWeapon<0>>,
 allocate<FocusBoostWeapon<1>>,allocate<FlagBoostWeapon<1>>,allocate<OrbitWeapon<1>>,allocate<BarWeapon<1>>,allocate<ShieldWeapon<1>>,allocate<YellowWeapon<1>>,allocate<RingWeapon<1>>,allocate<CloudWeapon<1>>,allocate<FocusBoostWeapon<1>>};
}
Weapon* create_weapon(int character,int stone){return factories[character*9+stone]();}
void destroy_weapon(Weapon* weapon){if(weapon){weapon->~Weapon();std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));::operator delete(weapon);}}
}
