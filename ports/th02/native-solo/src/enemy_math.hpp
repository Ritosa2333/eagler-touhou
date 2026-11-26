#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
namespace soew {
// Public-domain A.Koizuka tables, ReC98/libs/master.lib/sin8[data].asm,
// atan8[data].asm. Quarter-wave is exact integer data, NOT host sin()/cos().
inline constexpr std::array<int,65> enemy_sine_quarter={
0,6,13,19,25,31,38,44,50,56,62,68,74,80,86,92,
98,104,109,115,121,126,132,137,142,147,152,157,162,167,172,177,
181,185,190,194,198,202,206,209,213,216,220,223,226,229,231,234,
237,239,241,243,245,247,248,250,251,252,253,254,255,255,256,256,256};
inline constexpr std::array<unsigned,256> enemy_atan_table={
0,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,
3,3,3,3,3,3,3,4,4,4,4,4,4,5,5,5,
5,5,5,6,6,6,6,6,6,6,7,7,7,7,7,7,
8,8,8,8,8,8,8,9,9,9,9,9,9,10,10,10,
10,10,10,10,11,11,11,11,11,11,11,12,12,12,12,12,
12,12,13,13,13,13,13,13,13,14,14,14,14,14,14,14,
15,15,15,15,15,15,15,16,16,16,16,16,16,16,17,17,
17,17,17,17,17,17,18,18,18,18,18,18,18,19,19,19,
19,19,19,19,19,20,20,20,20,20,20,20,20,21,21,21,
21,21,21,21,21,21,22,22,22,22,22,22,22,22,23,23,
23,23,23,23,23,23,23,24,24,24,24,24,24,24,24,24,
25,25,25,25,25,25,25,25,25,25,26,26,26,26,26,26,
26,26,26,27,27,27,27,27,27,27,27,27,27,28,28,28,
28,28,28,28,28,28,28,28,29,29,29,29,29,29,29,29,
29,29,29,30,30,30,30,30,30,30,30,30,30,30,31,31,
31,31,31,31,31,31,31,31,31,31,32,32,32,32,32,32};
inline int enemy_sin(unsigned angle){angle&=255;const auto part=angle&63;switch(angle>>6){case 0:return enemy_sine_quarter[part];case 1:return enemy_sine_quarter[64-part];case 2:return -enemy_sine_quarter[part];default:return -enemy_sine_quarter[64-part];}}
inline int enemy_s16(unsigned value){value&=65535;return value>=32768?int(value)-65536:int(value);}
inline int enemy_s8(unsigned value){value&=255;return value>=128?int(value)-256:int(value);}
inline int enemy_shift8(int value){return value>=0?value/256:-((-value+255)/256);}
inline std::array<int,2> enemy_vector(unsigned angle,int length){return {enemy_shift8(length*enemy_sin(angle+64)),enemy_shift8(length*enemy_sin(angle))};}
// iatan2.asm uses signed comparisons after 16-bit abs. -32768 needs separate
// machine divide-fault modeling and is explicitly unsupported, never guessed.
inline unsigned enemy_atan2(int y,int x){
    if(x==-32768||y==-32768||x<-32768||x>32767||y<-32768||y>32767)throw std::runtime_error("Unsupported original atan extreme");
    if(x==0&&y==0)return 0;
    const unsigned ax=unsigned(x<0?-x:x),ay=unsigned(y<0?-y:y);
    unsigned angle=ax==ay?32:ax>ay?enemy_atan_table[(ay*256)/ax]:64-enemy_atan_table[(ax*256)/ay];
    if(x<0)angle=128-angle;
    if(y<0)angle=0-angle;
    return angle&255;
}
}
