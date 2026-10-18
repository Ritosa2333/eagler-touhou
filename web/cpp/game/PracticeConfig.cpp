#include "PracticeConfig.hpp"
#include "PracticeSections.hpp"
#include <cmath>
#include <algorithm>
namespace th11 {
void PracticeConfig::encode(double* out)const{const bool position=(section==TH11_ST4_RA2||section==TH11_ST4_RA_BOSS5)&&phase;const double words[]{double(mode),double(stage),double(section),double(phase),double(dlg),double(life),double(life_fragment),double(power),double(graze),double(signal),double(value),double(score),double(marisa_b_formation),2,position?double(boss_x):0,position?double(boss_y):0,section==10504?double(wave_passed):0,double(legacy_blue)};std::copy(words,words+word_count,out);}
bool PracticeConfig::decode(const double* words,u32 count){
    if(!words||!((count==14&&words[13]==1)||((count==17||count==word_count)&&words[13]==2)))return false;
    for(u32 i=0;i<14;++i)if(!std::isfinite(words[i])||std::trunc(words[i])!=words[i]||words[i]<0||(i==11?words[i]>9999999990.:words[i]>2147483647.))return false;
    if(words[4]>1)return false;PracticeConfig p;p.mode=i32(words[0]);p.stage=i32(words[1]);p.section=i32(words[2]);p.phase=i32(words[3]);p.dlg=words[4]!=0;p.life=i32(words[5]);p.life_fragment=i32(words[6]);p.power=i32(words[7]);p.graze=i32(words[8]);p.signal=i32(words[9]);p.value=i32(words[10]);p.score=i64(words[11]);p.marisa_b_formation=i32(words[12]);
    if(count>=17){if(!std::isfinite(words[14])||!std::isfinite(words[15])||!std::isfinite(words[16])||std::trunc(words[16])!=words[16]||words[16]<0||words[16]>40)return false;p.boss_x=float(words[14]);p.boss_y=float(words[15]);p.wave_passed=i32(words[16]);}
    if(count==word_count){if(words[17]!=0&&words[17]!=1)return false;p.legacy_blue=words[17]!=0;}
    if(!p.valid())return false;*this=p;return true;
}
bool PracticeConfig::valid()const {
    if(mode<0||mode>1||stage<0||stage>6||phase<0||phase>7||life<0||life>9||life_fragment<0||life_fragment>4||power<0||power>96||graze<0||graze>999999||signal<0||signal>100||value<0||value>999990||score<0||score>9999999990LL||marisa_b_formation<0||marisa_b_formation>4)return false;
    if(section>=10000){constexpr int portions[]{4,4,3,7,6,6,7};if(section>=20000||(section-10000)/100!=stage+1||section%100<1||section%100>portions[stage])return false;}
    else if(section<0||u32(section)>=sizeof(practice_sections)/sizeof(*practice_sections))return false;
    else if(section){const auto a=practice_sections[section].appearance;if((a>7&&stage!=3)||(a<=7&&a!=stage+1))return false;}
    if(!std::isfinite(boss_x)||!std::isfinite(boss_y)||wave_passed<0||wave_passed>40)return false;
    if((section==TH11_ST4_RA2||section==TH11_ST4_RA_BOSS5)&&phase&&(boss_x < -140||boss_x>140||boss_y<80||boss_y>176))return false;
    if(phase>=(legacy_blue&&section==TH11_ST7_END_S9?3:practice_phase_count(section)))return false;
    return true;
}
}
