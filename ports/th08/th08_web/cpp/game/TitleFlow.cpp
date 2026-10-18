// TH08 title startup and resource-loading order. Platform presentation is
// asynchronous; the game data transitions retain the original ordering.
#include "TitleFlow.hpp"
namespace th08 {
const char* TitleFlow::begin(){
    auto& context=menus.context;state.screen_effect_counter=0;state.pause_state=0;ascii.reset();ascii.initialize_vms(ascii_context);game_config={};game_values={};state.frame_rate=1;
    if(context.flags.isReplay)context.character=0;if(context.flags.isDemoMode)context.flags.isReplay=false;
    information_count=information_index=0;
    if(context.IsExtraUnlocked()&&!context.flags.isExtraUnlocked)information[information_count++]="title/info00.jpg";
    if(context.IsSpellPracticeUnlocked()&&!context.flags.isSpellPracticeUnlocked)information[information_count++]="title/info02.jpg";
    if(context.IsExtraUnlockedWithAllTeams()&&!context.flags.isExtraUnlockedWithAllTeams)information[information_count++]="title/info01.jpg";
    if(information_count)return information[0];finish_begin();return nullptr;
}
const char* TitleFlow::complete_information(){
    if(information_index<information_count&&++information_index<information_count)return information[information_index];
    if(information_count){information_count=0;finish_begin();}return nullptr;
}
void TitleFlow::finish_begin(){
    auto& context=menus.context;auto& title=menus.state;const auto data=actions.read_score();ScoreFile score;score.decode(data.data(),data.size());
    score.clears(context.clears);score.practice(context.practice_scores);score.spells(context.spells);score.copy_chapter(fourcc('F','L','S','P'),1,&context.last_words,sizeof(LastWords),true);
    context.flags.isExtraUnlocked=context.IsExtraUnlocked();context.flags.isSpellPracticeUnlocked=context.IsSpellPracticeUnlocked();context.flags.isExtraUnlockedWithAllTeams=context.IsExtraUnlockedWithAllTeams();menus.UnlockLastWordSpellCards();
    title.currentScreen=TitleCurrentScreen_StartMenu;for(auto& value:state.transition_counters)value=0;
    switch(context.supervisor_previous){case 2:case 3:case 6:title.cursor=context.difficulty>=4;break;case 5:title.cursor=5;break;case 8:title.cursor=6;break;default:title.cursor=0;break;}
    title.practiceState=0;if(context.flags.isPracticeMode){title.cursor=context.flags.isSpellPractice?2:3;title.practiceState=context.flags.isSpellPractice?2:1;}
    context.flags.isPracticeMode=context.flags.isSpellPractice=false;
    if(context.supervisor_previous==2){actions.loading(true);actions.start_effect();}else if(context.supervisor_previous!=0)actions.loading(false);
    context.flags.isDemoMode=false;title.state=TitleScreenState_Loading;actions.begin_loading();
}
bool TitleFlow::setup(){
    if(state.capture_pending)return false;auto& title=menus.state;auto& context=menus.context;
    title.titleAnm=actions.preload_animation(20,"title01.anm");if(!title.titleAnm){title.state=TitleScreenState_Close;return true;}
    title.resultTextAnm=actions.preload_animation(22,"resulttext.anm");if(!title.resultTextAnm){title.state=TitleScreenState_Close;return true;}
    if(state.close_requested)return true;if(!context.textAnm){title.state=TitleScreenState_Close;return true;}auto* previous_help=title.currentHelpTextVm;menus.initialize_help(*context.textAnm);title.currentHelpTextVm=previous_help;
    if(state.close_requested)return true;if(!actions.preload_background("title/title00.png")){title.state=TitleScreenState_Close;return true;}
    if(!context.flags.isDemoMode){if(context.supervisor_previous!=5)menus.actions.load_music(8,"bgm/th08_01.mid");actions.fade_in();}
    title.currentHelpTextVm=title.helpTextVms;title.state=TitleScreenState_Ready;actions.fade_loading();state.background_running=false;state.close_requested=false;state.background_wait=0;return true;
}
}
