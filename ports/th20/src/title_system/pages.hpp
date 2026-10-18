#pragma once
#include "title.hpp"
namespace th20::source::title {
extern int data_character,data_profile,last_profile; //5c6134,5c6138,5c6144
void stop_music(); //4d9bc0
void play_music(const char*); //4d9a70 then4d9b50 withindex0
namespace unrecovered {
// Actual original page entry points still being recovered. There are no
// fallback bodies; these keep incomplete Title pages visible to the linker.
void update_difficulty_00521bc0(TitleInf&);
void update_character_00521060(TitleInf&);
void update_loadout_0052b8e0(TitleInf&);
void update_stage_00529430(TitleInf&);
void update_player_data_005265a0(TitleInf&);
void update_data_detail_00524c10(TitleInf&);
void update_replay_00523440(TitleInf&);
void update_music_005205d0(TitleInf&);
void update_spell_005223e0(TitleInf&);
void update_data_page_00526a90(TitleInf&);
void update_gallery_00528fc0(TitleInf&);
void update_gallery_page_005284f0(TitleInf&);
void update_gallery_choice_00527f00(TitleInf&);
void update_stones_0052a8a0(TitleInf&);
void draw_stage_00529b40(TitleInf&);
void draw_data_detail_005257f0(TitleInf&);
void draw_replay_005240d0(TitleInf&);
void draw_music_00520c80(TitleInf&);
void draw_spell_00522e20(TitleInf&);
void draw_data_page_005277f0(TitleInf&);
void draw_gallery_00528a50(TitleInf&);
void draw_stones_0052b480(TitleInf&);
}
}
