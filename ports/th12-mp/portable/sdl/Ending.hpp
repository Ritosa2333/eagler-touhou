// The endings (e00..e11.msg) and the staff roll (staff.msg): port of upstream
// src/ending-timeline.mjs and the MSG reading of menu-resources.mjs /
// prepare-menu-assets.py (native interpreter 411420).
#pragma once
#include "Animation.hpp"
#include <functional>
#include <memory>
#include <optional>

namespace th12host {
struct EndingCommand {
    u16 time=0;u8 op=0;std::vector<i32> ints;std::u32string text;std::string file;
};
// Ok with the commands up to and including the end (op 0).
bool decode_ending_message(const std::vector<u8>& bytes,std::vector<EndingCommand>& out,std::string& error);
// 410d76: continued clears get endings 0..5, the others 6..11.
inline i32 select_ending(i32 character,i32 shot,bool continued){return character*2+shot+(continued?0:6);}

class EndingTimeline {
public:
    struct Hooks {
        std::function<i32(const std::string& file)> bank; // ANM file -> loaded bank slot (-1 missing)
        std::function<void(const std::string& track)> music;
        std::function<void()> fade_music;
        std::function<void(i32 id)> sound;
    };
    EndingTimeline(AnimationSampler& sampler,i32 text_bank,std::vector<EndingCommand> ending,std::vector<EndingCommand> credits,i32 difficulty,bool seen,Hooks hooks);
    void tick(u32 held,u32 pressed);
    bool finished()const{return done;}
    std::string error;
    void poses(std::vector<Appearance>& out)const;
    // A full-screen black rectangle's alpha (MSG 13 / 14 fades), or 0.
    float shade()const;
    bool credits()const{return in_credits;}
private:
    AnimationSampler& sampler;i32 text_bank;std::vector<EndingCommand> commands,staff;i32 difficulty;bool seen;Hooks hooks;
    std::vector<std::pair<i32,i32>> banks; // MSG bank slot -> loaded bank
    std::vector<std::pair<i32,std::unique_ptr<MenuAnimation>>> animations;
    std::vector<std::unique_ptr<MenuAnimation>> text;std::vector<MenuText> lines;
    i32 line_slot=0;u32 color=0xffffff;i32 clock=0;size_t pc=0;bool done=false,in_credits=false;
    struct Wait {i32 left=0;bool indefinite=false,clear=false;};std::optional<Wait> wait;
    struct Fade {i32 frames=1,age=0;bool out=false;};std::optional<Fade> fade;
    bool release_wait(u32 held,u32 pressed);
    void set_line(i32 slot,const std::u32string& text,u32 rgb);
};
}
