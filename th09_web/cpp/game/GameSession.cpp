#include "GameSession.hpp"
#include "Binary.hpp"
#include <algorithm>
namespace th09 {
bool GameSession::begin(const WorldConfiguration& configuration,const ReplayMetadata& metadata,const char* date){
    motion.clear();clear_motion();resources.cancel_preload();warmed_stage=-1;world.reset();ending.reset();error.clear();animations.invalid=false;initial=current=configuration;replay_metadata=metadata;recording.begin(metadata);demo_input={};demo_frames=0;is_replay=demo=false;recordable=true;continues=0;frames=0;inputs[0]={};inputs[1]={};inputs[2]={};scores={};result={};
    if(date)std::copy_n(date,std::min(std::strlen(date),result.date.size()-1),result.date.data());
    current.selection.stage=configuration.selection.mode==GameMode::versus?9:0;current.selection.round=0;current.selection.visited={};current.selection.continued=0;
    scores[0].lives=current.selection.mode==GameMode::story?float(current.starting_extra_lives)+2:0;current.game_flags=(current.game_flags&~0x7808u)|4;
    return start_stage(true);
}
bool GameSession::play(const ReplayFile& file,u32 stage,bool attract){
    if(stage>=10||file.data().size()<ReplayFile::metadata_end)return fail("Replay stage bounds");
    if(!motion.load(file.data().data(),file.data().size(),9))return fail("Invalid touch replay track");clear_motion();resources.cancel_preload();warmed_stage=-1;replay=file;is_replay=true;demo=attract;demo_input={};demo_frames=0;starting_replay_stage=stage;recordable=false;world.reset();ending.reset();error.clear();animations.invalid=false;current={};initial={};continues=0;frames=0;scores={};for(auto& in:inputs)in={};
    const auto* bytes=file.data().data();if(bytes[0x1e4]>2||bytes[0xd7]>4)return fail("Replay game mode");current.selection.mode=GameMode(bytes[0x1e4]);current.selection.stage=i32(stage);current.game_flags=4|8|(attract?2u:0u);return start_stage(true);
}
bool GameSession::restore_replay(){
    ReplayRoundSettings settings;if(!playback.begin(replay,u32(current.selection.stage),settings))return fail("Replay stage streams missing or invalid");
    const auto* bytes=replay.data().data();auto& selection=current.selection;selection.difficulty=settings.difficulty;selection.selector=settings.players[0].selector;selection.background=settings.players[0].background;
    for(i32 side=0;side<2;++side){const auto& p=settings.players[side];selection.characters[side]=p.character;selection.cpu_levels[side]=p.cpu_level;current.controllers[side]=p.cpu;current.health[side]=settings.settings[side];current.alternate[side]=settings.settings[side+2]!=0;current.automatic_focus[side]=settings.config[0xb4+side]!=0;
        scores[side].points=scores[side].displayed=p.points;scores[side].lives=float(p.lives);if(side==0)scores[side].extends=i32(p.extends);
    }
    current.starting_extra_lives=settings.config[0xac];current.selection.unlocked=records.versus_unlocked;
    if(selection.mode!=GameMode::versus)for(i32 previous=0;previous<selection.stage;++previous){const auto at=replay.stream_offset(1,u32(previous));if(!at||at+32>replay.data().size())return fail("Replay previous stage missing");const u32 character=bytes[at+6];if(character>=16)return fail("Replay previous opponent");selection.visited[character]=1;}
    state.random.seed=settings.players[0].seed;return true;
}
bool GameSession::start_stage(bool first){
    if(world){world->copy_inputs(inputs);scores=world->rules.scores;current=world->configuration;current.game_flags=world->battle->state.flags;world.reset();}
    ending.reset();animations.invalid=false;current.game_flags=(current.game_flags&~0x5800u)|4;current.selection.round=0;
    if(!first&&current.selection.mode!=GameMode::versus)++current.selection.stage;
    if(current.selection.stage>9)return fail("Campaign stage bounds");
    if(!first&&scores[0].eligible_for_extend(current.selection.mode)&&scores[0].extend(current.selection.mode))output.sound(28,0);
    const u16 capture_seed=state.random.seed;
    if(is_replay&&!restore_replay())return false;
    current.selection.lives=scores[0].lives;
    struct RouteOutput:StageSelectionActions {WorldPresentation& output;explicit RouteOutput(WorldPresentation& p):output(p){}void encounter(i32 c)override{output.encountered(c);}} route_output(output);
    if(!StageSelection::select(current.selection,state.random,route_output))return fail("Campaign route");
    if(!is_replay){
        ReplayRoundSettings header;for(i32 side=0;side<2;++side){auto& p=header.players[side];p.seed=capture_seed;p.points=scores[side].points;p.character=u8(current.selection.characters[side]);p.cpu=u8(current.controllers[side]);p.lives=u8(i32(scores[side].lives));p.cpu_level=u16(current.selection.cpu_levels[side]);}
        header.players[0].selector=i8(current.selection.selector);header.players[0].background=u8(current.selection.background);header.players[0].extends=u32(scores[0].extends);
        if(!recording.begin_stage(u32(current.selection.stage),header))return fail(recording.error);
        if(first){state.random.calls=0;for(auto& in:inputs)in.advance(in.device.held);}
    }
    // Campaign construction clears the opponent's previous score even when
    // playback starts directly at a later stage (original 0041af2d).
    if(current.selection.mode!=GameMode::versus)scores[1]={};
    world=std::make_unique<GameWorld>(state,resources,animations,output);if(!world->initialize(current,false,&scores))return fail(world->error+" "+resources.error);
    for(i32 side=0;side<2;++side){auto& p=*world->battle->fields[side].player;p.input=inputs[side];p.motion_source=this;motion.begin(current.selection.stage*2+side,false,is_replay,recordable);motion_input[side]={};}world->combined_input=inputs[2];resources.cancel_preload();warmed_stage=-1;phase=SessionPhase::match;return true;
}
void GameSession::capture_result(){if(!world)return;result.points=world->rules.scores[0].points;result.character=u8(world->configuration.selection.characters[0]);result.difficulty=u8(world->configuration.selection.difficulty);result.stage=0;result.continues=continues;}
bool GameSession::update(u16 left,u16 right,u16 menu){
    if(!error.empty())return false;
    if(demo&&phase==SessionPhase::match){demo_input.update(menu);if(demo_input.pressed||++demo_frames==6120){finish();return true;}if(demo_frames==6000){world->screen_effects.create({4,120,0,0,0,35,2});output.fade_music();}}
    if(phase==SessionPhase::ending){ending->timing=timing;animations.timing=timing;inputs[2].device.update(menu);if(!ending->update(inputs[2].device)){if(!ending->error.empty())return fail(ending->error);phase=SessionPhase::finished;}return true;}
    if(phase!=SessionPhase::match||!world||world->paused)return true;
    world->timing=timing;world->copy_inputs(inputs);const bool focus[2]={world->battle->state.automatic_focus[0],world->battle->state.automatic_focus[1]};
    if(is_replay){const auto action=playback.advance(world->battle->state.flags,inputs,focus);if(action==ReplayPlayback::Step::finished){finish();return true;}}
    else{state.random.calls=0;const u16 keys[3]={left,right,menu};for(i32 n=0;n<3;++n){inputs[n].device.update(keys[n]);inputs[n].advance(keys[n]);}for(i32 s=0;s<2;++s)inputs[s].update_auto_focus(focus[s]);}
    if(!world->update_prepared(inputs))return fail(world->error);world->copy_inputs(inputs);
    if(!is_replay){const bool cpu[2]={current.controllers[0]!=0,current.controllers[1]!=0};recording.record(world->battle->state.flags,false,inputs,cpu,60);}
    ++frames;if(!world->transition_pending)return true;capture_result();
    switch(world->transition){
    case SceneTransition::next_stage:return start_stage(false);
    case SceneTransition::ending:records.unlock_after_ending(result.character,result.difficulty);ending=std::make_unique<Ending>(resources,ending_output);if(!ending->initialize(result.character))return fail(ending->error);phase=SessionPhase::ending;break;
    case SceneTransition::game_over:phase=SessionPhase::game_over;break;
    case SceneTransition::match_complete:phase=SessionPhase::match_complete;break;
    case SceneTransition::title:finish();break;
    }return true;
}
void GameSession::warm_resources(){
    if(phase!=SessionPhase::match||!world||is_replay||world->rules.mode==GameMode::versus||(world->rules.progress.active_frames<60&&world->scene->phase<1)||world->rules.progress.stage>=8)return;
    if(warmed_stage!=world->rules.progress.stage){warmed_stage=world->rules.progress.stage;auto next=world->configuration.selection;++next.stage;
        for(const auto& route:StageSelection::candidates(next)){const auto* character=character_resources(route.opponent);const auto* bg=background_resources(route.background);if(!character||!bg)continue;
            resources.preload(AnimationFile::right_character,world->configuration.alternate[1]?character->alternate_animation:character->animation);resources.preload(AnimationFile::background,bg->animation);
            resources.preload(AnimationFile::enemies,route.background==0||route.background==3||route.background==7?"enemy1.anm":route.background==12||route.background==15?"enemy13.anm":"enemy.anm");
        }
    }resources.warm_one();
}
bool GameSession::retry(){if(is_replay)return play(replay,starting_replay_stage,demo);const auto date=result.date;return begin(initial,replay_metadata,date.data());}
bool GameSession::continue_game(){if(!world||phase!=SessionPhase::game_over||is_replay||current.selection.mode!=GameMode::story||continues>=3)return false;++continues;recordable=false;world->continue_game();current=world->configuration;phase=SessionPhase::match;return true;}
void GameSession::finish(){capture_result();phase=SessionPhase::finished;}
bool GameSession::movement(const PlayerMotion& p,float speed,float,float& x,float& y){
    const i32 track=current.selection.stage*2+i32(p.player);
    if(is_replay)return motion.playback(track,x,y);
    const auto& input=motion_input[p.player];const bool enabled=input.enabled&&world&&world->dialogue->id<0&&world->dialogue->id!=-2;
    if(enabled){x=input.x;y=input.y;
        // A networked gesture ships the absolute field target, so both peers
        // convert it here, at the frame that moves the player. Converting at
        // send time aims from a position `lead` frames ahead of this frame and
        // the player orbits the finger instead of reaching it.
        if(input.target){const float sx=p.base_scale.x*p.effect_scale.x,sy=p.base_scale.y*p.effect_scale.y;x=sx?(x-p.position.x)/sx:0;y=sy?(y-p.position.y)/sy:0;}
        if(!input.unlimited)touch::limit_vector(x,y,speed);}
    motion.record(track,enabled,x,y);return enabled;
}
ReplayFile GameSession::save_replay(const char* name){if(!recordable||is_replay){error="This run cannot be saved as a replay";return {};}const auto& c=world?world->configuration:current;auto file=recording.finish(state.random,name,result.date.data(),c.selection.characters[0],c.selection.characters[1]);if(file.data().empty()){error=recording.error;return file;}if(motion.used()){const auto tail=motion.trailer(9);if(tail.empty()){error="Touch replay recording exceeded its limit";return {};}auto extended=file.data();extended.insert(extended.end(),tail.begin(),tail.end());if(!file.assign(extended.data(),u32(extended.size()))){error="Touch replay size";return {};}}return file;}
}
