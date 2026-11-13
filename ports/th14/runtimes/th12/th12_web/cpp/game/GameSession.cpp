#include "GameSession.hpp"
#include "ShotFiles.hpp"
#include "MusicCatalog.hpp"
#include "ContinueTh14.hpp"
#include "ClockSeedTh14.hpp"
#if PORT_TITLE == 14
#include "ReplayTextTh14.hpp"
#include "ReplayRngRestoreTh14.hpp"
#include "StageCppWorkerTh14.hpp"
#include "StageCppDecodeTh14.hpp"
#include "StagePreRngTh14.hpp"
#include "StageScoreSaveTh14.hpp"
#include "StageCopyTailTh14.hpp"
#endif

namespace th11 {
#if defined(TH14_SECTION_TAIL_OBSERVER)
extern "C" void th14_observer_section_tail(u32,const GameSession*);
#endif

#if PORT_TITLE==14
GameSession::GameSession(u32 initial_ticks):GameSession(initial_ticks,true){}
GameSession::GameSession(u32 initial_ticks,bool eager_scores){
    resources.animation_manager=&animations;
    resources.loading_owner=&shared_loading;
    animations.before_initial_update=[this](const AnmVm&){
#if defined(TH14_CPP_STAGE_WORKER)
        if(StageCppWorkerScopeTh14::owns(this))return true;
#endif
        if(initial_battle_private())return true;
        return !battle||!battle->pending_stage||battle->poll_stage_initialization(stage_load_clock());
    };
    th14_seed_clock(initial_ticks,animations.script_rng,animations.visual_rng);
    font_bitmap.initialize(animations.script_rng);
    if(eager_scores)initialize_score_defaults();
}
void GameSession::initialize_score_defaults(){
    scores.initialize(animations.visual_rng);scores.read_records(spell_records,clear_records);
}
#else
GameSession::GameSession(){
    scores.initialize(animations.script_rng);scores.read_records(spell_records,clear_records);
}
#endif
bool GameSession::load_scores(const u8* data,u32 size){if(!scores.open(data,size))return false;scores.read_records(spell_records,clear_records);return true;}
bool GameSession::save_scores(std::vector<u8>& out){if(!title)scores.write_records(spell_records,clear_records);return scores.save(out);}

namespace {
bool clear_animations(AnmManager& manager,u32 keep_handle=0,u32 keep_family=0) {
#if PORT_TITLE == 14
    // Whole-session/direct-stage replacement cancels old copy requests.
    // Ordinary VM clear and overlapping NextStage retain the queue.
    manager.retire_deferred_captures_without_target();
#endif
    manager.clear(keep_handle,keep_family);
    return true;
}
#if PORT_TITLE == 14
struct StageMemoryTaskTh14 {
    StageRestoreMemoryTh14 bindings;
    std::unique_ptr<u8[]> stack=std::make_unique<u8[]>(65536+16);
    u32 stack_end()const noexcept{return u32(reinterpret_cast<uintptr_t>(stack.get()+65536+16))&~u32(15);}
};
#endif

// The attract playlist is data each title ships, not a name built on the fly: an executable holds a
// list of replay names, and its title-screen code steps an index over that list once the idle counter
// passes 0x708 (1800 frames) and no key was pressed. Reproducible with tools/th-demo-lists.py for
// the names; porting-plan sections 389 and 390 read the step out of each title's own code:
//   th12, th13  names demo2, demo1, demo0 and step (index+1)%3
//   th14        names demo0, demo1, demo2 and step (index+1)%3
//   th16        names demo1..demo4 and step (index+1)%3, so its fourth name is not reached
//   th17        names demo1, demo2, demo3 and step (index+1)%3 (an idiv by a literal three)
//   th18        names demo1..demo4 and step (index+1)%4 (an and-mask of three)
// The index itself ships as zero in the image and has no writer outside that code.
struct DemoPlaylist {
    const char* names[4];
    u32 step;
};

inline DemoPlaylist demo_playlist() {
    if(kPortTitle==14) return DemoPlaylist{{"demo0.rpy","demo1.rpy","demo2.rpy",nullptr},3u};
    if(kPortTitle==16) return DemoPlaylist{{"demo1.rpy","demo2.rpy","demo3.rpy","demo4.rpy"},3u};
    if(kPortTitle==17) return DemoPlaylist{{"demo1.rpy","demo2.rpy","demo3.rpy",nullptr},3u};
    if(kPortTitle==18) return DemoPlaylist{{"demo1.rpy","demo2.rpy","demo3.rpy","demo4.rpy"},4u};
    return DemoPlaylist{{"demo2.rpy","demo1.rpy","demo0.rpy",nullptr},3u};
}
}

#if PORT_TITLE == 14
#if defined(TH14_CPP_STAGE_WORKER)
void GameSession::stop_cpp_stage_worker()noexcept{
#if defined(TH14_CPP_STAGE_DECODE)
    if(stage_cpp_decode)stage_cpp_decode->cancel();
#endif
    if(stage_cpp_worker){stage_cpp_worker->cancel_and_join();stage_cpp_worker.reset();}
#if defined(TH14_CPP_STAGE_DECODE)
    stage_cpp_decode.reset();stage_cpp_decode_owner=nullptr;
#endif
}
#if defined(TH14_CPP_STAGE_DECODE)
bool GameSession::stage_loading_callback_busy()const noexcept{return stage_cpp_worker&&stage_cpp_worker->loading_callback_busy();}
u32 GameSession::stage_loading_finished_word()const noexcept{return stage_cpp_worker?stage_cpp_worker->loading_finished_word():0;}
bool GameSession::stage_loading_callback_allows_close()const noexcept{return stage_cpp_worker&&stage_cpp_worker->loading_callback_allows_close();}
StageResourceEffects::DecodeResult GameSession::cpp_stage_decode(StageResources& owner,StageResourceEffects::Phase phase,u32 selection){
    using Result=StageResourceEffects::DecodeResult;
    if(!stage_cpp_decode||stage_cpp_decode_owner!=&owner||!stage_cpp_worker)return Result::Failed;
    const auto result=Result(stage_cpp_decode->request(u32(phase),selection));
    if(result==Result::Ready&&phase==StageResourceEffects::Phase::Scene){
        // The loading worker has already read the camera, after its last
        // background-file ACK. Main consumes that private published result.
        const auto copied=battle->stage_camera_read_job.advance(compositor.world_camera,
            [](u32){return prototype::Work::Ready;},[this](u32 word,u32 offset,u32& value){
                return stage_cpp_decode->scene_camera_word(word,offset,value)?
                    prototype::Work::Ready:prototype::Work::Failed;
            });
        if(copied!=prototype::Work::Ready)return Result::Failed;
        if(!battle->incoming_scene_owner){
            battle->incoming_scene_owner=stage_cpp_decode->take_scene_owner();
            if(!battle->incoming_scene_owner)return Result::Failed;
        }
    }
    if(result==Result::Ready&&phase==StageResourceEffects::Phase::Timeline){
        // Ready3 is published before the Control-enable release. No decoder
        // data is written afterward, but the thread may still own its stack.
        const auto* control=animations.th14_control.get();
        if(!control||u32(control->callbacks_enabled)==0)return Result::Waiting;
        const auto status=stage_cpp_worker->poll();
        if(status!=ReplayRngRestoreResultTh14::Waiting){
            stage_cpp_worker->join();
            if(status!=ReplayRngRestoreResultTh14::Ready)return Result::Failed;
        }
    }
    return result;
}
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
bool GameSession::cpp_stage_pending_file(AnmResource*& resource,u16& file)const noexcept{
    return stage_cpp_decode&&stage_cpp_decode->pending_file(resource,file);
}
StageResourceEffects::DecodeResult GameSession::cpp_stage_upload_offer(StageResources*& owner,StageResourceEffects::Phase& phase,AnmResource*& resource,u16& file)const noexcept{
    using Result=StageResourceEffects::DecodeResult;
    owner=nullptr;resource=nullptr;file=0;phase=StageResourceEffects::Phase::Hud;
    if(!cpp_stage_pending_file(resource,file))return Result::Waiting;
    owner=stage_cpp_decode_owner;
    if(!owner||owner!=resources.loading_bundle())return Result::Failed;
    if(resource==&owner->logo&&file==6)return Result::Ready;
    if(resource==&owner->background&&file==u16((owner->stage_number&1)+3)){
        phase=StageResourceEffects::Phase::Scene;return Result::Ready;
    }
    for(u32 i=0;i<owner->ecl_animations.size();++i)
        if(resource==&owner->ecl_animations[i]&&file==u16(10+i)){
            phase=StageResourceEffects::Phase::Timeline;return Result::Ready;
        }
    return Result::Failed;
}
bool GameSession::cpp_stage_complete_file(AnmResource& resource,u16 file)noexcept{
    return stage_cpp_decode&&stage_cpp_decode->complete_file(resource,file);
}
#endif
#endif
ReplayRngRestoreResultTh14 GameSession::stage_cpp_worker_result()const noexcept{
    return stage_cpp_worker?stage_cpp_worker->poll():ReplayRngRestoreResultTh14::Failed;
}
bool GameSession::stage_cpp_worker_joined()const noexcept{return stage_cpp_worker&&stage_cpp_worker->joined();}
bool GameSession::start_cpp_stage_worker(){
    if(!stage_cpp_worker){error="TH14 C++ stage job was not prepared";return false;}
#if defined(TH14_CPP_STAGE_DECODE)
    std::shared_ptr<GameResources> reader;
    if(resources.effects&&resources.effects->stage_cpp_decode_enabled()){
        if(!source||!resources.loading_bundle()){error="TH14 C++ decode owner missing";return false;}
        // Clone lookup, archive index, LZSS dictionary and error/animation
        // scratch. Immutable archive bytes remain borrowed until joined.
        reader=std::make_shared<GameResources>(*source);
        stage_cpp_decode=std::make_shared<StageCppDecodeTh14>();stage_cpp_decode_owner=resources.loading_bundle();
    }
    const bool started=stage_cpp_worker->start([this,reader](){
#else
    const bool started=stage_cpp_worker->start([this](){
#endif
        StageCppWorkerScopeTh14 scope(this);
        AnmExecutionContextTh14 execution(static_cast<AnmEnvironment*>(&animations));
        if(!compositor.stage_start())return false;
#if defined(TH14_CPP_STAGE_DECODE)
        //435b44 then435b4e, after the copy0 update435b3a. Retain the
        // original helper's remaining stores after this intervening word.
        th14_stage_float_bits(animations.rate,0x3f800000u);
        if(auto* control=animations.th14_control.get())control->flags&=~0x4000u;
        th14_stage_store(battle->completion.state.ending_active,false);
        th14_stage_high_score(battle->hud.score.high,economy.score_units);
#else
        th14_stage_pre_rng(animations.rate,battle->completion.state.ending_active,battle->hud.score.high,economy.score_units);
#endif
#if defined(TH14_CPP_STAGE_DECODE)
        if(stage_cpp_decode){
#if defined(TH14_SECTION_TAIL_OBSERVER)
            th14_observer_section_tail(0,this);
#endif
            battle->enemy_commands.th14_reset_section_frame();
#if defined(TH14_SECTION_TAIL_OBSERVER)
            th14_observer_section_tail(1,this);
#endif
        }
#endif
        // Retail435b11..435b3a precedes456050 on the same worker.
        if(state.replay){
            th14_playback_gameplay_seed(animations.visual_rng,battle->replay_entry.seed);
            th14_playback_script_seed(animations.script_rng,battle->replay_entry.seed);
            th14_clear_gameplay_calls(animations.visual_rng);
            th14_project_stage_globals(battle->replay_entry,economy,&battle->enemy_environment.spell_practice_id,
                &battle->hud.score.continues,&battle->player->state.minimum_point_value,&state.character,&state.subtype);
        }else{
            const auto captured=th14_record_gameplay_seed(animations.visual_rng,recording_seed);
            th14_record_script_seed(animations.script_rng,captured);th14_clear_gameplay_calls(animations.visual_rng);
        }
#if defined(TH14_CPP_STAGE_DECODE)
        if(stage_cpp_decode){
            const auto character=state.character,subtype=state.subtype;
            if(!player_selection_valid(character,subtype)||
               !stage_cpp_decode->begin_hud_before_initialized(u32(character)*2u+u32(subtype)))return false;
            stage_cpp_decode->initialized();
            const bool decoded=stage_cpp_decode->run([this,&reader](u32 phase,u32 choice){
                auto& owner=*stage_cpp_decode_owner;
                if(phase==1&&!stage_cpp_decode->create_scene_owner(animations,owner.background,resources.core.text,
                        compositor.clear_color,owner.stage_number,compositor.world_camera))return false;
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
                const auto complete=[this](AnmResource& resource,u16 file){return stage_cpp_decode->publish_file(resource,file);};
                const bool loaded=phase==0?reader->load_stage_hud_files(owner.stage_number,owner,choice,complete):
                    phase==1?reader->load_stage_scene_files(owner.stage_number,owner,complete):reader->load_stage_timeline_files(owner.stage_number,owner,complete);
#else
                const bool loaded=phase==0?reader->load_stage_hud(owner.stage_number,owner,choice):
                    phase==1?reader->load_stage_scene(owner.stage_number,owner):reader->load_stage_timeline(owner.stage_number,owner);
#endif
                if(!loaded)return false;
                if(phase==0){
                    // Retail42eb30 loads logo then dialogue, then42ec42..75
                    // prepares the HUD on this loading thread before return.
                    battle->hud.prepare_stage(&battle->countdown_seconds);
                }
                if(phase==1){
                    if(!stage_cpp_decode->prepare_scene_storage(owner.scene.animation_count)||
                       !stage_cpp_decode->capture_scene_camera(compositor.world_camera)||
                       !stage_cpp_decode->initialize_scene_owner(owner.scene,owner.stage_number))return false;
                    // Retail435f4b returns directly into4228a0 on this thread.
                    // Publish the zeroed owner before loading its timeline.
                    return battle->enemies.initialize_empty(owner.timeline);
                }
                if(phase!=2)return true;
                if(!battle->enemies.finish_stage_owner())return false;
                if(!battle->prepare_next_tail_owners(state.character,state.subtype))return false;
                if(!resources.effects||!resources.effects->finish_cpp_stage_tail(
                    owner.stage_number,battle->enemy_commands.th14_run_flags,
                    battle->th14_control_clock,&animations.rate,*stage_cpp_worker))return false;
#if defined(TH14_SECTION_TAIL_OBSERVER)
                th14_observer_section_tail(2,this);
#endif
                // Retail436044 precedes Supervisor cleanup and436060's mask.
                // Supervisor/global/callback completion remains separate work.
                battle->enemy_commands.th14_set_stage_section(0);
                if(auto* control=animations.th14_control.get())control->flags&=~4u; //436053.
                battle->enemy_commands.th14_complete_loading_flags();
                stage_cpp_worker->publish_loading_completion();
                animations.th14_effect_stop=0; // Native43607b before copy interrupts.
                // Complete all four deferred copy labels before Ready can
                // enable Control. Main ANM updates may consume each word.
                th14_defer_stage_copy_tail(compositor.copies);
#if defined(TH14_SECTION_TAIL_OBSERVER)
                // A reused Control may already be enabled. Complete these
                // detailed reads before decoder ready3 permits Main adoption.
                th14_observer_section_tail(3,this);
#endif
                return true;
            });
            if(decoded){
                // Complete decoder ready3 bookkeeping before publishing
                //41d720. Main may adopt immediately after this release.
                if(auto* control=animations.th14_control.get())control->callbacks_enabled=1;
            }
            return decoded;
        }
#endif
        return true;
    },60000000,[this](){
        // Native tests close flags only while the first capture is pending.
        if(animations.first_deferred_capture_file_index()<0)return ReplayRngRestoreResultTh14::Ready;
        return (__atomic_load_n(&stage_load_host_flags,__ATOMIC_ACQUIRE)&0x180u)?
            ReplayRngRestoreResultTh14::Failed:ReplayRngRestoreResultTh14::Waiting;
    }
#if defined(TH14_CPP_STAGE_DECODE)
        ,[this](){if(auto* control=animations.th14_control.get())control->flags|=4u;}
#endif
);
    if(!started)error="TH14 C++ stage worker could not start";
    return started;
}
#endif
void GameSession::stop_stage_workers()noexcept{
#if defined(TH14_CPP_STAGE_WORKER)
    stop_cpp_stage_worker();
#endif
    if(resources.effects)resources.effects->stop_stage_workers();
}
void GameSession::shutdown()noexcept{reset_state();}
GameSession::~GameSession(){reset_state();}
#endif
void GameSession::reset_state(bool retain_transition) noexcept {
#if PORT_TITLE == 14
    const bool retain_title_animations=retain_transition&&bool(title)&&resources.effects&&resources.effects->initial_cpp_worker_enabled();
    stop_stage_workers();
#endif
    battle.reset();
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    if(th14_control_owner){animations.th14_control.retire(*th14_control_owner);th14_control_owner.reset();}
#endif
    if(ending&&resources.effects)for(auto& entry:ending->resources)resources.effects->release_animation(entry.second);
    ending.reset();
#if PORT_TITLE == 14
    for(auto* menu:{pause_menu.get(),idle_pause_menu.get()})if(menu&&menu->manual_resource){animations.retire_resource(*menu->manual_resource);if(resources.effects)resources.effects->release_animation(*menu->manual_resource);}
    idle_pause_menu.reset();
    if(title_manual&&title_manual->manual_resource){animations.retire_resource(*title_manual->manual_resource);if(resources.effects)resources.effects->release_animation(*title_manual->manual_resource);}
    title_manual.reset();
#endif
    pause_menu.reset();menu_input={};
#if PORT_TITLE == 14
    if(title&&retain_title_animations){
        // Title45982f calls47f140(handle[261],1): its ASCII19 root and
        // flattened ASCII18 children exit through their own ANM scripts.
        if(auto* root=animations.find(title->handles[261])){
            root->interrupt(1);
            if(!root->child.previous)for(auto* node=root->child.next;node;node=node->next)node->value->interrupt(1);
        }
    }
#endif
    title.reset();title_ascii.reset();title_fades.effects.clear();
    compositor.reset();
#if PORT_TITLE == 14
    if(retain_title_animations){
        // Retail Title destruction4597ae..459807 unloads only slots16/17.
        // 47d830 marks their VMs for the next ANM callback. ASCII title
        // effects keep executing their own scripts across Control creation.
        animations.retire_resource(resources.core.title);
        animations.retire_resource(resources.core.title_variant);
    }else clear_animations(animations,retain_transition?transition_type0:0,retain_transition?shared_loading.handle:0);
    if(!retain_transition){transition_type0=0;shared_loading.handle=0;}
#else
    clear_animations(animations);
#endif
#if PORT_TITLE == 14
    resources.cancel_stage_loading();
    resources.cancel_selected_player();
#endif
    if constexpr(kPortTitle==14||kPortTitle==16)animations.fallback_sprites=nullptr;
    state={};
    state.phase=GameSessionPhase::title;
    economy={};
    battle_attached=false;
#if PORT_TITLE == 14
    demo_title_pending=false;pending_initial_control_activation=false;
    pending_core_title=false;
#endif
    last_held=0;
    last_pause=false;
    game_input={};recording_active=false;
    error.clear();
}

bool GameSession::begin_replay(GameResources& source,const u8* data,u32 size,u32 stage,bool demo){
    if(!playback.open(data,size))return fail(playback.error().c_str());
    if(!stage)for(u32 n=1;n<=7;++n)if(playback.stage(n)){stage=n;break;}
    if(!playback.stage(stage))return fail("stage absent from replay");
    std::printf("[replay] %s: record stage %u, character %d subtype %d difficulty %d, %u bytes\n",
                demo?"demo":"selected", stage, playback.character(), playback.subtype(),
                playback.difficulty(), size);
    std::fflush(stdout);
    return begin(source,stage,playback.character(),playback.subtype(),playback.difficulty(),demo,true,(playback.decoded()[10]&1)!=0);
}
bool GameSession::begin(GameResources& source, u32 stage, i32 character,
                        i32 subtype, i32 difficulty, bool demo,bool replay,bool practice) {
    if(title){menu_selection=title->selection;config=title->config;scores.read_records(spell_records,clear_records);}
#if PORT_TITLE == 14
    if(replay){
        if(!replay_user_config)replay_user_config=std::make_unique<GameConfig>(config);
        std::memcpy(config.bytes.data(),playback.decoded().data()+0x18,config.bytes.size());
    }else if(replay_user_config){config=*replay_user_config;replay_user_config.reset();}
#endif
    reset_state(true);
    completed_run=false;
    this->source=&source;
    if(!player_selection_valid(character,subtype)||difficulty<0||difficulty>4)
        return fail("invalid TH11 session selection");
    if(!resources.load_core(source)) return fail(resources.error.c_str());
#if PORT_TITLE == 14
    if(!resources.select_player(u32(character),u32(subtype)))return fail(resources.error.c_str());
#endif
    if constexpr(kPortTitle==14||kPortTitle==16)animations.fallback_sprites=&resources.core.ascii;
    if(!initialize_compositor())return fail("screen compositor initialization failed");
    state.character=character;
    state.subtype=subtype;
    state.difficulty=difficulty;
    state.demo=demo;
    state.replay=replay;
    state.practice=practice;
#if PORT_TITLE == 14
    if(replay){if(playback.decoded()[10]&2){u32 id;std::memcpy(&id,playback.decoded().data()+0x90,4);state.spell_practice_id=signed_bits(id);}}
    else if((menu_selection.flags&48)==32)state.spell_practice_id=menu_selection.spell_id;
    if((replay&&(playback.decoded()[10]&2)||!replay&&(menu_selection.flags&48)==32)&&(state.spell_practice_id<0||state.spell_practice_id>=120))return fail("invalid TH14 spell practice selection");
    if(state.spell_practice_id>=0){state.practice=true;economy.lives=0;economy.th14_bombs=0;}
#endif
    initial_stage=stage;economy.rank=next_continues?-512:0;
    if(practice&&!replay
#if PORT_TITLE == 14
       &&state.spell_practice_id<0
#endif
    )economy.lives=menu_selection.practice_start?menu_selection.practice_start-1:9;
    economy.difficulty=difficulty;
    if(!replay&&!recording.begin(character,subtype,difficulty,practice,recording_timestamp))return fail("replay recorder initialization failed");
#if PORT_TITLE == 14
    if(!replay){std::memcpy(recording.header.data()+0x18,config.bytes.data(),config.bytes.size());if(state.spell_practice_id>=0){recording.header[10]=2;std::memcpy(recording.header.data()+0x90,&state.spell_practice_id,4);}}
#endif
    if(!replay){auto* counter=scores.characters[character*2+subtype].data()+ScoreFile::play_count_offset;i32 n;std::memcpy(&n,counter,4);if(n<99999){++n;std::memcpy(counter,&n,4);}}
    // TH14 own435caa reads 4d9cb8 / 4d9ca0, then scales by 100.
#if PORT_TITLE == 14
    constexpr i32 minimum[]={1000000,1000000,1000000,1000000,1000000};
    constexpr i32 maximum[]={10000000,25000000,50000000,50000000,50000000};
#else
    // Original 0x41f8f0 tables at 0x4a79e8/0x4a79fc, in stored units.
    constexpr i32 minimum[]={2500000,5000000,10000000,20000000,20000000};
    constexpr i32 maximum[]={5000000,10000000,20000000,40000000,40000000};
#endif
    economy.point_value=minimum[difficulty];economy.max_point_value=maximum[difficulty];
#if PORT_TITLE == 14
    // 435da5..435e17, after the player has installed its live step.
    if(!replay&&!(resources.effects&&resources.effects->initial_cpp_worker_enabled())){
        const auto& shot=resources.core.shots[shot_file_slot(u32(character),u32(subtype))].header;
        economy.power=std::min(stage==1&&state.spell_practice_id<0?100:400,shot.max_power_level*100);
    }
#else
    if(!replay&&stage>=2&&stage<=6){const auto& shot=resources.core.shots[shot_file_slot(u32(character),u32(subtype))].header;economy.power=shot.max_power_level*(kPortTitle==16?100:shot.power_step);}
#endif
    return start_stage(source,stage);
}

bool GameSession::start_stage(GameResources& source, u32 stage) {
    if(stage<1||stage>7) return fail("TH11 stage outside 1..7");
#if PORT_TITLE == 14
    stop_stage_workers();
#endif
    battle.reset();
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    if(th14_control_owner){animations.th14_control.retire(*th14_control_owner);th14_control_owner.reset();}
    th14_control_owner=std::make_unique<ControlStateTh14>();
    animations.th14_control.publish(*th14_control_owner); // Native4365ac/4365b9, before Player.
#endif
#if PORT_TITLE == 14
    // The native initial435c80 worker does not clear the animation manager.
    // Resource retirement and each VM's terminal script own their lifetimes.
    if(!(resources.effects&&resources.effects->initial_cpp_worker_enabled()))
        if(!clear_animations(animations,transition_type0,shared_loading.handle))return fail("ANM stage transition failed");
#else
    if(!clear_animations(animations))return fail("ANM stage transition failed");
#endif
#if PORT_TITLE == 14
    resources.cancel_stage_loading();
#endif
    if(!resources.load_stage(source,stage)) return fail(resources.error.c_str());
    resources.retire_previous();
#if PORT_TITLE == 14
    if((!resources.effects||!resources.effects->initial_cpp_worker_enabled())&&!compositor.stage_start())return fail("screen compositor stage reset failed");
#else
    if(!compositor.stage_start())return fail("screen compositor stage reset failed");
#endif
    // The direct stage-selection path replaces the resource bundle after
    // releasing its VMs. Native overlapping outgoing/incoming backgrounds
    // still require the game-control transition state machine.
    state.stage=stage;
    state.frame=0;
    state.phase=GameSessionPhase::stage;
    battle_attached=false;
#if PORT_TITLE == 14
    if(resources.effects&&!resources.effects->stage_ready()){
        state.phase=GameSessionPhase::loading;
        // prepare() ran before GameSessionResources adopted its stable owner.
        // Dispatch now, in the same Main callback, after Loading publication.
        if(resources.effects->initial_cpp_worker_enabled()&&!resources.effects->begin_initial_cpp_loading())
            return fail(resources.effects->last_error().c_str());
        return true;
    }
    return activate_stage();
}
#if defined(TH14_CPP_STAGE_WORKER) && defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
bool GameSession::prepare_initial_pause(){
    if(!StageCppWorkerScopeTh14::owns(this)||!initial_battle_private()||!battle||pause_menu||idle_pause_menu)
        return fail("invalid private TH14 pause construction");
    idle_pause_menu=std::make_unique<PauseMenu>(animations,resources.core.text,resources.core.front,scores);
    return true;
}
bool GameSession::construct_initial_player(){
    if(!StageCppWorkerScopeTh14::owns(this)||!initial_battle_private()||pending_core_title||battle||
       !player_selection_valid(state.character,state.subtype))
        return fail("no private TH14 initial player construction");
    battle=std::make_unique<GameBattle>(resources,animations,economy,spell_records,compositor,clear_records,true);
    return battle->construct_initial_player(state.character,state.subtype)||fail(battle->error.c_str());
}
#endif
bool GameSession::prepare_initial_player(){
#if defined(TH14_CPP_STAGE_WORKER) && defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
    if(state.phase!=GameSessionPhase::loading||pending_core_title||battle_attached||
       (battle&&(!StageCppWorkerScopeTh14::owns(this)||!battle->initial_player_waiting_for_resources)))
        return fail("no pending TH14 initial player load");
#else
    if(state.phase!=GameSessionPhase::loading||pending_core_title||battle_attached||battle)return fail("no pending TH14 initial player load");
#endif
    {
        // 435da5 initializes starting power after the selected SHT has loaded,
        // for ordinary play as well as before the first replay manager
        // copies the selected entry at 454a51 later in the worker.
        const auto& shot=resources.core.shots[shot_file_slot(u32(state.character),u32(state.subtype))].header;
        economy.power=std::min(state.stage==1&&state.spell_practice_id<0?100:400,shot.max_power_level*100);
    }
#if defined(TH14_CPP_STAGE_WORKER) && defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
    if(!battle)battle=std::make_unique<GameBattle>(resources,animations,economy,spell_records,compositor,clear_records,
        StageCppWorkerScopeTh14::owns(this));
#else
    battle=std::make_unique<GameBattle>(resources,animations,economy,spell_records,compositor,clear_records);
#endif
    // Keep the selected mode with the private owner before loader return.
    // The worker clears the transient bits; Main must not rebuild this word.
    battle->enemy_commands.th14_run_flags=state.demo?0x40u:
        (state.spell_practice_id>=0?0x20u:state.practice&&!state.replay?0x10u:0u);
    if(!battle->prepare_initial_player(state.character,state.subtype))return fail(battle->error.c_str());
    // 435e1d finishes Player options before 435edf creates the replay
    // manager. Its seed/counter/globals setup precedes Scene/HUD/Timeline.
    // Keep the entry with the existing private battle owner for activation.
    return prepare_initial_entry(battle->replay_entry);
}
bool GameSession::prepare_initial_entry(ReplayStageState& entry){
    auto& gameplay_rng=animations.visual_rng;
    recording_seed=gameplay_rng.seed_value();
    if(!state.replay)gameplay_rng.set_call_count(0);
    if(state.replay){
        if(!playback.select(state.stage))return fail("stage absent from replay");
        entry.read(playback.header(state.stage));
        const auto& shot=resources.core.shots[shot_file_slot(u32(state.character),u32(state.subtype))].header;
        economy.max_power=shot.max_power_level*shot.power_step;economy.power_step=shot.power_step;
        // This retains the existing first-manager seed semantics. Selected
        // replay UI mode routing is a separate, still unqualified contract.
        th14_restore_replay_rng(gameplay_rng,&animations.script_rng,entry.seed,true);
        th14_project_stage_globals(entry,economy,nullptr,nullptr,nullptr,&state.character,&state.subtype);
        state.difficulty=economy.difficulty;
    }
    return true;
}
bool GameSession::prepare_initial_scene(bool complete){
    if(state.phase!=GameSessionPhase::loading||pending_core_title||battle_attached||!battle)
        return fail("no pending TH14 initial scene load");
    return battle->prepare_initial_scene(complete)||fail(battle->error.c_str());
}
bool GameSession::finish_loading(bool defer_initial_control){
    if(state.phase!=GameSessionPhase::loading)return fail("no pending TH14 resource load");
    if(pending_core_title){
        if(resources.effects&&!resources.effects->core_ready())return fail("TH14 core resources still pending");
        if(!resources.prepare_core())return fail(resources.error.c_str());
        if(compositor.copies[0].resource!=&resources.core.text){
            if(!initialize_compositor())return fail(("title compositor initialization failed: "+compositor.last_error).c_str());
            reset_compositor_cameras(false);
        }
        pending_core_title=false;return activate_title(*source,pending_core_first,pending_core_screen);
    }
    if(resources.effects&&!resources.effects->stage_ready())return fail("TH14 stage resources still pending");
    if(!resources.prepare_stage())return fail(resources.error.c_str());
    if(battle&&battle->pending_stage){
        if(!battle->finish_stage_loading())return fail(battle->error.empty()?resources.error.c_str():battle->error.c_str());
        // Match the synchronous transition's completed session boundary.
        state.frame=1;state.phase=GameSessionPhase::stage;return true;
    }
    if(defer_initial_control){pending_initial_control_activation=true;return true;}
    return activate_stage();
}
bool GameSession::activate_stage(){
    const u32 stage=state.stage;state.phase=GameSessionPhase::stage;
#endif
    ReplayStageState entry;
#if PORT_TITLE == 14
    const bool initial_worker_owner=battle&&battle->player;
    if(initial_worker_owner)entry=battle->replay_entry;
    else if(!prepare_initial_entry(entry))return false;
#else
    auto& gameplay_rng=kPortTitle==14?animations.visual_rng:animations.script_rng;
    recording_seed=gameplay_rng.seed_value();
    if(!state.replay)gameplay_rng.set_call_count(0);
    if(state.replay){
        if(!playback.select(stage))return fail("stage absent from replay");
        entry.read(playback.header(stage));
        const auto& shot=resources.core.shots[shot_file_slot(u32(state.character),u32(state.subtype))].header;
        economy.max_power=shot.max_power_level*shot.power_step;economy.power_step=shot.power_step;
        // Both ordinary and attract startup create the replay manager here.
        // 454a41..454a51 restores gameplay RNG and globals but retains script
        // RNG. Only existing-controller stage entry uses 456050 (first=false).
#if PORT_TITLE == 14
        th14_restore_replay_rng(gameplay_rng,&animations.script_rng,entry.seed,true);
        th14_project_stage_globals(entry,economy,nullptr,nullptr,nullptr,&state.character,&state.subtype);
        state.difficulty=economy.difficulty;
#else
        entry.restore(economy,gameplay_rng,true,nullptr);
#endif
    }
#endif
#if PORT_TITLE == 14
    if(!battle)
#endif
    battle=std::make_unique<GameBattle>(resources,animations,economy,spell_records,compositor,clear_records);
#if PORT_TITLE == 14
    battle->enemy_environment.spell_practice_id=state.replay?entry.th14_practice_mode:state.spell_practice_id;
    // A joined initial worker already applied436060. Preserve its word,
    // including modes the frontend supplied to the existing private owner.
    // Direct/unprepared entry retains the existing mode selection fallback.
    if(!initial_worker_owner){
        battle->enemy_commands.th14_run_flags=state.demo?0x40u:
            (state.spell_practice_id>=0?0x20u:state.practice&&!state.replay?0x10u:0u);
        battle->enemy_commands.th14_complete_loading_flags();
    }else if(!(resources.effects&&resources.effects->initial_cpp_worker_enabled())){
        // Hosted/private Player preparation also retains the replay entry,
        // but has no C++ initial worker to execute436060. Clear only here.
        battle->enemy_commands.th14_complete_loading_flags();
    }
#if defined(TH14_CPP_STAGE_DECODE)
    // Direct entry has no loading worker; establish its completed Control.
    if(!(resources.effects&&resources.effects->initial_cpp_worker_enabled()))if(auto* control=animations.th14_control.get()){
        control->flags&=~4u;control->callbacks_enabled=1;
    }
#endif
    battle->th14_audio_flags=persistent_config().bytes[0x38];battle->th14_scores=&scores;
#endif
    battle->replaying=state.replay;battle->demo=state.demo;battle->replay_entry=entry;
#if PORT_TITLE == 14
    battle->on_game_over=[this](bool replay){
        if(replay)return !interactive||pause();
        return open_result(false);
    };
#endif
    battle->completion.mode.control_mode=state.replay||state.demo;
    battle->completion.mode.replay_mode=state.replay?1:0;
    battle->completion.mode.practice=state.practice&&!state.replay;battle->completion.mode.replay_practice=state.practice&&state.replay;
    if(state.practice){const auto* row=scores.characters[state.character*2+state.subtype].data()+ScoreFile::practice_score_offset+(i32(stage)+state.difficulty*ScoreFile::practice_stage_stride)*8;std::memcpy(&battle->practice_high,row,4);}
    const i32 score_selection=kPortTitle==16?state.character:state.character*2+state.subtype;
    battle->hud.score.high=scores.high_score(score_selection,state.difficulty);
    battle->hud.score.high_continues=scores.high_continues(score_selection,state.difficulty);
#if PORT_TITLE == 14
    if(state.spell_practice_id>=0){const auto* record=scores.characters[score_selection].data()+ScoreFile::spell_offset+state.spell_practice_id*ScoreFile::spell_stride;std::memcpy(&battle->hud.score.high,record+0x98,4);battle->hud.score.high_continues=0;battle->practice_high=battle->hud.score.high;}
#endif
    battle->hud.score.continues=next_continues;next_continues=0;
    if(state.replay)battle->hud.score.continues=entry.continues;
    if(!battle->initialize(state.character,state.subtype,state.difficulty)) {
        error="stage battle initialization failed: "+battle->error;
        battle.reset();
        return false;
    }
#if PORT_TITLE == 14
    constexpr i32 minimum[]={1000000,1000000,1000000,1000000,1000000};
#else
    constexpr i32 minimum[]={2500000,5000000,10000000,20000000,20000000};
#endif
    battle->player->state.minimum_point_value=minimum[state.difficulty];
#if PORT_TITLE == 14
    if(state.replay)battle->player->state.minimum_point_value=signed_bits(entry.th14_globals[10]);
#endif
    battle->player_input.replay=state.replay;
    battle->completion.mode.control_mode=state.replay||state.demo;
    battle->completion.mode.replay_mode=state.replay?1:0;
#if PORT_TITLE == 14
    // Initial Control11 activation follows overlay ANM8, as in 4369b3/4369d8.
    battle->initial_control_activation=[this](){
        shared_loading.finish(animations);
        if(auto* transition=animations.find(transition_type0))transition->interrupt(1);
    };
#endif
    battle_attached=true;
    if(!state.demo)scores.settings[0x26+stage_music(stage)]=1;
    if(!state.replay&&!record_stage_entry(true))return false;
    return true;
}

bool GameSession::record_stage_entry(bool initial){
    if(!battle||!battle->player)return fail("recording requires a live player");
    auto& e=economy;const auto& p=battle->player->motion.state;
    ReplayStageState s;s.seed=recording_seed;s.score=th14_hud_load(e.score_units);s.power=e.power;s.points=e.point_value;
    s.lives=e.lives;s.fragments=e.life_fragments;s.rank=e.rank;s.x=p.x;s.y=p.y;s.continues=battle->hud.score.continues;
    s.focused=p.focused;s.graze=e.graze;s.weapon=p.weapon_mode;
#if PORT_TITLE == 14
    s.th14_practice_mode=battle->enemy_environment.spell_practice_id;
    s.th14_globals={u32(state.character),u32(state.subtype),u32(th14_hud_load(e.score_units)),u32(state.difficulty),u32(s.continues),u32(e.rank),u32(e.graze),u32(s.th14_practice_mode),
        u32(e.th14_point_items),u32(e.point_value),u32(battle->player->state.minimum_point_value),u32(e.max_point_value),u32(e.power),u32(e.max_power),u32(e.power_step),
        u32(e.lives),u32(e.life_fragments),u32(e.th14_life_awards),u32(e.th14_bombs),u32(e.th14_bomb_fragments),u32(e.th14_combo.total),u32(e.th14_reserved_54),
        u32(e.th14_combo.count),u32(e.th14_combo.timer),float_bits(e.th14_combo.position.x),float_bits(e.th14_combo.position.y),float_bits(e.th14_combo.position.z),u32(e.th14_special_drop_count)};
    // Native D8 carries 4d9658 bit 0; the ordinary port session uses 0.
    s.th14_stage_flags=0;
#endif
    if(!recording.start_stage(state.stage,s,initial))return fail("replay stage recording failed");
    if(initial)for(u32 i=0;i<20;++i)battle->spell_timing.records[i]=i*0xdeaddeadu;
    recording_active=true;return true;
}
bool GameSession::save_replay(const char* name,std::vector<u8>& output,bool terminal,float slowdown){
    if(!battle){if(!completed_run)return false;return recording.save(name,completed_score,8,completed_continues,slowdown,output);}
    if(state.replay||!recording.selected)return false;
    recording.spell_times(battle->spell_timing.records);
    // Saving must not append multiple end markers or mutate an active run.
    auto snapshot=recording;if(terminal&&!snapshot.finish_stage(true))return false;
    const i32 reached=pause_menu&&pause_menu->completed&&!state.practice?8:i32(state.stage);
    return snapshot.save(name,th14_hud_load(economy.score_units),reached,battle->hud.score.continues,slowdown,output);
}

#if PORT_TITLE == 14
bool GameSession::update(const GameSessionInput& input){return update_impl(input,false);}
bool GameSession::update_after_initial_overlay(const GameSessionInput& input){
    if(state.phase!=GameSessionPhase::loading||!pending_initial_control_activation)
        return fail("TH14 initial overlay handoff without pending Control");
    return update_impl(input,true);
}
bool GameSession::update_impl(const GameSessionInput& input,bool overlay_updated_by_host) {
#else
bool GameSession::update(const GameSessionInput& input) {
#endif
    if(!error.empty()) return false;
    if(state.phase==GameSessionPhase::finished) return true;
#if PORT_TITLE == 14
    // HUD28 and Ending31 post scene requests after Supervisor1 has run.
    // Create the next scene here, then dispatch its callbacks in this pass.
    if(state.phase==GameSessionPhase::stage&&battle&&battle->completion.state.exit==StageExit::Ending){
        if(!open_ending())return false;
    }
    if(state.phase==GameSessionPhase::ending&&ending&&!ending->active){
        if(!open_results())return false;
    }
    bool initial_overlay_already_updated=overlay_updated_by_host;
    if(state.phase==GameSessionPhase::loading&&pending_initial_control_activation){
        // The ready worker enables Control11; overlay ANM8 still precedes it.
        if(!initial_overlay_already_updated&&!animations.update(true))return fail("TH14 initial activation overlay update failed");
        pending_initial_control_activation=false;
        if(!activate_stage())return false;
        initial_overlay_already_updated=true;
    }
    // Supervisor1 consumes requests posted by Title6 on the preceding pass.
    // Keep that request pass alive through overlay8 and normal ANM29.
    if(state.phase==GameSessionPhase::title&&title){
        const auto finish_scene_switch=[this](bool ok){
            if(!ok||state.phase!=GameSessionPhase::loading)return ok;
            if(!animations.update(true)||!animations.update(false))return fail("TH14 scene-switch ANM update failed");
            return true;
        };
        if(!source)return fail("title lifetime missing");
        if(title->start_requested){const auto choice=title->selection;return finish_scene_switch(begin(*source,choice.stage,choice.character,choice.partner,choice.difficulty,false,false,(choice.flags&16)!=0));}
        if(title->replay_start_requested){auto entry=title->replay_files[title->replay_file];last_replay_file=title->replay_file;const u32 stage=title->replay_stage+1;
            if(entry->file.empty()&&(!resources.effects||!resources.effects->read_replay(entry->path,entry->file)))return fail("selected replay could not be loaded");
            return finish_scene_switch(begin_replay(*source,entry->file.data(),u32(entry->file.size()),stage));}
        if(title_idle>=1800){
            const DemoPlaylist playlist=demo_playlist();const auto path=std::string(playlist.names[demo_index<playlist.step?demo_index:0]);
            demo_index=(demo_index+1)%playlist.step;title_idle=0;std::vector<u8> bytes;
            if(!source->read(path,bytes))return fail("original demo resource missing");
            return finish_scene_switch(begin_replay(*source,bytes.data(),u32(bytes.size()),0,true));
        }
    }
    if(state.phase==GameSessionPhase::loading)return true;
#if PORT_TITLE == 14
    // Consume the preceding dialogue/HUD request at Supervisor1, before the
    // incoming callback pass. The loader is launched here, not at ANM29's tail.
    if(state.phase==GameSessionPhase::stage&&battle&&battle->completion.state.exit==StageExit::NextStage){
        th14_stage_score_save(scores,spell_records,clear_records,[this](const std::vector<u8>& bytes){
            return !resources.effects||resources.effects->write_stage_score(bytes);
        });
        const u32 next=u32(battle->completion.state.next_stage);
        if(!state.replay){recording.spell_times(battle->spell_timing.records);recording_active=false;}
        if(state.replay){
            if(!playback.stage(next)){state.phase=GameSessionPhase::finished;return true;}
            battle->replay_entry.read(playback.header(next));
        }
#if defined(TH14_CPP_STAGE_WORKER)
        if(resources.effects&&resources.effects->stage_cpp_worker_enabled()){
            stop_cpp_stage_worker();stage_cpp_worker=std::make_shared<StageCppWorkerTh14>();
        }
#endif
        auto initialize=[this,restore=ReplayRngRestoreJobTh14{},worker=std::shared_ptr<StageMemoryTaskTh14>{}
        ]()mutable->prototype::Work {
#if defined(TH14_CPP_STAGE_WORKER)
            if(resources.effects&&resources.effects->stage_cpp_worker_enabled()){
                if(!stage_cpp_worker){error="TH14 C++ stage worker owner missing";return prototype::Work::Failed;}
                auto result=stage_cpp_worker->poll();
#if defined(TH14_CPP_STAGE_DECODE)
                if(stage_cpp_decode&&result!=ReplayRngRestoreResultTh14::Failed){
                    if(stage_cpp_decode->failure()){error="TH14 C++ resource decode failed";return prototype::Work::Failed;}
                    result=stage_cpp_decode->initialization_ready()?ReplayRngRestoreResultTh14::Ready:ReplayRngRestoreResultTh14::Waiting;
                }
#endif
                if(result==ReplayRngRestoreResultTh14::Waiting)return prototype::Work::Waiting;
#if defined(TH14_CPP_STAGE_DECODE)
                if(!stage_cpp_decode)
#endif
                stage_cpp_worker->join();
                if(result==ReplayRngRestoreResultTh14::Failed){error="TH14 C++ stage initialization failed";return prototype::Work::Failed;}
                if(state.replay){
                    battle->enemy_environment.character=state.character;battle->enemy_environment.subtype=state.subtype;
                    battle->enemy_environment.difficulty=state.difficulty=economy.difficulty;
                }
                return prototype::Work::Ready;
            }
#endif
            if(resources.effects&&resources.effects->stage_memory_worker_enabled()){
                if(!worker){
                    worker=std::make_shared<StageMemoryTaskTh14>();
                    worker->bindings={&animations.visual_rng,&animations.script_rng,&battle->replay_entry,&recording_seed,&economy,
                        &battle->enemy_environment.spell_practice_id,&battle->hud.score.continues,&battle->player->state.minimum_point_value,
                        &state.character,&state.subtype};
                }
                const auto result=resources.effects->stage_memory_worker(worker->bindings,worker->stack_end(),state.replay);
                if(result==ReplayRngRestoreResultTh14::Failed){error="TH14 physical stage restore failed";return prototype::Work::Failed;}
                if(result==ReplayRngRestoreResultTh14::Ready&&state.replay){
                    battle->enemy_environment.character=state.character;battle->enemy_environment.subtype=state.subtype;
                    battle->enemy_environment.difficulty=state.difficulty=economy.difficulty;
                }
                return result==ReplayRngRestoreResultTh14::Ready?prototype::Work::Ready:prototype::Work::Waiting;
            }
            auto& rng=animations.visual_rng;
            const auto ready=[this](ReplayRngRestorePointTh14 point){return resources.effects?resources.effects->stage_rng_store_readiness(point):ReplayRngRestoreResultTh14::Ready;};
            const auto committed=[this](ReplayRngRestorePointTh14 point){return !resources.effects||resources.effects->stage_rng_store_committed(point);};
            const auto progress=state.replay?restore.playback(rng,&animations.script_rng,battle->replay_entry.seed,false,ready,committed):
                restore.record(rng,animations.script_rng,recording_seed,ready,committed);
            if(progress==ReplayRngRestoreResultTh14::Waiting)return prototype::Work::Waiting;
            if(progress==ReplayRngRestoreResultTh14::Failed){error="TH14 stage RNG restore failed";return prototype::Work::Failed;}
            if(state.replay){
                const auto snapshot=resources.effects?resources.effects->stage_replay_snapshot_readiness():ReplayRngRestoreResultTh14::Ready;
                if(snapshot==ReplayRngRestoreResultTh14::Waiting)return prototype::Work::Waiting;
                if(snapshot==ReplayRngRestoreResultTh14::Failed){error="TH14 replay snapshot restore failed";return prototype::Work::Failed;}
                th14_project_stage_globals(battle->replay_entry,economy,&battle->enemy_environment.spell_practice_id,
                    &battle->hud.score.continues,&battle->player->state.minimum_point_value,&state.character,&state.subtype);
                battle->enemy_environment.character=state.character;battle->enemy_environment.subtype=state.subtype;
                battle->enemy_environment.difficulty=state.difficulty=economy.difficulty;
                if(resources.effects&&!resources.effects->stage_replay_snapshot_committed()){
                    error="TH14 replay snapshot acknowledgement failed";return prototype::Work::Failed;
                }
            }
            return prototype::Work::Ready;
        };
        if(!source||!battle->next_stage(*source,next,{},std::move(initialize))){error="stage transition failed: "+battle->error;return false;}
        state.stage=next;state.frame=0;
    }
#endif
    if(battle&&battle->pending_stage){
        // Only the first actual deferred copy slot guards worker entry;
        // graphics upload readiness is a separate resource phase boundary.
        if(!battle->pump_stage_loading(stage_load_clock(),animations.first_deferred_capture_file_index(),stage_load_host_flags))return fail(battle->error.c_str());
    }
#endif
    GameSessionInput edges=input;
    edges.pressed|=input.held&~last_held;
    edges.released|=last_held&~input.held;
#if PORT_TITLE == 14
    bool overlay_already_updated=initial_overlay_already_updated;
#endif
    const bool pause_edge=input.pause&&!last_pause;
    const bool confirm_edge=(edges.pressed&0x101u)!=0;
    last_held=input.held;
    last_pause=input.pause;
    if(state.phase==GameSessionPhase::ending){
        if(!ending)return fail("ending lifetime missing");
#if PORT_TITLE == 14
        const u32 raw=(input.held&~256u)|((input.held&256)?0x80000:0)|(input.pause?256:0);
        if(!ending->dispatch(game_input,raw))return fail(ending->error.c_str());
#else
        game_input.update((input.held&~256u)|((input.held&256)?0x80000:0));
        if(!ending->tick(game_input.held,game_input.pressed,0))return fail(ending->error.c_str());
        if(!ending->active)return open_results();
        if(!animations.update(true)||!animations.update(false))return fail("ending ANM update failed");
#endif
        ++state.frame;return true;
    }
#if PORT_TITLE == 14
    // PauseMenu9 reads Control+14 before Control11 advances its rate-scaled clock.
    if(pause_edge&&state.phase==GameSessionPhase::stage&&!state.demo&&battle&&battle->control_frame()>=30) {
#else
    if(pause_edge&&state.phase==GameSessionPhase::stage&&!state.demo&&state.frame>29) {
#endif
#if PORT_TITLE == 14
        // Overlay registry8 precedes the PauseMenu9 callback even on entry.
        if(!animations.update(true))return fail("pause entry overlay update failed");
#endif
        menu_input.update((input.held&~256u)|((input.held&256)?0x80000:0)|256);
        return pause();
    }
    if(state.phase==GameSessionPhase::paused||state.phase==GameSessionPhase::game_over){
        if(!pause_menu)return fail("pause menu lifetime missing");
        menu_input.update((input.held&~256u)|((input.held&256)?0x80000:0)|(input.pause?256:0));
#if PORT_TITLE == 14
        // Retail47aac5 registers overlay8 before448080's menu9 callback.
        if(!animations.update(true))return fail("pause ANM update failed");
#endif
        if(!pause_menu->update(menu_input.pressed,menu_input.long_repeat))return fail(pause_menu->error.c_str());
#if PORT_TITLE == 14
        auto& menu=*pause_menu;
        if(menu.manual_requested&&menu.timer.current>=20&&!menu.manual_resource){
            menu.manual_resource=std::make_unique<AnmResource>();
            if(!source||!source->open_anm("help.anm",*menu.manual_resource))return fail("TH14 help resource loading failed");
            if(!animations.prepare_resource(*menu.manual_resource,19))return fail("TH14 help template initialization failed");
            if(resources.effects&&!resources.effects->prepare_animation(*menu.manual_resource))return fail(resources.effects->last_error().c_str());
            menu.begin_manual();
        }
        if(menu.manual_image_requested>=0){
            char path[24];std::snprintf(path,sizeof path,"help_%.2d.png",menu.manual_image_requested+1);std::vector<u8> image;
            if(!source||!source->read(path,image))return fail("TH14 help page loading failed");
            if(resources.effects&&!resources.effects->prepare_manual_image(*menu.manual_resource,1,image.data(),u32(image.size())))return fail(resources.effects->last_error().c_str());
            menu.manual_image_requested=-1;menu.manual_substate=3;
        }
        if(menu.manual_finished&&menu.manual_resource){
            animations.retire_resource(*menu.manual_resource);
        }
#endif
#if PORT_TITLE != 14
        if(!animations.update(true))return fail("pause ANM update failed");
#endif
#if PORT_TITLE == 14
        if(menu.manual_finished&&menu.manual_resource&&!animations.references(*menu.manual_resource)){
            if(resources.effects)resources.effects->release_animation(*menu.manual_resource);menu.manual_resource.reset();
        }
#endif
        switch(pause_menu->action){
        case PauseAction::Resume:
#if PORT_TITLE == 14
            if(!resume())return false;
            overlay_already_updated=true;break;
#else
            return resume();
#endif
        case PauseAction::Title:return return_to_title();
        case PauseAction::Restart:return restart_run(false);
        case PauseAction::Continue:
#if PORT_TITLE == 14
            if(!continue_run_th14())return false;
            overlay_already_updated=true;break;
#else
            return restart_run(true);
#endif
        default:break;
        }
#if PORT_TITLE == 14
        if(!overlay_already_updated){
            // Retail434360 remains registered at priority17 while Menu9 pauses combat.
            if(battle&&!battle->completion.state.ending_active)battle->hud.score.update(economy.score_units);
            return true;
        }
#else
        return true;
#endif
    }
    if(state.phase==GameSessionPhase::title) {
        if(!title||!source)return fail("title lifetime missing");
        const u32 raw=(input.held&~256u)|((input.held&256)?0x80000:0)|(input.pause?256:0);
        // Retail459944..45995c increments idle only when Title+1c is already1.
        // Its state0 activation dispatches the first main-menu update later
        // in that callback. Our TitleMenu is constructed as Main, so its
        // first session frame must likewise leave the idle counter alone.
        if(title->screen==TitleScreen::Main&&(kPortTitle!=14||state.frame!=0)){++title_idle;if(raw)title_idle=0;
#if PORT_TITLE != 14
            if(title_idle>=1800){const DemoPlaylist playlist=demo_playlist();
                const auto path=std::string(playlist.names[demo_index<playlist.step?demo_index:0]);
                std::printf("[demo] list entry %u of %u: %s\n", demo_index, playlist.step,
                            path.c_str());
                std::fflush(stdout);
                demo_index=(demo_index+1)%playlist.step;std::vector<u8> bytes;
                if(!source->read(path,bytes))return fail("original demo resource missing");title_idle=0;
                std::printf("[demo] loaded %s, %u bytes\n", path.c_str(), u32(bytes.size()));
                std::fflush(stdout);
                return begin_replay(*source,bytes.data(),u32(bytes.size()),0,true);
            }
#endif
        }
        game_input.update(raw);title->held=raw;
#if PORT_TITLE == 14
        if(title_manual)title->manual_finished=title_manual->manual_finished;
#endif
        if(!title->update(game_input.pressed,game_input.long_repeat))return fail(title->error.c_str());
#if PORT_TITLE == 14
        if(title->manual_requested){
            title_manual=std::make_unique<PauseMenu>(animations,resources.core.ascii,resources.core.front,scores);
            auto& manual=*title_manual;manual.manual_resource=std::make_unique<AnmResource>();
            if(!source->open_anm("help.anm",*manual.manual_resource))return fail("TH14 title help resource loading failed");
            if(!animations.prepare_resource(*manual.manual_resource,19))return fail("TH14 title help template initialization failed");
            if(resources.effects&&!resources.effects->prepare_animation(*manual.manual_resource))return fail(resources.effects->last_error().c_str());
            manual.begin_manual();manual.manual_origin_x=title->manual_origin_x;title->manual_requested=false;
        }
        if(title_manual&&title->screen==TitleScreen::Manual){
            auto& manual=*title_manual;manual.sounds.clear();manual.manual_update(game_input.pressed,game_input.long_repeat);
            if(!manual.error.empty())return fail(manual.error.c_str());
            title->sounds.insert(title->sounds.end(),manual.sounds.begin(),manual.sounds.end());
            if(manual.manual_image_requested>=0){
                char path[24];std::snprintf(path,sizeof path,"help_%.2d.png",manual.manual_image_requested+1);std::vector<u8> image;
                if(!source->read(path,image))return fail("TH14 title help page loading failed");
                if(resources.effects&&!resources.effects->prepare_manual_image(*manual.manual_resource,1,image.data(),u32(image.size())))return fail(resources.effects->last_error().c_str());
                manual.manual_image_requested=-1;manual.manual_substate=3;
            }
        }
        if(title->manual_release_requested&&title_manual&&title_manual->manual_resource)animations.retire_resource(*title_manual->manual_resource);
#endif
        config=title->config;
        if(title->fade_requested){
#if PORT_TITLE == 14
            auto* transition=animations.create(resources.core.effect,0,8,u32(-1));
            if(!transition||!anm_th14_install_effect(*transition,0,animations))return fail("title transition initialization failed");
            transition_type0=transition->id;transition->interrupt(7);
            if(!shared_loading.begin(animations,resources.core.ascii))return fail("TH14 title loading animation creation failed");
#else
            title_fades.start(5,32,0,61,&animations.rate);
#endif
        }
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
        title_fades.update(false,false,&animations.th14_effect_stop,&animations.th14_control);
#else
        title_fades.update();
#endif
#if PORT_TITLE != 14
        if(title->start_requested){const auto choice=title->selection;return begin(*source,choice.stage,choice.character,choice.partner,choice.difficulty,false,false,(choice.flags&16)!=0);}
        if(title->replay_start_requested){auto entry=title->replay_files[title->replay_file];last_replay_file=title->replay_file;const u32 stage=title->replay_stage+1;
            if(entry->file.empty()&&(!resources.effects||!resources.effects->read_replay(entry->path,entry->file)))return fail("selected replay could not be loaded");
            return begin_replay(*source,entry->file.data(),u32(entry->file.size()),stage);}
#endif
        if(!animations.update(true)||!animations.update(false)) return fail("title ANM update failed");
#if PORT_TITLE == 14
        if(title->manual_release_requested&&title_manual&&title_manual->manual_resource&&!animations.references(*title_manual->manual_resource)){
            if(resources.effects)resources.effects->release_animation(*title_manual->manual_resource);
            title_manual.reset();title->manual_release_requested=false;title->manual_finished=false;
        }
#endif
        if(title->screen==TitleScreen::Exit
#if PORT_TITLE == 14
           &&!title->start_requested&&!title->replay_start_requested&&title_idle<1800
#endif
        )state.phase=GameSessionPhase::finished;
        ++state.frame;
        return true;
    }
    if(state.phase!=GameSessionPhase::stage) return true;
    if(interactive&&state.demo){const u32 raw=(input.held&~256u)|((input.held&256)?0x80000:0)|(input.pause?256:0);
#if PORT_TITLE == 14
        // 436c08..436c7b requests the outer title transition before the
        // controller increments its clock and the remaining combat callbacks.
        // Preserve that final combat frame; apply the request next update.
        if(demo_title_pending){
            if(!return_to_title())return false;
            // The key cancelling attract playback remains held across frames.
            // Seed the new menu's edges so it cannot also activate Game Start.
            game_input.update(raw);return true;
        }
#else
        if((raw&0x80103)||state.frame==3600)return return_to_title();
        if(state.frame==3540&&battle)battle->fades.start(5,60,0,67,&animations.rate);
#endif
    }
    // ANM uses the same 60 Hz rate as the original game.  Combat managers are
    // deliberately called at this exact boundary once attached by GameWorld.
    if(battle_attached) {
        if(!battle)return fail("battle lifetime missing");
        bool playback_ended=false;
        const bool stage_was_active=battle->stage_active;
        const bool updated=battle->update([&]{
#if PORT_TITLE == 14
        // Original436a40 ->455ed0 ->456170 clears input hold/repeat state
        // at activation, before the first incoming-stage Replay callback.
        if(!stage_was_active&&battle->stage_active)game_input={};
#endif
        if(state.replay){
            if(battle->stage_active&&playback.selected_stage()!=state.stage&&!playback.select(state.stage))return fail("replay activation stage missing");
            const auto recorded=playback.tick();
            if(recorded.end_marker){
                playback_ended=true;
#if PORT_TITLE == 14
                if(interactive){
                    // Replay12 clears the three masks, opens448ef0 and then
                    // returns to this callback pass. Hold/repeat clocks are
                    // not recomputed by the original terminal-marker branch.
                    game_input.held=game_input.pressed=game_input.released=0;
                    edges.held=edges.pressed=edges.released=0;
                    battle->completion.state.exit=StageExit::ReplayEnd;
                    if(!pause())return false;
                }else return false;
#else
                return false;
#endif
            }else{
            edges.held=recorded.held;edges.pressed=recorded.pressed;edges.released=recorded.released;
#if PORT_TITLE == 14
            // Retail455180 loads all three words, then41e710 recomputes the
            // edges from the current/previous normalized masks. Saved edges
            // can refer to a preceding stage when playback starts mid-run.
            game_input.update_normalized(recorded.held);
            edges.pressed=game_input.pressed;edges.released=game_input.released;
#endif
            auto& motion=battle->player_input.movement;motion.touch_mode=recorded.touch_mode;motion.touch_x=recorded.touch_x;motion.touch_y=recorded.touch_y;
            }
        }else{
            game_input.update(input.held,config.auto_focus());
            edges.held=game_input.held;edges.pressed=game_input.pressed;edges.released=game_input.released;
            if(battle->stage_active&&!recording_active){recording.finish_stage();if(!record_stage_entry(false))return false;}
            const auto& motion=battle->player_input.movement;
            if(!recording.tick(edges.held,edges.pressed,edges.released,input.recording_fps,motion.touch_mode,motion.touch_x,motion.touch_y))return fail("replay recording limit reached");
        }
#if PORT_TITLE == 14
        edges.dialogue_skip_frames=game_input.held_frames[9];
        edges.dialogue_fire_frames=game_input.held_frames[0];
#endif
        battle->set_input(edges);return true;
        }
#if PORT_TITLE == 14
        ,overlay_already_updated
        ,[this](){
            return !battle->pending_stage||battle->pump_stage_loading(stage_load_clock(),
                animations.first_deferred_capture_file_index(),stage_load_host_flags);
        }
        ,[this](const AnmVm& vm,AnmManager::VmBoundaryTh14 point){
            if(!battle->pending_stage)return true;
            if(stage_load_vm_boundary&&!stage_load_vm_boundary(vm,point))return false;
            if(point==AnmManager::VmBoundaryTh14::BeforeCall)return battle->poll_stage_initialization(stage_load_clock());
            return battle->pump_stage_loading(stage_load_clock(),
                animations.first_deferred_capture_file_index(),stage_load_host_flags);
        }
        ,[this](const Stage& owner,const AnmVm& vm,u32 object,u32 primitive,u32 animation,AnmManager::VmBoundaryTh14 point){
            if(!battle->pending_stage)return true;
            if(stage_load_object_boundary&&!stage_load_object_boundary(owner,vm,object,primitive,animation,point))return false;
            // The owner is executing: finish only Initialize, without replacing
            // or retiring the current scene/resources inside this borrowed call.
            return battle->poll_stage_initialization(stage_load_clock());
        }
        ,[this](const Stage& owner,u32 copied_words){
            if(!battle->pending_stage)return true;
            if(stage_load_camera_word_boundary&&!stage_load_camera_word_boundary(owner,copied_words))return false;
            // Only Scene may progress while the publisher is borrowed. Do not
            // run Timeline or the controller's later Stage retirement here.
            return battle->poll_stage_scene(stage_load_clock());
        }
        ,[this,&input](){
            if(!interactive||!state.demo)return;
            // Native436c08 reads Control+14 at priority11, not the physical
            // Session/Replay update count. A held rate may visit3840 more
            // than once; a faster rate may skip it. Preserve exact equality.
            const u32 raw=(input.held&~256u)|((input.held&256)?0x80000u:0u)|(input.pause?256u:0u);
            const i32 clock=battle->control_frame();
            if((raw&0x80103u)||(animations.th14_control.callback_flags()&0x70u)||clock==3900)
                demo_title_pending=true;
            if(clock==3840)battle->fades.start(5,60,0,48,&animations.rate);
        }
#endif
        );
        if(playback_ended){
#if PORT_TITLE == 14
            if(interactive)return state.phase==GameSessionPhase::paused;
#endif
            if(interactive)return return_to_title();state.phase=GameSessionPhase::finished;return true;
        }
        if(!updated) { if(error.empty())error="battle update failed: "+std::to_string(battle->last_error)+" "+battle->error; return false; }
        for(const auto& event:battle->events)if(event.kind==BattleEventKind::BossMusic||(event.kind==BattleEventKind::StageMusic&&!state.demo))scores.settings[0x26+stage_music(event.value,event.kind==BattleEventKind::BossMusic)]=1;
#if PORT_TITLE == 14
        if(!state.replay&&battle->stage_active&&battle->th14_control_counted){auto* counter=scores.characters[state.character*2+state.subtype].data()+ScoreFile::play_time_offset;i32 n;std::memcpy(&n,counter,4);if(n<215999999){++n;std::memcpy(counter,&n,4);}}
#else
        if(!state.replay&&battle->stage_active){auto* counter=scores.characters[state.character*2+state.subtype].data()+ScoreFile::play_time_offset;i32 n;std::memcpy(&n,counter,4);if(n<215999999){++n;std::memcpy(counter,&n,4);}}
#endif
        if(battle->game_over_requested){
            if(state.phase==GameSessionPhase::paused||state.phase==GameSessionPhase::game_over)return true;
            if(state.replay){
                if(interactive){
#if PORT_TITLE == 14
                    return pause();
#else
                    return return_to_title();
#endif
                }
                state.phase=GameSessionPhase::finished;return true;
            }
            return open_result(false);
        }
#if PORT_TITLE == 14
        if(battle->th14_spell_result_requested){if(state.replay){if(interactive)return return_to_title();state.phase=GameSessionPhase::finished;return true;}return open_result(true);}
#endif
        if(battle->completion.state.exit==StageExit::Results)return open_result(true);
#if PORT_TITLE != 14
        if(battle->completion.state.exit==StageExit::Ending)return open_ending();
#endif
        if(battle->completion.state.exit==StageExit::Title||battle->completion.state.exit==StageExit::ReplayEnd){
#if PORT_TITLE == 14
            // The completion effect opened the forced menu at its actual
            // dialogue/HUD request, before normal ANM29 in this same pass.
            if(interactive&&battle->completion.state.exit==StageExit::ReplayEnd)return state.phase==GameSessionPhase::paused;
#endif
            if(interactive)return return_to_title();state.phase=GameSessionPhase::finished;return true;
        }
#if PORT_TITLE == 14
        if(battle->completion.state.exit==StageExit::NextStage){state.stage=u32(battle->completion.state.next_stage);state.frame=0;}
#else
        if(battle->completion.state.exit==StageExit::NextStage){
            const u32 next=u32(battle->completion.state.next_stage);
            auto& gameplay_rng=kPortTitle==14?animations.visual_rng:animations.script_rng;
#if PORT_TITLE == 14
            if(!state.replay){recording.spell_times(battle->spell_timing.records);recording_active=false;}
            if(state.replay){
                if(!playback.stage(next)){state.phase=GameSessionPhase::finished;return true;}
                battle->replay_entry.read(playback.header(next));
            }
            auto initialize=[this,restore=ReplayRngRestoreJobTh14{}]()mutable->prototype::Work {
                auto& rng=animations.visual_rng;
                const auto ready=[this](ReplayRngRestorePointTh14 point){return resources.effects?resources.effects->stage_rng_store_readiness(point):ReplayRngRestoreResultTh14::Ready;};
                const auto progress=state.replay?restore.playback(rng,&animations.script_rng,battle->replay_entry.seed,false,ready):
                    restore.record(rng,animations.script_rng,recording_seed,ready);
                if(progress==ReplayRngRestoreResultTh14::Waiting)return prototype::Work::Waiting;
                if(progress==ReplayRngRestoreResultTh14::Failed){error="TH14 stage RNG restore failed";return prototype::Work::Failed;}
                if(state.replay){
                    battle->replay_entry.restore_economy_th14(economy);
                    battle->enemy_environment.spell_practice_id=battle->replay_entry.th14_practice_mode;
                    battle->hud.score.continues=battle->replay_entry.continues;
                    battle->player->state.minimum_point_value=signed_bits(battle->replay_entry.th14_globals[10]);
                }
                return prototype::Work::Ready;
            };
            if(!source||!battle->next_stage(*source,next,{},std::move(initialize))){error="stage transition failed: "+battle->error;return false;}
            state.stage=next;state.frame=0;
            if(!battle->pump_stage_loading(stage_load_clock(),animations.first_deferred_capture_file_index(),stage_load_host_flags))return fail(battle->error.c_str());
#else
            if(!state.replay){recording.spell_times(battle->spell_timing.records);recording_active=false;recording_seed=gameplay_rng.seed_value();gameplay_rng.set_call_count(0);}
#if PORT_TITLE == 14
            if(!state.replay)animations.script_rng.set_seed(u16(recording_seed));
#endif
            if(state.replay){
                if(!playback.stage(next)){state.phase=GameSessionPhase::finished;return true;}
                battle->replay_entry.read(playback.header(next));
                battle->replay_entry.restore(economy,gameplay_rng,false,kPortTitle==14?&animations.script_rng:nullptr);
#if PORT_TITLE == 14
                battle->enemy_environment.spell_practice_id=battle->replay_entry.th14_practice_mode;
#endif
                battle->hud.score.continues=battle->replay_entry.continues;
            }
            if(!source||!battle->next_stage(*source,next)){error="stage transition failed: "+battle->error;return false;}
#if PORT_TITLE == 14
            if(state.replay)battle->player->state.minimum_point_value=signed_bits(battle->replay_entry.th14_globals[10]);
#endif
            state.stage=next;state.frame=0;
#if PORT_TITLE == 14
            if(battle->pending_stage){state.phase=GameSessionPhase::loading;return true;}
#endif
#endif
        }
#endif
    } else if(!animations.update(false)) return fail("ANM update failed");
    ++state.frame;
    return true;
}

bool GameSession::pause() {
    if(state.phase!=GameSessionPhase::stage) return false;
#if PORT_TITLE == 14
    if(!pause_menu)pause_menu=std::move(idle_pause_menu);
    if(!pause_menu)
#endif
    pause_menu=std::make_unique<PauseMenu>(animations,resources.core.text,resources.core.front,scores);
    pause_menu->selection=state.character*2+state.subtype;pause_menu->difficulty=state.difficulty;pause_menu->stage=state.stage;pause_menu->practice=state.practice;
#if PORT_TITLE == 14
    pause_menu->spell_practice_id=state.spell_practice_id;
#if defined(TH14_CPP_STAGE_DECODE)
    pause_menu->th14_live_run_flags=battle?&battle->enemy_commands.th14_run_flags:nullptr;
#endif
    // Retail4490c0 reads the live4f5838 counter before disabling replay save.
    pause_menu->continues=battle?battle->hud.score.continues:0;
#endif
    paused_rate=animations.rate;
#if PORT_TITLE == 14
    const bool replay_end=state.replay&&battle&&(battle->game_over_requested||battle->completion.state.exit==StageExit::ReplayEnd);
    if(replay_end){
        if(!pause_menu->begin_replay_end())return fail(pause_menu->error.c_str());
        // This entry has no physical Pause edge to initialize menu input.
        menu_input.update((last_held&~256u)|((last_held&256u)?0x80000u:0u)|(last_pause?256u:0u));
    }else
#endif
    {
#if PORT_TITLE != 14
        animations.rate=1;
#endif
        if(!pause_menu->begin(state.replay))return fail(pause_menu->error.c_str());
    }
    animations.rate=1;state.phase=GameSessionPhase::paused;
    return true;
}

bool GameSession::open_result(bool completed){
#if PORT_TITLE == 14
    if(!pause_menu)pause_menu=std::move(idle_pause_menu);
    if(!pause_menu)
#endif
    pause_menu=std::make_unique<PauseMenu>(animations,resources.core.text,resources.core.front,scores);
    pause_menu->selection=state.character*2+state.subtype;pause_menu->difficulty=state.difficulty;pause_menu->stage=state.stage;
    pause_menu->result_score=th14_hud_load(economy.score_units);pause_menu->continues=battle->hud.score.continues;pause_menu->timestamp=recording_timestamp;
#if PORT_TITLE == 14
    pause_menu->spell_practice_id=state.spell_practice_id;
#if defined(TH14_CPP_STAGE_DECODE)
    pause_menu->th14_live_run_flags=battle?&battle->enemy_commands.th14_run_flags:nullptr;
#endif
    if(completed&&state.spell_practice_id>=0){
        // Retail437005 rounds the unsigned stored score down to tens, then
        // compares the existing practice high score as a signed word.
        const i32 value=signed_bits(u32(th14_hud_load(economy.score_units))/10*10);auto* record=scores.characters[state.character*2+state.subtype].data()+ScoreFile::spell_offset+state.spell_practice_id*ScoreFile::spell_stride+0x98;
        i32 best;std::memcpy(&best,record,4);if(best<value)std::memcpy(record,&value,4);
    }
#endif
#if PORT_TITLE == 16
    pause_menu->selection=state.character;pause_menu->th16_subseason=state.subtype;
    pause_menu->th16_continues_remaining=state.difficulty<4?5-pause_menu->continues:0;
#endif
    paused_rate=animations.rate;
#if PORT_TITLE != 14
    animations.rate=1;
#endif
    if(!pause_menu->begin_end(state.practice,completed))return fail(pause_menu->error.c_str());
    animations.rate=1;
    state.phase=GameSessionPhase::game_over;menu_input.update(last_held);return true;
}

bool GameSession::open_ending(){
    if(!source||!battle||state.replay)return fail("ending requires a completed live game");
    completed_score=th14_hud_load(economy.score_units);completed_continues=battle->hud.score.continues;
    recording.spell_times(battle->spell_timing.records);if(!recording.finish_stage(true))return fail("completed recording finalization failed");
    menu_selection.character=state.character;menu_selection.partner=state.subtype;menu_selection.difficulty=state.difficulty;
    scores.write_records(spell_records,clear_records);reset_state();completed_run=true;
    if(!initialize_compositor())return fail("ending compositor initialization failed");
    reset_compositor_cameras(false);animations.rate=1;
    ending=std::make_unique<Ending>(animations,resources.core.text,scores);
#if PORT_TITLE == 14
    ending->effects=resources.effects;
#endif
    if(!ending->begin(*source,menu_selection.character*2+menu_selection.partner,menu_selection.difficulty,completed_continues))return fail(ending->error.c_str());
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
    const i32 count=continuation&&previous.stage!=first?std::min(battle->hud.score.continues+1,9):0;
    scores.write_records(spell_records,clear_records);next_continues=count;
    if(previous.replay){if(!begin(*source,first,previous.character,previous.subtype,previous.difficulty,previous.demo,true,previous.practice))return false;}
    else if(!begin(*source,continuation?previous.stage:first,previous.character,previous.subtype,previous.difficulty,false,false,previous.practice))return false;
    initial_stage=first;return true;
}

#if PORT_TITLE == 14
bool GameSession::continue_run_th14(){
    if(state.phase!=GameSessionPhase::game_over||!battle||!battle->player)return fail("TH14 continue requires a live battle");
    // Retail44a165 restores the old rate before44ae10 refreshes HUD/options.
    animations.rate=paused_rate;battle->events.push_back({BattleEventKind::PauseResume,1});
    // 44ae10 restores counters in place. Keep the dead player's lifecycle,
    // stage scripts, enemies, bullets and recording at the same frame.
    struct LiveContinue:ContinueActionsTh14 {
        GameBattle& battle;
        explicit LiveContinue(GameBattle& b):battle(b){}
        bool bombs(i32 count,i32 fragments)noexcept override{battle.hud.display_bombs(count,fragments);return true;}
        bool power_notice()noexcept override{return battle.hud.notice(2,0);}
        bool options()noexcept override{return battle.power_changed();}
        bool lives(i32 count,i32 fragments)noexcept override{battle.hud.display_lives(count,fragments);return true;}
    } actions(*battle);
    if(!th14_continue_apply(economy,battle->hud.score.continues,actions))return fail("TH14 continue HUD/option refresh failed");
#if defined(TH14_CPP_STAGE_DECODE)
    if(auto* control=animations.th14_control.get())control->flags&=~0x10u; //44aee9, after continue counters/options.
#endif
    battle->game_over_requested=false;
    // PauseEnd already interrupts the capture and choice families with 1.
    if(pause_menu)pause_menu->state=0;
    idle_pause_menu=std::move(pause_menu);state.phase=GameSessionPhase::stage;
    return true;
}
#endif

bool GameSession::resume() {
    if(state.phase!=GameSessionPhase::paused) return false;
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    if(auto* control=animations.th14_control.get())control->flags&=~0x10u; //449043, before resume delivery.
#endif
#if PORT_TITLE == 14
    // Preserve an intervening resume even if this frame immediately pauses again.
    if(battle)battle->events.push_back({BattleEventKind::PauseResume,0});
#endif
    if(pause_menu){for(const auto id:{pause_menu->background_animation,pause_menu->menu_animation})if(auto* vm=animations.find(id)){
        vm->interrupt(1);if(!vm->child.previous)for(auto* n=vm->child.next;n;n=n->next)n->value->interrupt(1);
    }}
#if PORT_TITLE == 14
    if(pause_menu)pause_menu->state=0;
    idle_pause_menu=std::move(pause_menu);animations.rate=paused_rate;
#else
    pause_menu.reset();animations.rate=paused_rate;
#endif
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
#if PORT_TITLE == 14
    if(replay_user_config){config=*replay_user_config;replay_user_config.reset();}
#endif
    if(!title)scores.write_records(spell_records,clear_records);
    reset_state();source=&data;interactive=true;
    if(!resources.load_core(data))return fail(resources.error.c_str());
    if constexpr(kPortTitle==14||kPortTitle==16)animations.fallback_sprites=&resources.core.ascii;
#if PORT_TITLE == 14
    if(resources.effects&&!resources.effects->core_ready()){
        reset_compositor_cameras(false);
        pending_core_title=true;pending_core_first=first;pending_core_screen=screen;state.phase=GameSessionPhase::loading;return true;
    }
#endif
    if(!initialize_compositor())return fail(("title compositor initialization failed: "+compositor.last_error).c_str());
    reset_compositor_cameras(false);
#if PORT_TITLE == 14
    return activate_title(data,first,screen);
}
bool GameSession::activate_title(GameResources& data,bool first,TitleScreen screen){
    state.phase=GameSessionPhase::title;
    // Title6's initial459b19..459b4e marks the old file5/7/2/0 VMs
    // before creating title animations. Keep their slots live until the
    // following overlay8/normal29 callbacks consume the deletion markers.
    for(auto* file:{&resources.core.front,&resources.core.bullet,&resources.core.ascii,&resources.core.text})
        animations.retire_resource(*file);
#endif
    title=std::make_unique<TitleMenu>(animations,resources.core.title,resources.core.title_variant,resources.core.ascii,scores);
    title_ascii=std::make_unique<AsciiText>(resources.core.ascii);title->text_resource=&resources.core.text;
    title->selection=menu_selection;title->config=config;title->flags=first?3:1;
    title->change(screen);title->cursor.count=kPortTitle==14?10:8;
    if(menu_selection.difficulty==4)title->cursor.select(1);
    if(screen!=TitleScreen::Main){title->cursor.select(screen==TitleScreen::Replays?(kPortTitle==14?4:3):0);title->cursor.push();}
    std::vector<u8> comments;if(!data.read("musiccmt.txt",comments)||!title->load_music_comments(comments.data(),u32(comments.size())))return fail("title music metadata missing");
    return true;
}

#if PORT_TITLE == 14
bool GameSession::sample_spell_clock(double now){
    if(initial_battle_private())return true;
    return !battle||!battle->initialized||battle->spell_timing.update(battle->spell_flags,battle->spells.frame_count,now,state.replay);
}
#endif
bool GameSession::draw(AnmRenderer& renderer,AsciiText* overlay) {
    if(state.phase==GameSessionPhase::title&&title&&title_ascii){
        title_ascii->clear();title->queue_ascii(*title_ascii);
    }
    if(state.phase==GameSessionPhase::finished)return true;
    if(battle_attached&&battle)battle->ascii.clear();
    renderer.fog_origin=compositor.play_camera.position;
    const auto& fog=compositor.play_camera.fog;
    renderer.fog_channels={fog.channels[0],fog.channels[1],fog.channels[2]};renderer.fog_start=fog.near_distance;
#if PORT_TITLE == 14
    SceneCameraPerspectiveScopeTh14 perspective_scope(compositor.world_camera,this,
        [](void* owner,const SceneCamera& camera,u32 point){
            auto& session=*static_cast<GameSession*>(owner);
            if(session.initial_battle_private())return true;
            if(!session.battle||!session.battle->pending_stage)return true;
            if(session.stage_load_perspective_boundary&&!session.stage_load_perspective_boundary(camera,point))return false;
            // A draw borrows its Stage owner. Complete only an already-entered
            // Scene copy; Timeline/retirement remain outside this scope.
            return session.battle->poll_stage_scene(session.stage_load_clock());
        });
    const std::function<bool()> camera_published=[this](){
        return initial_battle_private()||!battle||!battle->pending_stage||battle->pump_stage_loading(stage_load_clock(),
            animations.first_deferred_capture_file_index(),stage_load_host_flags);
    };
#endif
#if PORT_TITLE == 14
    const auto append_overlay=[overlay](AsciiText& target){
        if(!overlay||overlay==&target)return true;
        // Native uses one320-request ASCII owner. Keep outer glyph order and
        // the shared limit instead of issuing another complete ASCII pass.
        for(const auto& request:overlay->requests)if(request.style.pass==0&&
            !target.add(request.text.c_str(),request.position,request.style))return false;
        return true;
    };
#endif
    u32 next_fade_priority=0;
    for(const auto& pass:scene_draw_passes) {
#if PORT_TITLE == 14
        // Native fades register independent draw callbacks (46d3a7).
        // Their priority need not coincide with any persistent scene pass:
        // attract fade 48 and title fade 61 are both between fixed passes.
        for(;next_fade_priority<=pass.priority;++next_fade_priority){
            // Replay's455eb0 draw callback registers at62 (454b13).
            // Queue after inner HUD/text, preserving the ASCII queue limit.
            if(next_fade_priority==62){
                AsciiRequest replay_fps;
#if defined(TH14_CPP_STAGE_DECODE)
                const u32 controller_flags=animations.th14_control.callback_flags();
#else
                const u32 controller_flags=state.phase==GameSessionPhase::loading?4u:0u;
#endif
                // Native4363d5/4363e0 and437410 disable Replay30/62 until
                //455ed0 at stage activation. Replay12 input remains separate.
                if(battle_attached&&battle&&battle->stage_active&&th14_replay_fps_request(true,state.replay?1u:0u,u8(playback.current().fps),replay_fps,controller_flags)&&
                   !battle->ascii.add(replay_fps.text.c_str(),replay_fps.position,replay_fps.style))return fail("replay FPS text queue failed");
            }
            if(next_fade_priority==60&&resources.effects&&!resources.effects->draw_startup_loading())
                return fail((std::string("startup loading creation failed: ")+resources.effects->last_error()).c_str());
            if(title)title_fades.draw(renderer,next_fade_priority);
            if(ending)ending->fades.draw(renderer,next_fade_priority);
            if(battle_attached&&battle)battle->fades.draw(renderer,next_fade_priority);
        }
#else
        if(title)title_fades.draw(renderer,pass.priority);
        if(ending){ending->fades.draw(renderer,pass.priority);if(pass.kind==SceneDrawKind::End)ending->fades.draw(renderer,67);}
        if(pass.kind==SceneDrawKind::End&&battle_attached&&battle)battle->fades.draw(renderer,67);
        if(battle_attached&&battle)battle->fades.draw(renderer,pass.priority);
#endif
#if PORT_TITLE == 14
        // Native40b5d9 registers ASCII50 with enabled bit2 cleared. This
        // traversal runs enabled callbacks; keep its direct draw method
        // available, but do not execute the disabled registration here.
        if(pass.kind==SceneDrawKind::AsciiInner)continue;
#endif
        if(pass.kind==SceneDrawKind::Animation){
            // Naming the pass matters: several animation passes run per frame, so a bare
            // "ANM draw failed" cannot tell a stage layer from a HUD, effect or text layer.
            if(!compositor.draw_animation(renderer,pass.layer)){error="ANM draw failed (priority "+std::to_string(pass.priority)+" kind "+std::to_string(u32(pass.kind))+" layer "+std::to_string(pass.layer)+")";return false;}
        }else if(scene_compositor_pass(pass.kind)){
            if(!compositor.draw(renderer,pass.kind))return fail("screen compositor draw failed");
        }else if(title&&pass.kind==SceneDrawKind::AsciiOuter){
#if PORT_TITLE == 14
            if(!append_overlay(*title_ascii))return fail("title overlay queue failed");
#endif
            if(!title_ascii->draw(renderer,0,compositor.full_camera,compositor.play_camera))return fail("title text draw failed");
        }else if(battle_attached&&battle){
            if(pass.kind==SceneDrawKind::AsciiOuter&&pause_menu)pause_menu->queue_ascii(battle->ascii);
#if PORT_TITLE == 14
            if(pass.kind==SceneDrawKind::AsciiOuter&&!append_overlay(battle->ascii))return fail("battle overlay queue failed");
#endif
            if(!battle->draw(renderer,pass.kind
#if PORT_TITLE == 14
            ,camera_published
#endif
))return fail(("battle draw failed: pass "+std::to_string(u32(pass.kind))+(battle->error.empty()?"":" "+battle->error)).c_str());
        }
#if PORT_TITLE == 14
        if(perspective_scope.failed)return fail("TH14 worker failed during perspective publication");
#endif
#if PORT_TITLE == 14
        if(overlay&&pass.kind==SceneDrawKind::AsciiOuter&&!title&&!(battle_attached&&battle)&&
           !overlay->draw(renderer,0,compositor.full_camera,compositor.play_camera))return fail("frame statistics draw failed");
#else
        if(overlay&&pass.kind==SceneDrawKind::AsciiOuter&&!overlay->draw(renderer,0,compositor.full_camera,compositor.play_camera))return fail("frame statistics draw failed");
#endif
    }
#if PORT_TITLE == 14
    // Native Main46a3c4/46a7d4/46a96f restores full after callback traversal.
    renderer.upload_camera(compositor.full_camera);
#endif
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
#if PORT_TITLE == 14
    case GameSessionPhase::loading:return "loading";
#endif
    }
    return "unknown";
}

}
