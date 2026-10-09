#include "GameSession.hpp"
#include "MusicCatalog.hpp"

namespace th11 {

GameSession::GameSession(){scores.initialize(animations.script_rng);scores.read_records(spell_records,clear_records);}
bool GameSession::load_scores(const u8* data,u32 size){if(!scores.open(data,size))return false;scores.read_records(spell_records,clear_records);return true;}
bool GameSession::save_scores(std::vector<u8>& out){
#ifdef TH11_MULTIPLAYER
    if(multiplayer_active){out.clear();return false;}
#endif
    if(!title)scores.write_records(spell_records,clear_records);return scores.save(out);}

namespace {
bool clear_animations(AnmManager& manager) {
    manager.clear();
    return true;
}
}

void GameSession::reset_state() noexcept {
    battle.reset();
    if(ending&&resources.effects)for(auto& entry:ending->resources)resources.effects->release_animation(entry.second);
    ending.reset();
    pause_menu.reset();menu_input={};
    title.reset();title_ascii.reset();title_fades.effects.clear();
    compositor.reset();
    clear_animations(animations);
    state={};
    state.phase=GameSessionPhase::title;
    economy={};
    battle_attached=false;
    last_held=0;
    last_pause=false;
    game_input={};recording_active=false;
    error.clear();
}

bool GameSession::begin_replay(GameResources& source,const u8* data,u32 size,u32 stage,bool demo){
    if(!playback.open(data,size))return fail(playback.error().c_str());
    if(!stage)for(u32 n=1;n<=7;++n)if(playback.stage(n)){stage=n;break;}
    if(!playback.stage(stage))return fail("stage absent from replay");
    PracticeConfig candidate;const bool custom=practice.enabled&&practice_replay_read(data,size,candidate)&&candidate.mode==1&&candidate.stage==i32(stage)-1;
    return begin(source,stage,playback.character(),playback.subtype(),playback.difficulty(),demo,true,playback.decoded()[10]!=0,custom?&candidate:nullptr);
}
bool GameSession::begin(GameResources& source, u32 stage, i32 character,
                        i32 subtype, i32 difficulty, bool demo,bool replay,bool practice,const PracticeConfig* parameters) {
    PracticeConfig requested;if(parameters)requested=*parameters;
    if(title){menu_selection=title->selection;config=title->config;scores.read_records(spell_records,clear_records);}
    reset_state();
#ifdef TH_ENABLE_THPRAC
    this->practice.clear_run();
    this->practice.replay=replay;
    this->practice.assisted=this->practice.enabled&&!replay&&this->practice.cheats!=0;
    if(parameters&&this->practice.enabled){if(!requested.valid()||requested.stage!=i32(stage)-1)return fail("invalid TH11 practice run parameters");this->practice.run=requested;this->practice.active=requested.mode==1;this->practice.replay=replay;}
#endif
    completed_run=false;
    this->source=&source;
    if(character<0||character>1||subtype<0||subtype>2||difficulty<0||difficulty>4)
        return fail("invalid TH11 session selection");
    if(!resources.load_core(source)) return fail(resources.error.c_str());
    if(!compositor.initialize(resources.core.text))return fail("screen compositor initialization failed");
    state.character=character;
    state.subtype=subtype;
    state.difficulty=difficulty;
    state.demo=demo;
    state.replay=replay;
    state.practice=practice;
    initial_stage=stage;economy.rank=next_continues?-512:0;
    if(practice&&!replay)economy.lives=menu_selection.practice_start?menu_selection.practice_start-1:9;
    economy.difficulty=difficulty;
    if(!replay
#ifdef TH11_MULTIPLAYER
       &&!multiplayer_active
#endif
       &&!recording.begin(character,subtype,difficulty,practice,recording_timestamp))return fail("replay recorder initialization failed");
    if(!replay){auto* counter=scores.characters[character*3+subtype].data()+0x588;i32 n;std::memcpy(&n,counter,4);if(n<99999){++n;std::memcpy(counter,&n,4);}}
    // Original 0x41f8f0 tables at 0x4a79e8/0x4a79fc, in stored units.
    constexpr i32 minimum[]={2500000,5000000,10000000,20000000,20000000};
    constexpr i32 maximum[]={5000000,10000000,20000000,40000000,40000000};
    economy.point_value=minimum[difficulty];economy.max_point_value=maximum[difficulty];
    if(!replay&&stage>=2&&stage<=6){const auto& shot=resources.core.shots[character*3+subtype].header;economy.power=shot.max_power_level*shot.power_step;}
    return start_stage(source,stage);
}

bool GameSession::start_stage(GameResources& source, u32 stage) {
    if(stage<1||stage>7) return fail("TH11 stage outside 1..7");
    battle.reset();
    if(!clear_animations(animations)) return fail("ANM stage transition failed");
#ifdef TH_ENABLE_THPRAC
    if(practice.enabled&&practice.active){
        auto next=std::make_unique<StageResources>();
        if(!source.load_practice_stage(stage,*next,practice.run))return fail(source.error().c_str());
        if(resources.effects&&!resources.effects->prepare(*next)){resources.effects->release(*next);return fail("practice graphics preparation failed");}
        resources.retire_previous();resources.previous_stage=std::move(resources.stage);resources.stage=std::move(next);resources.stage_number=stage;
        const auto& p=practice.run;
        if(!state.replay){economy.lives=p.life;economy.life_fragments=p.life_fragment;economy.power=p.power;economy.graze=p.graze;economy.point_value=p.value*100;economy.score_units=i32(p.score/10);}
    }else
#endif
    if(!resources.load_stage(source,stage)) return fail(resources.error.c_str());
    resources.retire_previous();
    if(!compositor.stage_start())return fail("screen compositor stage reset failed");
    // The direct stage-selection path replaces the resource bundle after
    // releasing its VMs. Native overlapping outgoing/incoming backgrounds
    // still require the game-control transition state machine.
    state.stage=stage;
    state.frame=0;
    state.phase=GameSessionPhase::stage;
    battle_attached=false;
    ReplayStageState entry;
    recording_seed=animations.script_rng.seed;
    if(!state.replay)animations.script_rng.calls=0;
    if(state.replay){
        if(!playback.select(stage))return fail("stage absent from replay");
        entry.read(playback.header(stage));
        const auto& shot=resources.core.shots[state.character*3+state.subtype].header;
        economy.max_power=shot.max_power_level*shot.power_step;economy.power_step=shot.power_step;
        entry.restore(economy,animations.script_rng,true);
    }
    battle=std::make_unique<GameBattle>(resources,animations,economy,spell_records,compositor,clear_records);
#ifdef TH11_MULTIPLAYER
    if(multiplayer_active)battle->enable_multiplayer(multiplayer_options);
#endif
    battle->replaying=state.replay;battle->demo=state.demo;battle->replay_entry=entry;
#ifdef TH_ENABLE_THPRAC
    battle->practice=practice.enabled?&practice:nullptr;
    battle->practice_replay_has_stage5=state.replay&&playback.stage(5);
    battle->enemy_commands.stage_section=resources.stage->practice_stage_section;
#endif
    battle->completion.mode.control_mode=state.replay||state.demo;
    battle->completion.mode.replay_mode=state.replay?1:0;
    battle->completion.mode.practice=state.practice&&!state.replay;battle->completion.mode.replay_practice=state.practice&&state.replay;
    if(state.practice){const auto* row=scores.characters[state.character*3+state.subtype].data()+0x59c+(i32(stage)+state.difficulty*6)*8;std::memcpy(&battle->practice_high,row,4);}
    battle->hud.score.high=scores.high_score(state.character*3+state.subtype,state.difficulty);
    battle->hud.score.high_continues=scores.high_continues(state.character*3+state.subtype,state.difficulty);
    battle->hud.score.continues=next_continues;next_continues=0;
    if(state.replay)battle->hud.score.continues=entry.continues;
    if(!battle->initialize(state.character,state.subtype,state.difficulty)) {
        error="stage battle initialization failed: "+battle->error;
        battle.reset();
        return false;
    }
    constexpr i32 minimum[]={2500000,5000000,10000000,20000000,20000000};
    battle->player->state.minimum_point_value=minimum[state.difficulty];
    battle->player_input.replay=state.replay;
    battle->completion.mode.control_mode=state.replay||state.demo;
    battle->completion.mode.replay_mode=state.replay?1:0;
    battle_attached=true;
    if(!state.demo)scores.settings[0x26+stage_music(stage)]=1;
    if(!state.replay
#ifdef TH11_MULTIPLAYER
       &&!multiplayer_active
#endif
       &&!record_stage_entry(true))return false;
    return true;
}

bool GameSession::record_stage_entry(bool initial){
    if(!battle||!battle->player)return fail("recording requires a live player");
    auto& e=economy;const auto& p=battle->player->motion.state;
    ReplayStageState s;s.seed=recording_seed;s.score=e.score_units;s.power=e.power;s.points=e.point_value;
    s.lives=e.lives;s.fragments=e.life_fragments;s.rank=e.rank;s.x=p.x;s.y=p.y;s.continues=battle->hud.score.continues;
    s.focused=p.focused;s.graze=e.graze;s.weapon=p.weapon_mode;
    if(!recording.start_stage(state.stage,s,initial))return fail("replay stage recording failed");
    if(initial)for(u32 i=0;i<20;++i)battle->spell_timing.records[i]=i*0xdeaddeadu;
    recording_active=true;return true;
}
bool GameSession::save_replay(const char* name,std::vector<u8>& output,bool terminal,float slowdown){
#ifdef TH11_MULTIPLAYER
    if(multiplayer_active)return false; // MP records all confirmed seats in the shared codec.
#endif
#ifdef TH_ENABLE_THPRAC
    if(practice.enabled&&practice.assisted)return false;
#endif
    if(!battle){if(!completed_run)return false;return recording.save(name,completed_score,8,completed_continues,slowdown,output);}
    if(state.replay||!recording.selected)return false;
    recording.spell_times(battle->spell_timing.records);
    // Saving must not append multiple end markers or mutate an active run.
    auto snapshot=recording;if(terminal&&!snapshot.finish_stage(true))return false;
    const i32 reached=pause_menu&&pause_menu->completed&&!state.practice?8:i32(state.stage);
    if(!snapshot.save(name,economy.score_units,reached,battle->hud.score.continues,slowdown,output))return false;
#ifdef TH_ENABLE_THPRAC
    if(practice.enabled&&practice.active){const auto block=practice_replay_block(practice.run);if(block.empty())return false;output.insert(output.end(),block.begin(),block.end());}
#endif
    return true;
}

bool GameSession::update(const GameSessionInput& input) {
#ifdef TH11_MULTIPLAYER
    if(multiplayer_active)return fail("MP requires a confirmed input frame");
#endif
    if(!error.empty()) return false;
    if(state.phase==GameSessionPhase::finished) return true;
    GameSessionInput edges=input;
    edges.pressed|=input.held&~last_held;
    edges.released|=last_held&~input.held;
    const bool pause_edge=input.pause&&!last_pause;
    const bool confirm_edge=(edges.pressed&0x101u)!=0;
    last_held=input.held;
    last_pause=input.pause;
    if(state.phase==GameSessionPhase::ending){
        if(!ending)return fail("ending lifetime missing");
        game_input.update((input.held&~256u)|((input.held&256)?0x80000:0));
        if(!ending->tick(game_input.held,game_input.pressed))return fail(ending->error.c_str());
        if(!ending->active)return open_results();
        if(!animations.update(true)||!animations.update(false))return fail("ending ANM update failed");
        ++state.frame;return true;
    }
    if(pause_edge&&state.phase==GameSessionPhase::stage&&!state.demo&&state.frame>29) {
        menu_input.update((input.held&~256u)|((input.held&256)?0x80000:0)|256);
        return pause();
    }
    if(state.phase==GameSessionPhase::paused||state.phase==GameSessionPhase::game_over){
        if(!pause_menu)return fail("pause menu lifetime missing");
#ifdef TH_ENABLE_THPRAC
        pause_menu->practice_save_disabled=practice.enabled&&practice.assisted;
#endif
        menu_input.update((input.held&~256u)|((input.held&256)?0x80000:0)|(input.pause?256:0));
        if(!pause_menu->update(menu_input.pressed,menu_input.long_repeat))return fail(pause_menu->error.c_str());
        if(!animations.update(true))return fail("pause ANM update failed");
        switch(pause_menu->action){
        case PauseAction::Resume:return resume();
        case PauseAction::Title:return return_to_title();
        case PauseAction::Restart:return restart_run(false);
        case PauseAction::Continue:return restart_run(true);
        default:break;
        }return true;
    }
    if(state.phase==GameSessionPhase::title) {
        if(!title||!source)return fail("title lifetime missing");
#ifdef TH_ENABLE_THPRAC
        if(practice.enabled&&title->screen==TitleScreen::Practice){
            if(!practice.menu){practice.clear_run();practice.menu=true;practice.configured.stage=std::clamp(title->selection.last_practice,0,5);practice.configured.section=0;practice.configured.phase=0;practice.configured.power=title->selection.character==1&&title->selection.partner==0?96:80;}
            if(practice.accepted){const auto p=practice.configured;const auto choice=title->selection;return begin(*source,p.stage+1,choice.character,choice.partner,p.stage==6?4:choice.difficulty,false,false,true,&p);}
            if(!animations.update(true)||!animations.update(false))return fail("practice title ANM update failed");
            ++state.frame;return true;
        }
#endif
        const u32 raw=(input.held&~256u)|((input.held&256)?0x80000:0)|(input.pause?256:0);
        if(title->screen==TitleScreen::Main){++title_idle;if(raw)title_idle=0;
            if(title_idle>=1800){const auto path=std::string("demo")+char('3'-(demo_index++&3))+".rpy";std::vector<u8> bytes;
                if(!source->read(path,bytes))return fail("original demo resource missing");title_idle=0;
                return begin_replay(*source,bytes.data(),u32(bytes.size()),0,true);
            }
        }
        game_input.update(raw);title->held=raw;
        if(!title->update(game_input.pressed,game_input.long_repeat))return fail(title->error.c_str());
        config=title->config;
        if(title->fade_requested)title_fades.start(5,32,0,61,&animations.rate);
        title_fades.update();
        if(title->start_requested){const auto choice=title->selection;return begin(*source,choice.stage,choice.character,choice.partner,choice.difficulty,false,false,(choice.flags&16)!=0);}
        if(title->replay_start_requested){auto entry=title->replay_files[title->replay_file];last_replay_file=title->replay_file;const u32 stage=title->replay_stage+1;
            if(entry->file.empty()&&(!resources.effects||!resources.effects->read_replay(entry->path,entry->file)))return fail("selected replay could not be loaded");
            return begin_replay(*source,entry->file.data(),u32(entry->file.size()),stage);}
        if(!animations.update(true)||!animations.update(false)) return fail("title ANM update failed");
        if(title->screen==TitleScreen::Exit)state.phase=GameSessionPhase::finished;
        ++state.frame;
        return true;
    }
    if(state.phase!=GameSessionPhase::stage) return true;
    if(interactive&&state.demo){const u32 raw=(input.held&~256u)|((input.held&256)?0x80000:0)|(input.pause?256:0);
        if((raw&0x80103)||state.frame==3600)return return_to_title();
        if(state.frame==3540&&battle)battle->fades.start(5,60,0,67,&animations.rate);
    }
    // ANM uses the same 60 Hz rate as the original game.  Combat managers are
    // deliberately called at this exact boundary once attached by GameWorld.
    if(battle_attached) {
        if(!battle)return fail("battle lifetime missing");
        bool playback_ended=false;
        const bool updated=battle->update([&]{
        if(state.replay){
            if(battle->stage_active&&playback.selected_stage()!=state.stage&&!playback.select(state.stage))return fail("replay activation stage missing");
            const auto recorded=playback.tick();
            if(recorded.end_marker){playback_ended=true;return false;}
            edges.held=recorded.held;edges.pressed=recorded.pressed;edges.released=recorded.released;
            auto& motion=battle->player_input.movement;motion.touch_mode=recorded.touch_mode;motion.touch_x=recorded.touch_x;motion.touch_y=recorded.touch_y;
        }else{
            game_input.update(input.held,config.auto_focus());
            edges.held=game_input.held;edges.pressed=game_input.pressed;edges.released=game_input.released;
            if(battle->stage_active&&!recording_active){recording.finish_stage();if(!record_stage_entry(false))return false;}
            const auto& motion=battle->player_input.movement;
            if(!recording.tick(edges.held,edges.pressed,edges.released,input.recording_fps,motion.touch_mode,motion.touch_x,motion.touch_y))return fail("replay recording limit reached");
        }
        battle->set_input(edges);return true;
        });
        if(playback_ended){if(interactive)return return_to_title();state.phase=GameSessionPhase::finished;return true;}
        if(!updated) { if(error.empty())error="battle update failed: "+std::to_string(battle->last_error)+" "+battle->error; return false; }
        for(const auto& event:battle->events)if(event.kind==BattleEventKind::BossMusic||(event.kind==BattleEventKind::StageMusic&&!state.demo))scores.settings[0x26+stage_music(event.value,event.kind==BattleEventKind::BossMusic)]=1;
        if(!state.replay&&battle->stage_active){auto* counter=scores.characters[state.character*3+state.subtype].data()+0x58c;i32 n;std::memcpy(&n,counter,4);if(n<215999999){++n;std::memcpy(counter,&n,4);}}
        if(battle->game_over_requested){if(state.replay){if(interactive)return return_to_title();state.phase=GameSessionPhase::finished;return true;}return open_result(false);}
        if(battle->completion.state.exit==StageExit::Results)return open_result(true);
        if(battle->completion.state.exit==StageExit::Ending)return open_ending();
        if(battle->completion.state.exit==StageExit::Title||battle->completion.state.exit==StageExit::ReplayEnd){if(interactive)return return_to_title();state.phase=GameSessionPhase::finished;return true;}
        if(battle->completion.state.exit==StageExit::NextStage){
            const u32 next=u32(battle->completion.state.next_stage);
            if(!state.replay){recording.spell_times(battle->spell_timing.records);recording_active=false;recording_seed=animations.script_rng.seed;animations.script_rng.calls=0;}
            if(state.replay){
                if(!playback.stage(next)){state.phase=GameSessionPhase::finished;return true;}
                battle->replay_entry.read(playback.header(next));
                battle->replay_entry.restore(economy,animations.script_rng,false);
                battle->hud.score.continues=battle->replay_entry.continues;
            }
            if(!source||!battle->next_stage(*source,next)){error="stage transition failed: "+battle->error;return false;}
            state.stage=next;state.frame=0;
        }
    } else if(!animations.update(false)) return fail("ANM update failed");
    ++state.frame;
    return true;
}

bool GameSession::pause() {
    if(state.phase!=GameSessionPhase::stage) return false;
    pause_menu=std::make_unique<PauseMenu>(animations,resources.core.text,resources.core.front,scores);
    pause_menu->selection=state.character*3+state.subtype;pause_menu->difficulty=state.difficulty;pause_menu->stage=state.stage;pause_menu->practice=state.practice;
    paused_rate=animations.rate;animations.rate=1;
    if(!pause_menu->begin(state.replay))return fail(pause_menu->error.c_str());
    state.phase=GameSessionPhase::paused;
    return true;
}

bool GameSession::open_result(bool completed){
    pause_menu=std::make_unique<PauseMenu>(animations,resources.core.text,resources.core.front,scores);
    pause_menu->selection=state.character*3+state.subtype;pause_menu->difficulty=state.difficulty;pause_menu->stage=state.stage;
    pause_menu->result_score=economy.score_units;pause_menu->continues=battle->hud.score.continues;pause_menu->timestamp=recording_timestamp;
    paused_rate=animations.rate;animations.rate=1;
    if(!pause_menu->begin_end(state.practice,completed))return fail(pause_menu->error.c_str());
    state.phase=GameSessionPhase::game_over;menu_input.update(last_held);return true;
}

bool GameSession::open_ending(){
    if(!source||!battle||state.replay)return fail("ending requires a completed live game");
    completed_score=economy.score_units;completed_continues=battle->hud.score.continues;
    recording.spell_times(battle->spell_timing.records);if(!recording.finish_stage(true))return fail("completed recording finalization failed");
    menu_selection.character=state.character;menu_selection.partner=state.subtype;menu_selection.difficulty=state.difficulty;
    scores.write_records(spell_records,clear_records);reset_state();completed_run=true;
    if(!compositor.initialize(resources.core.text))return fail("ending compositor initialization failed");
    compositor.reset_cameras(false);animations.rate=1;
    ending=std::make_unique<Ending>(animations,resources.core.text,scores);
    if(!ending->begin(*source,menu_selection.character*3+menu_selection.partner,menu_selection.difficulty,completed_continues))return fail(ending->error.c_str());
    if(resources.effects)for(auto& entry:ending->resources)if(!resources.effects->prepare_animation(entry.second))return fail("ending texture preparation failed");
    state.phase=GameSessionPhase::ending;return true;
}
bool GameSession::open_results(){
    if(!source||!completed_run)return fail("completed game metadata missing");
    if(!open_title(*source,false,TitleScreen::Results))return false;
    title->flags=0;title->result_score=completed_score;title->result_continues=completed_continues;title->result_timestamp=recording_timestamp;return true;
}

bool GameSession::restart_run(bool continuation){
    const auto previous=state;const u32 first=initial_stage;
    const auto practice_parameters=practice.run;
    const i32 count=continuation&&previous.stage!=first?std::min(battle->hud.score.continues+1,9):0;
    scores.write_records(spell_records,clear_records);next_continues=count;
    if(previous.replay){if(!begin(*source,first,previous.character,previous.subtype,previous.difficulty,previous.demo,true,previous.practice,practice.active?&practice_parameters:nullptr))return false;}
    else if(!begin(*source,continuation?previous.stage:first,previous.character,previous.subtype,previous.difficulty,false,false,previous.practice,practice.active?&practice_parameters:nullptr))return false;
    initial_stage=first;return true;
}

bool GameSession::resume() {
    if(state.phase!=GameSessionPhase::paused) return false;
    if(pause_menu){for(const auto id:{pause_menu->background_animation,pause_menu->menu_animation})if(auto* vm=animations.find(id)){
        vm->pending_interrupt=1;if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)n->value->pending_interrupt=1;
    }}
    pause_menu.reset();animations.rate=paused_rate;
    state.phase=GameSessionPhase::stage;
    return true;
}

