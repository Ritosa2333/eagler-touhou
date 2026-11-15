#include "StageEntry.hpp"
#include "../sdl/ApplicationState.hpp"
#include <eagler/netplay/SpectatorStageScalars.hpp>

namespace th17::multiplayer {
namespace {
template<class Codec> bool timer(Codec& c,Timer& t){return c.Values(t.previous,t.current,t.fractional,t.rate_index,t.flags);}
template<class Codec> bool visit(Codec& c,StageEntry& e,u32 schema){
    auto& s=e.setup;
    if(!c.Values(e.frame,e.app_frame,e.scene_flags,e.deferred,s.room,s.seed,s.seats,s.local,s.difficulty,s.delay,s.rollback)||
       !c.Array(s.build))return false;
    for(auto& p:s.loadouts)if(!c.Values(p.character,p.animal))return false;
    if(!c.Values(e.game.seed,e.game.calls,e.visual.seed,e.visual.calls,
        e.creation_game.seed,e.creation_game.calls,e.creation_visual.seed,e.creation_visual.calls))return false;
    auto& h=e.stage;auto& r=h.run;
    if(!c.Values(h.stage,h.seed,h.frame_gate,h.player.x,h.player.y,h.focus)||
       !c.Values(r.stage,r.initial_stage,r.chapter,r.character,r.animal,r.score,r.difficulty,r.continues,r.rank,
        r.grazes,r.chapter_grazes,r.spell_id,r.deaths,r.reserved_40,r.point_items,r.point_value,r.initial_point_value,r.max_point_value,
        r.power,r.max_power,r.power_step,r.reserved_60,r.lives,r.life_pieces,r.life_piece_tier,r.bombs,r.bomb_pieces,r.initial_bombs,
        r.chapter_total,r.chapter_defeated)||!c.Array(r.counters)||!c.Array(h.reserved))return false;
    auto& roar=r.roaring;
    if(!c.Values(roar.count,roar.reserve_count,roar.duration,roar.kind,roar.extension,roar.flags)||
       !c.Array(roar.inventory)||!c.Array(roar.reserve)||!timer(c,roar.reward_age)||!timer(c,roar.countdown))return false;
    for(auto& p:e.pilots)
        if(!c.Values(p.extra_lives,p.life_pieces,p.deaths,p.enemy_deaths,p.enemy_chain,p.bomb_state,p.power,p.power_step,
           p.bombs,p.bomb_pieces,p.initial_bombs,p.animal,p.replay_state,p.roaring_flags,p.bomb_available,p.roaring_breaks,p.practice_tools)||
           !timer(c,p.roaring_countdown))return false;
    for(auto& p:e.positions)if(!c.Values(p.x,p.y))return false;
    if(!c.Array(e.focus))return false;
    for(auto* input:{&e.menu,&e.input})
        if(!c.Values(input->held,input->previous,input->pressed,input->released,input->repeated,input->long_held)||
           !c.Array(input->duration)||!c.Array(input->repeat_age))return false;
    return c.Array(e.piece_tiers)&&(schema==1||c.Value(e.ghost_mask))&&(schema<4||c.Value(s.challenge));
}
}
bool decode_stage_entry(const std::vector<u8>& bytes,StageEntry& out){
    StageEntry e;Netplay::SpectatorScalars::Reader reader(bytes);
    u32 schema=0;
    if(!reader.Value(schema)||!StageEntry::supports(schema)||!visit(reader,e,schema)||!reader.Done()||!e.setup.valid()||
       e.stage.stage<1||e.stage.stage>7||e.stage.run.stage!=e.stage.stage||
       e.stage.run.difficulty!=e.setup.difficulty||e.stage.run.character!=e.setup.loadouts[0].character||
       e.stage.run.animal!=e.setup.loadouts[0].animal)return false;
    for(u32 n=0;n<e.setup.seats;++n){
        if(e.pilots[n].animal!=e.setup.loadouts[n].animal)return false;
        if(schema==1&&e.pilots[n].extra_lives<0)e.ghost_mask|=u8(1u<<n);
    }
    if(e.ghost_mask>>e.setup.seats)return false;
    out=std::move(e);return true;
}
bool capture_stage_entry(sdl::ApplicationState& app,const SessionSetup& setup,const GameInput& menu,u32 frame,std::vector<u8>& bytes){
    auto* stage=app.scene();
    if(!stage||stage->active()||!app.run_input||app.loading_destination!=AppScene::Uninitialized)return false;
    StageEntry e;e.setup=setup;e.frame=frame;e.app_frame=app.frames;e.scene_flags=app.progress.scene_flags;
    e.deferred=(e.scene_flags&0x800)!=0;e.game=app.environment.game_rng;e.visual=app.visual;e.menu=menu;e.input=app.run_input->input;
    e.creation_game=app.room_stage_game;e.creation_visual=app.room_stage_visual;
    e.stage.stage=u16(app.progress.stage);e.stage.seed=app.stage_seed;e.stage.frame_gate=app.flow.fresh?1:0;
    e.stage.run=stage->capture_run();e.stage.player=stage->battle.player->motion.position_fixed;
    e.stage.focus=stage->battle.player->motion.focus;
    for(u32 n=0;n<e.stage.reserved.size();++n)e.stage.reserved[n]=u32(stage->capture_times[n]);
    for(u32 n=0;n<setup.seats;++n){
        const auto& pilot=stage->battle.pilot(n);
        e.pilots[n]=pilot.session;e.positions[n]=pilot.motion.position_fixed;e.focus[n]=pilot.motion.focus;
        e.piece_tiers[n]=stage->battle.life_piece_tier(n);
        if(stage->battle.pilot_ghost(n))e.ghost_mask|=u8(1u<<n);
    }
    Netplay::SpectatorScalars::Writer writer;writer.Value(StageEntry::schema);
    if(!visit(writer,e,StageEntry::schema)||!writer.Good())return false;
    bytes=writer.Bytes();return true;
}
bool restore_stage_entry(sdl::ApplicationState& app,const SessionSetup& setup,GameInput& menu,u32 frame,const std::vector<u8>& bytes){
    StageEntry e;
    if(!decode_stage_entry(bytes,e)||e.frame!=frame||e.setup.room!=setup.room||e.setup.seed!=setup.seed||
       e.setup.gameplay_abi()!=setup.gameplay_abi()||e.setup.build!=setup.build)return false;
    if(!app.restore_room_stage(e))return false;
    menu=e.menu;return true;
}
}

