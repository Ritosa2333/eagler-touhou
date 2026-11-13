#pragma once
#include "GameResources.hpp"
#include "AnmManager.hpp"
#include "EnemyAnimations.hpp"
#include "Dialogue.hpp"
#include "ScoreFile.hpp"
#include "ScreenFade.hpp"
#include <map>
namespace th11 {
struct StageResourceEffects;
struct GameInput;
// Ending MSG and staff.msg use their own timed interpreter (TH14:4202d0),
// independent of the stage dialogue VM.
class Ending {
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    // First member is destroyed last, matching41fe9e after owner cleanup.
    struct EffectStopAfterDestruction {
        AnmManager& manager;
        ~EffectStopAfterDestruction(){manager.th14_effect_stop=0;}
    } effect_stop_cleanup;
#endif
public:
    Ending(AnmManager& a,AnmResource& text,ScoreFile& score):
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
        effect_stop_cleanup{a},
#endif
        animations(a),text(text),scores(score),families(a){}
    Timer elapsed,time,wait;
    u32 flags=1,color=0xffffff,seen=0,frames=0;
    i32 line=0,index=0,difficulty=0,music_request=-1,music_fade=-1;
    std::array<u32,5> text_lines{};
    std::array<u32,16> images{};
    std::array<AnmResource*,4> slots{};
    std::map<std::string,AnmResource> resources;
    std::map<std::string,std::vector<u8>> messages;
    const u8* instruction=nullptr;const u8* end=nullptr;
    std::vector<DialogueText> text_requests;
    std::vector<i32> sounds;
    ScreenFades fades;
    std::string message,error;
    bool active=false;
    bool begin(GameResources&,i32 selection,i32 difficulty,i32 continues);
    void record_progress(i32 selection,i32 level,i32 continues);
    bool update(u32 held,u32 pressed,u32 shoot_frames=0);
    bool tick(u32 held,u32 pressed,u32 shoot_frames=0);
#if PORT_TITLE == 14
    // Return-code6 predicate only; the session repeats the whole callback chain.
    bool repeat_requested(u32 held,u32 shoot_frames=0)const noexcept;
    bool dispatch(GameInput&,u32 raw);
#endif
#if PORT_TITLE == 14
    StageResourceEffects* effects=nullptr;
    bool resource_loading()const{return pending_resource;}
    const std::string& resource_loading_name()const{return pending_name;}
    bool finish_resource_loading();
#endif
private:
    AnmManager& animations;AnmResource& text;ScoreFile& scores;EnemyAnimations families;
    bool load_message(const std::string&,bool staff=false);
    bool write(u32 slot,const std::string&,u32 color);
    bool fail(const char* reason){error=reason;return false;}
#if PORT_TITLE == 14
    GameResources* source=nullptr;
    std::array<AnmResource*,4> loaded_slots{};
    bool pending_resource=false;
    i32 pending_slot=0;
    std::string pending_name;
#endif
};
}