bool GameSession::return_to_title() {
    if(state.phase==GameSessionPhase::title) return true;
    if(!source)return fail("title source missing");
    const bool demo=state.demo;const auto screen=state.replay&&!demo?TitleScreen::Replays:TitleScreen::Main;
    if(!open_title(*source,false,screen))return false;if(demo)title->flags&=~1u;
    if(screen==TitleScreen::Replays)title->last_replay=last_replay_file;return true;
}

bool GameSession::open_title(GameResources& data,bool first,TitleScreen screen){
    if(!title)scores.write_records(spell_records,clear_records);
    reset_state();source=&data;interactive=true;
#ifdef TH_ENABLE_THPRAC
    practice.clear_run();
#endif
    if(!resources.load_core(data))return fail(resources.error.c_str());
    if(!compositor.initialize(resources.core.text))return fail("title compositor initialization failed");
    compositor.reset_cameras(false);
    title=std::make_unique<TitleMenu>(animations,resources.core.title,resources.core.title_variant,resources.core.ascii,scores);
    title_ascii=std::make_unique<AsciiText>(resources.core.ascii);title->text_resource=&resources.core.text;
    title->selection=menu_selection;title->config=config;title->flags=first?3:1;
    title->change(screen);title->cursor.count=8;
    if(menu_selection.difficulty==4)title->cursor.select(1);
    if(screen!=TitleScreen::Main){title->cursor.select(screen==TitleScreen::Replays?3:0);title->cursor.push();}
    std::vector<u8> comments;if(!data.read("musiccmt.txt",comments)||!title->load_music_comments(comments.data(),u32(comments.size())))return fail("title music metadata missing");
    return true;
}

