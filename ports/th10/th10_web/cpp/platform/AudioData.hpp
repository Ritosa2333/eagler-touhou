#pragma once
#include "../game/AudioManager.hpp"
namespace th10::browser {
// Constant data recovered from the supplied Japanese 1.00a executable:
// sound definitions 0x4749c8, source names 0x474b40, algorithm 0x46a67c.
inline constexpr SoundDefinition sound_definitions[47]={
 {0,-1900,0},{0,-2100,0},{1,-1200,5},{1,-1500,5},{2,-1100,100},
 {3,-700,100},{4,-700,100},{5,-1900,50},{6,-2200,50},{7,-2400,50},
 {8,-500,100},{9,-400,100},{10,-800,10},{11,-1500,10},{12,-1000,100},
 {5,-1100,50},{13,-1300,50},{14,-1400,50},{15,-900,100},{16,-880,0},
 {17,-1500,0},{5,-300,20},{6,-1800,20},{7,-1800,20},{18,-1100,50},
 {19,-1300,50},{20,-1500,50},{21,-500,100},{22,-1100,20},{23,-800,90},
 {22,-1200,20},{18,-500,50},{24,-800,100},{25,-800,100},{26,-800,100},
 {27,-500,0},{28,-300,100},{29,0,100},{30,0,100},{30,-600,100},
 {8,-300,100},{31,-300,100},{32,-300,100},{33,-300,100},{34,-100,100},
 {35,0,100},{36,-800,50}
};
inline constexpr const char* sound_names[37]={
 "se_plst00.wav","se_enep00.wav","se_pldead00.wav","se_power0.wav","se_power1.wav",
 "se_tan00.wav","se_tan01.wav","se_tan02.wav","se_ok00.wav","se_cancel00.wav",
 "se_select00.wav","se_gun00.wav","se_cat00.wav","se_lazer00.wav","se_lazer01.wav",
 "se_enep01.wav","se_damage00.wav","se_item00.wav","se_kira00.wav","se_kira01.wav",
 "se_kira02.wav","se_timeout.wav","se_graze.wav","se_powerup.wav","se_pause.wav",
 "se_cardget.wav","se_option.wav","se_damage01.wav","se_timeout2.wav","se_invalid.wav",
 "se_slash.wav","se_ch00.wav","se_ch01.wav","se_hint00.wav","se_extend.wav",
 "se_bonus3.wav","se_water.wav"
};
inline constexpr u8 sound_algorithm[16]{};
}
