#pragma once
#include "Types.hpp"
namespace th19 {
struct StoryAnnotation { i32 line=0,x=0,advance=0; const char* text=nullptr; };
struct StoryPage {
    i32 side, face;
    u32 time, source_offset;
    const char* lines[4];
    i32 line_count;
    i32 faces[2];
    bool visible[2];
    float bubble_x, bubble_y;
    i32 bubble_style;
    i32 text_bytes=0;
    i32 music=-1;
    StoryAnnotation annotations[4]{};
    i32 annotation_count=0;
};
struct StoryEvent {u32 time; i32 op,arg;};
struct StoryScene { const StoryPage* pages; i32 count, source_entry; i32 final_music=-1; const StoryEvent* events=nullptr; i32 event_count=0; };
struct StoryStage { i32 opponent; StoryScene scenes[3]; i32 background=0; };
struct StoryRoute { const StoryStage* stages; i32 count; };
enum class StoryPhase { Before, Battle, Won, Lost, Ending, Failed };

// Owns progression only. The battle engine must supply an actual result;
// exhausting a dialogue or an ECL wave never counts as a win.
class StorySession {
public:
    bool begin(const StoryRoute& route) {
        route_=&route; stage_=page_=0; phase_=StoryPhase::Before;
        if (!route.stages || route.count<=0) { phase_=StoryPhase::Failed; return false; }
        settle_empty();
        return true;
    }
    StoryPhase phase() const { return phase_; }
    i32 stage_index() const { return stage_; }
    i32 page_index() const { return page_; }
    i32 opponent() const { return route_ && stage_<route_->count ? route_->stages[stage_].opponent : -1; }
    const StoryScene* scene() const {
        if (!route_ || stage_>=route_->count) return nullptr;
        i32 index=phase_==StoryPhase::Before ? 0 : phase_==StoryPhase::Won ? 1 : phase_==StoryPhase::Lost ? 2 : -1;
        return index<0 ? nullptr : &route_->stages[stage_].scenes[index];
    }
    const StoryPage* page() const {
        auto* s=scene(); return s && s->pages && page_<s->count ? &s->pages[page_] : nullptr;
    }
    void advance() {
        if (!scene()) return;
        ++page_;
        if (page_>=scene()->count) finish_scene();
        settle_empty();
    }
    void skip_scene() { if (scene()) { finish_scene(); settle_empty(); } }
    bool resolve_battle(bool won) {
        if (phase_!=StoryPhase::Battle) return false;
        phase_=won ? StoryPhase::Won : StoryPhase::Lost; page_=0;
        settle_empty(); return true;
    }
    bool retry_stage() {
        if (phase_!=StoryPhase::Failed || !route_ || stage_>=route_->count) return false;
        page_=0; phase_=StoryPhase::Before; settle_empty(); return true;
    }
private:
    const StoryRoute* route_=nullptr;
    i32 stage_=0,page_=0;
    StoryPhase phase_=StoryPhase::Failed;
    void finish_scene() {
        page_=0;
        if (phase_==StoryPhase::Before) phase_=StoryPhase::Battle;
        else if (phase_==StoryPhase::Lost) phase_=StoryPhase::Failed;
        else if (phase_==StoryPhase::Won) {
            if (++stage_>=route_->count) phase_=StoryPhase::Ending;
            else phase_=StoryPhase::Before;
        }
    }
    void settle_empty() {
        while (scene() && (!scene()->pages || scene()->count<=0)) finish_scene();
    }
};
}
