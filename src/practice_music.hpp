#pragma once
#include "practice_targets.hpp"
namespace soew {
// Azinn MAIN 0x26EEE; PMD absolute completed measure, MMD=192 raw ticks.
// Anchor lookup is independent of the capability gate: evidence is not an enabled entry.
inline unsigned practice_music_measure(const PracticeSpec &r){
 if(r.section==PracticeSection::Midboss){
     switch(r.stage){case 0:return 38;case 1:return 26;case 2:return stage3_practice_evidence[2].measure;case 3:return 39;case 5:return 33;default:return 0;}
 }
 if(r.section==PracticeSection::Midboss2&&r.stage==3)return 68;
 if(r.section==PracticeSection::Chapter)
     for(const auto &entry:practice_chapters)if(entry.stage==r.stage&&entry.id==r.chapter_id)return entry.measure;
 return 0;
}
}
