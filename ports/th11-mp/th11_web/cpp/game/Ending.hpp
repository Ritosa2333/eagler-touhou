#pragma once
#include "GameResources.hpp"
#include "AnmManager.hpp"
#include "EnemyAnimations.hpp"
#include "Dialogue.hpp"
#include "ScoreFile.hpp"
#include "ScreenFade.hpp"
#include <map>
namespace th11 {
// e00..e11.msg and staff.msg use their own timed interpreter (40f4a0),
// independent of the stage dialogue VM. Resources are prepared before playback.
class Ending {
public:
    Ending(AnmManager& a,AnmResource& text,ScoreFile& score):animations(a),text(text),scores(score),families(a){}
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
    bool update(u32 held,u32 pressed);
    bool tick(u32 held,u32 pressed);
private:
    AnmManager& animations;AnmResource& text;ScoreFile& scores;EnemyAnimations families;
    bool load_message(const std::string&,bool staff=false);
    bool write(u32 slot,const std::string&,u32 color);
    bool fail(const char* reason){error=reason;return false;}
};
}
