#pragma once
#include "stage_script.hpp"
#include <array>
#include <cstdint>
#include <stdexcept>
namespace soew {
// Translation of the default scroll path, not boss/midboss modifications or Hz.
// th02_main.asm:1591..1613,1614..1646,1676..1766; tile.cpp:181..205.
struct StageScroll {
    std::uint8_t speed=1,interval=4,cycle=0;
    int line=0,tile_line=0;
    unsigned top_row=23,display_pixels=0,attempted_pixels=0,progress=0;
    bool pending_progress=false,done=false;
    explicit StageScroll(int stage=0){
        if(stage<0||stage>5)throw std::runtime_error("Invalid scroll stage");
        if(stage==3){speed=2;interval=1;}else if(stage==4)interval=1;else if(stage==5)interval=2;
    }
    bool take_progress_event(){const bool event=pending_progress;pending_progress=false;return event;}
    // ASM 1614–1646: enemy and midboss updates see this attempted line,
    // before the progress event and tile-ring writes of ASM 1676–1715.
    unsigned calculate_tick(){
        if(done||cycle%interval!=0)return 0;
        line=(line+400-speed)%400;
        attempted_pixels+=speed;
        return speed;
    }
    static void validate_map_length(unsigned map_sections){
        if(map_sections<3||map_sections>320)throw std::runtime_error("Invalid scrolling map length");
    }
    // Returns true only for a displayed scroll step that reloaded a ring row.
    bool commit_tick(unsigned map_sections,unsigned delta){
        validate_map_length(map_sections);
        if(done)return false;
        bool reload=false;
        if(delta){
            if((line&7)==0){++progress;pending_progress=true;}
            tile_line-=int(delta);
            if(tile_line<0){
                tile_line+=16;++top_row;
                if(top_row/8>=map_sections){
                    // Original final step is not displayed; asm1714 increments line
                    // by ONE even on speed2. Keep diagnostics separate from pixels.
                    ++line;done=true;return false;
                }
                reload=true;
            }
            display_pixels+=delta;
        }
        cycle=static_cast<std::uint8_t>(cycle+1);
        return reload;
    }
    // Legacy consumers with no permanent VRAM writes can keep the combined step.
    void finish_tick(unsigned map_sections){
        validate_map_length(map_sections);
        commit_tick(map_sections,calculate_tick());
    }
};
// ONE row per scroll-progress notification, exact equality, original order.
// A missed/backwards trigger stalls cursor; no sorting/catch-up while-loop.
// This schedules intents only. Callback owns the original 25-slot enemy pool.
struct FormationDispatch {
    struct Intent {std::uint8_t template_id;unsigned lane;int x;std::size_t row;};
    std::size_t cursor=0;
    std::uint64_t rows=0,intents=0,accepted=0,suppressed=0,capacity_rejected=0;
    bool incomplete_boundary=false;
    template<class Spawn> bool dispatch(const StageScript &script,unsigned progress,bool midboss,Spawn spawn){
        if(cursor>=script.formations.size()){
            if(script.has_missing_formations(cursor))incomplete_boundary=true;
            return false;
        }
        const auto &formation=script.formations[cursor];if(formation.trigger!=progress)return false;
        for(unsigned i=0;i<48;++i){const auto id=formation.selectors[i];if(id==255)continue;
            ++intents;
            if(midboss){++suppressed;continue;}
            const Intent intent{id,i+1,int((i+1)*8+16),cursor};
            if(spawn(intent))++accepted;else ++capacity_rejected;
        }
        ++cursor;++rows;return true;
    }
};
}
