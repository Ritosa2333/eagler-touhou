#ifdef TH11_MULTIPLAYER
#include "GameSession.hpp"
#include <cmath>
namespace th11 {
namespace {u32 menu_keys(u32 held){return (held&~256u)|((held&256u)?0x80000u:0u);}}
bool GameSession::begin_multiplayer(GameResources& data,const MultiplayerOptions& options){
    if(!options.valid())return fail("Invalid TH11 multiplayer options");
    multiplayer_active=true;multiplayer_options=options;multiplayer_frame=0;multiplayer_pause_held={};
    reset_state();title.reset();practice={};interactive=false;next_continues=0;
    // A negotiated restart or replay seek can rebuild from a paused session.
    // Native owners bind their clocks during begin(), so restore normal time
    // before constructing the new shared world, without changing ordinary runs.
    animations.rate=paused_rate=1;
    animations.script_rng={u16(options.seed),0,0};animations.visual_rng={u16(options.seed>>16),0,0};
    scores.initialize(animations.script_rng);scores.read_records(spell_records,clear_records);
    for(auto& selection:clear_records.clears)selection.fill(1);
    for(auto& selection:clear_records.stages)for(auto& stage:selection)stage={1,1};
    scores.write_records(spell_records,clear_records);
    // Score initialization consumes random filler bytes. Gameplay always starts
    // from the negotiated seed after this memory-only unlocked profile exists.
    animations.script_rng={u16(options.seed),0,0};animations.visual_rng={u16(options.seed>>16),0,0};
    if(!begin(data,options.stage,options.selections[0]/3,options.selections[0]%3,options.difficulty,false,false,false))return false;
    multiplayer_frame=0;return true;
}
bool GameSession::update_multiplayer(const std::array<MultiplayerInput,3>& inputs){
    if(!multiplayer_active||!battle||!error.empty())return false;
    bool pause_edge=false;
    for(unsigned i=0;i<multiplayer_options.seat_count;++i){const auto& in=inputs[i];if(in.touch_mode<0||in.touch_mode>3||!std::isfinite(in.touch_x)||!std::isfinite(in.touch_y))return fail("Invalid MP movement input");pause_edge|=in.pause&&!multiplayer_pause_held[i];multiplayer_pause_held[i]=in.pause;}
    ++multiplayer_frame;
    if(state.phase==GameSessionPhase::finished)return true;
    if(state.phase==GameSessionPhase::ending){
        battle->events.clear();battle->dialogue_text_requests.clear();
        if(!ending)return fail("MP ending owner missing");
        menu_input.update(menu_keys(inputs[0].held));
        if(!ending->tick(menu_input.held,menu_input.pressed))return fail(ending->error.c_str());
        if(!ending->active)return multiplayer_open_clear_results(inputs[0].held);
        if(!animations.update(true)||!animations.update(false))return fail("MP ending ANM update failed");
        ++state.frame;return true;
    }
    if(state.phase==GameSessionPhase::game_over){
        battle->events.clear();battle->dialogue_text_requests.clear();menu_input.update(menu_keys(inputs[0].held));
        if(title&&title->multiplayer_result){
            title->held=menu_input.held;
            if(!title->update(menu_input.pressed,menu_input.long_repeat))return fail(title->error.c_str());
            if(!animations.update(true)||!animations.update(false))return fail("MP clear result ANM update failed");
            if(title->multiplayer_result_done)state.phase=GameSessionPhase::finished;
        }else{
            if(!pause_menu||!pause_menu->multiplayer_result)return fail("MP result owner missing");
            if(!pause_menu->update(menu_input.pressed,menu_input.long_repeat))return fail(pause_menu->error.c_str());
            if(!animations.update(true))return fail("MP result ANM update failed");
            if(pause_menu->action==PauseAction::Title)state.phase=GameSessionPhase::finished;
        }
        return true;
    }
    if(pause_edge&&state.phase==GameSessionPhase::stage){
        // Native menu overlays have their own fixed-rate presentation owner.
        // The shared combat ANMs and their bound clocks stay frozen at rate 0.
        battle->mp_presentation.rate=1;
        pause_menu=std::make_unique<PauseMenu>(battle->mp_presentation,resources.core.text,resources.core.front,scores);
        if(!pause_menu->begin_multiplayer_pause()||!battle->mp_presentation.update(true))return fail("MP pause presentation initialization failed");
        menu_input={};menu_input.update(menu_keys(inputs[0].held));
        paused_rate=animations.rate;animations.rate=0;state.phase=GameSessionPhase::paused;battle->mp_paused=true;return true;
    }
    // Restart/exit are protocol-generation operations. Never restart the world
    // from R/Enter in a still-live network generation.
    if(state.phase==GameSessionPhase::paused){
        battle->events.clear();battle->dialogue_text_requests.clear();
        if(!pause_menu||!pause_menu->multiplayer_pause)return fail("MP pause owner missing");
        menu_input.update(menu_keys(inputs[0].held));
        if(!pause_menu->update(menu_input.pressed|(pause_edge?256u:0u),menu_input.long_repeat)||!battle->mp_presentation.update(true))return fail("MP pause presentation update failed");
        if(pause_menu->action==PauseAction::Resume){pause_menu.reset();animations.rate=paused_rate;state.phase=GameSessionPhase::stage;battle->mp_paused=false;}
        return true;
    }
    if(state.phase!=GameSessionPhase::stage)return fail("Invalid MP phase");
    if(!battle->update([&]{battle->mp_set_input(inputs);return true;})){error="MP battle update failed: "+std::to_string(battle->last_error)+" "+battle->error;return false;}
    if(battle->game_over_requested)return multiplayer_open_result(false,inputs[0].held);
    const auto exit=battle->completion.state.exit;
    if(exit==StageExit::NextStage){const u32 next=u32(battle->completion.state.next_stage);if(!source||!battle->next_stage(*source,next))return fail("MP stage transition failed");state.stage=next;state.frame=0;}
    else if(exit==StageExit::Ending)return multiplayer_open_ending(inputs[0].held);
    else if(exit==StageExit::Results)return multiplayer_open_result(true,inputs[0].held);
    else if(exit==StageExit::ReplayEnd||exit==StageExit::Title)return multiplayer_open_result(false,inputs[0].held);
    ++state.frame;return true;
}
MultiplayerSeatView GameSession::multiplayer_seat(unsigned seat)const noexcept{return battle?battle->mp_seat(seat):MultiplayerSeatView{};}
bool GameSession::multiplayer_open_result(bool completed,u32 held){
    battle->mp_finished=true;battle->mp_paused=false;animations.rate=1;
    pause_menu=std::make_unique<PauseMenu>(animations,resources.core.text,resources.core.front,scores);
    pause_menu->selection=state.character*3+state.subtype;pause_menu->difficulty=state.difficulty;pause_menu->stage=state.stage;
    pause_menu->result_score=economy.score_units;
    if(!pause_menu->begin_multiplayer_end(completed))return fail(pause_menu->error.c_str());
    // Seed the native edge sampler from the final combat frame. Holding Shoot
    // through the wipe or clear cannot confirm the result menu on entry.
    menu_input={};menu_input.update(menu_keys(held));
    if(!animations.update(true))return fail("MP result first ANM update failed");
    state.phase=GameSessionPhase::game_over;return true;
}
bool GameSession::multiplayer_open_ending(u32 held){
    if(!source||!battle)return fail("MP ending requires completed combat owner");
    battle->mp_finished=true;battle->mp_paused=false;battle_attached=false;
    completed_run=true;completed_score=economy.score_units;completed_continues=0;
    // Retire only the combat presentation. Permanent seat/world owners and
    // their final resources remain available for MP replay/session metadata.
    animations.clear();compositor.reset();animations.rate=1;
    if(!compositor.initialize(resources.core.text))return fail("MP ending compositor initialization failed");
    compositor.reset_cameras(false);
    ending=std::make_unique<Ending>(animations,resources.core.text,scores);
    if(!ending->begin(*source,state.character*3+state.subtype,state.difficulty,0))return fail(ending->error.c_str());
    if(resources.effects)for(auto& entry:ending->resources)if(!resources.effects->prepare_animation(entry.second))return fail("MP ending texture preparation failed");
    menu_input={};menu_input.update(menu_keys(held));state.frame=0;state.phase=GameSessionPhase::ending;
    return true;
}
bool GameSession::multiplayer_open_clear_results(u32 held){
    animations.clear();
    if(ending&&resources.effects)for(auto& entry:ending->resources)resources.effects->release_animation(entry.second);
    ending.reset();compositor.reset();animations.rate=1;
    if(!compositor.initialize(resources.core.text))return fail("MP clear result compositor initialization failed");
    compositor.reset_cameras(false);
    title=std::make_unique<TitleMenu>(animations,resources.core.title,resources.core.title_variant,resources.core.ascii,scores);
    title_ascii=std::make_unique<AsciiText>(resources.core.ascii);title->text_resource=&resources.core.text;title->flags=0;
    title->selection.character=state.character;title->selection.partner=state.subtype;title->selection.difficulty=state.difficulty;
    title->multiplayer_result=true;title->multiplayer_seats=multiplayer_options.seat_count;
    for(unsigned i=0;i<multiplayer_options.seat_count;++i){title->multiplayer_scores[i]=battle->pilots[i]->economy.score_units;title->multiplayer_selections[i]=multiplayer_options.selections[i];}
    title->change(TitleScreen::Results);
    if(!title->update(0,0)||!animations.update(true)||!animations.update(false))return fail("MP clear result initial ANM update failed");
    menu_input={};menu_input.update(menu_keys(held));state.phase=GameSessionPhase::game_over;return true;
}
u32 GameSession::multiplayer_hash()const noexcept{
    u32 h=battle?battle->mp_hash():0;auto word=[&](u32 v){h=(h^v)*16777619u;};word(multiplayer_frame);word(u32(state.phase));
    if(state.phase==GameSessionPhase::paused||state.phase==GameSessionPhase::game_over||state.phase==GameSessionPhase::ending){word(menu_input.held);word(menu_input.pressed);
        if(pause_menu){word(pause_menu->state);word(pause_menu->timer.current);}
        if(title){word(u32(title->screen));word(title->substate);word(title->timer.current);}
        if(ending){word(ending->frames);word(ending->flags);word(ending->index);word(ending->time.current);word(ending->wait.current);word(ending->line);auto data=ending->messages.find(ending->message);if(data!=ending->messages.end()&&ending->instruction)word(u32(ending->instruction-data->second.data()));}
    }return h;
}
}
#endif
