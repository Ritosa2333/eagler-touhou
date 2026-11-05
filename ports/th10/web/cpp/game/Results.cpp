#include "Results.hpp"
namespace th10 {
void Results::initialize(Results** current) noexcept {std::memset(this,0,sizeof(*this));flags=2;*current=this;}
void Results::reset_timer(const float* rate) noexcept {if(!(timer_flags&1)){elapsed.rate=rate;timer_flags|=1;}elapsed.initialize(-1);}
void Results::activate() noexcept {if(update_entry)update_entry->flags|=2;if(draw_entry)draw_entry->flags|=2;}
// 0x421f60. The result screen ends score rolling immediately.
void finalize_score_display(Gui& gui,GameEconomy& game) noexcept {gui.displayed_score=game.score;if(game.high_score<game.score)game.high_score=game.score;}
// 0x421fa0. Ties precede the existing entry; the overwritten slot's padding is
// retained. Rank comparisons use score/10 and store the final decimal separately.
i32 insert_high_score(CharacterRecord& record,ResultsEnvironment& env){
    auto& game=*env.game;i32 index=0;for(;index<10;++index)if(record.high_scores[game.difficulty][index].score<=game.score)break;if(index==10)return -1;
    for(i32 i=9;i>index;--i)record.high_scores[game.difficulty][i]=record.high_scores[game.difficulty][i-1];
    record.high_scores[game.difficulty][index].score=game.score;record.high_scores[game.difficulty][index].score_units=game.score_units;record.high_scores[game.difficulty][index].stage=game.stage;
    env.timestamp(record.high_scores[game.difficulty][index].timestamp);std::memcpy(record.high_scores[game.difficulty][index].name,"        ",9);
    record.high_scores[game.difficulty][index].slow_rate=env.cheat_movement_used&&*env.cheat_movement_used?100.f:(number(100)-Extended::from_double(*env.active_time)/Extended::from_double(*env.total_time)*number(100)).to_float();return index;
}
// 0x422ab0 / 0x422c30. Paused menus always tick at unit rate and restore the
// prior rate only on resumption or dismissal.
void Results::pause(ResultsEnvironment& env){
    state=1;reset_timer(env.rate);*env.controller_flags|=0x10;
    background_animation=env.create_animation(**env.background_file,0);env.capture_background(background_animation);
    animation_file=(*env.gui)->animations;menu_animation=env.create_animation(*animation_file,121);env.interrupt(menu_animation,3);env.sound(32);env.music_command(6,"Pause");
    if(*env.replay_mode)env.delete_animation(env.child(menu_animation,117));saved_rate=*env.rate;*env.rate=1;
}
void Results::resume(ResultsEnvironment& env){*env.controller_flags&=~0x10u;env.music_command(7,"UnPause");env.delete_animation(auxiliary_animation);auxiliary_animation=0;*env.rate=saved_rate;}
// 0x4231d0 / 0x423370. Replay playback skips the interactive result UI.
void Results::show_result(bool clear,ResultsEnvironment& env){
    if(*env.replay_mode==1){*env.pending_screen=(*env.engine_flags&0x1000)?2:4;return;}
    state=6;reset_timer(env.rate);*env.controller_flags|=0x10;
    background_animation=env.create_animation(**env.background_file,0);env.capture_background(background_animation);
    animation_file=(*env.gui)->animations;menu_animation=env.create_animation(*animation_file,clear?129:128);env.result_music();
    if(*env.display_flags&0x10)env.music_command(4,"dummy");env.music_command(2,"dummy");
    (*env.scores)->settings.statistics[0x22]=1;cleared=clear;saved_rate=*env.rate;*env.rate=1;
}
void Results::dismiss(ResultsEnvironment& env){state=13;reset_timer(env.rate);env.interrupt(background_animation,1);env.interrupt(menu_animation,1);*env.rate=saved_rate;}
// 0x423570 retains the original Extra stage marker behavior: after assigning
// stage 8 the subsequent stage==7 check cannot restore it.
void Results::register_score(ResultsEnvironment& env){
    auto& game=*env.game;if(game.stage==7&&cleared){*env.current_stage=env.stages+8;game.stage=game.reserved_040=8;}
    i32 rank=insert_high_score((*env.scores)->characters[game.character*3+game.shot_type],env);
    if(game.stage==7&&cleared){*env.current_stage=env.stages+7;game.stage=game.reserved_040=7;}
    if(rank<0){no_rank=1;return;}menu.item_count=25;menu.wrap=1;menu.select(rank);initialize_name(env,false);no_rank=0;
}
void Results::initialize_name(ResultsEnvironment& env,bool for_replay){
    keyboard.select(0);keyboard.item_count=std::strlen(env.alphabet);keyboard.wrap=1;
    if(for_replay){auto* info=(*env.replay)->info;env.timestamp(info->timestamp);(*env.replay)->info->last_stage=cleared&&!(env.game->flags&0x10)?8:env.game->stage;}
    const auto* last=(*env.scores)->settings.last_name;std::memcpy(name,last,std::strlen(last)+1);name_length=0;
    if(std::memcmp(name,"        ",9))keyboard.move(-1);
    i32 length=8;while(length>0&&name[length-1]==' ')--length;name_length=length;
}
i32 Results::update(ResultsEnvironment& env){
    if(!state){if(!(env.game->flags&0x20)&&((*env.pressed&8)||(*env.engine_flags&0x10))&&*env.controller_update&&((*env.controller_update)->flags&2)&&env.controller_elapsed->current>=30)pause(env);}
    else if(state>=1&&state<=5)update_pause(env);else if(state>=6&&state<=13)update_results(env);
    elapsed.tick();return 1;
}
}
