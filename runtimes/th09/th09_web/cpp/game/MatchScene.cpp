#include "MatchScene.hpp"
#include <cmath>
namespace th09 {
namespace {i32 product(i32 a,i32 b){return signed_bits(u32(a)*u32(b));}}
void MatchScene::initialize(){
    for(i32 n=0;n<7;++n)services.start_animation(animations[n],OverlayResource::front,n);
    for(i32 n=7;n<9;++n){services.start_animation(animations[n],OverlayResource::ascii,0);animations[n].color1.a=0;}
    if(rules.mode!=GameMode::versus){services.start_animation(animations[17],OverlayResource::front,85);services.start_animation(animations[18],OverlayResource::front,86);services.set_sprite(animations[18],rules.progress.stage+73);}
    for(u32 side=0;side<2;++side){const float left=side?335.f:15.f,right=side?624.f:304.f;
        borders[side]={AttackColorVertex{{left,15,0},1,0x80ffffff},{{left,464,0},1,0x80ffffff},{{right,464,0},1,0x80ffffff},{{right,15,0},1,0x80ffffff},{{left,15,0},1,0x80ffffff}};}
    reset_round();
}
void MatchScene::reset_round(){services.begin_dialogue(rules.progress.round==0?services.route.opening:201,0);phase=ending_frames=winner=rewards_blocked=0;}
void MatchScene::end_round(i32 side){
    if(side<0||side>1)return;rewards_blocked=1;rules.progress.round=wrapping_add(rules.progress.round,1);services.wins[side]=wrapping_add(services.wins[side],1);
    if(services.wins[side]>=services.wins_required){
        for(u32 n=0;n<3;++n)services.start_animation(animations[10+n],OverlayResource::front,60+n);
        for(u32 n=11;n<13;++n)animations[n].pos.x=animations[n].pos2.x=-999;
        services.set_sprite(animations[11+side],44);services.set_sprite(animations[12-side],45);
    }winner=side;phase=1;ending_frames=retry=0;
}
void MatchScene::show_results(){
    services.start_animation(animations[13],OverlayResource::front,63);services.start_animation(animations[16],OverlayResource::front,66);phase=2;ending_frames=0;
    const auto& p=services.players[winner];result[0]=product(p.health,100000);result[1]=p.best_combo<1000?product(p.best_combo,10000):9999990;
    result[2]=p.spells<67?product(p.spells,150000):9999990;result[3]=p.bosses<34?product(p.bosses,300000):9999990;result[4]=p.counters<34?product(p.counters,300000):9999990;
    const float lives=rules.scores[winner].lives;result[5]=rules.progress.stage==8?(lives<10?product(i32(lives),10000000):99999990):0;
    result[6]=wrapping_add(wrapping_add(wrapping_add(wrapping_add(wrapping_add(result[5],result[2]),result[3]),result[4]),result[1]),result[0]);
    if(rules.mode!=GameMode::versus&&winner==0){
        services.record_defeat(services.opponent_character);
        if(rules.progress.stage==8){services.start_animation(animations[14],OverlayResource::front,64);rules.scores[0].extends=9999;}
        if(services.starting_extra_lives&&rules.mode==GameMode::story){services.start_animation(animations[15],OverlayResource::front,65);result[6]=0;}
    }rules.scores[winner].add(result[6]);
}
void MatchScene::boss_position(i32 side,const Vec3& position){
    if(side<0||side>1)return;auto& arrow=animations[7+side];
    if(position.x>=-112&&position.x<=112){const float distance=std::fabs(services.players[side].position.x-position.x);arrow.color1.a=distance>=64?255:u8(i32(distance)*3+64);arrow.pos=services.geometry[side].to_screen(position);arrow.pos.y=472;arrow.pos.z=0;}else arrow.color1.a=0;
}
void MatchScene::update(){
    for(u32 n=0;n<7;++n)services.advance_animation(animations[n]);services.advance_animation(animations[17]);services.advance_animation(animations[18]);services.advance_animation(animations[9]);
    if(displayed_music!=services.music_track){displayed_music=services.music_track;if(displayed_music>=0){services.start_animation(animations[9],OverlayResource::front,87);services.set_sprite(animations[9],displayed_music+82);}}
    for(u32 side=0;side<2;++side){u32 color=0x80ffffff;if(flash_frames[side]>0){color=flash_frames[side]&1?flash_colors[side]:0xffffffff;--flash_frames[side];}for(auto& v:borders[side])v.color=color;}
    services.update_dialogue();if(!phase)return;
    for(u32 n=10;n<13;++n)services.advance_animation(animations[n]);
    if(phase>1){
        for(u32 n=13;n<17;++n)services.advance_animation(animations[n]);
        if(services.dialogue_state()<0){
            if(rules.mode==GameMode::versus)services.transition(SceneTransition::match_complete);
            else if(winner)services.transition(SceneTransition::game_over);
            else services.transition(rules.progress.stage<8?SceneTransition::next_stage:services.game_flags&8?SceneTransition::title:SceneTransition::ending);
        }
    }else if(ending_frames==0){
        services.game_flags=(services.game_flags&~0x1800u)|(1u<<u32(12-winner));services.reset_attack_timer(1-winner);
        services.clear_round_hazards(0);services.clear_round_hazards(1);services.flush_combo(0);services.flush_combo(1);
    }else if(ending_frames==30){
        services.play_sound(49,0);services.background_transition(1-winner,1,30);services.fade(1,60,16,1-winner);services.fade(1,60,4,winner);
    }else if(ending_frames==60){
        if(rules.mode==GameMode::versus){if(services.wins[0]>=services.wins_required||services.wins[1]>=services.wins_required)services.victory_dialogue(winner);else retry=1;}
        else if(winner&&rules.scores[0].lives>0){rules.scores[0].lives-=1;services.players[0].rounds_lost+=1;retry=1;}
        else services.begin_dialogue(winner?services.route.defeat:services.route.victory,0);
    }else if(retry){
        if(ending_frames==160){services.fade_hud(0);services.fade_hud(1);}
        else if(ending_frames==190){services.restart_round();if(rules.mode!=GameMode::versus){const i32 round=rules.progress.round;services.cpu_level=StageSelection::story_policy(rules.world_difficulty(),rules.progress.stage,round<4||!(services.game_flags&0x4000)?round+1:4);}}
    }
    ending_frames=wrapping_add(ending_frames,1);
}
void MatchScene::place_draw(AnmVm& a,i32 side){const auto p=services.geometry[side].to_screen(a.pos2);a.pos.x=p.x;a.pos.y=p.y;services.draw_animation(a);}
void MatchScene::draw(){
    services.begin_view(2);for(u32 n=0;n<9;++n)services.draw_animation(animations[n]);for(auto& b:borders)services.outline(b.data(),4);services.draw_dialogue();
    if(phase){
        services.draw_animation(animations[10]);services.begin_view(0);place_draw(animations[11],0);
        if(phase>1&&winner==0)for(u32 n=13;n<17;++n)place_draw(animations[n],0);
        services.begin_view(1);place_draw(animations[12],1);
        if(phase>1){
            if(winner==1){place_draw(animations[13],1);place_draw(animations[16],1);}
            if(ending_frames>20){Vec3 position{winner?544.f:224.f,226,0};for(u32 n=0;n<5;++n){services.number(position,7,result[n]);position.y+=16;}position.x-=9;if(rules.progress.stage==8)services.number(position,8,result[5]);position.y+=32;services.number(position,8,result[6]);}
        }
    }
    services.begin_view(2);const i32 t=rules.progress.total_frames;services.clock({rules.mode==GameMode::versus?297.f:260.f,0,0},rules.mode==GameMode::versus?-1:rules.progress.stage+1,t/3600,t/60%60);
    services.draw_animation(animations[9]);services.draw_animation(animations[17]);services.draw_animation(animations[18]);
}
}
