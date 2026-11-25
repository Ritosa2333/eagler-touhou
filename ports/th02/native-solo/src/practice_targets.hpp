#pragma once
#include "practice_menu.hpp"
#include "stage3_practice_evidence.hpp"
#include <optional>

namespace soew {
// Azinn OP/MAIN identities and direct scroll coordinates; published selections
// continue to the final boss. A chapter coordinate is not elapsed game time.
enum class PracticeStartRule { LegacyV4, Calibrated };
struct PracticeCheckpoint {
    int stage=0,chapter_id=-1;
    unsigned progress=0;
    std::optional<std::size_t> formation_cursor;
    std::optional<unsigned> end_progress;
    bool end_at_preboss=false;
    bool dispatch_on_first_tick=false;
};
struct PracticeChapterTarget {
    int stage,id;
    const char *name;
    unsigned progress,measure;
    std::optional<unsigned> end;
    std::optional<std::size_t> cursor;
};
inline const auto practice_chapters=[] {
    std::vector<PracticeChapterTarget> out={
        {0,0,"CHAPTER 1",0,0,58,0},{0,1,"CHAPTER 2",58,19,112,17},
        {0,43,"CHAPTER 3",112,37,116,33},{0,44,"CHAPTER 4",198,66,250,65},{0,45,"PRE BOSS",250,83,{},92},
        {1,0,"CHAPTER 1",0,0,32,0},{1,2,"CHAPTER 2",32,10,54,17},{1,46,"CHAPTER 3",54,18,80,21},
        {1,47,"CHAPTER 4",100,33,176,33},{1,48,"CHAPTER 5",176,58,224,48},
        {1,49,"CHAPTER 6",224,74,272,73},{1,50,"PRE BOSS",272,90,{},84}
    };
    constexpr std::array<std::size_t,8> stage3_cursors={0,25,48,49,72,89,121,130};
    for(std::size_t i=0;i<stage3_practice_evidence.size();++i){
        const auto &entry=stage3_practice_evidence[i];
        if(entry.id==0x0f)continue; // Midboss is a separate selection.
        out.push_back({2,entry.id,entry.label,entry.progress,entry.measure,{},stage3_cursors[i]});
    }
    // Azinn OP 0x1519A/0x15222 (labels), MAIN 0x26A7F–0x26AF4,
    // 0x26C24–0x26C38, 0x26CE9–0x26D88 (scroll), 0x26EEE (music).
    const PracticeChapterTarget later[]={
        {3,0,"CHAPTER 1",0,0,{},0},{3,4,"CHAPTER 2",288,12,{},57},{3,5,"CHAPTER 3",448,18,{},69},
        {3,0x38,"CHAPTER 4",576,24,{},89},{3,0x39,"CHAPTER 5",764,31,{},117},
        {3,0x3a,"CHAPTER 6",1024,42,{},141},{3,0x3b,"CHAPTER 7",1824,76,{},185},{3,0x3c,"PRE BOSS",2144,89,{},251},
        {4,0,"CHAPTER 1",0,0,{},0},{4,0x3d,"PRE BOSS",128,10,{},65},
        {5,0,"CHAPTER 1",0,0,{},0},{5,6,"CHAPTER 2",96,16,{},33},
        {5,0x3e,"CHAPTER 3",112,18,{},37},{5,0x3f,"CHAPTER 4",176,29,{},41},
        {5,0x40,"CHAPTER 5",240,40,{},49},{5,0x41,"CHAPTER 6",280,46,{},61},
        {5,0x42,"CHAPTER 7",344,57,{},85},{5,0x43,"CHAPTER 8",400,66,{},105},
        {5,0x44,"CHAPTER 9",448,74,{},137},{5,0x45,"CHAPTER 10",560,93,{},161},
        {5,0x46,"CHAPTER 11",592,98,{},169},{5,0x47,"PRE BOSS",624,104,{},185}
    };
    out.insert(out.end(),std::begin(later),std::end(later));
    return out;
}();

inline PracticeCapabilities practice_capabilities(){
    PracticeCapabilities c;
    for(int stage=0;stage<practice_stage_count;++stage){
        c.allowed_sections[stage]={PracticeSection::StageStart,PracticeSection::Chapter,PracticeSection::Midboss,PracticeSection::Boss};
        if(stage==3)c.allowed_sections[stage].insert(c.allowed_sections[stage].end()-1,PracticeSection::Midboss2);
        if(stage==4)std::erase(c.allowed_sections[stage],PracticeSection::Midboss); // No stage 5 midboss routine.
        for(const auto &x:practice_chapters)if(x.stage==stage)c.chapters[stage].push_back({x.id,x.name});
    }
    c.boss_phases[0]=c.boss_phases[1]={"PHASE 1","PHASE 2","PHASE 3"};
    // Later phases use the native encounter's actual entry states (not a DOS save state).
    c.boss_phases[2]={"INNER STONES","OUTER STONES","NORTH OPENING","NORTH CLOSED","NORTH FINAL"};
    c.boss_phases[3]={"ROUND 1","ROUND 2","ROUND 3","ROUND 4","ROUND 5","ROUND 6","ROUND 7"};
    c.boss_phases[4]={"FIRST","SECOND","THIRD","MIXED","FINAL FORM"};
    c.boss_phases[5]={"PHASE 1","PHASE 2","PHASE 3","PHASE 4","FINAL PHASE"};
    return c;
}
inline PracticeStartRule practice_start_rule(const PracticeSpec &s){
    constexpr std::array<int,6> preboss_ids={45,50,0x37,0x3c,0x3d,0x47};
    return s.section==PracticeSection::Chapter&&s.stage>=0&&s.stage<int(preboss_ids.size())&&s.chapter_id==preboss_ids[s.stage]
        ?PracticeStartRule::Calibrated:PracticeStartRule::LegacyV4;
}
inline std::optional<PracticeCheckpoint> practice_checkpoint(const PracticeSpec &s,PracticeStartRule rule=PracticeStartRule::Calibrated){
    if(s.section!=PracticeSection::Chapter)return {};
    for(const auto &x:practice_chapters)if(x.stage==s.stage&&x.id==s.chapter_id){
        PracticeCheckpoint point{s.stage,x.id,x.progress,x.cursor,x.end,!x.end.has_value()};
        if(rule==PracticeStartRule::Calibrated&&practice_start_rule(s)==PracticeStartRule::Calibrated){
            // These six legacy cursors are exactly one row AFTER the closing wave's first emitter.
            --*point.formation_cursor;
            point.dispatch_on_first_tick=true;
        }
        return point;
    }
    throw std::runtime_error("Unknown verified practice chapter");
}
}
