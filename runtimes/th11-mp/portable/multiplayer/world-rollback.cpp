#include "../../th11_web/cpp/game/WorldRollback.hpp"
#include <eagler/netplay/RollbackJournal.hpp>
#include "../../th11_web/cpp/game/RollbackCapture.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <type_traits>
#include <vector>
using namespace th11;
namespace {
unsigned checks=0,cases=0,ticks=0;std::size_t peak_bytes=0,peak_blocks=0;
void check(bool value,const char* message){++checks;if(!value){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}}
template<class T>void append(std::vector<u8>& out,const T& value){
    static_assert(std::is_trivially_copyable<T>::value,"oracle serializes values, never owner headers");
    const auto* p=reinterpret_cast<const u8*>(&value);out.insert(out.end(),p,p+sizeof value);
}
std::string index(const std::string& p,std::size_t n){return p+"/"+std::to_string(n);}
AnmVm normalized(AnmVm vm){
    // Geometry allocation identity is not semantic identity. Its VM owner,
    // presence, length and every live geometry byte are recorded separately.
    vm.geometry=vm.geometry?reinterpret_cast<void*>(uintptr_t(1)):nullptr;return vm;
}
struct Image{
    std::map<std::string,std::vector<u8>> values;
    template<class... T>void put(const std::string& key,const T&... value){auto& out=values[key];(append(out,value),...);}
    void blob(const std::string& key,const void* data,std::size_t count){
        auto& out=values[key];if(count){const auto* p=static_cast<const u8*>(data);out.assign(p,p+count);}
    }
    void text(const std::string& key,const std::string& text){blob(key,text.data(),text.size());}
    template<class T>void array(const std::string& key,const std::vector<T>& v){blob(key,v.data(),v.size()*sizeof(T));}
    void vm(const std::string& key,const AnmVm& value){put(key,normalized(value));}
    template<class C>void vms(const std::string& key,const C& values){std::size_t n=0;for(auto& value:values)vm(index(key,n++),value);}
    void compare(const Image& actual,const char* phase)const{
        if(values.size()!=actual.values.size()){
            std::fprintf(stderr,"oracle population mismatch phase=%s expected=%zu actual=%zu\n",phase,values.size(),actual.values.size());
            for(const auto& [key,value]:values)if(!actual.values.count(key))std::fprintf(stderr,"missing owner key=%s\n",key.c_str());
            for(const auto& [key,value]:actual.values)if(!values.count(key))std::fprintf(stderr,"unexpected owner key=%s\n",key.c_str());
        }
        for(const auto& [key,expected]:values){
            auto at=actual.values.find(key);
            if(at==actual.values.end()||expected!=at->second){
                std::size_t byte=0;if(at!=actual.values.end())while(byte<expected.size()&&byte<at->second.size()&&expected[byte]==at->second[byte])++byte;
                std::fprintf(stderr,"oracle mismatch phase=%s key=%s byte=%zu expected_size=%zu actual_size=%zu\n",
                    phase,key.c_str(),byte,expected.size(),at==actual.values.end()?0:at->second.size());
                check(false,"independent complete owner oracle equality");
            }
            ++checks;
        }
        check(values.size()==actual.values.size(),"oracle owner/key population size matches");
    }
};
void economy(Image& out,const std::string& key,const GameEconomy& e){
    out.put(key,e.score_units,e.power,e.point_value,e.communication,e.lives,e.life_fragments,e.difficulty,e.rank,
        e.max_power,e.power_step,e.graze,e.max_point_value,&e.score_units);
}
}
namespace th11 {
struct AnmRollbackTestAccess{
    static void observe(Image& out,const std::string& key,const AnmManager& m){
        out.put(key+"/owner",m.rate,m.script_rng,m.visual_rng,m.reference_positions,m.camera_delta,m.default_tangent,
            m.last_error,m.next_id,m.spawn_depth,m.heads,m.tails,m.layers,m.multiplayer_owner,m.logical_storage_size_);
        out.array(key+"/free",m.available);
        std::map<u32,AnmVm*> active(m.active.begin(),m.active.end());
        for(auto& [id,vm]:active)out.put(index(key+"/active",id),vm);
        std::map<const AnmVm*,unsigned> tags(m.multiplayer_tags.begin(),m.multiplayer_tags.end());
        for(auto& [vm,tag]:tags)out.put(index(key+"/tag",uintptr_t(vm)),tag);
        for(std::size_t n=0;n<m.logical_storage_size_;++n){out.put(index(key+"/slot-address",n),m.storage[n].get());out.vm(index(key+"/slot",n),*m.storage[n]);}
        for(auto& [vm,g]:m.geometry_sizes){
            check(vm->geometry==g.memory.get(),"ANM geometry owner map matches VM pointer");
            const auto name=index(key+"/geometry",uintptr_t(vm));out.put(name+"/bytes",g.bytes);out.blob(name+"/payload",g.memory.get(),g.bytes);
        }
        std::set<const AnmVm*> visited;
        for(unsigned list=0;list<2;++list){
            const AnmVm::Node* previous=nullptr;const auto* node=m.heads[list];unsigned budget=0;
            for(;node;previous=node,node=node->next){
                check(++budget<=m.active.size(),"ANM registry has no cycles");
                check(node->previous==previous&&node->value&&m.active.count(node->value->id),"ANM live intrusive registry membership/backlinks");
                check(visited.insert(node->value).second,"ANM VM appears in only one live registry");
            }
            check(previous==m.tails[list],"ANM tail agrees with traversed live list");
        }
        check(visited.size()==m.active.size(),"ANM active map and registry roster agree");
    }
};
struct EnemyRollbackTestAccess{
    static void observe(Image& out,const std::string& key,const EnemyManager& m){
        out.put(key+"/metadata",m.first,m.last,m.count,m.total_created,m.timer,m.last_error,m.script_error,m.program,m.spawn_depth,m.rollback_logical_size_);
        out.array(key+"/free",m.rollback_available_);
        for(std::size_t i=0;i<m.rollback_logical_size_;++i){const auto& e=*m.rollback_storage_[i];
            out.put(index(key+"/slot",i),e.enemy,e.script.error,&e.script.globals.enemy,&e.script.globals.environment,&e.script.commands);
        }
        std::map<Enemy*,EnemyRollbackEntry*> active;
        for(auto& [enemy,entry]:m.entries)active[enemy]=entry.get();
        for(auto& [enemy,entry]:active)out.put(index(key+"/live",uintptr_t(enemy)),entry);
        const auto& allocator=m.rollback_allocator_;
        for(auto& [size,frontier]:allocator.state_.logical_sizes){
            const auto name=index(key+"/allocation-size",size);out.put(name+"/frontier",frontier);
            const auto& storage=allocator.storage_.at(size);
            for(u32 i=0;i<frontier;++i){out.put(index(name+"/address",i),storage[i].get());out.blob(index(name+"/payload",i),storage[i].get(),size);}
        }
        for(auto& [size,free]:allocator.state_.available)out.array(index(key+"/allocation-free",size),free);
        for(auto& [pointer,a]:allocator.state_.active)out.put(index(key+"/allocation-live",uintptr_t(pointer)),a.bytes,a.index);
        const EnemyLink* previous=nullptr;unsigned count=0;
        for(auto* node=m.first;node;previous=node,node=node->next){
            check(++count<=m.count&&node->previous==previous&&m.entries.count(node->value),"Enemy roster/backlink consistency");
            const auto& owner=node->value->script;const EclThread* prior=nullptr;unsigned threads=0;
            for(auto* thread=&owner.threads;thread;prior=thread,thread=thread->next){
                check(++threads<4096&&thread->previous==prior&&thread->value->owner==&owner,"ECL owner/thread topology");
                if(prior)check(allocator.state_.active.count(const_cast<EclThread*>(thread))&&allocator.state_.active.count(thread->value),"secondary ECL node/context have live allocator leases");
            }
        }
        check(previous==m.last&&count==m.count&&count==m.entries.size(),"Enemy list/tail/map counts agree");
    }
};
struct LaserRollbackTestAccess{
    static void observe(Image& out,const std::string& key,const LaserManager& m){
        out.put(key+"/owner",m.sentinel,m.tail,m.rate,m.player,m.active_count,m.next_id,m.last_error,m.last_center,m.last_size,m.updating,m.rollback_occupied);
        // Always observe the logical fixed pool, treating never-allocated slots as
        // their initial zero bytes. Future physical allocation is not live state.
        std::vector<u8> storage(m.slot_bytes*m.capacity);
        if(m.rollback_storage)std::memcpy(storage.data(),m.rollback_storage.get(),storage.size());
        for(u32 i=0;i<m.capacity;++i){
            auto* raw=storage.data()+i*m.slot_bytes;const auto* base=reinterpret_cast<const LaserState*>(raw);
            if(base->type==1){auto* laser=reinterpret_cast<LaserInfinite*>(raw);laser->body=normalized(laser->body);laser->origin=normalized(laser->origin);}
            else{auto* laser=reinterpret_cast<LaserLine*>(raw);laser->body=normalized(laser->body);laser->origin=normalized(laser->origin);}
        }
        out.array(key+"/all-slots",storage);
        const LaserState* previous=&m.sentinel;unsigned count=0;
        for(auto* p=m.sentinel.next;p;previous=p,p=p->next)check(++count<=m.capacity&&p->previous==previous,"laser topology/backlinks");
        check(count==m.active_count&&previous==m.tail,"laser list/tail/count agree");
    }
};
struct DeformationRollbackTestAccess{
    static void observe(Image& out,const std::string& key,const ScreenDeformationPool& p){
        out.put(key+"/logical-size",p.logical_size_);
        for(std::size_t i=0;i<p.logical_size_;++i){const auto& slot=p.storage_[i];const auto& d=*slot.object;const auto n=index(key,i);
            out.put(n+"/owner",slot.object.get(),slot.occupied,d.columns,d.rows);
            out.array(n+"/vertices",d.vertices);out.array(n+"/positions",d.positions);out.array(n+"/animations",d.animations);
        }
    }
};
struct WorldRollbackTestAccess{
    static void session(Image& out,const GameSession& s){
        out.put("session/private",s.battle_attached,s.last_held,s.last_pause,s.recording_active,s.recording_seed,s.completed_run,
            s.completed_score,s.completed_continues,s.initial_stage,s.next_continues,s.paused_rate,s.menu_input,s.title_idle,s.demo_index,s.last_replay_file,s.source);
    }
    static void bullets(Image& out,const BulletManager& m){out.put("bullets/private",m.heads,m.tails,m.fire_depth);}
    static void shots(Image& out,const std::string& key,const ShotManager& s){out.put(key+"/all-shots",s.shots,s.converted_laser_spec);}
    static void bomb(Image& out,const std::string& key,const BombController& b){
        out.put(key+"/state",b.state,b.selection,b.last_error,b.mesh);
        DeformationRollbackTestAccess::observe(out,key+"/deformations",b.deformation_pool);
    }
    static void dialogue(Image& out,const Dialogue& d){out.put("dialogue/end",d.end);}
};
}
namespace {
void stage(Image& out,const Stage& s,const std::string& key="stage"){
    auto state=s.state;for(auto& vm:state.script_animations)vm=normalized(vm);for(auto& vm:state.effect_animations)vm=normalized(vm);
    out.put(key+"/state",state);out.vms(key+"/object-vms",s.object_animations);
    out.put(key+"/owner",s.mesh,s.background_color,s.file_id,s.active_camera,s.practice_game_frame,s.practice_replay_has_stage5,
        s.multiplayer_frozen_draw,s.multiplayer_tint_owner,s.multiplayer_tint_source);
    out.text(key+"/error",s.error);DeformationRollbackTestAccess::observe(out,key+"/deformations",s.deformation_pool);
    for(std::size_t i=0;i<s.resource.objects.size();++i){const auto& o=s.resource.objects[i];out.put(index(key+"/object",i),o.id,o.layer,o.flags,o.position,o.size);out.array(index(key+"/primitives",i),o.primitives);}
    out.array(key+"/instances",s.resource.instances);
    for(std::size_t i=0;i<s.resource.instructions.size();++i){const auto& p=s.resource.instructions[i];out.put(index(key+"/instruction",i),p.time,p.opcode,p.length,p.offset);out.array(index(key+"/arguments",i),p.arguments);}
    out.text(key+"/resource-error",s.resource.error);
}
void resource(Image& out,const std::string& key,const AnmResource& r){
    out.array(key+"/sprites",r.sprites);
    for(std::size_t i=0;i<r.scripts.size();++i){const auto& script=r.scripts[i];out.put(index(key+"/script-meta",i),script.source_id,script.file_offset);out.array(index(key+"/script-bytes",i),script.bytes);}
}
void ecl(Image& out,const std::string& key,const EclProgram& program){
    for(std::size_t i=0;i<program.files.size();++i)out.array(index(key,i),program.files[i]->bytes);
}
Image observe(GameSession& s){
    Image out;auto& b=*s.battle;
    out.put("session/value",s.state,s.game_input,s.multiplayer_active,s.multiplayer_options,s.multiplayer_frame,s.multiplayer_pause_held,
        s.recording_timestamp,s.always_hitbox,s.interactive,s.spell_records,s.clear_records,s.practice,s.multiplayer_always_hitbox,s.multiplayer_local_visibility);
    out.put("session/graph",s.battle.get(),s.title.get(),s.pause_menu.get(),s.ending.get(),s.resources.stage.get(),s.resources.previous_stage.get(),s.resources.stage_number);
    WorldRollbackTestAccess::session(out,s);economy(out,"session/economy",s.economy);out.text("session/error",s.error);
    auto& ew=static_cast<const EnemyFrameWorld&>(b);
    out.put("world/combat",ew.practice_time_lock,ew.player_state,ew.special_active,ew.spell_id,ew.player_flags,ew.spell_flags,ew.bomb_animation_flags,
        ew.special_ending,ew.target_locked,ew.target,ew.countdown_seconds,ew.countdown_hundredths,ew.spell_elapsed,ew.spell_bonus,ew.shared_spell_state,ew.camera_position);
    auto& lw=static_cast<const LaserWorld&>(b);out.put("world/laser-shot",lw.rate,lw.player,static_cast<const ShotWorld&>(b).enemies);
    out.put("world/scalars",b.practice,b.practice_replay_has_stage5,b.transition_timer,b.transitioning,b.transition_started,b.stage_active,b.result_animation,
        b.player_input,b.cancellation,b.communication,b.spell_titles,b.spell_circle,b.frame,b.initialized,b.player_live,b.game_over_requested,
        b.practice_high,b.replaying,b.demo,b.replay_entry,b.last_error,b.always_hitbox,b.hitbox_marker,b.mp_options,b.mp_enabled,
        b.mp_failed,b.mp_always_hitbox,b.mp_local_visibility,b.mp_paused,b.mp_finished,b.mp_communication_modes,b.mp_communication_hidden,b.mp_display_lives,b.mp_display_fragments,
        b.mp_fragments,b.mp_wipe_frames,b.mp_tick,b.mp_effect_seat,b.mp_graze_mask,b.mp_rank,b.mp_enemy_graze);
    out.put("world/environment",b.enemy_environment);out.put("world/commands",b.enemy_commands);
    out.put("world/popup-timing",b.popups,b.spell_timing);out.text("world/error",b.error);
    out.array("world/fades",b.fades.effects);out.array("world/shakes",b.shakes.effects);out.put("world/shake-offset",b.shakes.offset);
    out.vms("world/spell-background",b.spell_backgrounds);out.vm("world/shared-callback",b.callback_shared_visual);
    for(unsigned i=0;i<3;++i){out.vms(index("world/life-icons",i),b.mp_life_icons[i]);out.vms(index("world/communication-icons",i),b.mp_communication_icons[i]);}
    for(auto& [enemy,remainder]:b.mp_damage_remainders)out.put(index("world/remainders",uintptr_t(enemy)),remainder);
    for(std::size_t i=0;i<b.events.size();++i){const auto& e=b.events[i];out.put(index("events",i),e.kind,e.value,e.extra,e.position,e.color,e.positional);}
    for(std::size_t i=0;i<b.dialogue_text_requests.size();++i){const auto& t=b.dialogue_text_requests[i];const auto key=index("dialogue-text",i);
        out.put(key,t.animation,t.color,t.offset,t.font,t.style,t.right_aligned,t.centered);out.text(key+"/bytes",t.bytes);}
    out.put("ascii/spacing",b.ascii.spacing);
    for(std::size_t i=0;i<b.ascii.requests.size();++i){const auto& r=b.ascii.requests[i];const auto key=index("ascii",i);
        out.text(key+"/text",r.text);out.put(key+"/fields",r.position,r.style.color,r.style.scale,r.style.font,r.style.shadow,r.style.playfield,r.style.pass);}
    out.put("completion",b.completion.mode,b.completion.state);
    auto& spell=b.spells;out.put("spell",spell.timer,spell.initial_bonus,spell.timeout,spell.frame_count,spell.circle_position,spell.name,
        spell.selection,spell.stage,spell.translation_rank,spell.replay,spell.bomb_active,spell.practice_time_lock);
    auto& h=b.hud;out.vms("hud/lives",h.lives);out.vms("hud/digits",h.digits);out.vms("hud/communication",h.communication);out.vm("hud/indicator",h.indicator);
    out.put("hud/state",h.elapsed,h.previous_seconds,h.score,h.displayed_health,h.target_health,h.health,h.frame_animation,h.boss_name,h.stars,
        h.difficulty_intro,h.difficulty_label,h.bonus_digits,h.spell_notice,h.item_notice,h.spell_time_animation,h.show_spell_time,h.practice_skip_logo,h.multiplayer);
    auto& c=s.compositor;out.vms("compositor/copies",c.copies);out.put("compositor/state",c.full_camera,c.play_camera,c.world_camera,c.region,c.clear_color);
    if(b.dialogue){auto& d=*b.dialogue;out.put("dialogue/state",d.state,d.active,d.character,d.stage,d.last_opcode);out.text("dialogue/error",d.error);WorldRollbackTestAccess::dialogue(out,d);}
    const auto& bm=static_cast<const BulletManager&>(b.bullets);out.put("bullets/owner",bm.rate,bm.player,bm.default_tangent,bm.cursor,bm.active_count,bm.exclusion_squared,bm.last_error,bm.multiplayer_grazed,bm.cancel_center,bm.cancel_size);WorldRollbackTestAccess::bullets(out,bm);
    std::vector<BulletState> bullets;bullets.reserve(bm.capacity);
    for(u32 i=0;i<bm.capacity;++i){bullets.push_back(bm.at(i));bullets.back().vm=normalized(bullets.back().vm);}out.array("bullets/all-slots",bullets);
    const auto& im=static_cast<const ItemManager&>(b.items);out.put("items/owner",im.player,im.cancel_cursor,im.cancel_spawn_count,im.active_count,im.last_error,im.multiplayer_targets,im.multiplayer_current_target,im.multiplayer_spawning_transfer,im.multiplayer_fan);
    std::vector<ItemState> items;items.reserve(im.capacity);for(u32 i=0;i<im.capacity;++i){items.push_back(im.at(i));items.back().vm=normalized(items.back().vm);}out.array("items/all-slots",items);
    EnemyRollbackTestAccess::observe(out,"enemies",b.enemies);LaserRollbackTestAccess::observe(out,"lasers",b.lasers);
    AnmRollbackTestAccess::observe(out,"anm/combat",s.animations);AnmRollbackTestAccess::observe(out,"anm/presentation",b.mp_presentation);AnmRollbackTestAccess::observe(out,"anm/hitbox",b.hitbox_presentation);
    DeformationRollbackTestAccess::observe(out,"world/deformations",b.deformation_pool);stage(out,*b.stage);
    out.put("stage/graph",b.stage.get(),b.outgoing_stage.get());
    if(b.outgoing_stage)stage(out,*b.outgoing_stage,"outgoing-stage");
    for(unsigned i=0;i<b.mp_options.seat_count;++i){
        const auto& p=*b.pilots[i];const auto key=index("pilot",i);
        out.put(key+"/state",p.input,p.keys,p.ghost,p.life_latched,p.force_attract,p.life_hold,p.power_taps,p.power_gap,p.ghost_clock,p.ghost_dx,p.ghost_dy,
            p.ghost_random,p.challenge_misses,p.enemies,p.player,p.bomb,p.seat);
        economy(out,key+"/economy",p.economy);economy(out,key+"/owned-economy",p.owned_economy);
        out.put(key+"/communication",p.communication,p.owned_communication);
        const auto& f=*p.player;out.put(key+"/player",f.practice,f.state,f.input,f.spell,f.last_error);
        out.put(key+"/motion",f.motion.state,f.motion.options,f.motion.focus_animation,f.motion.last_error);out.vm(key+"/body",f.motion.body);
        const auto& shots=f.shots;out.put(key+"/shots",shots.player,shots.schedule,shots.laser_active,shots.damage_areas,shots.locked_target,shots.target_locked,shots.last_error);
        WorldRollbackTestAccess::shots(out,key,shots);WorldRollbackTestAccess::bomb(out,key+"/bomb",*p.bomb);
    }
    auto& core=s.resources.core;
    resource(out,"resource/ascii",core.ascii);resource(out,"resource/bullet",core.bullet);resource(out,"resource/enemy",core.enemy);
    resource(out,"resource/front",core.front);resource(out,"resource/text",core.text);resource(out,"resource/player0",core.players[0]);resource(out,"resource/player1",core.players[1]);
    resource(out,"resource/stage-background",s.resources.stage->background);resource(out,"resource/stage-enemies",s.resources.stage->enemies);resource(out,"resource/stage-logo",s.resources.stage->logo);
    ecl(out,"resource/ecl",s.resources.stage->timeline);ecl(out,"resource/default-ecl",core.defaults);
    return out;
}

struct RecordingGraphics final:ZunGraphics{
    touhou::graphics::PipelineState state{};std::vector<u8> commands;unsigned draws=0;
    template<class... T>void record(u32 operation,const T&... args){append(commands,operation);(append(commands,args),...);}
    void pipeline_record(){
        auto combiner=[&](const auto& c){record(9,c.operation,c.first.source,c.first.complement,c.first.alphaOnly,c.second.source,c.second.complement,c.second.alphaOnly);};
        record(8,state.depthTest,state.depthWrite,state.blend,state.alphaTest,state.fog,state.rangeFog,state.dither,state.depthCompare,state.alphaCompare,
            state.sourceBlend,state.destinationBlend,state.blendEquation,state.cull,state.colorMask,state.alphaReference,state.fogColor,state.textureFactor,
            state.fogMode,state.fogNear,state.fogFar,state.fogDensity,state.textureTransform,state.minFilter,state.magFilter,state.addressU,state.addressV);
        combiner(state.color);combiner(state.alpha);
    }
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource& file,u32 index)override{
        // Stable within this one-instance oracle; no external GPU allocation.
        return u32((uintptr_t(&file)>>2)^(0x9e3779b9u*(index+1)));
    }
    void bind_texture(u32 handle)override{record(1,handle);}
    void set_layout(touhou::graphics::VertexLayout layout)override{record(2,layout);}
    void set_matrix(touhou::graphics::MatrixKind kind,const Matrix4& matrix)override{record(3,kind,matrix);}
    void draw(touhou::graphics::Topology topology,u32 count,const void* source,u32 stride){
        ++draws;pipeline_record();record(4,topology,count,stride);
        const auto size=std::size_t(touhou::graphics::vertex_count(topology,count))*stride;
        const auto* p=static_cast<const u8*>(source);commands.insert(commands.end(),p,p+size);
    }
    void triangles(u32 count,const void* vertices,u32 stride)override{draw(touhou::graphics::Topology::Triangles,count,vertices,stride);}
    void primitives(touhou::graphics::Topology topology,u32 count,const void* vertices,u32 stride)override{draw(topology,count,vertices,stride);}
    bool select_target(const AnmResource* file,u32 index)override{record(5,file,index);return true;}
    bool clear_target(u32 color,const GraphicsViewport* viewport)override{record(6,color,bool(viewport));if(viewport)append(commands,*viewport);return true;}
    void set_viewport(const GraphicsViewport& viewport)override{record(7,viewport);}
    void reset(){commands.clear();draws=0;state={};initialize_game_pipeline();}
};
struct Peer{
    GameResources resources;GameSession session;RecordingGraphics graphics;AnmRenderer renderer{graphics};
    explicit Peer(const std::vector<u8>& data){check(resources.open_archive(data.data(),u32(data.size())),"open real retail archive");}
    void draw(){
        // Device cache is an independent host owner. Reset only its recorder /
        // caches, symmetrically, without touching any authoritative game value.
        renderer.flush();renderer.invalidate();graphics.reset();
        if(!session.draw(renderer)){
            std::fprintf(stderr,"draw failed frame=%u error=%s battle=%s\n",session.multiplayer_frame,session.error.c_str(),session.battle->error.c_str());
            check(false,"real authored session Draw");
        }
        renderer.flush();check(graphics.draws>0,"authored Draw actually emitted primitives");
    }
    void step(const std::array<MultiplayerInput,3>& inputs){
        if(!session.update_multiplayer(inputs)){
            std::fprintf(stderr,"tick failed frame=%u error=%s battle=%s\n",session.multiplayer_frame,session.error.c_str(),session.battle->error.c_str());
            check(false,"real confirmed/rollback session Update");
        }
        draw();++ticks;
    }
};
using Inputs=std::array<MultiplayerInput,3>;
using Stimulus=std::function<void(Peer&,unsigned,bool)>;
std::unique_ptr<Peer> prepared(const std::vector<u8>& data,unsigned seats,std::array<int,3> selections){
    auto peer=std::make_unique<Peer>(data);MultiplayerOptions options;options.seat_count=seats;options.selections=selections;options.seed=0x31725119;
    check(peer->session.begin_multiplayer(peer->resources,options),"begin retail multiplayer session");
    Inputs input{};for(unsigned n=0;n<125;++n)peer->step(input);
    WorldRollback adapter(peer->session);check(adapter.CanCapture(),"stable activated stage accepted after real warmup");
    return peer;
}
Netplay::RollbackJournal journal(std::size_t max_bytes=32*1024*1024,std::size_t max_blocks=65536){
    Netplay::RollbackJournal j;check(j.Reset({14,max_bytes,max_blocks,true,true}),"production bulk/coalesced world journal configured");return j;
}
struct Segment{
    std::vector<std::unique_ptr<WorldRollbackFrame>> frames;
    std::vector<Image> images;std::vector<std::vector<u8>> draws;
};
Segment execute(Peer& p,WorldRollback& adapter,Netplay::RollbackJournal& j,const std::vector<Inputs>& inputs,
                const Stimulus& stimulus={},bool wrong=false,bool save_images=true){
    Segment out;
    for(unsigned n=0;n<inputs.size();++n){
        auto frame=std::make_unique<WorldRollbackFrame>();const u32 start=p.session.multiplayer_frame;
        check(j.BeginFrame(start),"open exact pre-tick world checkpoint");
        check(adapter.BeginRollbackFrame(j,*frame),"compose all real world owners");
        if(stimulus)stimulus(p,n,wrong);
        p.step(inputs[n]);adapter.EndRollbackFrame();
        check(j.EndFrame()&&!j.Failed(),"seal complete Update and authored Draw transaction");
        peak_bytes=std::max(peak_bytes,j.BytesForFrame(start));peak_blocks=std::max(peak_blocks,j.BlocksForFrame(start));
        if(save_images){out.images.push_back(observe(p.session));out.draws.push_back(p.graphics.commands);}
        out.frames.push_back(std::move(frame));
    }
    return out;
}
void rewind(Peer& p,WorldRollback& adapter,Netplay::RollbackJournal& j,Segment& segment){
    check(!segment.frames.empty(),"rewind has target checkpoint");const auto& frame=*segment.frames.front();const auto target=frame.frame();
    check(adapter.ValidateRollbackFrame(frame,target),"validate complete resource graph before raw Undo");
    u32 actual=~0u;check(j.UndoTo(target,&actual)&&actual==target,"UndoTo crosses all later world checkpoints");
    check(adapter.RestoreRollbackFrame(frame,actual),"restore all typed owners after complete raw Undo");
    segment.frames.clear();check(!p.session.battle->stage->RollbackHistoryRetained(),"retire Stage lease after journal records and typed restore");
}
std::vector<Inputs> motion_inputs(unsigned seats,unsigned frames,bool wrong=false){
    std::vector<Inputs> out(frames);
    for(unsigned n=0;n<frames;++n)for(unsigned seat=0;seat<seats;++seat){
        auto& input=out[n][seat];input.held=wrong?(n%2?0:8):(1|(n%3==1?8:0));
        input.touch_mode=seat%2?3:2;
        input.touch_x=seat%2?(wrong?-.75f:.625f):float((wrong?-1:1)*(int(n)*3+int(seat)*13-24))+.375f;
        input.touch_y=seat%2?(wrong?.5f:-.375f):380.25f+float(n)*(wrong?1.5f:-1.25f);
    }
    return out;
}
void correction(Peer& p,const std::vector<Inputs>& actual,const std::vector<Inputs>& wrong,const Stimulus& stimulus={},const std::function<void(Peer&)>& verified={}){
    WorldRollback adapter(p.session);auto j=journal();const auto baseline=observe(p.session);
    auto reference=execute(p,adapter,j,actual,stimulus,false,true);
    if(verified)verified(p);
    rewind(p,adapter,j,reference);baseline.compare(observe(p.session),"reference -> undo baseline");
    auto predicted=execute(p,adapter,j,wrong,stimulus,true,false);
    rewind(p,adapter,j,predicted);baseline.compare(observe(p.session),"wrong prediction -> undo baseline");
    auto replay=execute(p,adapter,j,actual,stimulus,false,true);
    check(reference.images.size()==replay.images.size(),"reference/replay frame span equal");
    for(std::size_t n=0;n<reference.images.size();++n){
        reference.images[n].compare(replay.images[n],"actual corrected full owner replay");
        if(reference.draws[n]!=replay.draws[n]){std::fprintf(stderr,"draw trace mismatch relative frame=%zu\n",n);check(false,"complete Draw command/vertex stream equality");}
        ++checks;
    }
    if(verified)verified(p);
    rewind(p,adapter,j,replay);baseline.compare(observe(p.session),"actual replay -> undo baseline");
    ++cases;
}
void movement_cases(const std::vector<u8>& bytes){
    for(unsigned count:{2u,3u}){
        auto p=prepared(bytes,count,{0,3,5});
        correction(*p,motion_inputs(count,8),motion_inputs(count,8,true));
    }
    std::puts("PASS world: 2P/3P retail Update+Draw, touch target/analog, shooting, 8-frame reference/wrong/corrected oracle");
}
void bomb_cases(const std::vector<u8>& bytes){
    for(unsigned group=0;group<2;++group){
        const std::array<int,3> selections=group?std::array<int,3>{3,4,5}:std::array<int,3>{0,1,2};
        auto p=prepared(bytes,3,selections);auto& b=*p->session.battle;
        for(auto& pilot:b.pilots){pilot->economy.power=pilot->economy.max_power;check(pilot->power_changed(),"real option rebuild at full Bomb power");pilot->player->state.invincibility.set(0,&b.animations.rate);}
        p->step(Inputs{});
        auto actual=motion_inputs(3,8),wrong=motion_inputs(3,8,true);
        for(unsigned seat=0;seat<3;++seat)actual[0][seat].held=2;
        bool shield_seen=false;
        Stimulus hook=[&](Peer& peer,unsigned n,bool wrong_branch){
            auto& world=*peer.session.battle;
            if(n==1&&!wrong_branch)for(unsigned seat=0;seat<3;++seat)check(world.pilots[seat]->bomb->state.active,"each of six actual selected Bombs started");
            if(group==1&&n==2&&!wrong_branch){
                auto& shield=*world.pilots[2];check(shield.bomb->state.active&&(shield.player->motion.state.flags&4),"real unused Nitori shield exists");
                check(world.mp_hit(2,{CollisionKind::Hit,true}),"actual MP collision dispatch hits Nitori shield");shield_seen=true;
            }
        };
        correction(*p,actual,wrong,hook,[&](Peer& peer){
            if(group==1)check(shield_seen&&peer.session.battle->pilots[2]->bomb->state.secondary.current>0,"Nitori hit shield secondary timer executed in full battle update");
        });
    }
    std::puts("PASS world: all six retail Bombs, simultaneous 3P, distortion/ring geometry, real shield hit and shared counters");
}
void put(PlayerFrame& p,float x,float y){p.motion.state.x=truncate_int(double(x)*128);p.motion.state.y=truncate_int(double(y)*128);p.motion.state.position={x,y,0};}
void rescue_case(const std::vector<u8>& bytes){
    auto p=prepared(bytes,2,{0,3,5});auto& world=*p->session.battle;auto& donor=*world.pilots[0];auto& ghost=*world.pilots[1];
    ghost.economy.lives=-1;check(ghost.game_over(false),"real ghost transition before rollback window");
    donor.economy.lives=3;donor.life_hold=86;donor.life_latched=false;
    put(*donor.player,0,400);put(*ghost.player,10,400);ghost.ghost_clock=1;ghost.ghost_dx=ghost.ghost_dy=0;
    donor.player->state.invincibility.set(100000,&world.animations.rate);
    std::vector<Inputs> actual(8),wrong(8);
    for(auto& input:actual){input[0].held=8;input[0].touch_mode=2;input[0].touch_y=400;}
    for(auto& input:wrong){input[0].held=1;input[0].touch_mode=3;input[0].touch_x=-1;}
    correction(*p,actual,wrong,{},[](Peer& peer){
        auto& b=*peer.session.battle;check(!b.pilots[1]->ghost&&b.pilots[0]->economy.lives==2,"native life-hold rescue crosses threshold in rollback window");
        check(b.pilots[1]->player->state.invincibility.current>270,"rescued player's native invincibility survives correction");
    });
    std::puts("PASS world: ghost state and actual life-hold rescue threshold through incorrect prediction");
}
void barrier_cases(const std::vector<u8>& bytes){
    {
        auto p=prepared(bytes,2,{0,3,5});WorldRollback adapter(p->session);auto j=journal();const auto baseline=observe(p->session);
        Segment segment;segment.frames.push_back(std::make_unique<WorldRollbackFrame>());const auto n=p->session.multiplayer_frame;
        check(j.BeginFrame(n)&&adapter.BeginRollbackFrame(j,*segment.frames[0]),"pause pre-action checkpoint");
        auto* battle=p->session.battle.get();auto* stage=battle->stage.get();auto* resource=p->session.resources.stage.get();
        Inputs input{};input[1].pause=true;
        check(!p->session.update_multiplayer(input)&&adapter.BoundaryRequested(),"pause requests barrier before constructing menu");
        check(!p->session.pause_menu&&p->session.state.phase==GameSessionPhase::stage&&p->session.battle.get()==battle&&battle->stage.get()==stage&&p->session.resources.stage.get()==resource,"pause barrier preserves resource graph");
        adapter.EndRollbackFrame();check(j.EndFrame(),"seal undoable partial pause input handling");rewind(*p,adapter,j,segment);baseline.compare(observe(p->session),"pause barrier undo");
        p->step(input);check(p->session.state.phase==GameSessionPhase::paused&&p->session.pause_menu&&!adapter.CanCapture(),"same pause input executes in P0 confirmed mode");++cases;
    }
    {
        auto p=prepared(bytes,2,{0,3,5});WorldRollback adapter(p->session);auto j=journal();const auto baseline=observe(p->session);
        Segment segment;segment.frames.push_back(std::make_unique<WorldRollbackFrame>());const auto n=p->session.multiplayer_frame;
        check(j.BeginFrame(n)&&adapter.BeginRollbackFrame(j,*segment.frames[0]),"stage resource pre-action checkpoint");
        auto* stage=p->session.battle->stage.get();auto* resource=p->session.resources.stage.get();
        check(!p->session.battle->next_stage(p->resources,2)&&adapter.BoundaryRequested(),"resource transition requests barrier before load/move/delete");
        check(p->session.battle->stage.get()==stage&&p->session.resources.stage.get()==resource&&!p->session.resources.previous_stage&&!p->session.battle->outgoing_stage,"resource barrier leaves original owners intact");
        adapter.EndRollbackFrame();check(j.EndFrame(),"seal undoable resource barrier");rewind(*p,adapter,j,segment);baseline.compare(observe(p->session),"resource barrier undo");
        check(p->session.battle->next_stage(p->resources,2),"confirmed P0 resource transition executes after leases retire");
        check(p->session.battle->stage.get()!=stage&&p->session.resources.previous_stage.get()==resource&&adapter.CanCapture(),"stable stage overlap supports rollback");
        ++cases;
    }
    std::puts("PASS world: Pause and stage-resource pre-action barriers, rollback of partial frame, then same confirmed P0 operation");
}
void transition_cases(const std::vector<u8>& bytes){
    for(unsigned seats:{2u,3u}){
        auto p=prepared(bytes,seats,{0,3,5});auto& s=p->session;auto& b=*s.battle;
        for(unsigned target=2;target<=6;++target){
        check(b.completion.complete(),"real stage-clear initializes result timer");
        p->step({});check(b.transitioning&&b.outgoing_stage&&s.state.stage==target,"real next-stage resource handoff");
        WorldRollback adapter(s);auto j=journal();unsigned captured=0,barriers=0;bool wait_checked=false,fade_checked=false;
        for(unsigned tick=0;tick<260&&b.transitioning;++tick){
            if(!wait_checked&&!b.transition_started){
                correction(*p,motion_inputs(seats,8),motion_inputs(seats,8,true));wait_checked=true;
            }
            if(!fade_checked&&b.transition_started&&!b.stage_active&&b.transition_timer.current<15){
                correction(*p,motion_inputs(seats,8),motion_inputs(seats,8,true));fade_checked=true;
            }
            check(adapter.CanCapture(),"transition resumes prediction between structural fences");
            const auto baseline=observe(s);Segment segment;segment.frames.push_back(std::make_unique<WorldRollbackFrame>());
            const auto n=s.multiplayer_frame;check(j.BeginFrame(n)&&adapter.BeginRollbackFrame(j,*segment.frames[0]),"capture both transition stages");
            const bool updated=s.update_multiplayer({});
            if(updated)p->draw();
            else check(adapter.BoundaryRequested(),"transition only stops for an explicit lifecycle fence");
            adapter.EndRollbackFrame();check(j.EndFrame(),"seal transition tick");
            if(updated){++captured;j.DiscardBefore(n+1);segment.frames.clear();}
            else{
                ++barriers;rewind(*p,adapter,j,segment);baseline.compare(observe(s),"partial transition boundary undo");
                p->step({});
            }
        }
        check(!b.transitioning&&b.stage_active&&!b.outgoing_stage&&!s.resources.previous_stage,"transition retires both old owners safely");
        check(wait_checked&&fade_checked&&captured>=140&&barriers<=4,"result and fade intervals predict instead of waiting for every input");
        ++cases;
        std::printf("PASS transition %uP -> stage %u: %u rollback-capable ticks, %u exact lifecycle fences\n",seats,target,captured,barriers);
        }
    }
}
void failure_and_observer_cases(const std::vector<u8>& bytes){
    auto p=prepared(bytes,2,{0,3,5});WorldRollback adapter(p->session);
    const auto baseline=observe(p->session);auto tiny=journal(64,1);WorldRollbackFrame frame;
    check(tiny.BeginFrame(1000)&&!adapter.BeginRollbackFrame(tiny,frame),"world capacity failure rejects Begin");
    check(tiny.Failed()&&!adapter.RollbackFrameActive()&&!p->session.animations.RollbackFrameActive()&&!p->session.battle->enemies.RollbackFrameActive()
        &&!p->session.battle->pilots[0]->bomb->RollbackFrameActive(),"failed Begin detaches every installed observer");
    check(!tiny.EndFrame(),"incomplete world checkpoint cannot seal");tiny.Clear();baseline.compare(observe(p->session),"failed world Begin makes no game changes");
    {
        auto foreign=journal();DeformationRollbackFrame bomb_frame;auto& bomb=*p->session.battle->pilots[0]->bomb;
        check(foreign.BeginFrame(2000)&&bomb.BeginRollbackFrame(foreign,bomb_frame),"external Bomb observer attached");
        auto test=journal();WorldRollbackFrame rejected;check(test.BeginFrame(2000)&&!adapter.BeginRollbackFrame(test,rejected),"world preflight rejects already observed Bomb");
        check(bomb.RollbackFrameActive(),"rejected world Begin does not detach external Bomb observer");
        bomb.EndRollbackFrame();check(foreign.EndFrame()&&test.EndFrame(),"foreign/empty journals finish");foreign.Clear();test.Clear();
    }
    {
        auto j=journal();auto segment=execute(*p,adapter,j,motion_inputs(2,1),{},false,false);
        auto foreign=journal();AnmRollbackFrame anm;check(foreign.BeginFrame(3000)&&p->session.animations.BeginRollbackFrame(foreign,anm),"external ANM observer after world End");
        check(!adapter.ValidateRollbackFrame(*segment.frames[0],segment.frames[0]->frame()),"pre-Undo Validate rejects external ANM observer");
        p->session.animations.EndRollbackFrame();check(foreign.EndFrame(),"detach and close external observer");foreign.Clear();
        rewind(*p,adapter,j,segment);baseline.compare(observe(p->session),"observer validation path undo");
    }
    ++cases;std::puts("PASS world: capacity failure gate and foreign observer preflight/Validate protection");
}
}
void contiguous_capture_cases(){
    struct Values {u32 a,b;std::string owner;u32 c,d;};
    Values x{1,2,"owned container is never copied",3,4};
    auto j=journal();check(j.BeginFrame(0),"contiguous fixture Begin");
    check(rollback_capture_contiguous_values(j,x.a,x.b,x.c,x.d),"adjacent fields register without their intervening owner");
    check(j.BytesForFrame(0)==16&&j.BlocksForFrame(0)==2,"only four field payloads, in two exact runs");
    x.a=9;x.b=10;x.c=11;x.d=12;x.owner="still owned after undo";
    check(j.EndFrame()&&j.UndoTo(0),"contiguous exact undo");
    check(x.a==1&&x.b==2&&x.c==3&&x.d==4&&x.owner=="still owned after undo","payload restores without touching container header");
    check(j.BeginFrame(1)&&rollback_capture_contiguous_values(j,x.b,x.a,x.b),"out of order and duplicate exact fields retain original journal checks");
    check(j.EndFrame()&&j.UndoTo(1),"duplicate exact payload undo");
    Netplay::RollbackJournal tiny;check(tiny.Reset({2,8,4,false,false})&&tiny.BeginFrame(0),"small capture budget");
    check(!rollback_capture_contiguous_values(tiny,x.a,x.b,x.c,x.d)&&tiny.Failed()&&!tiny.EndFrame(),"coalescing cannot bypass capacity failure");
    ++cases;
}
int main(int argc,char** argv){
    check(argc==2,"pass existing retail archive path");
    std::ifstream file(argv[1],std::ios::binary);check(bool(file),"retail archive available");
    const std::vector<u8> bytes((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    contiguous_capture_cases();movement_cases(bytes);bomb_cases(bytes);rescue_case(bytes);barrier_cases(bytes);transition_cases(bytes);failure_and_observer_cases(bytes);
    std::printf("TH11 retail world rollback: %u cases, %u checks, %u real Update+Draw ticks PASS; peak journal %zu bytes / %zu blocks\n",
        cases,checks,ticks,peak_bytes,peak_blocks);
    std::puts("SCOPE: stable stage real owners and authored CPU Draw command/geometry oracle. No host FontDevice textures, GPU pixels, Application/netplay, browser or performance acceptance.");
    return 0;
}
