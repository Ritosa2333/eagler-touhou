#include "PracticeConfig.hpp"
#include "PracticeSections.hpp"
#include <cmath>
#include <algorithm>
namespace th11 {
void PracticeConfig::encode(double* out)const{const double words[]{double(mode),double(stage),double(section),double(phase),double(dlg),double(life),double(life_fragment),double(power),double(graze),double(signal),double(value),double(score),double(marisa_b_formation),1};std::copy(words,words+word_count,out);}
bool PracticeConfig::decode(const double* words,u32 count){
    if(!words||count!=word_count||words[13]!=1)return false;
    for(u32 i=0;i<count;++i)if(!std::isfinite(words[i])||std::trunc(words[i])!=words[i]||words[i]<0||(i==11?words[i]>9999999990.:words[i]>2147483647.))return false;
    if(words[4]>1)return false;PracticeConfig p;p.mode=i32(words[0]);p.stage=i32(words[1]);p.section=i32(words[2]);p.phase=i32(words[3]);p.dlg=words[4]!=0;p.life=i32(words[5]);p.life_fragment=i32(words[6]);p.power=i32(words[7]);p.graze=i32(words[8]);p.signal=i32(words[9]);p.value=i32(words[10]);p.score=i64(words[11]);p.marisa_b_formation=i32(words[12]);if(!p.valid())return false;*this=p;return true;
}
bool PracticeConfig::valid()const {
    if(mode<0||mode>1||stage<0||stage>6||phase<0||phase>4||life<0||life>9||life_fragment<0||life_fragment>4||power<0||power>96||graze<0||graze>999999||signal<0||signal>100||value<0||value>999990||score<0||score>9999999990LL||marisa_b_formation<0||marisa_b_formation>4)return false;
    if(section>=10000){constexpr int portions[]{4,4,3,7,6,6,7};if(section>=20000||(section-10000)/100!=stage+1||section%100<1||section%100>portions[stage])return false;}
    else if(section<0||u32(section)>=sizeof(practice_sections)/sizeof(*practice_sections))return false;
    else if(section){const auto a=practice_sections[section].appearance;if((a>7&&stage!=3)||(a<=7&&a!=stage+1))return false;}
    if(phase){if(section==TH11_ST6_BOSS9)return phase<=4;if(section==TH11_ST7_END_S9)return phase<=2;if(section==TH11_ST7_END_S10)return phase<=3;return false;}
    return true;
}
}
