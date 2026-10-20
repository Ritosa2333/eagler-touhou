#include "GameSettings.hpp"
namespace th075 {
GameSettings::GameSettings(){mappings[0].bindings={0x22,0x24,0x15,0x31,0x10,0x11,0x12,0x20,0};mappings[1].bindings={0xcb,0xcd,0xc8,0xd0,0x91,0x1b,0x1a,0x1b,0};}
bool GameSettings::load(const void* source,std::size_t n){if(n!=90)return false;const auto* p=static_cast<const u8*>(source);
 for(u32 side=0;side<2;side++){joystickSelections[side]=0;for(u32 k=0;k<4;k++)joystickSelections[side]|=u32(p[side*40+k])<<(k*8);mappings[side].joystick=i8(joystickSelections[side]);for(u32 b=0;b<9;b++){u32 v=0;for(u32 k=0;k<4;k++)v|=u32(p[side*40+4+b*4+k])<<(k*8);mappings[side].bindings[b]=v;}}
 const std::array<u8*,8> fields={&animatedBackground,&deviceRefresh,&renderEveryFrame,&renderFlags,&timingFlags,&windowFlags,&difficulty,&recoveryMarkers};for(u32 i=0;i<fields.size();i++)*fields[i]=p[80+i];musicVolume=i8(p[88]);soundVolume=i8(p[89]);deviceRefresh=1;return true;
}
std::array<u8,90> GameSettings::save(){std::array<u8,90> p{};
 for(u32 side=0;side<2;side++){const u32 joystick=(joystickSelections[side]&0xffffff00u)|u8(mappings[side].joystick);for(u32 k=0;k<4;k++)p[side*40+k]=u8(joystick>>(k*8));for(u32 b=0;b<9;b++)for(u32 k=0;k<4;k++)p[side*40+4+b*4+k]=u8(mappings[side].bindings[b]>>(k*8));}
 const std::array<u8,10> fields={animatedBackground,deviceRefresh,renderEveryFrame,renderFlags,timingFlags,windowFlags,difficulty,recoveryMarkers,u8(musicVolume),u8(soundVolume)};for(u32 i=0;i<fields.size();i++)p[80+i]=fields[i];deviceRefresh=1;return p;
}
}
