#pragma once
#include "AnmManager.hpp"
#include "MenuCursor.hpp"
#include "ScoreFile.hpp"
#include "GameConfig.hpp"
#include "Dialogue.hpp"
#include "Replay.hpp"
#include "ReplayMenu.hpp"
#include "AsciiText.hpp"
#include <memory>
#include <array>
#include <vector>
namespace th11 {
enum class TitleScreen:i32 {Entrance,Main,Exit,Options,Keys,Difficulty,Character,Partner,Practice,Loading,Records,Replays,ReplayLoading,Music,Results,ReplaySave};
struct TitleSelection {
    i32 difficulty=1,last_difficulty=1,saved_difficulty=1;
    i32 character=0,partner=0,last_character=0,last_partner=0,stage=1;
    u32 flags=0;
    i32 last_practice=0,practice_start=0;
};
// Original title state machine. Animation files retain their own scripted
// transitions; menu logic communicates through the original ANM interrupts.
class TitleMenu {
public:
    TitleMenu(AnmManager& a,AnmResource& t,AnmResource& v,AnmResource& ascii,ScoreFile& s)
        :animations(a),title(t),variant(v),ascii(ascii),scores(s){cursor.wrap=1;timer.set(0,&a.rate);}
    TitleScreen screen=TitleScreen::Main;
    i32 substate=0;
    MenuCursor cursor;
    MenuCursor secondary,page;
    Timer timer;
    TitleSelection selection;
    GameConfig config;
    u32 flags=2;
    std::array<u32,512> handles{};
    std::vector<i32> sounds;
    bool start_requested=false,fade_requested=false;
    bool volume_changed=false;
    u32 number_keys=0;
    u32 held=0; i32 music_start_delay=0;
    u32 controller_buttons=0;
    std::array<i16,5> edited_buttons{};
    struct MusicEntry {std::string path,title;std::array<std::string,8> comment;};
    struct MusicState {i32 count=0,line=0,selected=0,prompt=0,scroll=0;} music_state;
    std::vector<MusicEntry> music_entries;
    std::vector<DialogueText> text_requests;
    AnmResource* text_resource=nullptr;
    i32 music_request=-1;
    bool music_pause=false;
    using ReplayEntry=ReplayMenuEntry;
#ifdef TH11_MULTIPLAYER
    // Retain the native 25-row pages while allowing every isolated MP save
    // slot and imported user Replay to remain reachable.
    std::vector<std::shared_ptr<ReplayEntry>> replay_files=std::vector<std::shared_ptr<ReplayEntry>>(100);
    bool multiplayer_replay_menu=false;
#else
    std::array<std::shared_ptr<ReplayEntry>,75> replay_files{};
#endif
    void clear_replays(){for(auto& entry:replay_files)entry.reset();}
    i32 last_replay=0,replay_file=0,replay_stage=0;
    bool replay_scan_requested=false,replay_start_requested=false;
    i32 record_rows=0,unlock_progress=0,unlock_timeout=0;
    std::array<u8,256> key_edges{};
    MenuCursor name_cursor;
    std::array<char,9> entered_name{};
    i32 name_length=0,result_unranked=0,result_score=0,result_continues=0;
    u64 result_timestamp=0;float result_slowdown=0;
    bool replay_save_requested=false;
#ifdef TH11_MULTIPLAYER
    bool multiplayer_result=false,multiplayer_result_done=false;
#endif
    std::shared_ptr<ReplayEntry> pending_replay;
    // The platform fills the catalog asynchronously; menu transitions wait for
    // completion exactly as the original directory-scanner thread did.
    void replay_scan_complete(){flags|=8;page.count=i32(replay_files.size()/25);page.select(page.selected);}
    bool load_music_comments(const u8*,u32);
    std::string error;
    bool update(u32 pressed,u32 repeat);
    void queue_ascii(AsciiText&)const;
    void change(TitleScreen);
    void step(i32);
    bool unlocked()const;
    bool available(i32 character,i32 partner)const;
private:
    AnmManager& animations;AnmResource& title;AnmResource& variant;AnmResource& ascii;ScoreFile& scores;
    u32 create(i32 script,i32 slot=-1,i32 file=0);
    void close(i32 slot);
    void interrupt(u32 id,i32 label,bool immediate=false);
    void family(i32 slot,i32 label,bool immediate=false){interrupt(handles[slot],label,immediate);}
    u32 child(i32 slot,i32 script);
    void hide(u32 id);
    bool exists(i32 slot){return animations.find(handles[slot])!=nullptr;}
    void disable_extra();
    void highlight(i32 slot,i32 base=7);
    void move(u32 keys,u32 negative,u32 positive);
    void main(u32,u32);
    void difficulty(u32,u32);
    void character(u32,u32);
    void partner(u32,u32);
    void practice(u32,u32);
    void options(u32,u32);
    void refresh_options();
    void keys(u32,u32);
    void refresh_keys();
    void restore_keys();
    void music_room(u32,u32);
    void music_comment();
    void music_position(i32,bool);
    void replays(u32,u32);
    void records(u32,u32);
    void record_spells();
    void results(u32,u32);
    void replay_save(u32,u32);
    void name_begin();
    void name_move(u32);
    // Returns -1 for an empty backspace, 0 for an edit, 1 for End.
    i32 name_confirm();
};
}
