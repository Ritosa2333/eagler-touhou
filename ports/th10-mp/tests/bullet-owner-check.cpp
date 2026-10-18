// Optional native-layout bullet-owner CPU diagnostic using private retail data.
// Actual spawn, commands, features, update and authored Draw run against original
// bullet.anm. This is not ECL gameplay, a browser/GPU test or a game-FPS claim.
#include "game-resource-fixture.hpp"
#include "../th10_web/cpp/game/BulletEmitter.hpp"
#include "../th10_web/cpp/game/HighRefresh.hpp"
#include "../th10_web/cpp/platform/GameplayData.hpp"
using namespace th10;
using namespace th10_test;
// Only response/effect outcomes excluded by scope are stubbed. Fail closed if
// a command requests one; never silently approximate successful gameplay.
namespace th10 {
u32 AnmManager::create_at(AnmFile&,i32,const Vec3&,bool,AnimationPlacement,AnmEnvironment&,AnmAllocationEnvironment&){CHECK(false);return 0;}
i32 EnemyBullet::cancel(BulletEffectEnvironment&){CHECK(false);return 0;}
}
namespace {
struct RejectAllocation final:AnmAllocationEnvironment {
    AnmVm* allocate_animation() override{CHECK(false);return nullptr;}
    void release_memory(void*) override{CHECK(false);}
    void preserve_animation_slot(AnmVm&) override{CHECK(false);}
};
struct Bullets final:BulletBehaviorEnvironment {
    Animations& animation_env;AnmRenderer& renderer;EnemyBulletManager& owner;RejectAllocation rejected_allocation;
    Vec3 target{32,380,0};u32 controller=0,commands_called=0,collision_queries=0,presentations=0,submissions=0;
    std::array<u32,10> features{};Netplay::RollbackJournal* journal=nullptr;Netplay::SparsePoolCapture<2000>* capture=nullptr;
    Bullets(Animations& a,AnmRenderer& r,EnemyBulletManager& b,AnmManager& m):animation_env(a),renderer(r),owner(b){
        default_rate=&a.speed;manager=&m;effect_file=&a.file;animations=&a;allocation=&rejected_allocation;controller_flags=&controller;player_position=&target;rng=&a.rng;
        sprite_scripts=browser::gameplay_data::sprite_scripts;cancel_types=browser::gameplay_data::cancel_types;cancel_scripts=browser::gameplay_data::cancel_scripts;draw_layers=browser::gameplay_data::draw_layers;hitbox_sizes=browser::gameplay_data::hitbox_sizes;
    }
    bool preserve_bullet(EnemyBullet& bullet) override{return !journal||capture->TouchSlot(owner.pool,&bullet,[&](void* p,std::size_t n){return journal->Touch(p,n);});}
    void spawn_faith(const Vec3&) override{CHECK(false);}
    void play_sound(i32,float) override{CHECK(false);}
    void play_turn_sound(i32) override{CHECK(false);}
    void run_commands(EnemyBullet& bullet) override{++commands_called;bullet.process_commands(*this);}
    void update_feature(EnemyBullet& bullet,BulletFeature feature) override{++features.at(static_cast<unsigned>(feature));bullet.update_feature(feature,*this);}
    i32 collide_player(const Vec3&,const Vec2&) override{++collision_queries;return 0;}
    void emit(const BulletEmitter&) override{CHECK(false);}
    void submit(AnmVm& vm) override{++submissions;renderer.draw(vm);}
    bool presentation(const EnemyBullet&,Vec3&,float&) override{++presentations;return false;}
    void reset_counts(){commands_called=collision_queries=presentations=submissions=0;features.fill(0);}
};
void float_arg(ProjectileCommand& command,unsigned index,float value){std::memcpy(command.arguments+index,&value,sizeof(value));}
void make_command(BulletEmitter& emitter,unsigned kind){
    auto& c=emitter.commands[0];
    switch(kind){
    case 0:break;
    case 1:c.type=0x10;float_arg(c,0,.035f);float_arg(c,1,1.2f);c.arguments[2]=8;break;
    case 2:c.type=0x20;float_arg(c,0,.015f);float_arg(c,1,.025f);c.arguments[2]=10;break;
    case 3:c.type=0x40;float_arg(c,0,.4f);float_arg(c,1,2.2f);c.arguments[2]=4;c.arguments[3]=2;break;
    case 4:c.type=0x80;float_arg(c,0,.15f);float_arg(c,1,2.1f);c.arguments[2]=5;c.arguments[3]=2;break;
    case 5:c.type=0x4000000;float_arg(c,0,.045f);float_arg(c,1,.1f);c.arguments[2]=9;break;
    case 6:c.type=1;break;
    case 7:c.type=0x100;float_arg(c,0,1.1f);float_arg(c,1,2.f);c.arguments[2]=3;c.arguments[3]=2;break;
    default:CHECK(false);
    }
}
void initialize_scene(EnemyBulletManager& bullets,AnmManager& manager,Bullets& env,const std::vector<Script>& scripts,unsigned count,bool features){
    CHECK(count==1200||count==2000);std::memset(&bullets,0,sizeof(bullets));bullets.animation_file=&env.animation_env.file;bullets.cursor=bullets.pool;bullets.pool[2000].state=5;
    for(unsigned i=0;i<2000;++i)bullets.pool[i].initialize();env.animation_env.rng={0x1234,0,0};manager.started_scripts=0;
    constexpr unsigned types[]={0,1,3,4,5,7,8,10,11,12,13,14,15,16,18,19,20,21,22,23};
    unsigned unique_scripts=0,oriented=0;std::vector<bool> selected(scripts.size(),false);
    for(unsigned group=0;group<count/100;++group){
        BulletEmitter emitter;emitter.initialize();emitter.sprite_type=types[group%(sizeof(types)/sizeof(types[0]))];emitter.color=emitter.sprite_type==18?0:emitter.sprite_type==23?group%3:group%4;emitter.count=100;emitter.layers=1;
        emitter.pattern=group%3==0?3:group%3==1?1:8;emitter.position={-96.f+float(group%5)*48.f,112.f+float((group/5)%4)*40.f,0};
        emitter.angle=emitter.pattern==8?2.7f:1.5707963705062866f;emitter.spread=emitter.pattern==8?.4f:.022f;emitter.speed_start=1.6f+float(group%4)*.35f;emitter.speed_end=1.3f;
        const auto script=env.sprite_scripts[emitter.sprite_type]+emitter.color;CHECK(script>=0&&std::size_t(script)<scripts.size()&&scripts[script].cpu_safe);CHECK(std::size_t(script)<selected.size());if(!selected[script]){selected[script]=true;++unique_scripts;}
        if(features)make_command(emitter,group%8);CHECK(emitter.fire(bullets,env)==0);
    }
    for(unsigned i=0;i<count;++i){CHECK(bullets.pool[i].state==1&&bullets.pool[i].animation.sprite);oriented+=(bullets.pool[i].animation.flags&0x8000000)!=0;}
    for(unsigned i=count;i<2000;++i)CHECK(!bullets.pool[i].state);CHECK(bullets.pool[2000].state==5);
    if(count==2000){const auto before=copy_bytes(&bullets,sizeof(bullets));BulletEmitter full;full.initialize();full.count=1;full.layers=1;full.sprite_type=3;full.speed_start=1.f;CHECK(full.fire(bullets,env)==0);CHECK(std::memcmp(&bullets,before.data(),before.size())==0);}
    std::printf("{\"suite\":\"native-bullet-scene\",\"instances\":%u,\"feature_commands\":%s,\"unique_scripts\":%u,\"directional_draw_vms\":%u,\"native_spawn_calls\":%u,\"patterns\":[1,3,8],\"authored_draw\":true}\n",count,features?"true":"false",unique_scripts,oriented,env.commands_called);
}
void render_bullets(EnemyBulletManager& bullets,AnmManager& manager,Backend& backend,AnmRenderer& renderer,Bullets& environment){
    CHECK(!high_refresh::render_only);backend.reset();manager.current_texture=nullptr;std::memset(manager.cached_draw_state,0xff,sizeof(manager.cached_draw_state));manager.submitted_draws=manager.flushed_batches=0;
    renderer.begin_frame();CHECK(bullets.draw(environment)==1);renderer.flush();CHECK(backend.quads==manager.submitted_draws&&backend.calls==manager.flushed_batches);
}
void dense_bullets(AnmManager& manager,AnmFile& file,const std::vector<Script>& scripts,unsigned count,u32 file_size,bool feature_commands){
    constexpr unsigned Frames=12,Trials=5,WarmupPasses=2;using Clock=std::chrono::steady_clock;
    const auto elapsed=[](Clock::time_point begin){return std::chrono::duration<double,std::milli>(Clock::now()-begin).count();};
    auto owner=std::make_unique<EnemyBulletManager>();Animations animations(file);Backend backend;AnmRenderer renderer{manager,backend};Bullets environment(animations,renderer,*owner,manager);
    initialize_scene(*owner,manager,environment,scripts,count,feature_commands);
    const auto initial=copy_bytes(owner.get(),sizeof(*owner)),program=copy_bytes(file.loaded,file_size);const auto initial_rng=animations.rng;const auto initial_started=manager.started_scripts;
    Netplay::RollbackJournal journal;CHECK(journal.Reset({Frames+2,sizeof(*owner)+8192,2010,true,true}));Netplay::SparsePoolCapture<2000> capture;
    const auto snapshot=[&](unsigned frame){
        CHECK(journal.BeginFrame(frame));
        // Exact owner recipe from RollbackState.cpp::touch_bullets, plus RNG and
        // started_scripts from the animation-engine recipe used by this scene.
        CHECK(journal.Touch(owner.get(),offsetof(EnemyBulletManager,pool)));CHECK(journal.Touch(&owner->animation_file,sizeof(owner->animation_file)));
        CHECK(capture.Capture(owner->pool,[](const EnemyBullet& b){return b.state!=0;},[&](void* p,std::size_t n){return journal.Touch(p,n);}));
        CHECK(journal.Touch(&animations.rng,sizeof(animations.rng)));CHECK(journal.Touch(&manager.started_scripts,sizeof(manager.started_scripts)));
    };
    const auto restored=[&](){CHECK(std::memcmp(owner.get(),initial.data(),initial.size())==0);CHECK(std::memcmp(&animations.rng,&initial_rng,sizeof(initial_rng))==0);CHECK(manager.started_scripts==initial_started);};
    const auto render=[&](){render_bullets(*owner,manager,backend,renderer,environment);};
    struct Evidence{std::vector<u8> vertices,state;u32 quads,calls,textures,commands,collisions,active;std::array<u32,10> features;};std::array<Evidence,Frames> frames;u64 draws_mutating=0;
    for(unsigned frame=0;frame<Frames;++frame){
        snapshot(frame);environment.reset_counts();CHECK(owner->update(environment)==1);CHECK(owner->active_count==i32(count));
        const auto before_render=copy_bytes(owner.get(),sizeof(*owner));render();
        for(unsigned i=0;i<count;++i){const auto* before=reinterpret_cast<const EnemyBulletManager*>(before_render.data());draws_mutating+=std::memcmp(&owner->pool[i],&before->pool[i],sizeof(EnemyBullet))!=0;}
        CHECK(environment.submissions==count&&environment.presentations==count);
        frames[frame]={copy_bytes(manager.vertex_buffer,std::size_t(manager.vertex_write-manager.vertex_buffer)*sizeof(AnmVertex)),copy_bytes(owner.get(),sizeof(*owner)),backend.quads,backend.calls,backend.textures,environment.commands_called,environment.collision_queries,static_cast<u32>(owner->active_count),environment.features};
        CHECK(journal.EndFrame());
    }
    CHECK(draws_mutating==u64(count)*Frames);const auto final_rng=animations.rng;const auto final_started=manager.started_scripts;
    const auto replayed=[&](){CHECK(std::memcmp(owner.get(),frames.back().state.data(),sizeof(*owner))==0);CHECK(std::memcmp(&animations.rng,&final_rng,sizeof(final_rng))==0);CHECK(manager.started_scripts==final_started);CHECK(std::memcmp(file.loaded,program.data(),program.size())==0);};
    const auto verify_frame=[&](unsigned frame){const auto& e=frames[frame];const auto bytes=std::size_t(manager.vertex_write-manager.vertex_buffer)*sizeof(AnmVertex);
        CHECK(bytes==e.vertices.size());CHECK(std::memcmp(manager.vertex_buffer,e.vertices.data(),bytes)==0);CHECK(std::memcmp(owner.get(),e.state.data(),sizeof(*owner))==0);
        CHECK(backend.quads==e.quads&&backend.calls==e.calls&&backend.textures==e.textures);CHECK(environment.commands_called==e.commands&&environment.collision_queries==e.collisions&&environment.features==e.features);CHECK(owner->active_count==i32(e.active));
    };
    CHECK(journal.UndoTo(0));restored();u64 quads=0,vertex_hash=14695981039346656037ull,command_calls=0,collision_queries=0;std::array<u32,10> feature_calls{};
    for(unsigned frame=0;frame<Frames;++frame){environment.reset_counts();CHECK(owner->update(environment)==1);render();verify_frame(frame);quads+=backend.quads;const auto& e=frames[frame];vertex_hash=hash_bytes(e.vertices.data(),e.vertices.size(),vertex_hash);command_calls+=e.commands;collision_queries+=e.collisions;for(unsigned i=0;i<feature_calls.size();++i)feature_calls[i]+=e.features[i];}
    replayed();
    std::printf("{\"suite\":\"native-bullet-replay\",\"instances\":%u,\"feature_commands\":%s,\"frames\":12,\"quads\":%llu,\"rewind_owner_byte_equal\":true,\"replay_owner_byte_equal\":true,\"replay_every_frame_owner_byte_equal\":true,\"vertex_byte_equal\":true,\"authored_draw_mutations\":%llu,\"resource_bytes_unchanged\":true,\"command_calls\":%llu,\"no_hit_queries\":%llu,\"vertex_hash\":\"%016llx\",\"feature_calls\":[",count,feature_commands?"true":"false",(unsigned long long)quads,(unsigned long long)draws_mutating,(unsigned long long)command_calls,(unsigned long long)collision_queries,(unsigned long long)vertex_hash);
    for(unsigned i=0;i<feature_calls.size();++i)std::printf("%s%u",i?",":"",feature_calls[i]);std::puts("]}");
    std::memcpy(owner.get(),initial.data(),initial.size());animations.rng=initial_rng;manager.started_scripts=initial_started;
    for(unsigned pass=0;pass<WarmupPasses+Trials;++pass){
        double snapshot_ms=0,update_ms=0,draw_ms=0,end_ms=0;u64 snapshot_bytes=0,snapshot_blocks=0;const auto growths=journal.ArenaGrowths();
        for(unsigned frame=0;frame<Frames;++frame){
            auto begin=Clock::now();snapshot(frame);snapshot_ms+=elapsed(begin);snapshot_bytes+=journal.BytesForFrame(frame);snapshot_blocks+=journal.BlocksForFrame(frame);environment.reset_counts();
            begin=Clock::now();CHECK(owner->update(environment)==1);update_ms+=elapsed(begin);
            begin=Clock::now();render();draw_ms+=elapsed(begin);
            begin=Clock::now();CHECK(journal.EndFrame());end_ms+=elapsed(begin);verify_frame(frame);
        }
        replayed();auto begin=Clock::now();CHECK(journal.UndoTo(0));const auto undo_ms=elapsed(begin);restored();if(pass<WarmupPasses)continue;CHECK(journal.ArenaGrowths()==growths);
        std::printf("{\"suite\":\"native-bullet-timing\",\"instances\":%u,\"feature_commands\":%s,\"trial\":%u,\"frames\":12,\"warmup_frames\":24,\"snapshot_ms\":%.6f,\"update_ms\":%.6f,\"authored_draw_ms\":%.6f,\"end_ms\":%.6f,\"undo_ms\":%.6f,\"snapshot_bytes\":%llu,\"snapshot_blocks\":%llu,\"arena_growths\":0,\"active_after\":%d,\"quads\":%llu,\"vertex_hash\":\"%016llx\"}\n",count,feature_commands?"true":"false",pass-WarmupPasses,snapshot_ms,update_ms,draw_ms,end_ms,undo_ms,(unsigned long long)snapshot_bytes,(unsigned long long)snapshot_blocks,count,(unsigned long long)quads,(unsigned long long)vertex_hash);
    }
}
// Focused sparse-slot regression: native spawn during open journal frames,
// ordinary offscreen retirement, then ring-spawn reuse through the wrap cursor.
// This is deliberately separate from fixed-density timing scenes.
void sparse_spawn_replay(AnmManager& manager,AnmFile& file,u32 file_size){
    constexpr unsigned Frames=12;auto owner=std::make_unique<EnemyBulletManager>();owner->animation_file=&file;owner->cursor=owner->pool;owner->pool[2000].state=5;
    for(unsigned i=0;i<2000;++i)owner->pool[i].initialize();Animations animations(file);animations.rng={0x6789,0,0};Backend backend;AnmRenderer renderer{manager,backend};Bullets environment(animations,renderer,*owner,manager);
    const auto initial=copy_bytes(owner.get(),sizeof(*owner)),program=copy_bytes(file.loaded,file_size);const auto initial_rng=animations.rng;const auto initial_started=manager.started_scripts;
    Netplay::RollbackJournal journal;CHECK(journal.Reset({Frames+2,sizeof(*owner)+8192,2010,true,true}));Netplay::SparsePoolCapture<2000> capture;environment.journal=&journal;environment.capture=&capture;
    struct Evidence{std::vector<u8> owner,vertices;u32 active,blocks;};std::array<Evidence,Frames> frames;
    const auto advance=[&](unsigned frame){
        if(frame==0||frame==4||frame==11){BulletEmitter emitter;emitter.initialize();emitter.count=frame==11?1400:600;emitter.layers=1;emitter.sprite_type=3;emitter.color=1;emitter.pattern=frame==11?3:1;
            emitter.position={0,frame==0?430.f:180.f,0};emitter.angle=1.5707963705062866f;emitter.spread=0;emitter.speed_start=frame==0?4.f:2.f;CHECK(emitter.fire(*owner,environment)==0);}
        CHECK(owner->update(environment)==1);render_bullets(*owner,manager,backend,renderer,environment);
        CHECK(owner->active_count==(frame<4?600:frame<10?1200:frame==10?600:2000));CHECK(owner->pool[2000].state==5);
    };
    for(unsigned frame=0;frame<Frames;++frame){
        CHECK(journal.BeginFrame(frame));CHECK(journal.Touch(owner.get(),offsetof(EnemyBulletManager,pool)));CHECK(journal.Touch(&owner->animation_file,sizeof(owner->animation_file)));
        CHECK(capture.Capture(owner->pool,[](const EnemyBullet& b){return b.state!=0;},[&](void* p,std::size_t n){return journal.Touch(p,n);}));
        CHECK(journal.Touch(&animations.rng,sizeof(animations.rng)));CHECK(journal.Touch(&manager.started_scripts,sizeof(manager.started_scripts)));
        advance(frame);frames[frame]={copy_bytes(owner.get(),sizeof(*owner)),copy_bytes(manager.vertex_buffer,std::size_t(manager.vertex_write-manager.vertex_buffer)*sizeof(AnmVertex)),static_cast<u32>(owner->active_count),static_cast<u32>(journal.BlocksForFrame(frame))};CHECK(journal.EndFrame());
    }
    const auto final_rng=animations.rng;const auto final_started=manager.started_scripts;CHECK(journal.UndoTo(0));CHECK(std::memcmp(owner.get(),initial.data(),initial.size())==0);CHECK(std::memcmp(&animations.rng,&initial_rng,sizeof(initial_rng))==0);CHECK(manager.started_scripts==initial_started);
    environment.journal=nullptr;u64 vertices=14695981039346656037ull;
    for(unsigned frame=0;frame<Frames;++frame){advance(frame);const auto& e=frames[frame];CHECK(std::memcmp(owner.get(),e.owner.data(),e.owner.size())==0);CHECK(std::size_t(manager.vertex_write-manager.vertex_buffer)*sizeof(AnmVertex)==e.vertices.size());CHECK(std::memcmp(manager.vertex_buffer,e.vertices.data(),e.vertices.size())==0);vertices=hash_bytes(e.vertices.data(),e.vertices.size(),vertices);}
    CHECK(std::memcmp(&animations.rng,&final_rng,sizeof(final_rng))==0);CHECK(manager.started_scripts==final_started);CHECK(std::memcmp(file.loaded,program.data(),program.size())==0);
    std::printf("{\"suite\":\"native-bullet-sparse-replay\",\"frames\":12,\"spawned\":2600,\"native_offscreen_retirements\":600,\"reused_slots\":600,\"final_active\":2000,\"rewind_owner_byte_equal\":true,\"replay_every_frame_owner_byte_equal\":true,\"vertex_byte_equal\":true,\"resource_bytes_unchanged\":true,\"vertex_hash\":\"%016llx\",\"active_per_frame\":[",(unsigned long long)vertices);
    for(unsigned i=0;i<Frames;++i)std::printf("%s%u",i?",":"",frames[i].active);std::printf("],\"capture_blocks_per_frame\":[");for(unsigned i=0;i<Frames;++i)std::printf("%s%u",i?",":"",frames[i].blocks);std::puts("]}");
}
} // namespace
int main(){
    std::puts("{\"suite\":\"scope\",\"runtime\":\"wasm32-wasip1\",\"native_bullet_owner\":true,\"original_bullet_anm\":true,\"native_spawn_patterns\":true,\"native_commands_features\":true,\"no_hit_collision_stub\":true,\"effect_and_cancel_paths_fail_closed\":true,\"ecl\":false,\"gpu\":false,\"browser\":false,\"mobile\":false,\"full_game_fps\":false,\"timing_informational\":true}");
    arithmetic_mode(Precision::Single,Rounding::NearestEven);Host host;browser::FileSystem files(host);CHECK(files.attach_archive("/input/th10.dat"));auto manager=std::make_unique<AnmManager>();Resources resources(files,*manager);
    u32 file_size=0;for(i32 i=0;i<files.resources.count;++i)if(std::strcmp(files.resources.entries[i].name,"bullet.anm")==0)file_size=files.resources.entries[i].size;CHECK(file_size);
    std::vector<u8> bytes(file_size);CHECK(files.resources.read("bullet.anm",bytes.data(),files.archives)==bytes.data());const auto scripts=inspect_anm(bytes.data(),bytes.size());auto* file=manager->load(7,"bullet.anm",resources);CHECK(file&&manager->resources_ready());CHECK(file->script_count==i32(scripts.size()));
    for(bool features:{false,true})for(unsigned count:{1200u,2000u})dense_bullets(*manager,*file,scripts,count,file_size,features);
    sparse_spawn_replay(*manager,*file,file_size);manager->unload(7,resources);std::puts("{\"suite\":\"result\",\"passed\":true}");
}
