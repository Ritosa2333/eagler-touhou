#include "PracticeConfig.hpp"
#include "PracticeSections.hpp"
#include <cmath>
namespace th15 {
bool PracticeConfig::valid(i32 difficulty)const noexcept {
 if(mode<0||mode>1||stage<0||stage>6||difficulty<0||difficulty>4||phase<0||phase>=practice_phase_count(section,difficulty)||life<0||life>8||life_fragment<0||life_fragment>(stage==6?5:3)||bomb<0||bomb>8||bomb_fragment<0||bomb_fragment>4||power<0||power>400||value<0||value>999990||graze<0||graze>999999||score<0||score>9999999990LL||reisen_shield<0||reisen_shield>3||!std::isfinite(doremy_normal_1_phase)||doremy_normal_1_phase<-3.14159265f||doremy_normal_1_phase>3.14159265f||!std::isfinite(enhanced_para)||enhanced_para<0||enhanced_para>1)return false;
 if(section>=10000){constexpr int chapters[]{6,6,6,6,10,4,9};return section<20000&&(section-10000)/100==stage+1&&section%100>=1&&section%100<=chapters[stage];}
 if(section<0||u32(section)>=sizeof(practice_sections)/sizeof(*practice_sections))return false;
 return !section||practice_sections[section].appearance==stage+1;
}
}
