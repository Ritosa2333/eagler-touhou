// Original result presentation; MIT reference GensokyoClub/th08.
#include "ResultView.hpp"
#include "SpellCatalogData.hpp"
#include <algorithm>
namespace th08 {
namespace {
float shift(float value,float delta){return Scalar::add(value,delta);}
constexpr i32 stage_numbers[]{1,2,3,4,4,5,6,6,1,1,1};
constexpr const char* short_difficulties[]{"E","N","H","L","-"};
constexpr const char* aligned_difficulties[]{"      Easy","    Normal","      Hard","   Lunatic","     Extra"};
constexpr const char* characters[]{"Rm & Yk","Ms & Al","Sk & Rr","Ym & Yy","Reimu  ","Yukari ","Marisa ","Alice  ","Sakuya ","Remilia","Youmu  ","Yuyuko "};
constexpr char alphabet[]="ABCDEFGHIJKLMNOP" "QRSTUVWXYZ.,:;_@" "abcdefghijklmnop" "qrstuvwxyz+-/*=%" "0123456789#!?'\"$" "(){}[]<>&\\|~^ --";
void cursor_mark(char (&mark)[9],i32 cursor){std::memcpy(mark,"        ",9);if(cursor>=0)mark[std::min(cursor,7)]='_';}
}
void ResultView::high_scores(Vec3 pos){
    if(u32(state.selectedDifficulty)>=5||u32(state.selectedHighScoreCharacter)>=12)return;
    pos.y=shift(pos.y,18);pos.x=shift(pos.x,24);ascii.state.color=0xffe0e0ef;add(pos,"No  Name       Score(Stage)   Date   Slow");pos.y=shift(pos.y,18);
    const auto& list=scores.list(state.selectedDifficulty,state.selectedHighScoreCharacter);
    for(u32 i=0;i<std::min<std::size_t>(10,list.size());i++){
        const auto& entry=list[i];const bool typing=state.currentState==RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME;
        ascii.state.color=typing?(entry.header.unknown?0xfff0f0ff:0xc0ffc0c0):0xffffc0c0;add(pos,"%2d",i+1);pos.x=shift(pos.x,48);
        if(typing&&entry.header.unknown){char mark[9];cursor_mark(mark,state.cursor);add(pos,"%8s",mark);}
        char name[10]{},date[7]{};std::memcpy(name,entry.name,9);std::memcpy(date,entry.date,6);
        if(entry.stage<99)add(pos,"%8s %9d%1d(%d)",name,i32(entry.score),entry.retries>=10?9:i32(entry.retries),entry.stage<11?stage_numbers[entry.stage]:0);
        else add(pos,"%8s %9d%1d(C)",name,i32(entry.score),entry.retries>=10?9:i32(entry.retries));
        pos.x=shift(pos.x,320);add(pos," %5s   %3.2f",date,double(entry.lag));pos.y=shift(pos.y,18);pos.x=shift(pos.x,-368);
    }
}
void ResultView::spell_cards(Vec3 pos){
    auto& s=state;if(u32(s.selectedSpellcardDifficulty)>=6||u32(s.previousShotType)>=13||s.spellcardPage<0)return;
    s.textVms[10].pos=pos;s.textVms[10].pos.x=shift(s.textVms[10].pos.x,320);s.textVms[10].pos.y=shift(s.textVms[10].pos.y,-16);renderer.draw_no_rotation(s.textVms[10]);pos.y=shift(pos.y,16);
    for(i32 i=0;i<10;i++){
        const i32 index=s.spellcardPage*10+i;if(index>=spell_difficulty_counts[s.selectedSpellcardDifficulty])break;
        const float x=pos.x;pos.x=shift(pos.x,320);pos.y=shift(pos.y,16);s.listingDividerSprite.pos=pos;s.listingDividerSprite.scale.x=2.375f;renderer.draw_no_rotation(s.listingDividerSprite);pos.y=shift(pos.y,-16);pos.x=x;s.textVms[i].pos=pos;
        const i32 spell=spells_by_difficulty[s.selectedSpellcardDifficulty][index],shot=s.previousShotType;const auto& record=session.records[spell];
        ascii.state.color=!record.game.attempts[shot]?0xc0c0c0ff:!record.game.captures[shot]?0xffc0a0a0:0xfff0f0ff-u32(i*0x80800);add(pos,"No.%.2d",spell+1);
        s.textVms[i].pos.x=shift(s.textVms[i].pos.x,78);renderer.draw_no_rotation(s.textVms[i]);pos.x=shift(pos.x,446);
        const auto& history=spell<205?record.game:record.practice;if(!history.attempts[shot])add(pos,"---/---(-)");else add(pos,"%3d/%3d(%s)",history.captures[shot],history.attempts[shot],record.difficulty<5?short_difficulties[record.difficulty]:"-");
        pos.x=shift(pos.x,-446);pos.x=shift(pos.x,424);pos.y=shift(pos.y,-13);ascii.state.color=0xffa08090;ascii.state.scale_x=ascii.state.scale_y=.8f;
        if(record.game.captures[shot])add(pos,"MaxBonus %8d",record.game.max_bonus[shot]);
        pos.x=shift(pos.x,-424);pos.y=shift(pos.y,13);ascii.state.scale_x=ascii.state.scale_y=1;
        const i32 distance=!s.updateSpellcardResults?33:s.frameTimer<10?(10-s.frameTimer)*33/10:(s.frameTimer-10)*33/10;pos.y=shift(pos.y,Extended::from_int(distance).to_float());
    }
    if(s.frameTimer>=20)s.updateSpellcardResults=0;
}
void ResultView::keyboard(){
    Vec3 pos{160,356,0};for(i32 row=0;row<6;row++){
        for(i32 column=0;column<16;column++){
            float offset=0;if(state.selectedCharacter==row*16+column){
                ascii.state.color=0xffffffc0;const auto phase=Extended::from_int(state.frameTimer%32)*number(.8f)/number(32);const float scale=(state.frameTimer%64<32?phase+number(1.2f):number(2)-phase).to_float();ascii.state.scale_x=ascii.state.scale_y=scale;offset=(-(number(scale)-number(1))*number(8)).to_float();
            }else{ascii.state.color=0xc0c0c0c0;ascii.state.scale_x=ascii.state.scale_y=1;}
            Vec3 actual=pos;actual.x=shift(actual.x,offset);actual.y=shift(actual.y,offset);char letter[]{alphabet[row*16+column],0};if(row==5&&column>=14)letter[0]=char(128+column-14);ascii.add_string(actual,letter,context.software_texturing);pos.x=shift(pos.x,20);
        }pos.x=shift(pos.x,-320);pos.y=shift(pos.y,18);
    }
}
void ResultView::replays(){
    for(i32 i=49;i<55;i++)renderer.draw_no_rotation(state.spriteVms[i]);add(state.spriteVms[55].pos,"No.   Name     Date   Player Score");
    for(i32 i=0;i<15;i++){
        const auto pos=state.spriteVms[56+i].pos;ascii.state.color=i==state.selectedReplay?0xffff8080:0xff808080;
        if(state.currentState==RESULT_SCREEN_STATE_WRITING_REPLAY_NAME){
            add(pos,"No.%.2d %8s %5s  %7s %9d0",i+1,state.lastName,state.currentReplay.date,characters[u32(context.character)<12?context.character:0],i32(state.currentReplay.spell_score));ascii.state.color=0xfff0f0ff;char mark[9];cursor_mark(mark,state.cursor);add(pos,"      %8s",mark);
        }else{const auto& replay=state.replays[i];if(replay.header.magic!=fourcc('T','8','R','P')||(replay.header.version&0xfff)!=6)add(pos,"No.%.2d -------- --/--  -------          0",i+1);
            else{char name[9]{},date[7]{};std::memcpy(name,replay.player_name,8);std::memcpy(date,replay.date,6);add(pos,"No.%.2d %8s %5s  %7s %9d0",i+1,name,date,characters[replay.shot_type<12?replay.shot_type:0],i32(replay.spell_score));}
        }
    }
}
i32 ResultView::final_statistics(){
    if(state.currentState!=RESULT_SCREEN_STATE_STATS_SCREEN&&state.currentState!=RESULT_SCREEN_STATE_STATS_TO_SAVE_TRANSITION)return 0;
    if(u32(context.difficulty)>=5)return 0;const auto& n=session.numbers;auto& vm=state.spriteVms[71];ascii.state.color=vm.color1.d3dColor;
    float completion=(Extended::from_int(context.total_game_frames)/number(context.difficulty<4?195559.0f:80000.0f)).to_float();Vec3 pos=vm.pos;pos.x=shift(pos.x,210);pos.y=shift(pos.y,32);add(pos,"%9d",i32(n.display_score));
    const float name_width=Extended::from_int(signed_bits(u32(ascii.state.space_width)*9)).to_float();pos.x=shift(pos.x,name_width);add(pos,"%1d",n.retries>=10?9:i32(n.retries));pos.x=shift(pos.x,-name_width);
    pos.y=shift(pos.y,22);ascii.add_string(pos,aligned_difficulties[context.difficulty],context.software_texturing);pos.x=shift(pos.x,Extended::from_int(ascii.state.space_width).to_float());pos.y=shift(pos.y,22);
    if(!(context.flags&16)){if(completion>=1)completion=.99f;add(pos,"    %3.2f%%",(number(completion)*number(100)).to_double());}else add(pos,"      100%%");
    pos.y=shift(pos.y,22);add(pos,"%9d",i32(n.retries));pos.y=shift(pos.y,22);add(pos,"%9d",Scalar::truncate(n.deaths));pos.y=shift(pos.y,22);add(pos,"%9d",Scalar::truncate(n.bombs_used));pos.y=shift(pos.y,22);add(pos,"%9d",n.captured_spells);
    const auto fraction=number(context.rendered_frames)/number(context.total_frames)-number(.5f);float rate=(fraction+fraction).to_float();if(rate<0)rate=0;else if(rate>=1)rate=1;const float lag=context.cheat_movement_used?100.f:((number(1)-number(rate))*number(100)).to_float();pos.y=shift(pos.y,22);add(pos,"    %3.2f%%",double(lag));ascii.state.color=0xffffffff;return 0;
}
i32 ResultView::draw(){
    renderer.flush();auto viewport=renderer.viewport;viewport.x=viewport.y=0;viewport.width=640;viewport.height=480;renderer.set_viewport(viewport);result.actions.draw_background();
    for(auto& vm:state.spriteVms){const auto pos=vm.pos;vm.pos={shift(pos.x,vm.pos2.x),shift(pos.y,vm.pos2.y),shift(pos.z,vm.pos2.z)};renderer.draw_no_rotation(vm);vm.pos=pos;}
    if(state.spriteVms[40].pos.x<640){if(state.currentState==RESULT_SCREEN_STATE_SPELLCARDS)spell_cards(state.spriteVms[40].pos);else high_scores(state.spriteVms[40].pos);}
    if(state.currentState==RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME||state.currentState==RESULT_SCREEN_STATE_WRITING_REPLAY_NAME)keyboard();ascii.state.scale_x=ascii.state.scale_y=1;
    if(state.currentState>=RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION&&state.currentState<=RESULT_SCREEN_STATE_OVERWRITE_REPLAY_FILE)replays();ascii.state.color=0xffffffff;final_statistics();
    if(state.currentState>=RESULT_SCREEN_STATE_OTHER_STATS_SCREEN_INIT&&state.currentState<=RESULT_SCREEN_STATE_OTHER_STATS_TO_INIT_TRANSITION)for(i32 i=0;i<20;i++)renderer.draw_no_rotation(state.textVms[i]);return 1;
}
}
