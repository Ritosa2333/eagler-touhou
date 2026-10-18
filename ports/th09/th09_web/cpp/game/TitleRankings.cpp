#include "TitleMenus.hpp"
#include <algorithm>
#include <cstdio>
namespace th09 {
namespace {
#include "TitleData.inc"
}
void TitleMenus::ranking_portrait(i32 character){for(i32 n=0;n<16;++n){auto& a=animations[60+n*2];resources.sprite(AnimationFile::menu,a,a.baseSpriteIndex+1);a.pendingInterrupt=18;}auto& a=animations[60+character*2];resources.sprite(AnimationFile::menu,a,a.baseSpriteIndex);a.pendingInterrupt=17;}
bool TitleMenus::rankings(){
    auto& s=state;const auto& in=input[2];
    if(!s.state){if(!s.load_frame){if(!output.title_background("result00.png"))return false;interrupt(16);advance();s.selection=s.ranking_character=0;ranking_portrait(ranking_order[0]);s.state=s.frames=0;s.description=-1;s.ranking_slot=0;}if(s.load_frame==8)s.state=1;}
    else if(s.state==1){auto& progress=settings.secret_progress;const u16 expected=progress<3?0x2000:progress<7?0x400:progress<8?0x200:0x4000;if(progress<10){if(in.pressed&expected)++progress;else if(in.pressed)progress=0;}if(progress>9){settings.versus_unlocked.fill(1);std::fill_n(settings.story_unlocked.begin(),14,1);std::fill_n(settings.extra_unlocked.begin(),14,1);auto& records=output.title_records();records.versus_unlocked=settings.versus_unlocked;records.story_unlocked=settings.story_unlocked;records.extra_unlocked=settings.extra_unlocked;output.title_sound(28);progress=0;}
        i32 step=0;const u16 keys=in.pressed|in.repeat;if(keys&64)step=-1;else if(keys&128)step=1;if(step){output.title_sound(12);s.selection=(s.selection+step+14)%14;for(i32 attempts=0;attempts<16&&!settings.story_unlocked[s.selection]&&!settings.extra_unlocked[s.selection];++attempts)s.selection=(s.selection+step+16)%16;const i32 character=ranking_order[s.selection];ranking_portrait(character);s.ranking_character=character;}
        if(in.pressed&10){output.title_sound(11);change(TitleScreen::main);s.selection=4;return true;}}
    tick();return true;
}
void TitleMenus::move_name_cursor(){auto& s=state;const u16 keys=input[2].pressed|input[2].repeat;if(keys&16){output.title_sound(12);s.selection=(s.selection+80)%96;if(s.selection==93)s.selection=77;s.frames=0;}if(keys&32){output.title_sound(12);s.selection=(s.selection+16)%96;if(s.selection==93)s.selection=13;s.frames=0;}if(keys&64){output.title_sound(12);s.selection=s.selection%16==0?s.selection+15:s.selection-1;if(s.selection==93)s.selection=92;s.frames=0;}if(keys&128){output.title_sound(12);s.selection=s.selection%16==15?s.selection-15:s.selection+1;if(s.selection==93)s.selection=94;s.frames=0;}}
bool TitleMenus::score_name(){
    auto& s=state;const auto& in=input[2];auto& records=output.title_records();
    if(!s.state){if(!s.load_frame){output.title_music(-1);if(!output.title_background("result00.png")||!create_animations())return false;interrupt(16);advance();if(i32(result_mode)==2){change(TitleScreen::replay_name);return true;}s.ranking_character=score_candidate.character;s.name_cursor=0;ranking_portrait(s.ranking_character);s.selection=95;s.state=s.frames=0;s.description=-1;s.ranking_slot=records.insert(score_candidate);if(s.ranking_slot<5){auto& entry=records.scores[s.ranking_character][settings.difficulty][s.ranking_slot];std::copy_n(records.last_name.begin(),8,entry.name.begin());entry.name[8]=0;}}if(s.load_frame==8)s.state=1;}
    else if(s.state==1){if(s.ranking_slot<5)move_name_cursor();if(in.pressed&0x1001){if(s.selection==95){output.title_save_records();output.title_sound(11);change(TitleScreen::replay_name);if(s.ranking_slot<5){const auto& entry=records.scores[s.ranking_character][settings.difficulty][s.ranking_slot];std::copy_n(entry.name.begin(),9,records.last_name.begin());}return true;}
        if(s.ranking_slot<5){auto& name=records.scores[s.ranking_character][settings.difficulty][s.ranking_slot].name;name[s.name_cursor]=s.selection==94?' ':name_characters[s.selection];output.title_sound(10);if(s.name_cursor<7)++s.name_cursor;else s.selection=95;s.frames=0;}}
        if(in.pressed&10){output.title_sound(11);if(s.ranking_slot<5){auto& name=records.scores[s.ranking_character][settings.difficulty][s.ranking_slot].name;if(s.name_cursor==7&&name[7]!=' ')name[7]=' ';else if(s.name_cursor>0)name[--s.name_cursor]=' ';}s.frames=0;}}
    tick();return true;
}
void TitleMenus::draw_name_grid(float x,float y){for(i32 row=0;row<6;++row)for(i32 column=0;column<16;++column){const i32 n=row*16+column;char text[2]{name_characters[n],0};if(n==94)text[0]=127;else if(n==95)text[0]=char(128);float scale=1,offset=0;u32 color=0xc0c0c0c0;if(n==state.selection){const i32 phase=state.frames%64;scale=phase<32?float(phase%32)*.025f+1.2f:2.f-float(phase%32)*.025f;offset=(scale-1.f)*-8.f;color=0xffffffc0;}output.title_ascii({x+float(column)*12.f+offset,y+float(row)*16.f+offset,0},text,color,{scale,scale});}}
void TitleMenus::draw_rankings(){
    const auto& s=state;auto& records=output.title_records();const bool entering=s.screen==TitleScreen::score_name;const i32 selected=entering?s.ranking_slot:99;const u32 alpha=u32(std::min(255,s.load_frame*255/30))<<24;
    constexpr const char* titles[]={"Easy Ranking","Normal Ranking","Hard Ranking","Lunatic Ranking","Extra Ranking"};constexpr u32 colors[]={0xd0ffd0,0xd0ffd0,0xfff0d0,0xffc0c0,0xffc0f0};
    for(i32 d=0;d<5;++d){const bool active=!entering||d==settings.difficulty;const float x=d&1?358.f:102.f,y=80.f+float(d/2)*112.f;output.title_ascii({x,y,0},titles[d],alpha|(active?colors[d]:0x808080),{1,1});
        for(i32 rank=0;rank<5;++rank){const auto* entry=records.score(s.ranking_character,d,rank);if(!entry)continue;const u32 color=alpha|(!active?0x809090:rank==selected?0xfff0ef:0xc0f0ff);const float row_y=y+float(rank+1)*16.f;if(active&&rank==selected)output.title_ascii({x+float((s.name_cursor+4)*9),row_y,0},"_",color,{1,1});char row[96];std::snprintf(row,sizeof(row),"%s %.8s %.9d%d",ranking_labels[rank],entry->name.data(),signed_bits(entry->points),i32(i8(entry->continues)));output.title_ascii({x,row_y,0},row,color,{1,1});}}
    if(entering&&selected<5&&s.state>0)draw_name_grid(360,336);
}
}
