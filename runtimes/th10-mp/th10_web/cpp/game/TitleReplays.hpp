#pragma once
#include "TitleSelection.hpp"
#include "Replay.hpp"
#include "ResultsDraw.hpp"
namespace th10 {
struct ReplaySearchEntry {u8 metadata[44];char filename[260];u8 alternate[16];};
static_assert(sizeof(ReplaySearchEntry)==320);
struct TitleReplayEnvironment : TitleSelectionEnvironment {
    i32 *remembered_replay,*return_screen;
    char* replay_filename;
    virtual Replay* preview(const char* filename)=0;
    virtual void delete_replay(Replay* replay)=0;
    virtual void make_directory(const char* directory)=0;
    virtual void change_directory(const char* directory)=0;
    virtual u32 find_first(const char* pattern,ReplaySearchEntry& entry)=0;
    virtual bool find_next(u32 handle,ReplaySearchEntry& entry)=0;
    virtual void find_close(u32 handle)=0;
#ifdef TH_ENABLE_THPRAC
    // THGuiRep State(1/2/3): reset, inspect the opened replay's PRAC block and
    // commit it when playback is accepted. The environment owns the raw bytes.
    virtual void reset_practice()=0;
    virtual void check_practice(const char* filename)=0;
    virtual void activate_practice()=0;
#endif
};
struct TitleReplays {
    TitleMenu& title;
    TitleReplayEnvironment& environment;
    i32 update();
};
void replay_slot_filename(char (&filename)[12],i32 slot) noexcept;
i32 draw_title_replays(const TitleMenu& title,ResultsDrawEnvironment& environment,const char* const* long_difficulties);
void draw_replay_description(const ReplayInfo& info,i32 slot,const Vec3& position,ResultsDrawEnvironment& environment,const char* const* long_difficulties);
}