namespace th17::sdl {
bool ApplicationState::restore_room_stage(const multiplayer::StageEntry& entry){
    if(!initialized||!entry.setup.valid())return false;
    room_seats=entry.setup.seats;room_challenge=entry.setup.challenge;
    for(u32 n=0;n<room_seats;++n)room_loadouts[n]={entry.setup.loadouts[n].character,entry.setup.loadouts[n].animal};
    flow.pending=AppScene::Title;
    release_ending();release_game();release_title();
    recording.reset();selected_replay.reset();fades.clear();
    room_partners={};physical_input={};retained_input={};touch={};
    pending_page=pending_bank=-1;loading_destination=AppScene::Uninitialized;
    loading_handle=transition_overlay=0;
    exiting=false;failure.clear();progress={};
    progress.stage=entry.stage.stage;progress.initial_stage=entry.stage.run.initial_stage;
    progress.character=entry.setup.loadouts[0].character;progress.animal=entry.setup.loadouts[0].animal;
    progress.difficulty=entry.setup.difficulty;progress.spell_id=-1;
    environment.game_rng=entry.creation_game;visual=entry.creation_visual;stage_seed=entry.stage.seed;
    reset_graphics();scene_effects.reset();
    run=std::make_unique<RunGameplay>(*this,animations,environment,environment.game_rng,visual,progress,
        records.game,records.spells,*this,&shared);
    if(!run->load(entry.stage.stage,camera,&entry.stage,{},entry.deferred))return fail(run->error);
    for(u32 seat=1;seat<room_seats;++seat)
        if(!run->add_coop_player(room_loadouts[seat][0],room_loadouts[seat][1]))return fail(run->error);
    for(u32 seat=0;seat<room_seats;++seat){
        auto& pilot=scene()->battle.pilot(seat);
        pilot.session=entry.pilots[seat];
        if(room_challenge)pilot.session.enable_challenge();
        auto state=entry.stage;state.player=entry.positions[seat];state.focus=entry.focus[seat];
        state.restore_player(pilot.motion);
        if(!pilot.configure_options())return fail(pilot.options.error);
        scene()->battle.restore_pilot_stock(seat,entry.piece_tiers[seat],(entry.ghost_mask&(1u<<seat))!=0);
    }
    if(!preload_run())return false;
    run_input=std::make_unique<RunInput>(*scene(),progress,*this,nullptr,nullptr,stage_seed,!entry.deferred);
    run_input->auto_focus=false;run_input->input=entry.input;
    flow.current=flow.pending=AppScene::Game;flow.fresh=!entry.deferred;
    progress.scene_flags=entry.scene_flags;frames=entry.app_frame;
    environment.game_rng=entry.game;visual=entry.visual;
    if(!bind_display())return false;
    if(!audio_device.restore_stage_music(*assets()->definition))return fail(audio_device.error);
    return display->draw(double(frames)/60);
}
}