bool GameSession::draw(AnmRenderer& renderer,AsciiText* overlay) {
#ifdef TH11_MULTIPLAYER
    if(multiplayer_active&&battle){battle->mp_always_hitbox=multiplayer_always_hitbox;battle->mp_prepare_presentation(renderer);}
#endif
    if(title&&title_ascii&&(state.phase==GameSessionPhase::title
#ifdef TH11_MULTIPLAYER
       ||(multiplayer_active&&title->multiplayer_result)
#endif
    )){
        title_ascii->clear();title->queue_ascii(*title_ascii);
    }
    if(state.phase==GameSessionPhase::finished
#ifdef TH11_MULTIPLAYER
       &&!multiplayer_active
#endif
    )return true;
    if(battle_attached&&battle)battle->ascii.clear();
    renderer.fog_origin=compositor.play_camera.position;
    const auto& fog=compositor.play_camera.fog;
    renderer.fog_channels={fog.channels[0],fog.channels[1],fog.channels[2]};renderer.fog_start=fog.near_distance;
    for(const auto& pass:scene_draw_passes) {
        if(title)title_fades.draw(renderer,pass.priority);
        if(ending){ending->fades.draw(renderer,pass.priority);if(pass.kind==SceneDrawKind::End)ending->fades.draw(renderer,67);}
        if(pass.kind==SceneDrawKind::End&&battle_attached&&battle)battle->fades.draw(renderer,67);
        if(battle_attached&&battle)battle->fades.draw(renderer,pass.priority);
        if(pass.kind==SceneDrawKind::Animation){
            if(pass.layer==3){renderer.set_camera(compositor.world_camera,true);renderer.pipeline().fog=false;}
            if(pass.layer==19){compositor.world_camera.offset={};renderer.offset={};}
            if(pass.layer==20||pass.layer==29)renderer.set_camera(compositor.full_camera,true);
            if(renderer.draw_layer(animations.layer_first(pass.layer))==-2)return fail("ANM draw failed");
#ifdef TH11_MULTIPLAYER
            if(multiplayer_active&&battle&&pause_menu&&pause_menu->multiplayer_pause&&pass.layer==29)
                if(renderer.draw_layer(battle->mp_presentation.layer_first(29))==-2)return fail("MP pause ANM draw failed");
#endif
        }else if(u32(pass.kind)>=u32(SceneDrawKind::Begin)&&u32(pass.kind)<=u32(SceneDrawKind::End)){
            if(!compositor.draw(renderer,pass.kind))return fail("screen compositor draw failed");
        }else if(title&&pass.kind==SceneDrawKind::AsciiOuter){
            if(!title_ascii->draw(renderer,0,compositor.full_camera,compositor.play_camera))return fail("title text draw failed");
        }else if(battle_attached&&battle){
            if(pass.kind==SceneDrawKind::AsciiOuter&&pause_menu)pause_menu->queue_ascii(battle->ascii);
            if(!battle->draw(renderer,pass.kind))return fail("battle draw failed");
        }
        if(overlay&&pass.kind==SceneDrawKind::AsciiOuter&&!overlay->draw(renderer,0,compositor.full_camera,compositor.play_camera))return fail("frame statistics draw failed");
    }
    renderer.flush();
    return true;
}

const char* GameSession::phase_name() const noexcept {
    switch(state.phase) {
    case GameSessionPhase::title:return "title";
    case GameSessionPhase::stage:return "stage";
    case GameSessionPhase::paused:return "paused";
    case GameSessionPhase::game_over:return "game_over";
    case GameSessionPhase::finished:return "finished";
    case GameSessionPhase::ending:return "ending";
    }
    return "unknown";
}

}
