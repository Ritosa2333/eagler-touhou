#include "WorldJournal.hpp"
#include "../platform/BrowserRuntime.hpp"
#include "../game/Presentation.hpp"
#ifdef TH_MULTIPLAYER_FIXTURES
#include <emscripten.h>
#endif

namespace th08::multiplayer {
#ifdef TH_MULTIPLAYER_FIXTURES
namespace {bool fixture_bomb_lazy_enabled=true,fixture_shot_sparse_enabled=true,fixture_record_sparse_enabled=true,fixture_title_spells_lazy_enabled=true;}
bool fixture_bomb_lazy(bool enabled){fixture_bomb_lazy_enabled=enabled;return fixture_bomb_lazy_enabled;}
bool fixture_shot_sparse(bool enabled){fixture_shot_sparse_enabled=enabled;return fixture_shot_sparse_enabled;}
bool fixture_record_sparse(bool enabled){fixture_record_sparse_enabled=enabled;return fixture_record_sparse_enabled;}
bool fixture_title_spells_lazy(bool enabled){fixture_title_spells_lazy_enabled=enabled;return fixture_title_spells_lazy_enabled;}
#endif
namespace {
u32 hash_bytes(const void* p,std::size_t n,u32 hash=2166136261u){
    auto* bytes=static_cast<const u8*>(p);for(std::size_t i=0;i<n;++i){hash^=bytes[i];hash*=16777619u;}return hash;
}
template<class T>u32 hash_value(const T& v,u32 h){return hash_bytes(&v,sizeof(v),h);}
std::size_t replay_bytes(const ReplayRecording& value){
    std::size_t size=sizeof(value);
    for(i32 stage=0;stage<9;++stage){const auto* s=value.stage(stage);size+=s->stream.input.size()+s->stream.timing.size();}
    return size;
}
}
void WorldJournal::Raw(void* address,std::size_t size,Group group,const char* name,SnapshotMode mode){
    if(size)blocks.push_back({address,size,group,name,mode});
}
bool WorldJournal::Inventory(){
    auto& r=*runtime;auto& a=*app;auto& g=a.game;auto& s=a.session;
#define ADD(field,group) Add(field,group,#field)
    ADD(s.practice,Session);ADD(s.random,Session);ADD(s.numbers,Session);
    ADD(s.config,Session);ADD(s.display_config,Session);ADD(s.history,Session);
    Raw(s.records,sizeof(s.records),Session,"s.records",SnapshotMode::Record);
    Raw(s.previous_records,sizeof(s.previous_records),Session,"s.previous_records",SnapshotMode::PreviousRecord);ADD(s.clears,Session);
    ADD(s.practices,Session);ADD(s.statistics,Session);ADD(s.pilot_resources,Session);
    ADD(s.thresholds,Session);ADD(s.guest_thresholds,Session);ADD(s.rank,Session);
    ADD(s.stall_frames,Session);ADD(s.stage_copy,Session);ADD(s.replay_seed,Session);
    ADD(s.multiplayer_cheat_movement_used,Session);
    ADD(s.multiplayer_route_state,Session);
    ADD(s.last_words,Session);ADD(s.last_name,Session);ADD(s.total_clock,Session);
    ADD(g.roster,Player);ADD(g.cooperation,Player);ADD(g.committed_buttons,Player);ADD(g.previous_buttons,Player);
    for(u32 seat=0;seat<s.player_count;++seat){
        auto& p=g.pilot(seat);auto& services=g.pilot_services(seat);
        constexpr auto shot_start=offsetof(PlayerSimulationState,shots)+offsetof(PlayerShotsState,shots);
        constexpr auto shot_end=shot_start+sizeof(p.state.shots.shots);
        Raw(&p.state,shot_start,Player,"p.state before shot pool");
        Raw(&p.state.shots.shots,sizeof(p.state.shots.shots),Player,"p.state.shots.shots",SnapshotMode::Shot);
        Raw(reinterpret_cast<u8*>(&p.state)+shot_end,
            offsetof(PlayerSimulationState,bomb_objects)-shot_end,Player,"p.state after shot pool");
        Raw(&p.state.bomb_objects,sizeof(p.state.bomb_objects),Player,"p.state.bomb_objects",SnapshotMode::Bomb);
        Raw(reinterpret_cast<u8*>(&p.state.bomb_objects)+sizeof(p.state.bomb_objects),
            sizeof(p.state)-offsetof(PlayerSimulationState,bomb_objects)-sizeof(p.state.bomb_objects),Player,"p.state after bomb");
        ADD(p.timing,Player);ADD(p.failed,Player);ADD(p.initialized,Player);
        ADD(p.shots.timing,Player);ADD(p.shots.failure,Player);ADD(p.shots.authored_colors,Player);
        ADD(p.patterns.frame,Player);ADD(services.failed,Player);
        ADD(services.boss_views,Player);ADD(services.boss_owners,Player);
    }
    ADD(g.globals,Enemy);ADD(g.enemies.state,Enemy);ADD(g.enemies.input,Enemy);
    ADD(g.enemies.simulation.failed,Enemy);ADD(g.enemies.failed,Enemy);
    ADD(g.enemies.native_services,Enemy);ADD(g.enemies.time_item_threshold,Enemy);
    ADD(g.hud,Scene);ADD(g.display,Scene);ADD(g.dialogue_context,Scene);ADD(g.gui_context,Scene);
    ADD(g.background,Scene);ADD(g.background_context,Scene);ADD(g.background_script.invalid,Scene);
    ADD(g.gui.clock_intro_enabled,Scene);ADD(g.presentation.context,Scene);
    ADD(g.ascii_context,Scene);ADD(a.ascii.state,Scene);ADD(a.ascii_context,Scene);
    ADD(g.menus.context,Scene);ADD(g.control.state,Scene);ADD(g.control.input,Scene);
    ADD(g.background_flow.context,Scene);ADD(g.bullet_flow.context,Scene);ADD(g.enemy_flow.context,Scene);
    ADD(g.effect_flow.context,Scene);ADD(g.gui_flow.context,Scene);ADD(g.spell_flow.context,Scene);
    ADD(g.screen_counter,Scene);ADD(g.replay_stage_mask,Scene);
    ADD(g.time_stopped,Scene);ADD(g.paused,Scene);ADD(g.retrying,Scene);ADD(g.always_hitbox,Scene);
    ADD(g.loaded,Scene);ADD(g.failed,Scene);ADD(g.playing_replay,Scene);ADD(g.recording_game,Scene);
    ADD(g.owned_resources,Scene);ADD(g.request,Scene);ADD(g.previous_input,Scene);ADD(g.debug_frame,Scene);
    ADD(g.animations.timing,Scene);ADD(g.animations.executed,Scene);ADD(g.animations.invalid,Scene);
    Raw(const_cast<void*>(stdData),stdSize,Scene,"native STD buffer");
    Raw(const_cast<void*>(quads),quadCount*sizeof(AnmVm),Scene,"native background quad VMs");
    Raw(const_cast<void*>(msgData),msgSize,Scene,"native MSG buffer");
    ADD(a.initialized,Clock);ADD(a.running,Clock);ADD(a.failed,Clock);ADD(a.stopping,Clock);
    ADD(a.game_attached,Clock);ADD(a.loading_gate,Clock);ADD(a.loading_hidden,Clock);
    ADD(a.timing,Clock);ADD(a.last_game,Clock);ADD(a.transition_effect,Clock);
    ADD(a.supervisor.state,Clock);ADD(a.supervisor.input,Clock);
    ADD(a.statistics.state,Clock);ADD(a.statistics.context,Clock);
    ADD(a.loading.phase,Clock);ADD(a.loading.vms,Clock);ADD(a.loading.software_texturing,Clock);
    constexpr auto title_spells_start=offsetof(TitleContext,spells);
    constexpr auto title_spells_end=title_spells_start+sizeof(a.title.context.spells);
    Raw(&a.title.context,title_spells_start,Clock,"a.title.context before spells");
    Raw(&a.title.context.spells,sizeof(a.title.context.spells),Clock,"a.title.context.spells",SnapshotMode::TitleSpells);
    Raw(reinterpret_cast<u8*>(&a.title.context)+title_spells_end,sizeof(a.title.context)-title_spells_end,Clock,"a.title.context after spells");
    ADD(a.results.controls.context,Clock);ADD(a.music.context,Clock);
    ADD(a.animations.timing,Clock);ADD(a.animations.executed,Clock);ADD(a.animations.invalid,Clock);
    ADD(r.multiplayer_logic_frame,Clock);
    // r.input is the live device sampler (including shot-slow hold counting),
    // not an input lane. Rewinding it would replay physical sampling time even
    // though correction reuses captures and never polls those devices again.
    // Authoritative camera/projection/mix state. Device command buffers and
    // redundant bind caches are invalidated after undo instead of copied.
    auto& v=a.renderer;
    ADD(v.viewport,Render);ADD(v.view_matrix,Render);ADD(v.projection_matrix,Render);
    ADD(v.last_world_matrix,Render);ADD(v.scene_camera,Render);ADD(v.shake,Render);
    ADD(v.mix_color,Render);ADD(v.mix_enabled,Render);ADD(v.depth_test_disabled,Render);
    ADD(v.fog_enabled,Render);ADD(a.presentation_shake,Render);ADD(a.presentation_shake_valid,Render);
#undef ADD
    return true;
}
bool WorldJournal::Bind(BrowserRuntime& value){
    if(app||value.app.world_journal||!value.app.in_game()||value.app.loading_game()||
       value.app.game.playing_replay||value.app.invalid()||value.app.session.network_frame_open||
       value.app.supervisor.state.target!=i32(th08::Scene::Game))return false;
    runtime=&value;app=&value.app;stage=app->game.globals.stage;error="";failed=false;
    auto& g=app->game;
    stdData=g.background_script.program.header();stdSize=g.background_script.program.bytes_size();
    quads=g.background_script.quads.data();quadCount=g.background_script.quads.size();
    msgData=g.dialogue.program.bytes();msgSize=g.dialogue.program.size();
    Netplay::RollbackJournalConfig config;config.maxFrames=History;
    config.maxBytesPerFrame=16*1024*1024;config.maxBlocksPerFrame=10000;
    config.fastBulkCopy=true;config.coalesceRestore=true;
    if(!main.Reset(config)||!Inventory()||!enemies.Bind(g.enemies.population)||
       !pools.Bind(g.bullets,g.items,g.effect_system,app->session.random)||!resources.Bind(app->library)){
        Clear();error="world journal bootstrap failed";return false;
    }
    app->world_journal=this;g.player_world.world_journal=this;g.world_journal=this;
    g.spells.record_touch_context=this;
    g.spells.record_touch=[](void* context,SpellRecord& record){return static_cast<WorldJournal*>(context)->TouchRecord(record);};
#ifdef TH_MULTIPLAYER_FIXTURES
    bomb_lazy=fixture_bomb_lazy_enabled;
    shot_sparse=fixture_shot_sparse_enabled;
    record_sparse=fixture_record_sparse_enabled;
    title_spells_lazy=fixture_title_spells_lazy_enabled;
#endif
    return true;
}
bool WorldJournal::StableGraph()const{
    if(!app||!app->game_attached)return false;
    const auto& g=app->game;
    return const_cast<StageProgram&>(g.background_script.program).header()==stdData&&
        g.background_script.program.bytes_size()==stdSize&&g.background_script.quads.data()==quads&&
        g.background_script.quads.size()==quadCount&&g.dialogue.program.bytes()==msgData&&g.dialogue.program.size()==msgSize;
}
bool WorldJournal::CanAdvance()const{
    return app&&!Failed()&&StableGraph()&&app->active()&&!app->invalid()&&!app->loading_game()&&
        app->game.globals.stage==stage&&app->supervisor.state.target==i32(th08::Scene::Game)&&
        app->game.menus.context.supervisor_state==i32(th08::Scene::Game)&&!app->supervisor.state.close_requested;
}
#ifdef TH_MULTIPLAYER_FIXTURES
bool WorldJournal::DiagnosticInputSampler(){
    if(!runtime||HasHistory())return false;
    auto& sampler=runtime->input;const auto before=sampler.focus_conflict;
    if(!BeginFrame(0))return false;
    ControllerSnapshot pad;pad.available=true;pad.buttons[0]=128;
    auto config=app->session.display_config;config.shot_slow=1;config.controller[0]=0;
    sampler.controller(0,pad,config);const auto sampled=sampler.focus_conflict;
    return sampled!=before&&EndFrame()&&UndoTo(0)&&sampler.focus_conflict==sampled;
}
#endif
bool WorldJournal::CanExtend(u32 frame,u32 span)const{
    return !records.empty()&&records.back().end+1==frame&&frame-records.back().frame<span;
}
u32 WorldJournal::CheckpointStart(u32 frame)const{
    for(const auto& r:records)if(r.frame<=frame&&frame<=r.end)return r.frame;
    return Netplay::INVALID_FRAME;
}
bool WorldJournal::BeginFrame(u32 frame,bool extend){
    if(!CanAdvance()||main.IsFrameOpen()||(!extend&&records.size()>=History)||
       (extend&&!CanExtend(frame,3)))return Fail("frame admission or retirement fence");
    if(replay_bytes(app->game.recording)>16*1024*1024)return Fail("native recording exceeds checkpoint budget");
#ifdef TH_MULTIPLAYER_FIXTURES
    const auto started=emscripten_get_now();
    double replay_elapsed=0;
#endif
    if(!main.BeginFrame(frame,extend))return Fail("main frame order");
    if(extend)records.back().end=frame;
    else{
#ifdef TH_MULTIPLAYER_FIXTURES
        const auto replay_started=emscripten_get_now();
#endif
        records.push_back({frame,frame,std::make_unique<ReplayRecording>(app->game.recording)});
#ifdef TH_MULTIPLAYER_FIXTURES
        replay_elapsed=emscripten_get_now()-replay_started;diagnostic_capture_ms[4]+=replay_elapsed;
#endif
        for(const auto& block:blocks){
            const bool eager=block.mode==SnapshotMode::Always||
                (block.mode==SnapshotMode::Bomb&&!bomb_lazy)||
                (block.mode==SnapshotMode::Shot&&!shot_sparse)||
                (block.mode==SnapshotMode::Record&&!record_sparse)||
                (block.mode==SnapshotMode::PreviousRecord&&!record_sparse)||
                (block.mode==SnapshotMode::TitleSpells&&!title_spells_lazy);
            if(eager&&!main.Touch(block.address,block.bytes))return Fail(block.name);
        }
    }
    if(!app->screen.capture_rollback(main)||!app->game.screen.capture_rollback(main))return Fail("screen callback ownership");
#ifdef TH_MULTIPLAYER_FIXTURES
    diagnostic_capture_ms[0]+=emscripten_get_now()-started-replay_elapsed;
    auto owner_started=emscripten_get_now();
#endif
    if(!enemies.BeginFrame(frame,extend))return Fail("enemy ownership");
#ifdef TH_MULTIPLAYER_FIXTURES
    diagnostic_capture_ms[1]+=emscripten_get_now()-owner_started;owner_started=emscripten_get_now();
#endif
    if(!pools.BeginFrame(frame,extend))return Fail("pool ownership");
#ifdef TH_MULTIPLAYER_FIXTURES
    diagnostic_capture_ms[2]+=emscripten_get_now()-owner_started;owner_started=emscripten_get_now();
#endif
    if(!resources.BeginFrame(frame,extend))return Fail("resource ownership");
#ifdef TH_MULTIPLAYER_FIXTURES
    diagnostic_capture_ms[3]+=emscripten_get_now()-owner_started;
#endif
    return true;
}
bool WorldJournal::EndFrame(){
    if(!StableGraph()||!enemies.EndFrame()||!pools.EndFrame()||!resources.EndFrame()||!main.EndFrame())return Fail("end frame owner failure");
    return true;
}
bool WorldJournal::UndoTo(u32 frame){
    if(!app||failed||main.IsFrameOpen()||!StableGraph())return false;
    auto target=records.begin();while(target!=records.end()&&target->frame!=frame)++target;
    if(target==records.end())return false;
    if(!main.UndoTo(frame)||!enemies.UndoTo(frame)||!pools.UndoTo(frame)||!resources.UndoTo(frame))return Fail("incomplete owner restore");
    app->game.recording=*target->replay; // deep vector copy; no ownership word dump
    while(!records.empty()&&records.back().frame>=frame)records.pop_back();
    RebasePresentation();return true;
}
void WorldJournal::RebasePresentation(){
    auto& g=app->game;
    app->screen.rebase_after_rollback();g.screen.rebase_after_rollback();
    g.enemies.drawing.previous.clear();g.enemies.drawing.presentation_marker={};
    for(u32 seat=0;seat<app->session.player_count;++seat){auto& p=g.pilot(seat);
        p.presentation_marker={};p.presentation_valid=false;p.shots.presentation_marker={};p.patterns.presentation_marker={};}
    g.background_script.presentation_marker={};g.background_script.presentation_camera_valid=false;
    g.background_view.presentation_marker={};g.background_view.presentation_spell_valid=false;
    g.gui.presentation_marker={};g.gui.presentation_state.valid=false;g.dialogue.presentation_valid=false;
    g.spell_drawing.presentation_marker={};g.spell_drawing.presentation_valid=false;
    g.menus.pause_presentation_valid=g.menus.retry_presentation_valid=false;
    app->ascii.presentation_marker={};app->ascii.popup_marker={};app->ascii.presentation_state.valid=false;
    app->renderer.begin_background();
}
void WorldJournal::DiscardBefore(u32 frame){
    if(main.IsFrameOpen())return;
    enemies.DiscardBefore(frame);pools.DiscardBefore(frame);resources.DiscardBefore(frame);main.DiscardBefore(frame);
    while(!records.empty()&&records.front().end<frame)records.pop_front();
}
void WorldJournal::Clear(){
    if(app&&app->game.world_journal==this)app->game.world_journal=nullptr;
    if(app&&app->game.spells.record_touch_context==this){app->game.spells.record_touch_context=nullptr;app->game.spells.record_touch=nullptr;}
    if(app&&app->game.player_world.world_journal==this)app->game.player_world.world_journal=nullptr;
    if(app&&app->world_journal==this)app->world_journal=nullptr;
    enemies.Clear();pools.Clear();resources.Clear();main.Clear();records.clear();blocks.clear();
    app=nullptr;runtime=nullptr;stage=-1;failed=false;
}
bool WorldJournal::TouchBomb(PlayerBombObjects& objects){
    if(!bomb_lazy)return true;
    // The presentation-only pass draws from temporary ANM copies. It runs
    // outside checkpoints and must never mutate the authoritative Bomb pool.
    // BlockHashes includes all 128 objects so fixture presentation checks
    // detect a violation of that contract.
    if(!main.IsFrameOpen())return !HasHistory()||presentation::render_only||Fail("bomb write outside checkpoint");
    return main.Touch(&objects,sizeof(objects))||Fail("bomb ownership");
}
bool WorldJournal::TouchShot(PlayerShot& shot){
    if(!shot_sparse)return true;
    if(!main.IsFrameOpen())return !HasHistory()||presentation::render_only||Fail("shot write outside checkpoint");
    return main.Touch(&shot,sizeof(shot))||Fail("shot ownership");
}
bool WorldJournal::TouchRecord(SpellRecord& record){
    if(!record_sparse)return true;
    if(!main.IsFrameOpen())return !HasHistory()||Fail("record write outside checkpoint");
    return main.Touch(&record,sizeof(record))||Fail("record ownership");
}
bool WorldJournal::TouchAllRecords(){
    if(!record_sparse)return true;
    if(!main.IsFrameOpen())return !HasHistory()||Fail("score table write outside checkpoint");
    for(auto& record:app->session.records)if(!TouchRecord(record))return false;
    if(!main.Touch(&app->session.previous_records,sizeof(app->session.previous_records)))return Fail("previous score table ownership");
    return true;
}
bool WorldJournal::TouchTitleSpells(){
    if(!title_spells_lazy)return true;
    if(!main.IsFrameOpen())return !HasHistory()||Fail("title spells write outside checkpoint");
    return main.Touch(&app->title.context.spells,sizeof(app->title.context.spells))||Fail("title spells ownership");
}
std::size_t WorldJournal::BytesForFrame(u32 frame)const{
    std::size_t size=main.BytesForFrame(frame)+enemies.BytesForFrame(frame)+pools.BytesForFrame(frame)+resources.BytesForFrame(frame);
    for(const auto& record:records)if(record.frame<=frame&&frame<=record.end)size+=replay_bytes(*record.replay);
    return size;
}
#ifdef TH_MULTIPLAYER_FIXTURES
std::array<std::size_t,5> WorldJournal::DiagnosticBytesForFrame(u32 frame)const{
    std::size_t replay=0;
    for(const auto& record:records)if(record.frame<=frame&&frame<=record.end){replay=replay_bytes(*record.replay);break;}
    return {main.BytesForFrame(frame),enemies.BytesForFrame(frame),pools.BytesForFrame(frame),resources.BytesForFrame(frame),replay};
}
#endif
std::vector<u32> WorldJournal::BlockHashes()const{
    std::vector<u32> values;values.reserve(blocks.size());
    for(const auto& block:blocks)values.push_back(hash_bytes(block.address,block.bytes));return values;
}
std::array<u32,WorldJournal::GroupCount> WorldJournal::AuditHash()const{
    std::array<u32,GroupCount> h;h.fill(2166136261u);if(!app)return h;
    for(const auto& block:blocks)h[block.group]=hash_bytes(block.address,block.bytes,h[block.group]);
    const auto& recorder=app->game.recording;
    h[Recording]=hash_value(recorder.metadata(),h[Recording]);
    h[Recording]=hash_value(recorder.input,h[Recording]);h[Recording]=hash_value(recorder.frame_state,h[Recording]);
    const auto current=recorder.current();h[Recording]=hash_value(current,h[Recording]);
    for(i32 i=0;i<9;++i){const auto* s=recorder.stage(i);const auto& r=s->stream;
        h[Recording]=hash_value(s->present,h[Recording]);h[Recording]=hash_value(s->snapshot,h[Recording]);
        const u32 words[]{u32(r.frame),u32(r.transition_frames),r.input_cursor,r.timing_cursor,r.input_end,r.timing_end,r.stride};
        h[Recording]=hash_value(words,h[Recording]);
        h[Recording]=hash_bytes(r.input.data(),r.input.size(),h[Recording]);
        h[Recording]=hash_bytes(r.timing.data(),r.timing.size(),h[Recording]);}
    for(const auto* screen:{&app->screen,&app->game.screen}){
        h[Screens]=hash_value(screen->context,h[Screens]);
        for(const auto& i:screen->instances){h[Screens]=hash_value(i.state,h[Screens]);
            h[Screens]=hash_value(i.calculation,h[Screens]);h[Screens]=hash_value(i.drawing,h[Screens]);h[Screens]=hash_value(i.occupied,h[Screens]);}}
    for(auto* root:{&app->chain.calculation,&app->chain.drawing})
        for(auto* node=root;node;node=node->next)h[Screens]=hash_value(*node,h[Screens]);
    h[Ecl]=enemies.AuditHash();h[Pools]=pools.AuditHash();h[Resources]=resources.AuditHash();
    for(u32 group=0;group<Composite;++group)h[Composite]=hash_value(h[group],h[Composite]);return h;
}
}
