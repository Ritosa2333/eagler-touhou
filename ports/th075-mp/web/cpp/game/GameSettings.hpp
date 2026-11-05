#pragma once
#include "InputMapping.hpp"
namespace th075 {
struct GameSettings {
 std::array<InputMapping,2> mappings;std::array<u32,2> joystickSelections{255,255};
 GameSettings();
 u8 animatedBackground=1,deviceRefresh=1,renderEveryFrame=1,renderFlags=0,timingFlags=0,windowFlags=0,difficulty=1,recoveryMarkers=0;i8 musicVolume=100,soundVolume=100;
 bool load(const void*,std::size_t);std::array<u8,90> save();
};
}
