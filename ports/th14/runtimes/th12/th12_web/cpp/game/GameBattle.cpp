#include "GameBattle.hpp"
#include "MusicCatalog.hpp"
#include "StageFiles.hpp"
#include "ShotFiles.hpp"
#include "Trace.hpp"
#include "Localization.hpp"
#include "GameSession.hpp"
#include "StagePreRngTh14.hpp"
#include "SpellTh14Visuals.hpp"
#include "SpellTh16Visuals.hpp"
#include "BulletTh16.hpp"
#include <algorithm>

namespace th11 {
GameBattle::GameBattle(GameSessionResources& r,AnmManager& a,GameEconomy& e,SpellRecords& records,SceneCompositor& scene,ClearRecords& clears)
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
    :GameBattle(r,a,e,records,scene,clears,false){}
void GameBattle::initialize_initial_pools(){
    // First-entry worker 435f07 and 435f10 construct these pools in order.
    bullets.initialize_storage();
    items.initialize_storage();
}
GameBattle::GameBattle(GameSessionResources& r,AnmManager& a,GameEconomy& e,SpellRecords& records,SceneCompositor& scene,ClearRecords& clears,bool defer_initial_pools)
#endif
    :resources(r),animations(a),economy(e),compositor(scene),completion(e,clears,*this,&a.rate),
     hud(a,r.core.front,r.core.ascii,e,enemy_commands,completion.state,*this),ascii(r.core.ascii),enemy_animations(a),
     enemies(resources.stage->timeline,enemy_environment,enemy_commands,*this),
     enemy_callbacks(enemy_environment,enemy_commands,*this),
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
     bullets(resources.core.bullet,a,*this,7,defer_initial_pools),lasers(resources.core.bullet,a,*this,7),
     items(resources.core.bullet,a,*this,7,defer_initial_pools),
#else
     bullets(resources.core.bullet,a,*this,(kPortTitle==14||kPortTitle==16)?7:6),lasers(resources.core.bullet,a,*this,(kPortTitle==14||kPortTitle==16)?7:6),
     items(resources.core.bullet,a,*this,(kPortTitle==14||kPortTitle==16)?7:6),
#endif
     item_rewards(e,*this),spells(*this,e,*this,records,&a.rate) {
    enemy_commands.animations=&enemy_animations;
    enemy_commands.bullets=&bullets;enemy_commands.lasers=&lasers;
    enemy_commands.manager=&enemies;enemy_commands.cancellation=&cancellation;
    enemy_commands.spells=&spells;
    enemy_commands.shakes=&shakes;
    enemy_commands.dialogue=this;
    enemy_commands.deformations=this;
    enemy_commands.callbacks=&enemy_callbacks;
    // Native manager +0x40 is bullet.anm; ECL ANIM entries follow it.
    enemy_animations.resources[0]={&resources.core.bullet,6};
    enemy_animations.resources[1]={&resources.core.enemy,8};
    enemy_animations.resources[2]={&resources.stage->enemies,9};
#if PORT_TITLE == 16
    enemy_animations.stage_logo_resource={&resources.core.front,5};
    enemy_animations.th16_registration_user=this;
    enemy_animations.th16_register_effect=[](void* p,u32 id){return static_cast<GameBattle*>(p)->th16_register_hit_effect(id);};
#endif
#if PORT_TITLE == 14
    enemy_commands.th14_rate_target=&animations.rate;
#endif
#if PORT_TITLE == 14 || PORT_TITLE == 16
    enemy_animations.resources[0].id=7;
    enemy_animations.resources[1]={&resources.core.effect,8};
    for(u32 i=0;i<resources.stage->ecl_animations.size();++i)
        enemy_animations.resources[i+2]={resources.stage->ecl_animation_names[i].empty()?nullptr:&resources.stage->ecl_animations[i],u16(10+i)};
    if constexpr(kPortTitle==14)enemy_animations.stage_logo_resource={&resources.stage->logo,6};
#endif
    enemy_commands.animation_user=&enemy_animations;
    enemy_commands.animation_visibility=[](u32 id,bool visible,void* p){static_cast<EnemyAnimations*>(p)->visible(id,visible);};
    enemy_environment.animation_user=&enemy_animations;
    enemy_environment.animation_sprite=[](u32 id,void* p){return static_cast<EnemyAnimations*>(p)->sprite(id);};
    // TH14 ECL getters/movement use 4db510; ANM and shake use 4db508.
    enemy_environment.shared_random=kPortTitle==14?&animations.visual_rng:&animations.script_rng;
#if PORT_TITLE == 16
    enemy_environment.th16_visual_random=&animations.visual_rng;
#endif
    enemy_environment.rate=animations.rate;
    communication.reset(&animations.rate);
    callback_shared_visual.initialize();
}

GameBattle::~GameBattle(){for(auto* node=enemies.first;node;node=node->next)release_deformation(node->value->state);if(dialogue)dialogue->clear();for(auto& vm:spell_backgrounds)animations.release_geometry(vm);}
bool GameBattle::create_deformation(EnemyState& e){auto effect=std::make_unique<ScreenDeformation>(animations);if(!effect->initialize(resources.core.text))return false;e.deformation=effect.release();return true;}
#if PORT_TITLE == 14
bool GameBattle::prepare_initial_scene(bool complete){
    if(initialized||!player||stage||!resources.stage)return unavailable("invalid initial scene owner");
    if(!complete){
        if(incoming_scene_owner||!stage_camera_read_job.reset())return unavailable("initial scene already prepared");
        incoming_scene_owner=std::make_unique<Stage>(animations,resources.stage->background,resources.core.text,
            compositor.clear_color,u16((resources.stage_number&1)+3));
        incoming_scene_owner->active_camera=&compositor.world_camera;return true;
    }
    if(!incoming_scene_owner||stage_camera_read_job.copied_words())return unavailable("initial scene completion owner changed");
    // Match the existing C++ loading-thread camera reader. This hosted
    // initial path still has no independent thread; do not dispatch another
    // shared-scene host job or convert an asynchronous wait into failure.
    if(!incoming_scene_owner->prepare_object_storage(resources.stage->scene.animation_count))return unavailable("initial scene VM storage preparation");
    const auto copied=stage_camera_read_job.advance(compositor.world_camera,[](u32){return prototype::Work::Ready;},
        [this](u32,u32 offset,u32& value){
            const auto* address=reinterpret_cast<const u8*>(&compositor.world_camera)+offset;
            if(reinterpret_cast<uintptr_t>(address)&3u)return prototype::Work::Failed;
            const auto* aligned=static_cast<const u32*>(__builtin_assume_aligned(address,4));
            value=__atomic_load_n(aligned,__ATOMIC_RELAXED);
            return prototype::Work::Ready;
        });
    if(copied!=prototype::Work::Ready)return unavailable("initial scene camera read failed");
    if(!incoming_scene_owner->initialize(resources.stage->scene,resources.stage_number,stage_camera_read_job.snapshot(),false)){
        error=incoming_scene_owner->error;return false;
    }
    return true;
}
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
bool GameBattle::construct_initial_player(i32 character,i32 subtype){
    if(initialized||player||!player_selection_valid(character,subtype))return unavailable("invalid initial player construction");
    player_input={};player_input.movement.character=character;player_input.movement.subtype=subtype;
    player_input.movement.enemy_manager=true;
    player=std::make_unique<PlayerFrame>(resources.core.shots[shot_file_slot(u32(character),u32(subtype))],resources.core.players[character],resources.core.bullet,animations,economy,*this,9,6);
    player->input=player_input;
    initial_player_waiting_for_resources=true;
    return true;
}
#endif
bool GameBattle::prepare_initial_player(i32 character,i32 subtype){
#if defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
    if(initialized||!player_selection_valid(character,subtype)||
       (player&&(!initial_player_waiting_for_resources||player->input.movement.character!=character||player->input.movement.subtype!=subtype)))
        return unavailable("invalid initial player preparation");
    if(!player&&!construct_initial_player(character,subtype))return false;
    initial_player_waiting_for_resources=false;
#else
    if(initialized||player||!player_selection_valid(character,subtype))return unavailable("invalid initial player preparation");
    player_input={};player_input.movement.character=character;player_input.movement.subtype=subtype;
    player_input.movement.enemy_manager=true;
    player=std::make_unique<PlayerFrame>(resources.core.shots[shot_file_slot(u32(character),u32(subtype))],resources.core.players[character],resources.core.bullet,animations,economy,*this,9,6);
    player->input=player_input;
#endif
    // Retail44c4c6 resumes after the selected file9 upload; 435e1d then
    // creates options while background/HUD uploads still remain pending.
    if(!player->initialize()){last_error=-601;error="player initialization";return false;}
    if(!player->rebuild_th14_options()){last_error=-601;error="initial player option preparation";return false;}
    return true;
}
bool GameBattle::prepare_initial_tail_owners(i32 character,i32 subtype){
    if(initialized||!player||bomb||stage||!incoming_scene_owner||stage_camera_read_job.copied_words()!=75||
       !player_selection_valid(character,subtype)||player->input.movement.character!=character||
       player->input.movement.subtype!=subtype||enemies.first||enemies.last||enemies.count)
        return unavailable("invalid private initial tail owner preparation");
    // 435fb4->435a00 resets the already-empty initial enemy owner.
    // Existing list storage is private and remains empty until Main's ECL.
    enemies.timer.set(0,&enemy_environment.rate);
    // 411bc0 constructs Bomb (Main20/Draw38 bit2 enabled). No VM,
    // RNG or stage reset belongs to this constructor.
    bomb=std::make_unique<BombController>(animations,resources.core.players[character],*player,*this,9,&resources.core.text);
    bomb->selection=character*2+subtype;
    // 41c510 is Spell, not Dialogue. The by-value inert storage already
    // exists privately; establish its constructor timer at this boundary.
    // Native inactive Main26/Draw12 do not change their owner state.
    spells.timer.set(0,&animations.rate);
    return true;
}
#endif
bool GameBattle::initialize(i32 character,i32 subtype,i32 difficulty) {
    if(!player_selection_valid(character,subtype)||difficulty<0||difficulty>4)return unavailable("invalid battle selection");
#if PORT_TITLE == 14
    const bool prepared_bomb=!initialized&&player&&bomb&&incoming_scene_owner&&stage_camera_read_job.copied_words()==75;
    if(prepared_bomb&&bomb->selection!=character*2+subtype)return unavailable("prepared initial Bomb selection changed");
#endif
    initialized=false;last_error=0;frame=0;error.clear();events.clear();
#if PORT_TITLE == 14
    // A joined initial worker already performed436015. Do not repeat its
    // reset during Main activation. Direct/unprepared callers retain the
    // ordinary initialization; flags1 is Timer's existing initialized bit.
    if(!prepared_bomb||!(th14_control_clock.flags&1))th14_control_clock.set(0,&animations.rate);
    // Initial Player construction can precede scene/HUD/ECL decoding. Bind
    // the actual incoming ANIM owners when the worker has completed them.
    for(u32 i=0;i<resources.stage->ecl_animations.size();++i)
        enemy_animations.resources[i+2]={resources.stage->ecl_animation_names[i].empty()?nullptr:&resources.stage->ecl_animations[i],u16(10+i)};
    enemy_animations.stage_logo_resource={&resources.stage->logo,6};
#endif
    completion.mode.stage=i32(resources.stage_number);completion.mode.selection=character*2+subtype;
    if(replaying)spell_timing.records=replay_entry.spell_times;
    enemy_environment.character=character;enemy_environment.subtype=subtype;
    enemy_environment.difficulty=difficulty;enemy_environment.rank=economy.rank;
    enemy_environment.player_position={0,400,0};
    trace_point=11;
    // ECL world-space spawns and enemy movement read camera 0 (4c3484),
    // while background ANM reads camera 2 (4c36b4).
    camera_position=compositor.play_camera.position;
    enemy_environment.camera_delta=compositor.play_camera.animation_delta;
    economy.difficulty=difficulty;
#if PORT_TITLE == 14
    // Retail46f921..46f95c reads world-camera position plus eye_offset.
    animations.reference_positions[0]={compositor.world_camera.position.x+compositor.world_camera.eye_offset.x,
        compositor.world_camera.position.y+compositor.world_camera.eye_offset.y,compositor.world_camera.position.z+compositor.world_camera.eye_offset.z};
#else
    animations.reference_positions[0]=compositor.world_camera.position;
#endif
    animations.reference_positions[1]=compositor.world_camera.reserved24;
    animations.camera_delta=compositor.world_camera.animation_delta;
    trace_point=12;
#if PORT_TITLE == 14
    if(incoming_scene_owner){
        if(stage_camera_read_job.copied_words()!=75)return unavailable("initial scene camera incomplete");
        stage=std::move(incoming_scene_owner);
        if(!stage->start_objects()){last_error=-1008;error=stage->error;return false;}
    }else
#endif
    {
        stage=std::make_unique<Stage>(animations,resources.stage->background,resources.core.text,compositor.clear_color,u16((resources.stage_number&1)+3));
        stage->active_camera=&compositor.world_camera;
        if(!stage->initialize(resources.stage->scene,resources.stage_number,compositor.world_camera)){last_error=-1008;error=stage->error;return false;}
    }
    trace_point=13;
    enemy_commands.stage=stage.get();
    spells.selection=kPortTitle==16?character+subtype:character*2+subtype;spells.stage=resources.stage_number;spells.replay=replaying;
    create_dialogue();
    trace_point=14;
    player_input={};player_input.movement.character=character;player_input.movement.subtype=subtype;
    player_input.movement.enemy_manager=true;
#if PORT_TITLE == 14
    if(!prepared_bomb)bomb.reset();
#else
    bomb.reset();
#endif
#if PORT_TITLE == 14
    if(!player){if(!prepare_initial_player(character,subtype))return false;}
    else if(player->input.movement.character!=character||player->input.movement.subtype!=subtype)return unavailable("prepared player selection changed");
    player->input=player_input;
#else
    player=std::make_unique<PlayerFrame>(resources.core.shots[shot_file_slot(u32(character),u32(subtype))],resources.core.players[character],resources.core.bullet,animations,economy,*this,(kPortTitle==14||kPortTitle==16)?9:7,kPortTitle==16?7:6);
#if PORT_TITLE == 16
    if(economy.th16_subseason<0||economy.th16_subseason>=5)return unavailable("invalid TH16 subseason");
    player->th16_season_shots=&resources.core.season_shots[economy.th16_subseason];
    player->th16_season_animation=&resources.core.season_players[economy.th16_subseason];
#endif
    trace_point=15;
    player->input=player_input;
    if(!player->initialize()){last_error=-601;error="player initialization";return false;}
#endif
    trace_point=16;
    // TH14 Control43681c calls44cc10 after the worker's435e1d option
    // preparation and before Replay455ed0 restores the stage entry. This
    // reset includes its own44cf98 option refresh, also at maximum power.
    // Legacy420390 calls42fec0 before the first player tick.
    if(!player->start_stage()){last_error=-601;error="initial player stage reset ("+std::to_string(player->last_error)+")";return false;}
    trace_point=17;
    if(replaying&&!player->restore_replay_position(replay_entry)){error="replay player entry";return false;}
#if PORT_TITLE == 14
    if(!prepared_bomb){
#endif
    bomb=std::make_unique<BombController>(animations,resources.core.players[character],*player,*this,kPortTitle==14?9:7,&resources.core.text);
    bomb->selection=character*2+subtype;
#if PORT_TITLE == 14
    spells.timer.set(0,&animations.rate);
    }
#endif
    synchronize_player();
    trace_point=18;
    if(!spawn_root()){last_error=-1001;error="stage main: ECL opcode "+std::to_string(enemies.script_error.opcode);return false;}
    trace_point=19;
    th14_hud_store(countdown_seconds,i32(-1));
    if(!hud.start_stage(resources.stage->logo,hud_input(),demo,true,completion.mode.control_mode,hud.score.continues)){error="HUD initialization";return false;}
#if PORT_TITLE == 14
    // 4368aa refreshes max-power auxiliaries after the ECL/HUD entry work.
    if(!player->rebuild_th14_options()){error="TH14 initial option refresh";return false;}
#endif
    trace_point=20;
    initialized=true;return true;
}
void GameBattle::create_dialogue(){
    dialogue=std::make_unique<Dialogue>(animations,resources.core.text,resources.core.players[enemy_environment.character],resources.core.front,resources.stage->enemies,resources.stage->logo,*this);
    dialogue->character=enemy_environment.character;dialogue->stage=resources.stage_number;
    if constexpr(kPortTitle==14)dialogue->th14_animations=&enemy_animations;
}
bool GameBattle::stage_result_animation(){
    auto* vm=animations.create(resources.core.front,kPortTitle==14?112:72,5,22);
    result_animation=vm?vm->id:0;return vm!=nullptr;
}
bool GameBattle::next_stage(GameResources& source,u32 number){
    if(transitioning||number<2||number>6)return false;
#if PORT_TITLE == 14
    return next_stage(source,number,{});
#else
    // TH14 4360e0 retains the world on the next-stage path; legacy 41fe10.
    // Stage-local
    // scripts and animations are retired before replacing their owners.
    if(!resources.load_stage(source,number)){error=resources.error;return false;}
#endif
#if PORT_TITLE == 14
}
namespace {
struct BattleStageActions final:prototype::StageActions {
    GameBattle& battle;explicit BattleStageActions(GameBattle& b):battle(b){}
    bool retire_owner()override{return battle.retire_stage_owner();}
    bool begin_worker_wait(bool initial)override{return !battle.resources.effects||battle.resources.effects->begin_stage_worker_wait(initial);}
    bool worker_owns_capture_wait()const override{
#if defined(TH14_CPP_STAGE_WORKER)
        return battle.resources.effects&&battle.resources.effects->stage_cpp_worker_enabled();
#else
        return false;
#endif
    }
    bool worker_wait_ready()const override{return !battle.resources.effects||battle.resources.effects->stage_worker_wait_ready();}
    bool enter_work_and_initialize()override{return battle.initialize_stage_work();}
    prototype::Work advance_work_and_initialize()override{return battle.advance_stage_work();}
    prototype::Work prepare_hud()override{return battle.prepare_stage_hud();}
    prototype::Work prepare_scene()override{return battle.prepare_stage_scene();}
    bool publish_empty_owner()override{return battle.publish_stage_owner();}
    prototype::Work prepare_timeline()override{return battle.prepare_stage_timeline();}
};
prototype::Work phase_work(GameSessionResources::PhaseResult result){
    return result==GameSessionResources::PhaseResult::Waiting?prototype::Work::Waiting:
        result==GameSessionResources::PhaseResult::Ready?prototype::Work::Ready:prototype::Work::Failed;
}
}
bool GameBattle::next_stage(GameResources& source,u32 number,std::function<bool()> initialize){
    return next_stage(source,number,std::move(initialize),{});
}
bool GameBattle::next_stage(GameResources& source,u32 number,std::function<bool()> initialize,std::function<prototype::Work()> initialize_step){
    if(transitioning||pending_stage||number<2||number>6)return false;
    if(!stage_camera_read_job.reset()){error="TH14 scene camera read still active";return false;}
    incoming_scene_owner.reset();
    pending_stage=number;next_stage_source=&source;next_stage_initializer=std::move(initialize);
    next_stage_initializer_step=std::move(initialize_step);stage_initialization_started=stage_initialization_finished=false;
    BattleStageActions actions(*this);
    if(!stage_loader.request(actions,false)){error="TH14 staged owner retirement failed";return false;}
    if(!resources.begin_stage_loading(number)){error=resources.error;return false;}
    stage_active=false;transitioning=true;transition_started=false;
    completion.state.exit=StageExit::None;completion.mode.stage=i32(number);
    transition_timer.set(0,&animations.rate);th14_control_clock.set(0,&animations.rate);frame=0;
    // The result_timer continues from StageCompletion::complete; the resource
    // wait and work overlap that existing120 ticks, rather than restarting it.
    // The completion effect already recalled options at retail436fb3.
    // Starting the loader must not send label3 again after ANM29 has run.
    outgoing_stage=std::move(stage);
    if(!stage_loading.begin(animations,resources.core.ascii)){error="TH14 next-stage loading animation creation failed";return false;}
#if defined(TH14_CPP_STAGE_DECODE)
    animations.th14_effect_stop=1; // Native43654d, after loading17 creation.
    if(auto* control=animations.th14_control.get())control->callbacks_enabled=0;
#endif
    return true;
}
bool GameBattle::pump_stage_loading(u64 now,i32 first_capture_index,u32 supervisor_flags){
    if(!pending_stage)return true;
    BattleStageActions actions(*this);
    if(!stage_loader.pump(actions,now,first_capture_index,supervisor_flags)){
        if(error.empty())error=resources.error.empty()?"TH14 staged loader stopped":resources.error;return false;
    }
    return true;
}
bool GameBattle::poll_stage_initialization(u64 now){
    if(!pending_stage)return true;
    BattleStageActions actions(*this);
    if(stage_loader.poll_initialization(actions,now))return true;
    if(error.empty())error="TH14 stage initialization stopped";
    return false;
}
bool GameBattle::poll_stage_scene(u64 now){
    if(!pending_stage)return true;
    BattleStageActions actions(*this);
    if(stage_loader.poll_scene(actions,now))return true;
    if(error.empty())error="TH14 stage scene copy stopped";
    return false;
}
bool GameBattle::retire_stage_owner(){
    // Retail436100 resets the shared speed before retiring pools, creating
    // loading17, or dispatching the replacement worker.
    th14_stage_float_bits(animations.rate,0x3f800000u);
    enemy_environment.rate=1;
    // Retail4363ea clears the item pools before retiring the enemy owner.
    // Items must not remain collectible during the incoming worker wait.
    items.reset();
    if(!enemies.retire())return false;
    // 4227d6..42281d unloads the six ECL animation files during owner
    // retirement. Their live VMs must leave ANM29 before loading's children.
    if(auto* old=resources.stage.get())
        for(auto& file:old->ecl_animations)animations.retire_resource(file);
    // 436425 calls41e9f0 while the outgoing stage is still attached.
    // Mark effect8 and bullet7 before ANM29, rather than waiting for the
    // worker's initialization phase. The normal registry returns their slots
    // before loading17 publishes its overlay children on its first update.
    animations.retire_resource(resources.core.effect);
    animations.retire_resource(resources.core.bullet);
    // Retail43642a calls411f40/411980 before the worker wait. Returning
    // Bomb slots after that wait changes loading's LIFO allocation order.
    bomb.reset();
#if defined(TH14_CPP_STAGE_DECODE)
    th14_next_tail_prepared.store(false,std::memory_order_release);
#endif
    if(dialogue)dialogue->clear();dialogue.reset();
    return true;
}
bool GameBattle::initialize_stage_work(){return advance_stage_work()==prototype::Work::Ready;}
prototype::Work GameBattle::advance_stage_work(){
    if(stage_initialization_finished)return prototype::Work::Ready;
    if(!stage_initialization_started){
        bool deferred=false;
#if defined(TH14_CPP_STAGE_WORKER)
        deferred=resources.effects&&resources.effects->stage_cpp_worker_enabled();
#endif
        if(!deferred){
            if(!compositor.stage_start()){
                error="TH14 worker copy0 reset failed";return prototype::Work::Failed;
            }
            // Retail435b44 follows copy0 and precedes456050.
            th14_stage_pre_rng(animations.rate,completion.state.ending_active,hud.score.high,economy.score_units);
        }
        stage_initialization_started=true;
    }
    if(next_stage_initializer_step){
        const auto result=next_stage_initializer_step();
        if(result!=prototype::Work::Ready)return result;
    }else if(next_stage_initializer&&!next_stage_initializer())return prototype::Work::Failed;
    next_stage_initializer={};next_stage_initializer_step={};
    if(dialogue)dialogue->clear();dialogue.reset();
    animations.retire_resource(resources.core.enemy);
    if(auto* old=resources.stage.get()){
        animations.retire_resource(old->enemies);animations.retire_resource(old->logo);
    }
    for(auto id:spell_titles)enemy_animations.erase(id);
    enemy_animations.erase(spell_circle);spell_circle=0;
    enemy_animations.erase(th14_spell_background);th14_spell_background=0;
    for(auto& id:spell_titles)id=0;
    for(auto& vm:spell_backgrounds){animations.release_geometry(vm);vm.initialize();}
    spell_flags=0;spell_elapsed=spell_bonus=spell_id=0;
    spell_timing={};if(replaying)spell_timing.records=replay_entry.spell_times;
#if defined(TH14_CPP_STAGE_DECODE)
    // The real decode worker performs435b62 early and436044 late. Main must
    // retain the outgoing section while it waits for that worker's tail.
    if(!(resources.effects&&resources.effects->stage_cpp_decode_enabled()))
#endif
    {enemy_commands.stage_section=0;enemy_commands.section_frame=0;}
    for(auto& value:enemy_environment.shared_integers)value=0;
    shakes.effects.clear();shakes.offset={};items.reset();enemy_environment.rate=animations.rate;
    economy.communication=0;communication.reset(&animations.rate);stage_initialization_finished=true;return prototype::Work::Ready;
}
prototype::Work GameBattle::prepare_stage_hud(){
    const auto result=resources.advance_stage_phase(*next_stage_source,StageResourceEffects::Phase::Hud,u32(enemy_environment.character*2+enemy_environment.subtype));
    if(result==GameSessionResources::PhaseResult::Ready){
#if defined(TH14_CPP_STAGE_DECODE)
        // The C++ resource worker already prepared the HUD before its release
        // of phase Ready. Repeating it here adds a second score read on Main.
        if(!(resources.effects&&resources.effects->stage_cpp_decode_enabled()))
#endif
        {hud.prepare_stage(&countdown_seconds);}
    }
    return phase_work(result);
}
prototype::Work GameBattle::prepare_stage_scene(){
    const auto result=resources.advance_stage_phase(*next_stage_source,StageResourceEffects::Phase::Scene,0);
    if(result!=GameSessionResources::PhaseResult::Ready)return phase_work(result);
    auto* incoming=resources.loading_bundle();if(!incoming){error="TH14 incoming scene bundle missing";return prototype::Work::Failed;}
    bool worker_initialized=false;
#if defined(TH14_CPP_STAGE_DECODE)
    worker_initialized=resources.effects&&resources.effects->stage_cpp_decode_enabled();
    if(worker_initialized&&!incoming_scene_owner){error="TH14 C++ scene owner missing";return prototype::Work::Failed;}
#endif
    // Native40d445 reads75 words while the main publisher may be active.
    // Keep the incoming owner private until all reads and initialization finish.
    if(!incoming_scene_owner){
        incoming_scene_owner=std::make_unique<Stage>(animations,incoming->background,resources.core.text,compositor.clear_color,u16((pending_stage&1)+3));
        incoming_scene_owner->active_camera=&compositor.world_camera;
    }
    StageResourceEffects synchronous;
    const auto copied=stage_camera_read_job.advance(compositor.world_camera,[this](u32 word){
        return stage_camera_read_boundary?stage_camera_read_boundary(word):prototype::Work::Ready;
    },[this,&synchronous](u32 word,u32 offset,u32& value){
        auto& reader=resources.effects?*resources.effects:synchronous;
        using Result=StageResourceEffects::CameraReadResult;
        const auto result=reader.read_stage_camera_word(compositor.world_camera,word,offset,value);
        return result==Result::Waiting?prototype::Work::Waiting:
            result==Result::Ready?prototype::Work::Ready:prototype::Work::Failed;
    });
    if(copied!=prototype::Work::Ready){
        if(copied==prototype::Work::Failed)error="TH14 scene camera read failed";
        return copied;
    }
    if(!worker_initialized&&!incoming_scene_owner->initialize(incoming->scene,pending_stage,stage_camera_read_job.snapshot(),false)){error=incoming_scene_owner->error;return prototype::Work::Failed;}
    stage=std::move(incoming_scene_owner);
    if(!resources.publish_stage_scene()){error=resources.error;return prototype::Work::Failed;}
    enemy_commands.stage=stage.get();return prototype::Work::Ready;
}
bool GameBattle::publish_stage_owner(){
#if defined(TH14_CPP_STAGE_DECODE)
    if(resources.effects&&resources.effects->stage_cpp_decode_enabled())return enemies.owner_present();
#endif
    return enemies.initialize_empty(resources.stage->timeline);
}
#if defined(TH14_CPP_STAGE_DECODE)
bool GameBattle::prepare_next_tail_owners(i32 character,i32 subtype){
    if(!pending_stage||!player||bomb||!player_selection_valid(character,subtype)||
       th14_next_tail_prepared.load(std::memory_order_acquire))return false;
    // 435fba follows4228a0 on the same loading thread. Complete selection
    // before publishing so Main never observes the default Reimu-A choice.
    auto incoming=std::make_unique<BombController>(animations,resources.core.players[character],*player,*this,9,&resources.core.text);
    incoming->selection=character*2+subtype;
    bomb=std::move(incoming);
    // 435fc3/41c510 follows Bomb publication. Inactive Spell callbacks do not
    // read this timer; phase Ready publishes it before Main activates ECL.
    spells.timer.set(0,&animations.rate);
    th14_next_tail_prepared.store(true,std::memory_order_release);
    return true;
}
#endif
prototype::Work GameBattle::prepare_stage_timeline(){
    const auto result=resources.advance_stage_phase(*next_stage_source,StageResourceEffects::Phase::Timeline,0);
    if(result!=GameSessionResources::PhaseResult::Ready)return phase_work(result);
#if defined(TH14_CPP_STAGE_DECODE)
    if(!(resources.effects&&resources.effects->stage_cpp_decode_enabled()))
#endif
    if(!enemies.finish_stage_owner()){error="TH14 enemy owner initialization";return prototype::Work::Failed;}
    for(u32 i=0;i<resources.stage->ecl_animations.size();++i)
        enemy_animations.resources[i+2]={resources.stage->ecl_animation_names[i].empty()?nullptr:&resources.stage->ecl_animations[i],u16(10+i)};
    enemy_animations.stage_logo_resource={&resources.stage->logo,6};
    create_dialogue();spells.stage=pending_stage;
#if defined(TH14_CPP_STAGE_DECODE)
    const bool worker_owners=resources.effects&&resources.effects->stage_cpp_decode_enabled();
    if(worker_owners){
        if(!th14_next_tail_prepared.load(std::memory_order_acquire)||!bomb||
           bomb->selection!=enemy_environment.character*2+enemy_environment.subtype){
            error="TH14 worker tail owner publication missing";return prototype::Work::Failed;
        }
    }else
#endif
    {
        bomb=std::make_unique<BombController>(animations,resources.core.players[enemy_environment.character],*player,*this,kPortTitle==14?9:7,&resources.core.text);
        bomb->selection=enemy_environment.character*2+enemy_environment.subtype;
        if constexpr(kPortTitle==14)spells.timer.set(0,&animations.rate);
    }
    resources.finish_stage_parts();pending_stage=0;next_stage_source=nullptr;return prototype::Work::Ready;
}
bool GameBattle::finish_stage_loading(){
    return !pending_stage;
}
bool GameBattle::enter_stage_transition(u32 number){
#endif
#if PORT_TITLE == 14
    // Original4358e0 sets Player+182d4 bit2 and interrupts both option
    // VMs with3 before the incoming stage becomes active.
    player->th14_scale.flags|=2;
    player->motion.recall_options(true);
#endif
    if(dialogue)dialogue->clear();dialogue.reset();bomb.reset();
    if(!enemies.reset(resources.stage->timeline)){last_error=-1001;return false;}
    animations.retire_resource(resources.core.enemy);
    if(auto* old=resources.previous_stage.get()){
        animations.retire_resource(old->enemies);animations.retire_resource(old->logo);
#if PORT_TITLE == 14 || PORT_TITLE == 16
        for(auto& file:old->ecl_animations)animations.retire_resource(file);
#endif
    }
    for(auto id:spell_titles)enemy_animations.erase(id);
    enemy_animations.erase(spell_circle);spell_circle=0;
#if PORT_TITLE == 16
    enemy_animations.erase(th16_spell_background);th16_spell_background=0;
#endif
#if PORT_TITLE == 14
    enemy_animations.erase(th14_spell_background);th14_spell_background=0;
#endif
    for(auto& id:spell_titles)id=0;
    for(auto& vm:spell_backgrounds){animations.release_geometry(vm);vm.initialize();}
    spell_flags=0;spell_elapsed=spell_bonus=spell_id=0;
    spell_timing={};if(replaying)spell_timing.records=replay_entry.spell_times;
    enemy_commands.stage_section=0;enemy_commands.section_frame=0;
    for(auto& value:enemy_environment.shared_integers)value=0;
    shakes.effects.clear();shakes.offset={};
    items.reset();animations.rate=enemy_environment.rate=1;
    economy.communication=0;communication.reset(&animations.rate);
    outgoing_stage=std::move(stage);
    stage=std::make_unique<Stage>(animations,resources.stage->background,resources.core.text,compositor.clear_color,u16((number&1)+3));
    stage->active_camera=&compositor.world_camera;
    if(!stage->initialize(resources.stage->scene,number,compositor.world_camera,false)){error=stage->error;return false;}
    // The original lists each stage's two animation files in a per-stage manifest, together with the
    // file ids they are cached under: stage 5 is 9 and 10, stage 6 is 11 and 12, stage 7 is 13 and 14,
    // so the pair is 2N-1 and 2N. The scripts address them as animation files 2 and 3, and the second
    // of the two had never been registered, which is what stopped the fifth stage. Both files are
    // opened per stage, so both the entries and their ids have to be refreshed here rather than once
    // in the constructor, where the previous id had been pinned to the value that happens to suit
    // stage 5.
    enemy_commands.stage=stage.get();
    const u32 stage_number=number;
    enemy_animations.resources[2]={&resources.stage->enemies,u16(stage_number*2-1)};
    enemy_animations.resources[3]={&resources.stage->logo,u16(kPortTitle==14?6:stage_number*2)};
#if PORT_TITLE == 14 || PORT_TITLE == 16
    for(u32 i=0;i<resources.stage->ecl_animations.size();++i)
        enemy_animations.resources[i+2]={resources.stage->ecl_animation_names[i].empty()?nullptr:&resources.stage->ecl_animations[i],u16(10+i)};
    if constexpr(kPortTitle==14)enemy_animations.stage_logo_resource={&resources.stage->logo,6};
#endif
    create_dialogue();spells.stage=number;
    bomb=std::make_unique<BombController>(animations,resources.core.players[enemy_environment.character],*player,*this,kPortTitle==14?9:7,&resources.core.text);
    bomb->selection=enemy_environment.character*2+enemy_environment.subtype;
    if constexpr(kPortTitle==14)spells.timer.set(0,&animations.rate);
    completion.mode.stage=i32(number);completion.state.exit=StageExit::None;
    hud.prepare_stage(&countdown_seconds);
    transition_timer.set(0,&animations.rate);frame=0;
#if PORT_TITLE == 14
    th14_control_clock.set(0,&animations.rate);
#endif
    transitioning=true;transition_started=false;stage_active=false;
    return true;
}
bool GameBattle::activate_stage(){
    bullets.reset();animations.retire_resource(resources.core.bullet);
    if(!player->start_stage())return false;
    if(replaying&&!player->restore_replay_position(replay_entry))return false;
    items.reset();
#if PORT_TITLE == 14
    if(!enemies.owner_present())return false;
#else
    if(!enemies.reset(resources.stage->timeline))return false;
#endif
    animations.retire_resource(resources.core.enemy);animations.retire_resource(resources.stage->enemies);
    lasers.clear();enemy_commands.section_frame=0;
    if(!spawn_root())return false;
#if PORT_TITLE != 14
    if(!compositor.stage_start())return false;
#endif
    if(!hud.start_stage(resources.stage->logo,hud_input(),false,false,completion.mode.control_mode)){error="HUD stage activation";return false;}
#if PORT_TITLE == 14
    // 436a9a runs after replay position/weapon restoration, ECL main and HUD.
    // Even when the option count is unchanged, full power recreates auxiliaries.
    if(!player->rebuild_th14_options()){error="TH14 next-stage option refresh";return false;}
    stage_loading.finish(animations);
#endif
    stage_active=true;events.push_back({BattleEventKind::StageMusic,i32(resources.stage_number)});
    // TH14 436b98..436ba5 resets the clock at activation; legacy 42068b.
    frame=0;transition_timer.set(0,&animations.rate);
#if PORT_TITLE == 14
    th14_control_clock.set(0,&animations.rate);
#endif
    return true;
}
bool GameBattle::stage_control(){
    if(!transitioning)return true;
#if PORT_TITLE == 14
    if(pending_stage)return true;
#endif
    if(!completion.result_delay_elapsed())return true;
    if(!transition_started){
        enemy_animations.interrupt(result_animation,1);
        completion.state.hud_flags&=~kStageResultFlag;completion.state.result_timer.set(0,&animations.rate);
        if(!stage->start_objects())return false;
        outgoing_stage->fade_out(fades);stage->resume_background();transition_started=true;
    }else if(transition_timer.current==30&&!stage_active){
        if(!activate_stage())return false;
    }
    if(outgoing_stage&&(outgoing_stage->state.draw_flags&8)){
        if(resources.previous_stage)animations.retire_resource(resources.previous_stage->background);
        outgoing_stage.reset();
    }
    transition_timer.tick();return true;
}
void GameBattle::set_input(const GameSessionInput& input) {
#if PORT_TITLE == 14
    th14_dialogue_skip_frames=input.dialogue_skip_frames;
    th14_dialogue_fire_frames=input.dialogue_fire_frames;
#endif
    player_input.movement.held=input.held;player_input.movement.pressed=input.pressed;
    player_input.movement.enemy_manager=
#if PORT_TITLE == 14
        enemies.owner_present();
#else
        true;
#endif
    player_input.movement.enemies=enemies.count!=0;
    // Legacy name: movement.bomb represents HUD +4438 (dialogue), not the
    // bomb key or the separate special-attack controller +3c.
    player_input.movement.bomb=dialogue&&dialogue->active;
    if(player)player->input=player_input;
}
void GameBattle::synchronize_player() {
    if(!player)return;
    player_input.movement.bomb=dialogue&&dialogue->active;
    player->input.movement.bomb=player_input.movement.bomb;
#if PORT_TITLE == 14
    // Retail 4122f0 requires an idle HUD and a live enemy manager.
    player->input.special_available=player_input.special_available=bool(bomb)&&!player_input.movement.bomb&&enemies.count!=0;
#else
    player->input.special_available=player_input.special_available=bool(bomb);
#endif
    player->input.shooting_blocked=player_input.shooting_blocked=(completion.state.hud_flags&0x10)!=0;
    const auto position=player->motion.state.position;
    enemy_environment.player_position=position;enemy_environment.rank=economy.rank;
    enemy_environment.rate=animations.rate;
    bullets.player=position;LaserWorld::player=position;LaserWorld::rate=animations.rate;
    ShotWorld::enemies=enemies.first;
    player_state=player->state.life_state;player_flags=player->motion.state.flags;
    special_active=bomb?bomb->state.active:0;
    player->input.special_active=player_input.special_active=special_active;
    // 410690 tests the player's invincibility timer and state flag 2, not
    // the Bomb controller's secondary timer (used by Nitori's shield).
    special_ending=player->state.invincibility.current!=0||(player_flags&2)!=0;
    spells.bomb_active=kPortTitle==14?special_active==1:special_active!=0;spells.replay=player_input.replay;
    cancellation={spell_flags,spell_id};
    player->copy_item_state(items.player);
    items.player.force_attract=player_input.movement.bomb;
#if PORT_TITLE == 14
    items.player.character=enemy_environment.character;
    items.player.bomb_attract=bomb&&special_active==1&&bomb->state.elapsed.current<60;
#endif
#if PORT_TITLE == 16
    items.player.character=enemy_environment.character;
    items.player.bomb_attract=bomb&&special_active==1&&bomb->state.elapsed.current<60;
#endif
}
bool GameBattle::spawn_root() {
    // 0x420495/0x4205d2 pass "main" with a zeroed 0x50-byte spawn record.
    // MainSub00 is only the first wave and must not replace this controller.
    return enemies.spawn("main",EnemySpawn{})!=nullptr;
}
bool GameBattle::update(const std::function<bool()>& sample_input
#if PORT_TITLE == 14
    ,bool overlay_already_updated
    ,const std::function<bool()>& before_normal_animation_update
    ,const AnmManager::VmBoundaryCallbackTh14& normal_vm_boundary
    ,const StageObjectVmBoundaryTh14& stage_object_boundary
    ,const StageCameraWordBoundaryTh14& camera_word_boundary
    ,const std::function<void()>& control_update
#endif
) {
    if(!initialized||last_error)return false;
#if PORT_TITLE == 14
    th14_control_counted=false;
#endif
#if PORT_TITLE == 14
    // Menu9 may have queued Resume/Continue events before controller11.
    if(!overlay_already_updated)
#endif
    events.clear();
    dialogue_text_requests.clear();
    // TH14 live registry: overlay ANM 8, game control 11, replay 12, stage 13,
    // score 17, player 18, Bomb 20, enemies 21, lasers 22, bullets 23, items 24,
    // spell 26, dialogue 27, HUD 28, normal ANM 29. Effects run before score.
#if PORT_TITLE == 14
    // The worker may finish during overlay ANM8 as well as normal ANM29.
    // Keep the legacy normal-only callback out of the overlay traversal.
    bool overlay_loading_failed=false;
    AnmManager::VmBoundaryCallbackTh14 overlay_boundary;
    if(pending_stage&&normal_vm_boundary)overlay_boundary=[&](const AnmVm& vm,AnmManager::VmBoundaryTh14 point){
        if(normal_vm_boundary(vm,point))return true;
        overlay_loading_failed=true;return false;
    };
    // Resume at priority9 continues this frame; overlay8 has already run.
    if(!overlay_already_updated&&!animations.update(true,overlay_boundary)){
        last_error=overlay_loading_failed?-1009:-1005;
        if(error.empty())error=overlay_loading_failed?"loading at overlay VM boundary":"overlay animation update: "+animations.last_error_text();
        return false;
    }
#else
    if(!animations.update(true)){last_error=-1005;if(error.empty())error="overlay animation update: "+animations.last_error_text();return false;}
#endif
#if PORT_TITLE == 14
    if(initial_control_activation){auto activate=std::move(initial_control_activation);initial_control_activation=nullptr;activate();}
#endif
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    const bool control_enabled=animations.th14_control.callbacks_enabled();
#else
    constexpr bool control_enabled=true;
#endif
    if(control_enabled&&!stage_control()){last_error=-1009;return false;}
#if PORT_TITLE == 14
    bool control_running=control_enabled&&(!transitioning||transition_started);
    // Native436be7 reads this live word after stage activation. Bit2 sets
    // bit7 and returns1: later Main callbacks still run, while Control's
    // attract/score/compositor/clock tail does not. Keep its separate load/store.
    if(control_running)if(auto* control=animations.th14_control.get()){
        const u32 flags=control->flags;
        if(flags&4u){control->flags=flags|0x80u;control_running=false;}
    }
#else
    const bool control_running=control_enabled&&(!transitioning||transition_started);
#endif
#if PORT_TITLE == 14
    // The attract decision belongs to Control11, after Overlay8 and stage
    // activation, before the rate-scaled Control timer advances.
    if(control_running&&control_update)control_update();
#endif
    // Legacy420752 holds stage-six music for300 ticks. TH14's complete436620
    // callback plays at activation and has no clock300 music-resume branch.
    if(kPortTitle!=14&&control_running&&!demo&&resources.stage_number==6&&frame==300)events.push_back({BattleEventKind::MusicResume});
    if(control_running)hud.score.update(economy.score_units);
#if PORT_TITLE == 14
    // 436c80..436c98 returns3 for bits4/5/6 after the score callback.
    // Main401280 then stops this pass before Replay12 and all later callbacks.
    if(control_running&&(animations.th14_control.callback_flags()&0x70u))return true;
#endif
    const i32 elapsed_control_frame=
#if PORT_TITLE == 14
        control_frame();
#else
        i32(frame);
#endif
    if(control_running&&elapsed_control_frame>=90&&player&&!(dialogue&&dialogue->active))communication.update(economy,player->motion.state.position.y);
    if(control_running)enemy_commands.section_frame=wrapping_add(enemy_commands.section_frame,1);
    // Retail removes Control11 during loading and re-enables its replacement
    // only at4360c1. The HUD result gate436769..43678a then returns before
    //436cd7..436cfe while its120 ticks remain. Detached copies must not advance
    // on a pass where that controller body does not run.
    if((kPortTitle!=14||control_running)&&!compositor.update()){
        last_error=-1005;error="screen compositor animation update";return false;
    }
#if PORT_TITLE == 14
    // Native436d10 ticks controller+10 before Replay/player callbacks.
    if(control_running){th14_control_clock.tick();th14_control_counted=true;}
#endif
    // Original replay/input callbacks run after game control (priority 10),
    // which selects the incoming stage's stream at activation, before players.
    if(sample_input&&!sample_input())return false;
    // Equal-priority callbacks insert newest first (456b70/456c10).
    if((!transitioning||transition_started)&&!stage->update(
#if PORT_TITLE == 14
        stage_object_boundary,pending_stage?camera_word_boundary:StageCameraWordBoundaryTh14{}
#endif
    )){last_error=-1008;error=stage->error;return false;}
    if(outgoing_stage&&!outgoing_stage->update(
#if PORT_TITLE == 14
        stage_object_boundary,pending_stage?camera_word_boundary:StageCameraWordBoundaryTh14{}
#endif
    )){last_error=-1008;error=outgoing_stage->error;return false;}
    camera_position=compositor.play_camera.position;
    enemy_environment.camera_delta=compositor.play_camera.animation_delta;
    shakes.offset=compositor.world_camera.offset;
    #if PORT_TITLE == 14
#if defined(TH14_CPP_STAGE_DECODE)
    shakes.update(animations.script_rng,false,&animations.visual_rng,&animations.th14_native_stack,&animations.th14_effect_stop,&animations.th14_control);
#else
    shakes.update(animations.script_rng,false,&animations.visual_rng,&animations.th14_native_stack);
#endif
#else
    shakes.update(animations.script_rng,false,kPortTitle==14?&animations.visual_rng:nullptr);
#endif
    compositor.world_camera.offset=shakes.offset;
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    fades.update(false,false,&animations.th14_effect_stop,&animations.th14_control);
#else
    fades.update();
#endif
    // TH14 4577bf registers score at 17; 44c52f registers player at 18.
    // The previous frame's item pickups advance before this player snapshot.
    popups.update(animations.rate);
    synchronize_player();
    if(player_live&&player){
        player->spell={spell_flags,spell_elapsed,spell_bonus};
        if(before_player_update)before_player_update(*this);
        if(!player->update()){if(!last_error)last_error=-601;const std::string inner="player update failed: "+std::to_string(player->last_error)+" motion="+std::to_string(player->motion.last_error)+" shots="+std::to_string(player->shots.last_error)+" state="+std::to_string(player->state.life_state)+" timer="+std::to_string(player->state.state_timer.current);error=error.empty()?inner:inner+" ("+error+")";return false;}
        spell_flags=player->spell.flags;spell_bonus=player->spell.bonus;
    }
    // Bomb activation enables its callback itself (406510); it may run during
    // the outgoing-background overlap while new enemy callbacks are disabled.
    if(bomb){
        if(!bomb->update()){if(error.empty())error="unimplemented or failed Bomb branch "+std::to_string(bomb->selection);last_error=-608;return false;}
        spell_flags=player->spell.flags;spell_bonus=player->spell.bonus;
    }
    synchronize_player();
    target_locked=false;
    if(stage_active&&
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
       !animations.th14_control.enemy_updates_blocked()&&
#endif
       !enemies.update()){last_error=-1001;if(error.empty())error="ECL opcode "+std::to_string(enemies.script_error.opcode)+" at time "+std::to_string(enemies.script_error.time)+(g_th14_fire_stage?" (fire stage "+std::to_string(g_th14_fire_stage)+" bullet error "+std::to_string(g_th14_fire_bullet_error)+" sprite "+std::to_string(g_th14_bullet_sprite)+" transform "+std::to_string(g_th14_bullet_transform_index)+" opcode "+std::to_string(g_th14_bullet_transform_opcode)+" program ["+([&]{std::string s;for(u32 n=0;n<18;++n){if(n)s+=",";s+=std::to_string(g_th14_bullet_program[2*n])+":"+std::to_string(g_th14_bullet_program[2*n+1]);}return s;})()+"] skipped "+std::to_string(g_th14_bullet_unhandled):"");return false;}
    animations.rate=enemy_environment.rate;
    synchronize_player();
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    const u32 laser_control_flags=animations.th14_control.callback_flags();
    //One native43a6a9 word drives both the skip and zero-rate branches.
    if(!(laser_control_flags&0x405u)&&!lasers.update(false,(laser_control_flags&2u)!=0)){last_error=-1003;return false;}
#else
    if(!lasers.update(false,false)){last_error=-1003;return false;}
#endif
    if(stage_active&&
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
       !animations.th14_control.frozen()&&
#endif
       !bullets.update(false)){last_error=-1000-bullets.last_error;return false;}
    synchronize_player();
    if(
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
       !animations.th14_control.item_updates_blocked()&&
#endif
       !items.update()){last_error=-1004;error="item update failed: "+std::to_string(items.last_error);return false;}
    if(spell_flags&1){Vec3 boss;if(!boss_position(boss)||!spells.update(player->motion.state.position.y,boss,kPortTitle==14?special_active==1:special_active!=0)){last_error=-1006;if(error.empty())error="spell update";return false;}}
    completion.update();
    // TH14 boss rings/stars precede the MSG update at430d7b. The indicator
    // follows it at430dfe, including the frame that retires the dialogue.
    if(!hud.update(hud_input())){last_error=-1010;error="HUD update";return false;}
    if(dialogue&&dialogue->active){const i32 result=dialogue->update(player_input.movement.held,player_input.movement.pressed
#if PORT_TITLE == 14
        ,th14_dialogue_skip_frames,th14_dialogue_fire_frames
#endif
    );if(result<0){last_error=-1007;if(error.empty())error=dialogue->error+" MSG opcode "+std::to_string(dialogue->last_opcode);return false;}if(result){
#if PORT_TITLE != 14
        if(enemy_commands.stage_section)enemy_commands.section_frame=0;enemy_commands.stage_section=0;
#endif
    }else dialogue->state.elapsed.tick();}
    if(!hud.finish_update(hud_input())){last_error=-1010;error="HUD indicator update";return false;}
#if PORT_TITLE == 14
    if(last_error)return false;
    // Retail47e5e0 can be entered after the loading worker has restored the
    // incoming Replay entry. Poll at this registry boundary as well as at
    // Supervisor1 so normal ANM consumes the newly ready stage's RNG state.
    if(before_normal_animation_update&&!before_normal_animation_update()){
        last_error=-1009;return false;
    }
#endif
#if PORT_TITLE == 14
    bool loading_failed=false;
    AnmManager::VmBoundaryCallbackTh14 vm_boundary;
    if(pending_stage&&(normal_vm_boundary||before_normal_animation_update))vm_boundary=[&](const AnmVm& vm,AnmManager::VmBoundaryTh14 point){
        if(normal_vm_boundary){if(normal_vm_boundary(vm,point))return true;}
        else if(point==AnmManager::VmBoundaryTh14::BeforeCall||before_normal_animation_update())return true;
        loading_failed=true;return false;
    };
    if(!animations.update(false,vm_boundary)){
        last_error=loading_failed?-1009:-1005;
        if(error.empty())error=loading_failed?"loading at normal VM boundary":"animation update: "+animations.last_error_text();
        return false;
    }
#else
    if(!animations.update(false)){last_error=-1005;if(error.empty())error="animation update: "+animations.last_error_text();return false;}
#endif
    if(control_running)++frame;
    // Both VM registries have now consumed resource retirement markers.
    if(transitioning&&stage_active&&!outgoing_stage){
        auto* old=resources.previous_stage.get();
        if(!old||(!animations.references(old->background)&&!animations.references(old->logo)&&!animations.references(old->enemies))){resources.retire_previous();transitioning=false;}
    }
    return true;
}
bool GameBattle::draw(AnmRenderer& renderer,SceneDrawKind kind
#if PORT_TITLE == 14
        ,const std::function<bool()>& camera_published
#endif
) {
    switch(kind){
    case SceneDrawKind::ScorePopups:
#if PORT_TITLE == 14
        if(!popups.queue_bonuses(ascii))return false;
#endif
        return popups.draw(renderer,resources.core.ascii,player->motion.state.position);
    case SceneDrawKind::HudInner:
#if PORT_TITLE == 14
        // Retail431270..431285 retires the time VM when the spell manager
        // is absent during next-stage loading;47f6e0 defers family deletion.
        if(pending_stage&&hud.show_spell_time){
            enemy_animations.erase(hud.spell_time_animation);
            hud.spell_time_animation=0;hud.show_spell_time=false;
        }
#endif
        return hud.draw_inner(renderer,hud_input())&&hud.queue_text(ascii,hud_input());
    case SceneDrawKind::HudOuter:return hud.draw_outer(renderer,hud_input());
    case SceneDrawKind::AsciiInner:return ascii.draw(renderer,1,compositor.full_camera,compositor.play_camera);
    case SceneDrawKind::AsciiOuter:return ascii.draw(renderer,0,compositor.full_camera,compositor.play_camera);
    case SceneDrawKind::AsciiPlayfield:return ascii.draw(renderer,2,compositor.full_camera,compositor.play_camera);
    case SceneDrawKind::StageBackground:
        if((!transitioning||transition_started)&&stage&&!stage->draw_background(renderer,compositor.world_camera,fades
#if PORT_TITLE == 14
            ,camera_published
#endif
))return false;
        return !outgoing_stage||outgoing_stage->draw_background(renderer,compositor.world_camera,fades
#if PORT_TITLE == 14
            ,camera_published
#endif
);
    case SceneDrawKind::StageForeground:
        if((!transitioning||transition_started)&&stage&&!stage->draw_foreground(renderer,compositor.world_camera
#if PORT_TITLE == 14
            ,camera_published
#endif
))return false;
        return !outgoing_stage||outgoing_stage->draw_foreground(renderer,compositor.world_camera
#if PORT_TITLE == 14
            ,camera_published
#endif
);
    case SceneDrawKind::Spell:
        if(spell_flags&1){
            // TH14 backgrounds join the ANM manager in 41cb20; its layer
            // passes draw them. These two embedded VMs belong to older titles.
            if constexpr(kPortTitle!=14&&kPortTitle!=16)for(auto& vm:spell_backgrounds)if(renderer.draw(vm)==-2){
                error="spell background file="+std::to_string(vm.file_index)+" script="+std::to_string(vm.script_index)+" sprite="+std::to_string(vm.sprite_index)+" mode="+std::to_string((vm.flags>>22)&15);return false;
            }
            if(auto* title=animations.find(spell_titles[2]))return spells.queue_text(ascii,title->color);
            spell_titles[2]=0;
        }
        return true;
    case SceneDrawKind::Player:return player&&player->draw(renderer);
    case SceneDrawKind::Items:
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
        if(animations.th14_control.combat_draw_blocked())return true;
#endif
        return items.draw(renderer);
    case SceneDrawKind::Lasers:
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
        if(animations.th14_control.combat_draw_blocked())return true;
#endif
        return lasers.draw(renderer,false);
    case SceneDrawKind::Bullets:
        if(!stage_active)return true;
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
        if(animations.th14_control.combat_draw_blocked())return true;
#endif
        for(u32 group=0;group<6;++group)if(!bullets.draw_group(group,renderer))return false;
        return true;
    default:return false;
    }
}
HudInput GameBattle::hud_input()const{
    HudInput input;input.player_present=bool(player);
#if PORT_TITLE == 14
    input.character=enemy_environment.character;
    // Boss-name4350b0 reads global4f5844 (practice selection), not Spell+78.
    input.subtype=enemy_environment.subtype;input.spell_id=enemy_environment.spell_practice_id;input.spell_flags=spell_flags;
#endif
    if(player)input.player_position=player->motion.state.position;
    input.dialogue=dialogue&&dialogue->active;input.spell=(spell_flags&1)!=0;
    input.stage=i32(resources.stage_number);input.section=enemy_commands.stage_section;
    input.seconds=th14_hud_load(countdown_seconds);input.hundredths=countdown_hundredths;
    input.practice=completion.mode.practice;input.practice_high=practice_high;input.result_animation=result_animation;
    input.spell_frames=spell_timing.last_frames;input.spell_time=spell_timing.encoded;return input;
}
bool GameBattle::project_spawn(Vec3 position,Vec3& projected){
    // 412f7c selects camera 0, then projects with identity world transform.
    auto& camera=compositor.play_camera;camera.perspective();Matrix4 world;world.identity();
    projected=GraphicsMath::project(position,camera.viewport,camera.projection,camera.view,world);return true;
}
bool GameBattle::start_bomb(){
    if(!bomb||!bomb->supported())return unavailable("unreconstructed Bomb activation");
    const i32 result=bomb->start();
    if(result==-2)error="bomb start failed: bomb_error="+std::to_string(bomb->last_error)+" supported="+std::to_string(bomb->supported()?1:0)+" script="+std::to_string(bomb->create_script)+" opcode="+std::to_string(bomb->create_opcode)+" depth="+std::to_string(bomb->create_depth)+" scripts="+std::to_string(bomb->create_scripts)+" previous=["+error+"]";
#if PORT_TITLE == 14
    if(result==0)enemy_environment.shared_integers[2]=0;
#endif
    return result!=-2;
}
bool GameBattle::callback_spawn(const EnemyState& e,Vec3 p){EnemySpawn spawn;spawn.position=p;spawn.health=spawn.score=10;spawn.drop=-2;std::memcpy(spawn.integers,e.integers,48);return enemies.spawn("MBossCard2_at2",spawn)!=nullptr;}
bool GameBattle::callback_move_player(Vec3 p){
    if(!player)return false;auto& motion=player->motion;
    motion.state.x=truncate_int(double(p.x)*128);motion.state.y=truncate_int(double(p.y)*128);
    motion.state.position.x=float(double(motion.state.x)*.0078125);motion.state.position.y=float(double(motion.state.y)*.0078125);
    for(auto& option:motion.options)option.snap=1;
    // Later enemies in this same traversal read the live player position.
    enemy_environment.player_position=motion.state.position;bullets.player=motion.state.position;
    return true;
}
bool GameBattle::callback_indicator(i16 interrupt,bool update){if(interrupt)callback_shared_visual.interrupt(interrupt);return !update||callback_shared_visual.update(animations)!=-2;}
bool GameBattle::special_damage(const Vec3& p,const Vec2& size,i32& damage){return bomb&&bomb->damage(p,size,damage);}
bool GameBattle::bomb_cancel(Vec3 p,float radius,u32 rewards,bool convert){
    const BulletCancelContext context{player->spell.flags,spell_id};
    if(convert?!bullets.convert_circle(p,radius,rewards&1,context):!bullets.cancel_circle(p,radius,rewards&1,true,context))return false;
    return lasers.cancel_circle(p,radius,rewards,true)>=0;
}
bool GameBattle::th14_bomb_freeze(bool value){
#if PORT_TITLE == 14
    if(!lasers.freeze_th14(value))return false;
    for(auto* b=bullets.first_th14();b;b=b->th14_active_next)b->flags=value?b->flags|0x100u:b->flags&~0x100u;
    return true;
#else
    return false;
#endif
}
bool GameBattle::th14_bomb_cancel(Vec3 p,float radius,u32 rewards){
#if PORT_TITLE == 14
    return bullets.cancel_circle_th14(p,radius,rewards,true)&&lasers.cancel_circle(p,radius,rewards,true)>=0;
#else
    return false;
#endif
}
bool GameBattle::bomb_cancel_beam(Vec3 p,bool reward){
    const BulletCancelContext context{player->spell.flags,spell_id};
    if(!bullets.cancel_circle(p,48,reward,true,context)||!bullets.cancel_beam(p,32,reward,context)||lasers.cancel_circle(p,48,reward,true)<0)return false;
    p.y=float(double(p.y)-224);return lasers.cancel_rectangle(p,{32,448,0},reward)>=0;
}
bool GameBattle::cancel_shot(Vec3 position,float angle) {
    if(!player)return false;
    player->shots.locked_target=EnemyFrameWorld::target;
    // A full native 256-shot pool silently discards the emission.
    return player->shots.spawn_converted_laser(position,angle)!=-2;
}
#if PORT_TITLE == 14
bool GameBattle::th14_probe_damage(EnemyState& e,i32& out,bool& bomb_contact){
    if(!player)return false;const auto& timer=player->state.state_timer;
    return player->shots.damage_th14(e.current.position,e.flags&0x1000?&e.hitbox:nullptr,e.flags&0x1000?0:e.hitbox.x*.5f,e.flags&0x1000?e.animation_angle:0,timer.previous!=timer.current,economy,out,u32(e.instance_id),u32(e.instance_id),e.script_owner,true,&bomb_contact);
}
#endif
bool GameBattle::shot_damage(EnemyState& e,i32& out) {
    if(!player)return false;
    const auto& timer=player->state.state_timer;
    const bool hit=
#if PORT_TITLE == 14
        // Retail 423a3e..423a7f: bit 1000 selects the rectangle pointer;
        // an unset bit passes a null size and half the hitbox width as radius.
        player->shots.damage_th14(e.current.position,e.flags&0x1000?&e.hitbox:nullptr,e.flags&0x1000?0:e.hitbox.x*.5f,e.flags&0x1000?e.animation_angle:0,timer.previous!=timer.current,economy,out,u32(e.instance_id),u32(e.instance_id),e.script_owner);
#elif PORT_TITLE == 16
        player->shots.damage_th16({e.current.position,e.flags&0x1000?nullptr:&e.hitbox,e.flags&0x1000?e.hitbox.x*.5f:0,0,timer.previous!=timer.current,false,u32(e.instance_id),0},out);
#else
        player->shots.damage(e.current.position,e.hitbox,timer.previous!=timer.current,economy,out);
#endif
    if(hit)damage_total=wrapping_add(damage_total,u32(out));
    else if(error.empty())error="player damage collection failed: shots="+std::to_string(player->shots.last_error)+" state="+std::to_string(player->state.life_state)+" timer="+std::to_string(player->state.state_timer.current);
    return hit;
}
i32 GameBattle::apply_collision(PlayerCollisionResult result) {
    if(result.trigger_hit){
        if(!player){last_error=-602;return -2;}
        player->spell={spell_flags,spell_elapsed,spell_bonus};
        if(!player->hit()){last_error=-602;return -2;}
        spell_flags=player->spell.flags;spell_bonus=player->spell.bonus;
    }
    return i32(result.kind);
}
bool GameBattle::player_collision(EnemyState& e,float radius,i32& out) {
    if(!player)return false;
#if PORT_TITLE == 14
    if(e.flags&0x1000){
        // 423dad..423e8b uses a rotated finite laser-shaped body. The
        // start comes from root-VM rotation+pi/2, while the query angle is
        // the shared enemy+120 field; these can differ within an update.
        const auto* vm=animations.find(e.animation_handle(0));if(!vm)return false;
        const float angle=normalize_angle(vm->rotation.z+1.5707963705062866f);
        const float half=e.collision_box.y*.5f;
        const float sine=float(std::sin(double(angle))),cosine=float(std::cos(double(angle)));
        const Vec3 start{e.current.position.x-sine*half,e.current.position.y+cosine*half,e.current.position.z};
        out=collision(start,e.animation_angle,e.collision_box.x,e.collision_box.y);return out>=0;
    }
#endif
    auto shape=player->collision();shape.dialogue_active=dialogue&&dialogue->active;
    out=apply_collision(shape.circle({e.current.position.x,e.current.position.y},radius));return out>=0;
}
i32 GameBattle::collision(const BulletState& b) {
    if(!player)return -2;
    auto shape=player->collision();const Vec2 p{b.position.x,b.position.y};
    shape.dialogue_active=dialogue&&dialogue->active;
#if PORT_TITLE == 14 || PORT_TITLE == 16
    const float size_factor=
#if PORT_TITLE == 16
        b.th16_size;
#else
        b.th14_size;
#endif
    if(b.flags&0x40){const Vec2 size{b.hitbox.x*size_factor,b.hitbox.y*size_factor};return apply_collision(b.flags&0x10?shape.circle(p,size.x):shape.rectangle(p,size));}
#endif
    return apply_collision(b.flags&0x10?shape.circle(p,b.hitbox.x):shape.rectangle(p,b.hitbox));
}
i32 GameBattle::th14_probe_collision(const BulletState& b){
    if(!player)return -2;auto shape=player->collision();shape.dialogue_active=dialogue&&dialogue->active;
    const Vec2 p{b.position.x,b.position.y};Vec2 size=b.hitbox;
#if PORT_TITLE == 14
    if(b.flags&0x40){size.x*=b.th14_size;size.y*=b.th14_size;}
#endif
    return i32((b.flags&0x10?shape.circle(p,size.x,true):shape.rectangle(p,size,true)).kind);
}
i32 GameBattle::collision(Vec3 p,float angle,float width,float length) {
    if(!player)return -2;const auto half=player->state.hit_half;
    auto shape=player->collision();
#if PORT_TITLE == 14
    shape.bomb_active=dialogue&&dialogue->active;
#elif PORT_TITLE == 16
    shape.dialogue_active=dialogue&&dialogue->active;
#endif
    return apply_collision(shape.laser({p.x,p.y},{half.x,half.y},angle,width,length));
}
i32 GameBattle::warning_collision(Vec3 p,float angle,float width,float length) {
    if(!player)return -2;const auto half=player->state.hit_half;
    auto shape=player->collision();
#if PORT_TITLE == 14
    shape.bomb_active=dialogue&&dialogue->active;
#elif PORT_TITLE == 16
    shape.dialogue_active=dialogue&&dialogue->active;
#endif
    return i32(shape.laser({p.x,p.y},{half.x,half.y},angle,width,length,true).kind);
}
void GameBattle::clear_shot_targets(Enemy* enemy) { if(player)player->shots.invalidate_target(enemy);
#if PORT_TITLE == 14
    if(bomb)bomb->invalidate_target(enemy);
#endif
    ShotWorld::enemies=enemies.first; }
bool GameBattle::boss_position(Vec3& out) { if(!enemy_commands.bosses[0])return false;out=enemy_commands.bosses[0]->current.position;return true; }
bool GameBattle::visual(Vec3 p,i32 script) {
#if PORT_TITLE == 16
    // Retail40e5c0 cancellation factory: raw entity position, default layer,
    // flags2 bit10 established before the first script update.
    const float rotation=0;return animations.create(resources.core.bullet,script,7,u32(-1),false,false,&p,nullptr,nullptr,false,&rotation,0x400)!=nullptr;
#elif PORT_TITLE == 14
    // 41acf9 preserves the cancellation script's template layer (199:21).
    return animations.create(resources.core.bullet,script,7,u32(-1),false,false,&p)!=nullptr;
#else
    p.x=float(double(p.x)+224);p.y=float(double(p.y)+16);return animations.create(resources.core.bullet,script,6,22,false,false,&p)!=nullptr;
#endif
}
#if PORT_TITLE == 14
bool GameBattle::th14_bullet_hit_effect(const Vec3& position,i32 script,const Vec3& velocity){
    auto* vm=animations.create(resources.core.bullet,script,7,u32(-1),false,false,&position);
    if(!vm)return false;
    // Retail416e8d reads the live rate after creation, scales each velocity
    // component in two MULSS steps, then41be50 initializes the ANM tween.
    auto& p=vm->position_interpolation;
    p.start[0]=p.start[1]=p.start[2]=0;
    p.end[0]=float(float(velocity.x*animations.rate)*10.f);
    p.end[1]=float(float(velocity.y*animations.rate)*10.f);
    p.end[2]=float(float(velocity.z*animations.rate)*10.f);
    std::memset(p.initial_tangent,0,sizeof(p.initial_tangent));
    std::memset(p.final_tangent,0,sizeof(p.final_tangent));
    p.duration=30;p.mode=InterpolationMode::Decelerate4;p.timer.set(0,&animations.rate);
    return true;
}
#endif
bool GameBattle::th16_hit_effect(const Vec3& position,i32 script,const Vec3& velocity){
#if PORT_TITLE == 16
    const float rotation=0;auto* vm=animations.create(resources.core.bullet,script,7,u32(-1),false,false,&position,nullptr,nullptr,false,&rotation,0x400);
    if(!vm)return false;
    // Native4173f0: current/start zero, goal the already scaled velocity,
    // duration30 and mode6. Preserve the constructor's remaining ANM state.
    bullet_th16_hit_effect_setup(*vm,velocity,&animations.rate);return true;
#else
    (void)position;(void)script;(void)velocity;return false;
#endif
}
bool GameBattle::colored_effect(const Vec3& position,i32 script,u32 color){Vec3 p=position;p.x=float(double(p.x)+224);p.y=float(double(p.y)+16);auto* vm=animations.create(resources.core.bullet,script,6,22,false,false,&p);if(!vm)return false;vm->color=color;return true;}
bool GameBattle::th16_graze(const Vec3& contact){
#if PORT_TITLE == 16
    if(!player)return false;
    struct Services final:Th16GrazeServices {
        GameBattle& battle;explicit Services(GameBattle& b):battle(b){}
        bool effect(Vec3 p,i32 script)override{
            const float rotation=0;
            return battle.animations.create(battle.resources.core.effect,script,8,u32(-1),false,false,&p,nullptr,nullptr,false,&rotation,0x400)!=nullptr;
        }
        bool popup(Vec3 p,i32 value,u32 color)override{return battle.popup(p,value,color);}
        bool sound(i32 id,float x)override{return battle.sound(id,x,true);}
        bool item(i32 kind,Vec3 p,float angle,float speed,i32 force)override{return battle.items.spawn_cancel_th16(kind,p,angle,speed,force)==0;}
    } services(*this);
    return player_th16_graze(economy,player->motion.state.position,contact,services);
#else
    (void)contact;return false;
#endif
}
#if PORT_TITLE == 16
bool GameBattle::th16_line_reward(Vec3 p,u32 rewards){return bullet_th16_cancel_reward(p,rewards,bullets.th16_reward_count);}
#endif
bool GameBattle::season_damage_drop(EnemyState& e){
#if PORT_TITLE == 16
    struct Services final:Th16EnemyItemServices {
        ItemManager& items;explicit Services(ItemManager& m):items(m){}
        bool spawn(Vec3 p,float angle,float speed)override{return items.spawn_cancel_th16(16,p,angle,speed,0)==0;}
    } services(items);
    const bool result=enemy_th16_damage_drop(e,animations.visual_rng,services);
    if(!result)error="TH16 damage seasonal item factory: "+std::to_string(items.last_error);
    return result;
#else
    (void)e;return false;
#endif
}
bool GameBattle::point_reward(Vec3 p) { return items.spawn(8,p,0xffffffff,-1.5707963705062866f,.6f)==0; }
#if PORT_TITLE == 16
bool GameBattle::item_th16_spawn_notice(Vec3 p,i32 type){
    // Complete430d10 request: effect resource, script101, zero rotation;
    // the native sound is non-positional,74 for lives and48 for bombs.
    const float rotation=0;
    return animations.create(resources.core.effect,101,8,u32(-1),false,false,&p,nullptr,nullptr,false,&rotation,0x400)&&sound(type==4||type==5?74:48,0,false);
}
bool GameBattle::item_th16_collect(ItemState& item){
    if(item.type>=1&&item.type<=8)return item_rewards.th16_collect(item,items.player);
    if(item.type!=16&&(item.type<9||item.type>14)){items.last_error=1620+item.type;return false;}
    if(item.type==16&&economy.th16_season.collect()&&!power_changed())return false;
    const i32 amount=item.type==16?10:item_th16_cancel_reward(item.type,economy.point_value,economy.max_point_value,economy.th16_cancel_energy);
    if(!item_rewards.th16_add_score(amount)){items.last_error=1691;return false;}
    return item.type!=16||hud.update_season_th16();
}
#endif
bool GameBattle::sound(i32 id,float x,bool positional) {events.push_back({BattleEventKind::Sound,id,0,{x,0,0},0,positional});return true;}
bool GameBattle::popup(Vec3 p,i32 value,u32 color) {popups.add(p,value,color,&animations.rate);events.push_back({BattleEventKind::Popup,value,0,p,color});return true;}
bool GameBattle::notify(i32 value) {events.push_back({BattleEventKind::Notification,value});return hud.notice(value);}
bool GameBattle::display_lives(i32 lives,i32 fragments) {hud.display_lives(lives,fragments);events.push_back({BattleEventKind::Lives,lives,fragments});return true;}
bool GameBattle::power_changed() {
#if PORT_TITLE == 16
    if(!player||!player->rebuild_th16_options()){items.last_error=1690;return false;}return true;
#else
    return player&&player->motion.rebuild_options(resources.core.shots[shot_file_slot(u32(enemy_environment.character),u32(enemy_environment.subtype))].header,economy,enemy_environment.character*2+enemy_environment.subtype);
#endif
}
bool GameBattle::record_death() {events.push_back({BattleEventKind::Death});return true;}
bool GameBattle::enemy_death() {
    // 0x431307 increments manager +0x10 and resets +0x18. This is a player
    // death counter visible to ECL, not the erase-all-enemies operation.
    enemy_environment.shared_integers[0]=wrapping_add(enemy_environment.shared_integers[0],1);
    enemy_environment.shared_integers[2]=0;return true;
}
bool GameBattle::game_over(bool replay) {
    game_over_requested=true;events.push_back({BattleEventKind::GameOver,i32(replay)});
#if PORT_TITLE == 14
    // 44defc/44df05 opens the menu before the remaining Player updates.
    if(on_game_over)return on_game_over(replay);
#endif
    return true;
}
bool GameBattle::register_graze() { if(economy.graze<99999999)++economy.graze;return true; }
#if PORT_TITLE == 14
bool GameBattle::th14_curve_graze(Vec3 contact){
    if(!player)return false;const Vec3 position=th14_graze(economy,player->motion.state.position,contact);
    // 4504a9 passes -1; effect24 keeps its template layer18.
    if(!animations.create(resources.core.effect,24,8,u32(-1),false,false,&position))return false;
    sound(42,contact.x,true);return true;
}
#endif
bool GameBattle::communication_reward(i32 amount) { communication.reward(economy,amount,&animations.rate);return true; }
bool GameBattle::reward_graze() {constexpr i32 amounts[]={800,500,500,500,500};return communication_reward(amounts[economy.difficulty]);}
bool GameBattle::graze_reward() {constexpr i32 amounts[]={500,400,300,200,200};return communication_reward(amounts[economy.difficulty]);}
bool GameBattle::graze(EnemyState& e) {
#if PORT_TITLE == 14
    // 423eb3..423ebe passes Player+5e0 to the shared450410 consumer.
    return player&&th14_curve_graze(player->motion.state.position);
#else
    return register_graze()&&visual(e.current.position,156)&&reward_graze()&&sound(28,e.current.position.x,true);
#endif
}
bool GameBattle::cancel_bullets(const Vec3* center,float radius,bool skip_delayed) {
#if PORT_TITLE == 14
    // Player 44dc88 uses 41b3f0, always filtering delayed bullets and
    // delivering reward type zero. The legacy spell/erase-enemy path differs.
    return center?bullets.cancel_circle_th14(*center,radius,false,true):bullets.cancel_all_th14();
#else
    cancellation={spell_flags,spell_id};
    // Player respawn passes zero for item rewards and the final argument for
    // delayed-projectile filtering (43113f/43115e). Bombs use bomb_cancel.
    return center?bullets.cancel_circle(*center,radius,false,skip_delayed,cancellation):bullets.cancel_all(skip_delayed,cancellation);
#endif
}
bool GameBattle::spell_begin_visuals(i32 id,i32 timeout,const char* name){
    cancellation={spell_flags,spell_id};
#if PORT_TITLE == 16
    spells.run_flags=enemy_commands.th16_run_flags;
    return spell_th16_create_visuals(animations,enemy_animations,resources.core.ascii,resources.core.text,resources.core.effect,
        resources.stage_number,enemy_commands.stage_section,spells.th16_effect_variant,timeout,spells.circle_position,spell_titles,spell_circle,th16_spell_background,spell_flags,
        [&](u32 title){dialogue_text_requests.push_back({title,0xffffff,0,0,0,Localization::SpellName(u32(id),name?name:"") ,true});events.push_back({BattleEventKind::SpellTitle,i32(title),id});return true;},
        [&](i32 sound_id){return sound(sound_id,0,false);});
#endif
#if PORT_TITLE == 14
    spells.run_flags=enemy_commands.th14_run_flags;
    return spell_th14_create_visuals(animations,enemy_animations,resources.core.ascii,resources.core.text,resources.core.effect,
        resources.stage_number,enemy_commands.stage_section,id,timeout,spells.circle_position,spell_titles,spell_circle,th14_spell_background,
        [&](u32 title){dialogue_text_requests.push_back({title,0xffffff,0,0,0,Localization::SpellName(u32(id),name?name:""),true});events.push_back({BattleEventKind::SpellTitle,i32(title),id});return true;},
        [&](i32 sound_id){return sound(sound_id,0,false);});
#endif
    auto create=[&](AnmResource& resource,i32 script,u16 file,u32* result=nullptr){auto* vm=animations.create(resource,script,file,22,false,false);if(result)*result=vm?vm->id:0;return vm;};
    if(!create(resources.core.ascii,1,2,&spell_titles[0])||!create(resources.core.text,74,0,&spell_titles[1])||!create(resources.core.ascii,2,2,&spell_titles[2]))return false;
    dialogue_text_requests.push_back({spell_titles[1],0xffffff,0,0,0,Localization::SpellName(u32(id),name?name:""),true});
    events.push_back({BattleEventKind::SpellTitle,i32(spell_titles[1]),id});
    if(!sound(14,0,false))return false;
    auto* circle=create(resources.core.bullet,139,6,&spell_circle);if(!circle)return false;
    spell_circle_position(spells.circle_position);
    for(i32 script:{137,138}){AnmVm* found=nullptr;for(auto* node=&circle->child;node;node=node->next)if(node->value->script_index==script){found=node->value;break;}if(!found)return false;found->integers[2]=timeout;}
    if(!create(resources.core.bullet,149,6))return false;
    i32 bg[2]{},effect=10;
    switch(resources.stage_number){
    case 1:bg[0]=7;bg[1]=8;effect=id<2?11:10;break;
    case 2:bg[0]=7;bg[1]=8;break;
    case 3:bg[0]=8;bg[1]=9;effect=11;break;
    case 4:bg[0]=12;bg[1]=13;effect=15;break;
    case 5:bg[0]=16;bg[1]=17;effect=id>=122?19:-1;break;
    case 6:if(enemy_commands.stage_section<24){bg[0]=21;bg[1]=22;effect=23;}else{bg[0]=18;bg[1]=17;effect=20;}break;
    case 7:if(enemy_commands.stage_section<24){bg[0]=11;bg[1]=12;effect=13;}else{bg[0]=7;bg[1]=8;}break;
    default:return false;
    }
    for(u32 i=0;i<2;++i){auto& vm=spell_backgrounds[i];animations.release_geometry(vm);if(!vm.bind_script(resources.stage->enemies,bg[i],9,&animations.rate)||vm.update(animations)<0)return false;}
    return effect<0||create(resources.stage->enemies,effect,9);
}
bool GameBattle::spell_update_visuals(){for(auto& vm:spell_backgrounds)if(vm.update(animations)<0)return false;return true;}
bool GameBattle::spell_timer_interrupt(i16 label){
#if PORT_TITLE == 14
    for(u32 n=0;n<2;++n){auto* vm=hud.digit_th14(n);if(!vm)return false;vm->interrupt(label);if(vm->update(animations)<0)return false;}
#else
    for(auto& vm:hud.digits){vm.interrupt(label);if(vm.update(animations)<0)return false;}
#endif
    return true;
}
void GameBattle::spell_background_end(){
#if PORT_TITLE == 16
    enemy_animations.erase(th16_spell_background);th16_spell_background=0;
#endif
#if PORT_TITLE == 14
    enemy_animations.erase(th14_spell_background);th14_spell_background=0;
#endif
}
bool GameBattle::dialogue_close(){if(dialogue)dialogue->clear();return true;}
bool GameBattle::start_dialogue(i32 id){
    if constexpr(kPortTitle==14){
        // Retail432100 treats music and result sentinels separately from MSG.
        if(id==-3||id==-1){
#if PORT_TITLE == 14
            const i32 cue=stage_music(i32(resources.stage_number),id==-1);
            if((enemy_commands.th14_run_flags&48)==32&&!replaying&&th14_current_music==cue)return true;
            if(th14_audio_flags&16)events.push_back({BattleEventKind::MusicStop});
            if(th14_scores)th14_scores->settings[0x26+cue]=1;
#endif
            events.push_back({id==-1?BattleEventKind::BossMusic:BattleEventKind::StageMusic,i32(resources.stage_number)});
            auto* vm=animations.create(resources.core.front,id==-1?2:1,5,~0u);
            if(!vm){error="TH14 music-transition animation: "+animations.last_error_text();return false;}
            vm->flags2|=0x200;vm->position={};return true;
        }
        if(id==-2){
            if(spell_flags&0x80)return game_over(replaying);
#if PORT_TITLE == 14
            if((enemy_commands.th14_run_flags&48)==32){th14_spell_result_requested=true;return true;}
#endif
            return dialogue_stage_complete();
        }
    }
    if constexpr(kPortTitle==16){
        // Retail 429ff0: -3 selects the stage track and front script 1;
        // -1 selects the boss track and front script 2. Neither replaces MSG.
        if(id==-3||id==-1){
            events.push_back({id==-1?BattleEventKind::BossMusic:BattleEventKind::StageMusic,i32(resources.stage_number)});
            if(!animations.create(resources.core.front,id==-1?2:1,5,~0u)){
                error="TH16 music-transition animation: "+animations.last_error_text();return false;
            }
            return true;
        }
        if(id==-2){error="TH16 ending/results request is not yet implemented";return false;}
    }
    if(!dialogue||!dialogue->begin(resources.stage->messages[stage_dialogue_index(u32(enemy_environment.character),u32(enemy_environment.subtype))],id)){if(dialogue)error=dialogue->error;return false;}
#if PORT_TITLE == 14
        // Retail MSG constructor 4320b1..4320c4 cancels bullets/lasers
        // and calls 424cb0 before ECL 518 repeats those calls at 42882f.
        // Even empty drops consume RNG, so these two clears cannot merge.
        return bullets.cancel_all_th14()&&lasers.cancel_all(false,false)&&enemies.erase_stage_enemies();
#endif
    if constexpr(kPortTitle==16)return true;
    // 41cfe0 uses 40b5c0; ECL 415ade then uses the distinct 40b3a0
    // rectangle conversion. Already-cancelled bullets are excluded.
    if(!bullets.cancel_all(false,cancellation)||!lasers.cancel_all(false,false)||!enemies.erase_stage_enemies())return false;
    const i32 section=wrapping_add(id,1);if(enemy_commands.stage_section!=section)enemy_commands.section_frame=0;enemy_commands.stage_section=section;
    return bullets.cancel_rectangle(false,cancellation)&&lasers.cancel_all(false,false)&&enemies.erase_stage_enemies();
}
}
