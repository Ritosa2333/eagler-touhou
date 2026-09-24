#include "WorldJournal.hpp"
#include "../platform/BrowserRuntime.hpp"

namespace th08::multiplayer {
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
void WorldJournal::Raw(void* address,std::size_t size,Group group,const char* name){
    if(size)blocks.push_back({address,size,group,name});
}
bool WorldJournal::Inventory(){
    auto& r=*runtime;auto& a=*app;auto& g=a.game;auto& s=a.session;
#define ADD(field,group) Add(field,group,#field)
    ADD(s.practice,Session);ADD(s.random,Session);ADD(s.numbers,Session);
    ADD(s.config,Session);ADD(s.display_config,Session);ADD(s.history,Session);
    ADD(s.records,Session);ADD(s.previous_records,Session);ADD(s.clears,Session);
    ADD(s.practices,Session);ADD(s.statistics,Session);ADD(s.pilot_resources,Session);
    ADD(s.thresholds,Session);ADD(s.guest_thresholds,Session);ADD(s.rank,Session);
    ADD(s.stall_frames,Session);ADD(s.stage_copy,Session);ADD(s.replay_seed,Session);
    ADD(s.last_words,Session);ADD(s.last_name,Session);ADD(s.total_clock,Session);
    ADD(g.roster,Player);ADD(g.cooperation,Player);ADD(g.committed_buttons,Player);ADD(g.previous_buttons,Player);
    for(u32 seat=0;seat<s.player_count;++seat){
        auto& p=g.pilot(seat);auto& services=g.pilot_services(seat);
        ADD(p.state,Player);ADD(p.timing,Player);ADD(p.failed,Player);ADD(p.initialized,Player);
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
    ADD(a.title.context,Clock);ADD(a.results.controls.context,Clock);ADD(a.music.context,Clock);
    ADD(a.animations.timing,Clock);ADD(a.animations.executed,Clock);ADD(a.animations.invalid,Clock);
    ADD(r.multiplayer_logic_frame,Clock);ADD(r.input,Clock);
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
    if(!main.Reset(config)||!Inventory()||!enemies.Bind(g.enemies.population)||
       !pools.Bind(g.bullets,g.items,g.effect_system,app->session.random)||!resources.Bind(app->library)){
        Clear();error="world journal bootstrap failed";return false;
    }
    app->world_journal=this;return true;
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
bool WorldJournal::BeginFrame(u32 frame){
    if(!CanAdvance()||main.IsFrameOpen()||records.size()>=History)return Fail("frame admission or retirement fence");
    if(replay_bytes(app->game.recording)>16*1024*1024)return Fail("native recording exceeds checkpoint budget");
    if(!main.BeginFrame(frame))return Fail("main frame order");
    records.push_back({frame,std::make_unique<ReplayRecording>(app->game.recording)});
    for(const auto& block:blocks)if(!main.Touch(block.address,block.bytes))return Fail(block.name);
    if(!app->screen.capture_rollback(main)||!app->game.screen.capture_rollback(main))return Fail("screen callback ownership");
    if(!enemies.BeginFrame(frame))return Fail("enemy ownership");
    if(!pools.BeginFrame(frame))return Fail("pool ownership");
    if(!resources.BeginFrame(frame))return Fail("resource ownership");
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
    while(!records.empty()&&records.front().frame<frame)records.pop_front();
}
void WorldJournal::Clear(){
    if(app&&app->world_journal==this)app->world_journal=nullptr;
    enemies.Clear();pools.Clear();resources.Clear();main.Clear();records.clear();blocks.clear();
    app=nullptr;runtime=nullptr;stage=-1;failed=false;
}
std::size_t WorldJournal::BytesForFrame(u32 frame)const{
    std::size_t size=main.BytesForFrame(frame)+enemies.BytesForFrame(frame)+pools.BytesForFrame(frame)+resources.BytesForFrame(frame);
    for(const auto& record:records)if(record.frame==frame)size+=replay_bytes(*record.replay);
    return size;
}
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
