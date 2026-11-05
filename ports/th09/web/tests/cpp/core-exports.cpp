#include "../../cpp/game/Rng.hpp"
#include "ending-fixture.hpp"
#include "../../cpp/game/Timer.hpp"
#include "../../cpp/game/InputFrame.hpp"
#include "../../cpp/game/Archive.hpp"
#include "../../cpp/game/ResourceCrypt.hpp"
#include "../../cpp/game/ScoreFile.hpp"
#include "../../cpp/game/ReplayFile.hpp"
#include "../../cpp/game/ReplaySession.hpp"
#include "../../cpp/game/PlayerMotion.hpp"
#include "../../cpp/game/PlayerCollision.hpp"
#include "../../cpp/game/ChargeGauge.hpp"
#include "../../cpp/game/GameMath.hpp"
#include "../../cpp/game/BulletEmission.hpp"
#include "../../cpp/game/CharacterAttackMeter.hpp"
#include "../../cpp/game/CharacterCapture.hpp"
#include "../../cpp/game/PlayerHazards.hpp"
#include <cmath>
#include <cstdlib>
#include "anm-fixture.hpp"
#include "ecl-fixture.hpp"
#include "ecl-special-fixture.hpp"
#include "bullet-fixture.hpp"
#include "laser-fixture.hpp"
#include "timeline-fixture.hpp"
#include "enemy-manager-fixture.hpp"
#include "enemy-frame-fixture.hpp"
#include "attack-queue-fixture.hpp"
#include "attack-areas-fixture.hpp"
#include "player-shots-fixture.hpp"
#include "shot-control-fixture.hpp"
#include "player-items-fixture.hpp"
#include "player-life-fixture.hpp"
#include "cpu-player-fixture.hpp"
#include "player-frame-fixture.hpp"
#include "battle-fixture.hpp"
#include "hud-fixture.hpp"
#include "world-fixture.hpp"
#include "title-fixture.hpp"
#include "screen-effects-fixture.hpp"
#include "texture-image-fixture.hpp"
#include "effects-fixture.hpp"
#include "match-rules-fixture.hpp"
#include "attack-controller-fixture.hpp"
#include "dialogue-fixture.hpp"
#include "stage-selection-fixture.hpp"
#include "match-scene-fixture.hpp"
#include "background-fixture.hpp"
#include "renderer-fixture.hpp"
#include "stage-renderer-fixture.hpp"
#include "background-draw-fixture.hpp"
#include "game-resources-fixture.hpp"
#include "bullet-draw-fixture.hpp"
#include "enemy-draw-fixture.hpp"
#include "../../cpp/game/Camera.hpp"
#include "enemy-death-fixture.hpp"
#include "combo-fixture.hpp"
using namespace th09;
#define API(name) extern "C" __attribute__((export_name(#name)))
API(allocate) void* allocate(u32 n){return std::calloc(n?n:1,1);}
API(release) void release(void* p){std::free(p);}
API(rng_word) u32 rng_word(Rng* p,u32 kind,u32 max){return kind==0?p->next16():kind==1?p->next32():p->below(u16(max));}
API(rng_float) float rng_float(Rng* p,u32 kind){return float(kind?p->signed_unit():p->unit());}
API(timer_reset) void timer_reset(Timer* p,i32 value){p->reset(value);}
API(timer_advance) void timer_advance(Timer* p,float amount,float rate,u32 flags){p->advance(amount,rate,flags);}
API(timer_tick) void timer_tick(Timer* p,float rate){p->tick({rate,false});}
API(input_update) void input_update(InputFrame* p,u32 keys){p->update(u16(keys));}
API(codec_create) Lzss* codec_create(){return new Lzss;}
API(codec_delete) void codec_delete(Lzss* p){delete p;}
API(codec_dictionary) u8* codec_dictionary(Lzss* p){return p->dictionary;}
API(codec_decode) i32 codec_decode(Lzss* p,const u8* in,u32 size,u8* out,u32 capacity){u32 written=0;return p->decode(in,size,out,capacity,written)?i32(written):-1;}
API(codec_encode) i32 codec_encode(Lzss* p,const u8* in,u32 size,u8* out,u32 capacity){auto b=p->encode(in,size);if(b.size()>capacity)return -1;std::memcpy(out,b.data(),b.size());return b.size();}
API(crypt) i32 crypt(const u8* in,u8* out,u32 size,u32 key,u32 step,u32 block,u32 limit,u32 encrypt){return resource_crypt(in,out,size,{u8(key),u8(step),block,limit},encrypt);}
API(archive_create) Archive* archive_create(){return new Archive;}
API(archive_delete) void archive_delete(Archive* p){delete p;}
API(archive_open) i32 archive_open(Archive* p,const u8* data,u32 size){return p->open(data,size);}
API(archive_count) u32 archive_count(Archive* p){return p->contents().size();}
API(archive_name) const char* archive_name(Archive* p,u32 index){return index<p->contents().size()?p->contents()[index].name.c_str():nullptr;}
API(archive_field) u32 archive_field(Archive* p,u32 index,u32 field){if(index>=p->contents().size())return 0;const auto& e=p->contents()[index];return field==0?e.offset:field==1?e.size:e.compressed;}
API(archive_read) i32 archive_read(Archive* p,const char* name,u8* out,u32 capacity,u32 decrypt){std::vector<u8>b;if(!p->read(name,b,decrypt)||b.size()>capacity)return -1;std::memcpy(out,b.data(),b.size());return b.size();}
API(score_create) ScoreFile* score_create(){return new ScoreFile;}
API(score_delete) void score_delete(ScoreFile* p){delete p;}
API(score_decode) i32 score_decode(ScoreFile* p,const u8* data,u32 size){return p->decode(data,size);}
API(score_data) const u8* score_data(ScoreFile* p){return p->data().data();}
API(score_size) u32 score_size(ScoreFile* p){return p->data().size();}
API(score_encode) i32 score_encode(ScoreFile* p,Rng* rng,u8* out,u32 capacity){auto b=p->encode(*rng);if(b.empty()||b.size()>capacity)return -1;std::memcpy(out,b.data(),b.size());return b.size();}
API(replay_create) ReplayFile* replay_create(){return new ReplayFile;}
API(replay_delete) void replay_delete(ReplayFile* p){delete p;}
API(replay_decode) i32 replay_decode(ReplayFile* p,const u8* data,u32 size){return p->decode(data,size);}
API(replay_data) const u8* replay_data(ReplayFile* p){return p->data().data();}
API(replay_size) u32 replay_size(ReplayFile* p){return p->data().size();}
API(replay_offset) u32 replay_offset(ReplayFile* p,u32 group,u32 round){return p->stream_offset(group,round);}
API(replay_encode) i32 replay_encode(ReplayFile* p,u8* out,u32 capacity){auto b=p->encode();if(b.empty()||b.size()>capacity)return -1;std::memcpy(out,b.data(),b.size());return b.size();}
API(game_input) void game_input(GameInput* p,u32 keys,u32 focus){p->advance(u16(keys));p->update_auto_focus(focus);}
API(replay_play_create) ReplayPlayback* replay_play_create(){return new ReplayPlayback;}
API(replay_play_delete) void replay_play_delete(ReplayPlayback* p){delete p;}
API(replay_play_begin) u32 replay_play_begin(ReplayPlayback* p,ReplayFile* file,u32 round,ReplayRoundSettings* settings){return p->begin(*file,round,*settings);}
API(replay_play_step) u32 replay_play_step(ReplayPlayback* p,u32 flags,GameInput (*inputs)[3],u32 focus){const bool modes[2]={bool(focus&1),bool(focus&2)};return u32(p->advance(flags,*inputs,modes));}
API(replay_play_field) i32 replay_play_field(ReplayPlayback* p,u32 which){return which==0?p->frame():which==1?p->length():which==2?p->sample_cursor():which==3?p->frame_rate:p->slowdown;}
API(replay_play_after) u32 replay_play_after(ReplayPlayback* p,u32 flags,i32 dialogue){return p->after_update(flags,dialogue);}
API(input_capture) void input_capture(InputCapture* p,GameInput (*inputs)[3],u32 focus,Rng* rng,i32* event){const bool modes[2]={bool(focus&1),bool(focus&2)};p->sample(*inputs,modes,*rng,*event);}
API(record_create) ReplayRecording* record_create(){auto p=new ReplayRecording;p->begin();return p;}
API(record_delete) void record_delete(ReplayRecording* p){delete p;}
API(record_step) u32 record_step(ReplayRecording* p,u32 flags,u32 paused,GameInput (*inputs)[3],u32 cpu,u32 rate){const bool modes[2]={bool(cpu&1),bool(cpu&2)};return p->advance(flags,paused,*inputs,modes,u8(rate));}
API(record_field) u32 record_field(ReplayRecording* p,u32 which){return which==0?p->frame():which==1?p->ending_frames():p->chunks().size();}
API(record_stream) i32 record_stream(ReplayRecording* p,u32 group,u8* out,u32 capacity){auto b=p->stream(group);if(b.size()>capacity)return -1;std::memcpy(out,b.data(),b.size());return b.size();}
struct MotionField{u32 original,offset,size;};
#define MOTION_FIELD(original,name) {original,offsetof(PlayerMotion,name),sizeof(PlayerMotion::name)}
static const MotionField motion_fields[]={
    MOTION_FIELD(0xa8,health),MOTION_FIELD(8,player),MOTION_FIELD(0x1b80,flags),MOTION_FIELD(0x30358,direction),
    MOTION_FIELD(0x1b88,position),MOTION_FIELD(0x1ca8,hit_extent),MOTION_FIELD(0x1cb4,graze_extent),MOTION_FIELD(0x1cc0,item_extent),
    MOTION_FIELD(0x1cdc,base_scale),MOTION_FIELD(0x1ce4,effect_scale),MOTION_FIELD(0x3035c,velocity),MOTION_FIELD(0x1ccc,step),MOTION_FIELD(0x1cd8,angle),
    MOTION_FIELD(0x1c60,hit_bounds),MOTION_FIELD(0x1c78,graze_bounds),MOTION_FIELD(0x1c90,item_bounds),MOTION_FIELD(0x1ba0,history)};
API(motion_fields) const MotionField* motion_fields_ptr(){return motion_fields;}
API(motion_field_count) u32 motion_field_count(){return sizeof(motion_fields)/sizeof(MotionField);}
API(motion_create) PlayerMotion* motion_create(){return new PlayerMotion;}
API(motion_delete) void motion_delete(PlayerMotion* p){delete p;}
static std::vector<u32> motion_events;
struct TestMotionEffects:MotionEffects {
    u32 options=0;
    void begin_focus(const Vec3&,u32 player,u32 character)override{
        static const u32 types[]={10,16,17,18,26,24,27,25,28,29,30,31,32,33,36,37};
        motion_events.insert(motion_events.end(),{1,7,player,1,types[character%16],player+2});
    }
    void end_focus()override{motion_events.insert(motion_events.end(),{2,0,0});}
    void animation_interrupt(u32 label)override{motion_events.insert(motion_events.end(),{3,label,0});}
    void update_options()override{for(u32 i=0;i<4;++i)if(options&(1u<<i))motion_events.insert(motion_events.end(),{4,i,0});}
};
API(motion_step) void motion_step(PlayerMotion* p,u32 keys,u32 auto_focus,u32 fire_frames,u32 character,const MovementSpeeds* speeds,const PlayfieldLimits* limits,float rate,u32 options){
    p->character=character;motion_events.clear();TestMotionEffects effects;effects.options=options;p->update(u16(keys),auto_focus,u16(fire_frames),*speeds,*limits,rate,effects);
}
API(motion_events_data) const u32* motion_events_data(){return motion_events.data();}
API(motion_events_size) u32 motion_events_size(){return motion_events.size();}
API(charge_add) i32 charge_add(ChargeGauge* p,float amount,u32 character){return p->add(amount,character);}
API(collision_box) i32 collision_box(const Bounds* p,const Vec2* c,const Vec2* e){return intersects_box(*p,*c,*e);}
API(collision_circle) i32 collision_circle(const Vec2* p,float r,const Vec2* c,float s){return intersects_circle(*p,r,*c,s);}
API(collision_item) i32 collision_item(i32 life,const Bounds* p,const Vec2* c,const Vec2* e){return collects_item(life,*p,*c,*e);}
API(angle_add) float angle_add(float a,float d){return float(add_angle(a,d));}
API(vector_rotate) void vector_rotate(Vec2* out,const Vec2* in,float angle){rotate(*out,*in,angle);}
API(curve_hermite) float curve_hermite(float a,float b,float ta,float tb,float t){return float(hermite(a,b,ta,tb,t));}
API(collision_rotated) u32 collision_rotated(const Vec2* player,const Vec2* hit,const Vec2* center,const Vec2* extent,const Vec2* pivot,float angle,u32 near){return intersects_rotated_box(*player,*hit,*center,*extent,*pivot,angle,near);}

API(anm_create) anm_test::Fixture* anm_create(){return new anm_test::Fixture();}
API(anm_delete) void anm_delete(anm_test::Fixture* f){delete f;}
API(anm_start) void anm_start(anm_test::Fixture* f,const u8* script,u32 size,float rate,u32 flags){f->start(script,size,rate,flags);}
API(anm_step) u32 anm_step(anm_test::Fixture* f,i32 interrupt){if(interrupt)f->vm.pendingInterrupt=i16(interrupt);return f->executor.execute(f->vm);}
API(anm_vm) AnmVm* anm_vm(anm_test::Fixture* f){return &f->vm;}
API(anm_file) AnmLoaded* anm_file(anm_test::Fixture* f){return &f->file;}
API(anm_script) const u8* anm_script(anm_test::Fixture* f){return f->script.data();}
API(anm_sprites) const AnmLoadedSprite* anm_sprites(anm_test::Fixture* f){return f->file.sprites;}
API(anm_rng) Rng* anm_rng(anm_test::Fixture* f){return &f->rng;}
API(anm_count) u32 anm_count(anm_test::Fixture* f){return f->executor.executed;}
API(anm_invalid) u32 anm_invalid(anm_test::Fixture* f){return f->executor.invalid;}
API(anm_load) u32 anm_load(anm_test::Fixture* f,const u8* bytes,u32 size){return f->load(bytes,size);}
API(anm_raw) const u8* anm_raw(anm_test::Fixture* f){return f->resource.data().data();}
API(anm_script_count) u32 anm_script_count(anm_test::Fixture* f){return f->resource.script_count();}
API(anm_sprite_count) u32 anm_sprite_count(anm_test::Fixture* f){return f->file.spriteCount;}
API(anm_script_offset) u32 anm_script_offset(anm_test::Fixture* f,u32 index){return index<f->resource.script_count()?u32(reinterpret_cast<u8*>(f->file.scripts[index])-f->resource.data().data()):0;}
API(anm_start_index) u32 anm_start_index(anm_test::Fixture* f,u32 index,float rate){return f->start_index(index,rate);}

API(ecl_vars_create) ecl_test::Fixture* ecl_vars_create(){return new ecl_test::Fixture;}
API(ecl_vars_delete) void ecl_vars_delete(ecl_test::Fixture* p){delete p;}
API(ecl_var_fields) const ecl_test::Field* ecl_var_fields(){return ecl_test::fields;}
API(ecl_var_field_count) u32 ecl_var_field_count(){return sizeof(ecl_test::fields)/sizeof(ecl_test::Field);}
API(ecl_read_int) i32 ecl_read_int(ecl_test::Fixture* f,i32 id){return f->variables.read_int(id);}
API(ecl_read_float) float ecl_read_float(ecl_test::Fixture* f,float id){return f->variables.read_float(id);}
API(ecl_write_target) void* ecl_write_target(ecl_test::Fixture* f,i32 id,u32 real){return real?static_cast<void*>(f->variables.float_field(id)):static_cast<void*>(f->variables.integer_field(id));}
API(ecl_program_create) EclProgram* ecl_program_create(){return new EclProgram;}
API(ecl_program_delete) void ecl_program_delete(EclProgram* p){delete p;}
API(ecl_program_load) u32 ecl_program_load(EclProgram* p,const u8* data,u32 size){return p->load(data,size);}
API(ecl_program_data) const u8* ecl_program_data(EclProgram* p){return p->data();}
API(ecl_program_count) u32 ecl_program_count(EclProgram* p,u32 timeline){return timeline?p->timeline_count():p->sub_count();}
API(ecl_program_offset) i32 ecl_program_offset(EclProgram* p,u32 index,u32 timeline){const void* start=timeline?static_cast<void*>(p->timeline(index)):static_cast<void*>(p->sub(index));return start?i32(static_cast<const u8*>(start)-p->data()):-1;}
API(ecl_vm_create) ecl_test::VmFixture* ecl_vm_create(){return new ecl_test::VmFixture;}
API(ecl_vm_delete) void ecl_vm_delete(ecl_test::VmFixture* f){delete f;}
API(ecl_vm_load) u32 ecl_vm_load(ecl_test::VmFixture* f,const u8* bytes,u32 size,i32 sub){if(!f->program.load(bytes,size))return false;return EclExecutor({},1,1).start(f->vm,f->program,sub);}
API(ecl_vm_field) u8* ecl_vm_field(ecl_test::VmFixture* f,u32 index){return f->field_pointer(index);}
API(ecl_vm_data) const u8* ecl_vm_data(ecl_test::VmFixture* f){return f->program.data();}
API(ecl_vm_step) i32 ecl_vm_step(ecl_test::VmFixture* f,float rate,float step,u32 mask,u32 flags,u32 motion,u32 emission,u32 animation){
    f->emissions.events.clear();f->emissions.laser_events.clear();f->emissions.animation_events.clear();bool ok=EclExecutor({rate,bool(flags&32)},step,mask,{nullptr,nullptr,&f->emissions,&f->emissions,&f->emissions}).step(f->vm);
    if(ok&&motion)f->vm.movement.update_velocity(f->vm,{rate,bool(flags&32)},step);
    if(ok&&emission)ok=f->vm.emitter.update(f->vm,{rate,bool(flags&32)},f->emissions);
    if(ok&&animation)ok=f->vm.animation.update_pose(f->vm,f->emissions);
    return f->vm.invalid?-2:f->vm.finished?-1:ok?0:-3;
}
API(ecl_vm_emissions) const BulletEmission* ecl_vm_emissions(ecl_test::VmFixture* f){return f->emissions.events.data();}
API(ecl_vm_emission_count) u32 ecl_vm_emission_count(ecl_test::VmFixture* f){return f->emissions.events.size();}
API(ecl_vm_laser_events) const BulletEmission* ecl_vm_laser_events(ecl_test::VmFixture* f){return f->emissions.laser_events.data();}
API(ecl_vm_laser_event_count) u32 ecl_vm_laser_event_count(ecl_test::VmFixture* f){return f->emissions.laser_events.size();}
API(ecl_vm_laser) Laser* ecl_vm_laser(ecl_test::VmFixture* f,u32 index){return f->vm.lasers.references[index%32];}
API(ecl_vm_animation_events) const void* ecl_vm_animation_events(ecl_test::VmFixture* f){return f->emissions.animation_events.data();}
API(ecl_vm_animation_event_count) u32 ecl_vm_animation_event_count(ecl_test::VmFixture* f){return f->emissions.animation_events.size();}
API(ecl_vm_context) void* ecl_vm_context(ecl_test::VmFixture* f,i32 slot,u32 part){auto c=f->context(slot);if(!c)return nullptr;return part==0?static_cast<void*>(&c->locals):part==1?static_cast<void*>(&c->timer):part==2?static_cast<void*>(&c->wait):static_cast<void*>(&c->interpolations);}
API(ecl_vm_meta) i32 ecl_vm_meta(ecl_test::VmFixture* f,i32 slot,u32 part){auto c=f->context(slot);if(!c)return -999;
    return part==0?c->subroutine:part==1?i32(reinterpret_cast<const u8*>(c->instruction)-f->program.data()):part==2?c->depth:part==3?c->native_callback:part==4?f->vm.failed_opcode:c->returned;}
API(ecl_vm_motion) Vec3* ecl_vm_motion(ecl_test::VmFixture* f,u32 part){return part?&f->vm.velocity:&f->vm.position_offset;}
API(ecl_vm_set_flags) void ecl_vm_set_flags(ecl_test::VmFixture* f,u32 flags,u32 difficulty){f->vm.behavior_flags=flags;f->vm.difficulty_flags=difficulty;}
API(ecl_vm_pending) void ecl_vm_pending(ecl_test::VmFixture* f,i32 label){f->vm.pending_interrupt=i16(label);}
API(ecl_vm_extras) const ecl_test::ExtraField* ecl_vm_extras(){return ecl_test::extra_fields;}
API(ecl_vm_extra_count) u32 ecl_vm_extra_count(){return sizeof(ecl_test::extra_fields)/sizeof(ecl_test::ExtraField);}
API(ecl_vm_base) EclVm* ecl_vm_base(ecl_test::VmFixture* f){return &f->vm;}
API(bullet_pattern_values) void bullet_pattern_values(const BulletEmission* e,i32 index,i32 layer,float aim,float rate,Rng* rng,float* out){
    const auto p=bullet_pattern(*e,index,layer,aim,*rng);out[0]=float(add_angle(p.angle,0));out[1]=p.speed;
    const float speed=rate*p.speed;out[2]=float(std::cos(double(p.angle))*double(speed));out[3]=float(std::sin(double(p.angle))*double(speed));out[4]=0;
}
API(special_create) special_test::Fixture* special_create(){return new special_test::Fixture;}
API(special_delete) void special_delete(special_test::Fixture* f){delete f;}
API(special_part) void* special_part(special_test::Fixture* f,u32 part){return part==0?static_cast<void*>(&f->vm.values.position):part==1?static_cast<void*>(&f->vm.primary.locals):static_cast<void*>(&f->world.random);}
API(special_field) EclPlayfieldState* special_field(special_test::Fixture* f,u32 side){return &f->fields[side&1];}
API(special_set_flags) void special_set_flags(special_test::Fixture* f,u32 side,u32 flags){f->fields[side&1].flags=flags;}
API(special_flags) u32 special_flags(special_test::Fixture* f,u32 side){return f->fields[side&1].flags;}
API(special_run) u32 special_run(special_test::Fixture* f,i32 callback,u32 side,const EclInstruction* i){f->events.clear();f->vm.values.field=&f->fields[side&1];f->vm.values.opponent=&f->fields[(side&1)^1];return EclSpecial(*f).execute(f->vm,callback,*i);}
API(special_events) const special_test::Event* special_events(special_test::Fixture* f){return f->events.data();}
API(special_event_count) u32 special_event_count(special_test::Fixture* f){return f->events.size();}
API(special_bullet) Bullet* special_bullet(special_test::Fixture* f,u32 side,u32 index){return &f->pool[side&1][index%536];}
API(special_bullet_fields) const special_test::Field* special_bullet_fields(){return special_test::fields;}
API(special_bullet_field_count) u32 special_bullet_field_count(){return sizeof(special_test::fields)/sizeof(special_test::Field);}
API(bullet_create) bullet_test::Fixture* bullet_create(){return new bullet_test::Fixture;}
API(bullet_delete) void bullet_delete(bullet_test::Fixture* f){delete f;}
API(bullet_state) Bullet* bullet_state(bullet_test::Fixture* f){return &f->bullet;}
API(bullet_fields) const bullet_test::Field* bullet_fields(){return bullet_test::fields;}
API(bullet_field_count) u32 bullet_field_count(){return sizeof(bullet_test::fields)/sizeof(bullet_test::Field);}
API(bullet_player) Vec3* bullet_player(bullet_test::Fixture* f){return &f->player;}
API(bullet_cull_size) void bullet_cull_size(bullet_test::Fixture* f,float w,float h){f->bullet.cull_width=w;f->bullet.cull_height=h;}
API(bullet_extra_run) u32 bullet_extra_run(bullet_test::Fixture* f,float rate,u32 flags,u32 update){f->events.clear();f->emissions.clear();BulletExtras extras({rate,bool(flags&32)},f->player,*f);if(update){extras.update(f->bullet);return true;}return extras.begin(f->bullet);}
API(bullet_events) const bullet_test::Event* bullet_events(bullet_test::Fixture* f){return f->events.data();}
API(bullet_event_count) u32 bullet_event_count(bullet_test::Fixture* f){return f->events.size();}
API(bullet_children) const BulletEmission* bullet_children(bullet_test::Fixture* f){return f->emissions.data();}
API(bullet_children_count) u32 bullet_children_count(bullet_test::Fixture* f){return f->emissions.size();}
API(bullet_manager_create) bullet_test::ManagerFixture* bullet_manager_create(){return new bullet_test::ManagerFixture;}
API(bullet_manager_delete) void bullet_manager_delete(bullet_test::ManagerFixture* f){delete f;}
API(bullet_manager_at) Bullet* bullet_manager_at(bullet_test::ManagerFixture* f,u32 index){return &f->manager.pool[index];}
API(bullet_manager_cull_size) void bullet_manager_cull_size(bullet_test::ManagerFixture* f,u32 index,float w,float h){auto& b=f->manager.pool[index];b.cull_width=w;b.cull_height=h;}
API(bullet_manager_step) u32 bullet_manager_step(bullet_test::ManagerFixture* f,float rate,u32 timing_flags,u32 field_flags,u32 game_flags,u32 test_frame){
    f->test_frame=test_frame;f->events.clear();f->emissions.clear();const FrameTiming timing{rate,bool(timing_flags&32)};
    const bool result=f->manager.update_bullets(timing,f->player,field_flags,game_flags,*f);f->manager.finish_frame(timing,field_flags,game_flags);return result;
}
API(bullet_manager_metadata) i32 bullet_manager_metadata(bullet_test::ManagerFixture* f,u32 field){const auto& m=f->manager;return field==0?m.total:field==1?m.first_count:field==2?m.second_count:field==3?m.frame:field==4?m.cancel_frames:m.draw_heads[(field-5)%6];}
API(bullet_manager_timer) Timer* bullet_manager_timer(bullet_test::ManagerFixture* f){return &f->manager.lifetime;}
API(bullet_manager_draw_next) i32 bullet_manager_draw_next(bullet_test::ManagerFixture* f,u32 index){return f->manager.pool[index].draw_next;}
API(bullet_visuals_create) bullet_test::VisualFixture* bullet_visuals_create(){return new bullet_test::VisualFixture;}
API(bullet_visuals_delete) void bullet_visuals_delete(bullet_test::VisualFixture* f){delete f;}
API(bullet_visuals_load) u32 bullet_visuals_load(bullet_test::VisualFixture* f,const u8* data,u32 size){return f->load(data,size);}
API(bullet_visuals_part) void* bullet_visuals_part(bullet_test::VisualFixture* f,u32 part){return part==0?static_cast<void*>(&f->resource.view()):part==1?static_cast<void*>(f->resource.view().rawData):part==2?static_cast<void*>(f->resource.view().sprites):static_cast<void*>(&f->random);}
API(bullet_visuals_template) AnmVm* bullet_visuals_template(bullet_test::VisualFixture* f,u32 type,u32 animation){return &f->visuals->appearances[type].animations[animation];}
API(bullet_visuals_instance) AnmVm* bullet_visuals_instance(bullet_test::VisualFixture* f,u32 slot,u32 animation){return &f->visuals->instances[slot][animation];}
API(bullet_visuals_metadata) const void* bullet_visuals_metadata(bullet_test::VisualFixture* f,u32 type){return &f->visuals->appearances[type].hitbox;}
API(bullet_visuals_spawn) i32 bullet_visuals_spawn(bullet_test::VisualFixture* f,const BulletEmission* e,i32 index,i32 layer,float aim,u32 second_pool,float rate){
    f->events.clear();f->emissions.clear();f->executor.timing={rate,false};
    const auto b=f->manager.create(*e,index,layer,aim,second_pool,{rate,false},f->random,f->player,*f);return b?i32(b-f->manager.pool.data()):-1;
}
API(bullet_visuals_frame) u32 bullet_visuals_frame(bullet_test::VisualFixture* f,float rate,u32 timing_flags,u32 field_flags,u32 game_flags,u32 test_frame){
    f->executor.timing={rate,bool(timing_flags&32)};return bullet_manager_step(f,rate,timing_flags,field_flags,game_flags,test_frame);
}
API(laser_create_fixture) laser_test::Fixture* laser_create_fixture(){return new laser_test::Fixture;}
API(laser_delete_fixture) void laser_delete_fixture(laser_test::Fixture* f){delete f;}
API(laser_load) u32 laser_load(laser_test::Fixture* f,const u8* data,u32 size){return f->load(data,size);}
API(laser_part) void* laser_part(laser_test::Fixture* f,u32 part){return part==0?static_cast<void*>(&f->resource.view()):part==1?static_cast<void*>(f->resource.view().rawData):part==2?static_cast<void*>(f->resource.view().sprites):part==3?static_cast<void*>(&f->random):static_cast<void*>(&f->player);}
API(laser_at) Laser* laser_at(laser_test::Fixture* f,u32 index){return &f->manager->pool[index];}
API(laser_spawn) i32 laser_spawn(laser_test::Fixture* f,const BulletEmission* e,i32 cancel,float rate){f->executor.timing={rate,false};const auto l=f->manager->create(*e,f->player,cancel);return l?i32(l-f->manager->pool.data()):-1;}
API(laser_step) u32 laser_step(laser_test::Fixture* f,float rate,u32 flags,u32 field_flags,u32 game_flags){f->collisions.clear();return f->manager->update({rate,bool(flags&32)},field_flags,game_flags,*f);}
API(laser_collisions) const laser_test::Collision* laser_collisions(laser_test::Fixture* f){return f->collisions.data();}
API(laser_collision_count) u32 laser_collision_count(laser_test::Fixture* f){return f->collisions.size();}
API(timeline_create) timeline_test::Fixture* timeline_create(){return new timeline_test::Fixture;}
API(timeline_delete) void timeline_delete(timeline_test::Fixture* f){delete f;}
API(timeline_load) u32 timeline_load(timeline_test::Fixture* f,const u8* data,u32 size,u32 index,i32 mirrored){return f->program.load(data,size)&&f->timeline.start(f->program,index,mirrored);}
API(timeline_part) void* timeline_part(timeline_test::Fixture* f,u32 part){return part==0?static_cast<void*>(&f->timeline.time):part==1?static_cast<void*>(&f->timeline.wait):part==2?static_cast<void*>(&f->random):static_cast<void*>(f->events);}
API(timeline_meta) i32 timeline_meta(timeline_test::Fixture* f,u32 part){return part==0?f->timeline.instruction_offset():part==1?f->timeline.mirrored:part==2?f->finished:part==3?f->created.values.drop_count:part==4?f->created.values.drop_item:f->created.values.flags;}
API(timeline_boss) i32 timeline_boss(timeline_test::Fixture* f,u32 index){return f->bosses[index%4].pending_interrupt;}
API(timeline_set_boss) void timeline_set_boss(timeline_test::Fixture* f,u32 index,u32 active,u32 exists){f->bosses[index%4].behavior_flags=active;f->boss_mask=(f->boss_mask&~(1u<<(index%4)))|(exists?1u<<(index%4):0);}
API(timeline_step) i32 timeline_step(timeline_test::Fixture* f,float rate,float step,u32 flags,u32 mask,i32 dialogue){f->spawns.clear();return f->timeline.step({rate,bool(flags&32)},step,u8(mask),dialogue,f->random,*f);}
API(timeline_spawns) const EnemySpawn* timeline_spawns(timeline_test::Fixture* f){return f->spawns.data();}
API(timeline_spawn_count) u32 timeline_spawn_count(timeline_test::Fixture* f){return f->spawns.size();}
API(enemy_manager_fixture) enemy_manager_test::Fixture* enemy_manager_fixture(){return new enemy_manager_test::Fixture;}
API(enemy_manager_delete) void enemy_manager_delete(enemy_manager_test::Fixture* f){delete f;}
API(enemy_manager_load) u32 enemy_manager_load(enemy_manager_test::Fixture* f,const u8* bytes,u32 size,u32 character){return (character?f->manager.character_program:f->manager.common_program).load(bytes,size);}
API(enemy_manager_field) void* enemy_manager_field(enemy_manager_test::Fixture* f,u32 actor,u32 field){return f->value(actor,field);}
API(enemy_manager_actor) EclVm* enemy_manager_actor(enemy_manager_test::Fixture* f,u32 index){return &f->actor(index);}
API(enemy_manager_active) void enemy_manager_active(enemy_manager_test::Fixture* f,u32 index,u32 active){f->actor(index).behavior_flags=(f->actor(index).behavior_flags&~1u)|(active&1u);}
API(enemy_manager_spawn) i32 enemy_manager_spawn(enemy_manager_test::Fixture* f,const EnemySpawn* request,const EclLocals* locals,float rate){f->manager.timing={rate,false};auto e=f->manager.create(*request,locals);return i32(e-f->manager.enemies.data());}
API(enemy_manager_failed) u32 enemy_manager_failed(enemy_manager_test::Fixture* f){return f->manager.allocation_failed;}
API(enemy_manager_script) i32 enemy_manager_script(enemy_manager_test::Fixture* f,u32 index,u32 part){const auto& a=f->actor(index);const auto& c=a.primary;
    return part==0?c.subroutine:part==1?(c.program==&f->manager.character_program):part==2?(c.instruction&&c.program?i32(reinterpret_cast<const u8*>(c.instruction)-c.program->data()):-1):part==3?c.depth:a.failed_opcode;}
API(enemy_manager_time) Timer* enemy_manager_time(enemy_manager_test::Fixture* f,u32 index){return &f->actor(index).primary.timer;}
API(ecl_vm_trail) EnemyTrailVertex* ecl_vm_trail(ecl_test::VmFixture* f){return f->vm.trail.vertices.data();}
API(ecl_vm_sprite) AnmLoadedSprite* ecl_vm_sprite(ecl_test::VmFixture* f){f->vm.animation.layers[0].loadedSprite=&f->test_sprite;return &f->test_sprite;}
API(enemy_manager_boss) i32 enemy_manager_boss(enemy_manager_test::Fixture* f,u32 slot){const auto e=f->manager.bosses[slot%8];return e?i32(e-f->manager.enemies.data()):-1;}
API(enemy_manager_set_boss) void enemy_manager_set_boss(enemy_manager_test::Fixture* f,u32 slot,i32 index){f->manager.bosses[slot%8]=index<0?nullptr:&f->actor(u32(index));}
API(enemy_manager_run) u32 enemy_manager_run(enemy_manager_test::Fixture* f,u32 index,float rate){f->scene_events.clear();f->manager.timing={rate,false};return f->manager.run_script(f->actor(index));}
API(enemy_manager_scene_events) const void* enemy_manager_scene_events(enemy_manager_test::Fixture* f){return f->scene_events.data();}
API(enemy_manager_scene_count) u32 enemy_manager_scene_count(enemy_manager_test::Fixture* f){return f->scene_events.size();}
API(enemy_manager_scene_value) i32 enemy_manager_scene_value(enemy_manager_test::Fixture* f,u32 which){return which==0?f->scene_value:which==1?f->rank_time:which==2?f->manager.timeline_control:f->manager.attack_control;}
API(enemy_death_fixture) enemy_death_test::Fixture* enemy_death_fixture(){return new enemy_death_test::Fixture;}
API(enemy_death_delete) void enemy_death_delete(enemy_death_test::Fixture* f){delete f;}
API(enemy_death_setup) void enemy_death_setup(enemy_death_test::Fixture* f,u32 flags,i32 index,i32 effect,i32 count,i32 opposing,i32 side,i32 character,float gauge,u32 seed){
    f->enemy.values.flags=flags;f->enemy.status.index=index;f->enemy.status.effects[0]=i8(effect);f->enemy.values.item_reward=3;f->count=count;f->opponent_count=opposing;f->field.side=side;f->field.character=character;f->gauge.value=gauge;
    f->enemy.values.position={71.375f,95.625f,.5f};f->enemy.values.resolved_position={76.875f,100.75f,0};f->geometry={u32(32+side*304),16,{17.25f,13.125f},384};f->world.random={u16(seed),0,0};f->transfer={};
}
API(enemy_death_run) u32 enemy_death_run(enemy_death_test::Fixture* f,i32 source){f->events.clear();return EnemyDeath::apply(f->enemy,source,f->geometry,f->opposing_width,*f);}
API(enemy_death_events) const void* enemy_death_events(enemy_death_test::Fixture* f){return f->events.data();}
API(enemy_death_event_count) u32 enemy_death_event_count(enemy_death_test::Fixture* f){return f->events.size();}
API(enemy_death_part) void* enemy_death_part(enemy_death_test::Fixture* f,u32 part){return part==0?static_cast<void*>(&f->world.random):part==1?static_cast<void*>(&f->gauge.value):part==2?static_cast<void*>(&f->transfer):static_cast<void*>(&f->count);}
API(combo_fixture) combo_test::Fixture* combo_fixture(){return new combo_test::Fixture;}
API(combo_delete) void combo_delete(combo_test::Fixture* f){delete f;}
API(combo_setup) void combo_setup(combo_test::Fixture* f,i32 rank,i32 difficulty,i32 side,u32 blocked,u32 boss,i32 spirits,float rate,u32 flags,u32 seed){
    f->world.rank=rank;f->world.difficulty=difficulty;f->side=side;f->blocked=blocked;f->boss=boss;f->spirits=spirits;f->timing={rate,bool(flags&32)};f->world.random={u16(seed),0,0};f->geometry={u32(32+side*304),16,{17.25f,13.125f},384};f->events.clear();f->next_transfer=0;
}
API(combo_part) void* combo_part(combo_test::Fixture* f,u32 part){return part==0?static_cast<void*>(&f->state):static_cast<void*>(&f->world.random);}
API(combo_add) u32 combo_add(combo_test::Fixture* f,const Vec3* p,i32 normal,i32 spirit,i32 character,i32 score,u32 kill){return kill?f->system().kill(f->state,*p,normal,spirit,character,score):f->system().add(f->state,*p,normal,spirit,character,score);}
API(combo_reset) void combo_reset(combo_test::Fixture* f,u32 flush){f->events.clear();if(flush)f->system().flush(f->state);else f->state.reset_chain();}
API(combo_events) const void* combo_events(combo_test::Fixture* f){return f->events.data();}
API(combo_event_count) u32 combo_event_count(combo_test::Fixture* f){return f->events.size();}
API(combo_transfer_count) u32 combo_transfer_count(combo_test::Fixture* f){return f->next_transfer;}
API(combo_transfer) const TransferParameters* combo_transfer(combo_test::Fixture* f,u32 index){return &f->transfers[index%f->transfers.size()];}
API(character_attack_meter) u32 character_attack_meter(special_test::Fixture* f,ComboState* combo,i32 character,i32 rank,i32 side,const Vec3* p){f->events.clear();return CharacterAttackMeter::consume(*combo,character,rank,side,*p,*f);}
API(capture_contains) u32 capture_contains(i32 character,const Vec3* player,const Vec3* target,const CaptureArea* area){return CharacterCapture::contains(character,*player,*target,*area);}
API(capture_motion) u32 capture_motion(ecl_test::VmFixture* f,i32 character,float rate){return CharacterCapture::update_motion(f->vm,character,{rate,false});}
API(enemy_integrate) void enemy_integrate(ecl_test::VmFixture* f,float rate){f->vm.movement.integrate_position(f->vm,{rate,false});}
API(enemy_frame_create) enemy_frame_test::Fixture* enemy_frame_create(){return new enemy_frame_test::Fixture;}
API(enemy_frame_delete) void enemy_frame_delete(enemy_frame_test::Fixture* f){delete f;}
API(enemy_frame_base) enemy_manager_test::Fixture* enemy_frame_base(enemy_frame_test::Fixture* f){return f;}
API(enemy_frame_fields) const void* enemy_frame_fields(){return enemy_frame_test::fields;}
API(enemy_frame_field_count) u32 enemy_frame_field_count(){return std::size(enemy_frame_test::fields);}
API(enemy_frame_field) void* enemy_frame_field(enemy_frame_test::Fixture* f,u32 index){const auto& v=enemy_frame_test::fields[index];return (v.base?reinterpret_cast<u8*>(&f->player):reinterpret_cast<u8*>(&f->manager))+v.offset;}
API(enemy_frame_extras) const void* enemy_frame_extras(){return enemy_frame_test::extras;}
API(enemy_frame_extra_count) u32 enemy_frame_extra_count(){return std::size(enemy_frame_test::extras);}
API(enemy_frame_capture) CaptureArea* enemy_frame_capture(enemy_frame_test::Fixture* f){return &f->player.capture;}
API(enemy_frame_side) void enemy_frame_side(enemy_frame_test::Fixture* f,i32 side){f->field.side=side;}
API(enemy_frame_field_flags) void enemy_frame_field_flags(enemy_frame_test::Fixture* f,u32 flags){f->field.flags=flags;}
API(enemy_frame_shots) void* enemy_frame_shots(enemy_frame_test::Fixture* f){return f->shots;}
API(enemy_frame_settings) EnemyFrameSettings* enemy_frame_settings(enemy_frame_test::Fixture* f){return &f->settings;}
API(enemy_frame_sprite) void enemy_frame_sprite(enemy_frame_test::Fixture* f,u32 index,u32 exists){f->manager.enemies[index].animation.layers[0].loadedSprite=exists?&f->sprite:nullptr;}
API(enemy_frame_step) u32 enemy_frame_step(enemy_frame_test::Fixture* f,float rate,u32 flags){f->clear_events();f->manager.timing={rate,bool(flags&32)};return f->manager.step_frame(f->settings);}
API(enemy_frame_events) const void* enemy_frame_events(enemy_frame_test::Fixture* f){return f->frame_events.data();}
API(enemy_frame_event_count) u32 enemy_frame_event_count(enemy_frame_test::Fixture* f){return f->frame_events.size();}
API(enemy_frame_emissions) const void* enemy_frame_emissions(enemy_frame_test::Fixture* f){return f->events.data();}
API(enemy_frame_emission_count) u32 enemy_frame_emission_count(enemy_frame_test::Fixture* f){return f->events.size();}
API(enemy_frame_target) i32 enemy_frame_target(enemy_frame_test::Fixture* f,u32 which){auto p=which==0?f->player.target:which==1?f->manager.priority_target:f->manager.first_target;return p?i32(p-f->manager.enemies.data()):-1;}
API(enemy_frame_set_target) void enemy_frame_set_target(enemy_frame_test::Fixture* f,i32 index){f->player.target=index<0?nullptr:&f->manager.enemies[index];}
API(enemy_frame_draw) i32 enemy_frame_draw(enemy_frame_test::Fixture* f,u32 layer,u32 index){auto& list=f->manager.draw_lists[layer%4];return index>=list.size()?-1:i32(list[index]-f->manager.enemies.data());}
API(enemy_frame_timeline_part) void* enemy_frame_timeline_part(enemy_frame_test::Fixture* f,u32 part){return part==0?static_cast<void*>(&f->manager.timeline.time):part==1?static_cast<void*>(&f->manager.timeline.wait):static_cast<void*>(&f->manager.timeline.mirrored);}
API(enemy_frame_timeline_cursor) i32 enemy_frame_timeline_cursor(enemy_frame_test::Fixture* f){return f->manager.timeline.instruction_offset();}
API(attack_queue_fixture) attack_queue_test::Fixture* attack_queue_fixture(){return new attack_queue_test::Fixture;}
API(attack_queue_delete) void attack_queue_delete(attack_queue_test::Fixture* f){delete f;}
API(attack_queue_part) void* attack_queue_part(attack_queue_test::Fixture* f,u32 which){return which==0?static_cast<void*>(&f->rng):which==1?static_cast<void*>(f->geometry):which==2?static_cast<void*>(f->queue.limits):static_cast<void*>(f->queue.counts);}
API(attack_queue_fields) const void* attack_queue_fields(){return attack_queue_test::fields;}
API(attack_queue_field_count) u32 attack_queue_field_count(){return std::size(attack_queue_test::fields);}
API(attack_queue_actor) AttackActor* attack_queue_actor(attack_queue_test::Fixture* f,u32 index){return &f->queue.actors[index%(AttackQueue::capacity+1)];}
API(attack_queue_create) i32 attack_queue_create(attack_queue_test::Fixture* f,i32 kind,i32 side,const Vec3* pos,const Vec3* extra){f->clear_events();auto a=f->queue.create(kind,side,*pos,extra);return a?i32(a-f->queue.actors.data()):-1;}
API(attack_queue_step) u32 attack_queue_step(attack_queue_test::Fixture* f,float rate,u32 flags,u32 paused,u32 frozen,i32 collision){f->clear_events();f->timing={rate,bool(flags&32)};f->collision_result=collision;return f->queue.update(f->timing,paused,frozen);}
API(attack_queue_clear) void attack_queue_clear(attack_queue_test::Fixture* f){f->clear_events();f->queue.clear();}
API(attack_queue_event_count) u32 attack_queue_event_count(attack_queue_test::Fixture* f){return f->events.size();}
API(attack_queue_events) const void* attack_queue_events(attack_queue_test::Fixture* f){return f->events.data();}
API(attack_queue_draw) i32 attack_queue_draw(attack_queue_test::Fixture* f,u32 layer,u32 index){auto& list=f->queue.draw_lists[layer%3];return index>=list.size()?-1:i32(list[index]-f->queue.actors.data());}
API(attack_queue_animation) AnmVm* attack_queue_animation(attack_queue_test::Fixture* f,u32 index,u32 slot){auto& a=f->queue.actors[index];return slot<a.animations.size()?&a.animations[slot]:nullptr;}
API(attack_queue_animation_count) u32 attack_queue_animation_count(attack_queue_test::Fixture* f,u32 index){return f->queue.actors[index].animations.size();}
API(attack_queue_travel) const void* attack_queue_travel(attack_queue_test::Fixture* f,u32 index){return f->snapshot(index);}
API(attack_queue_travel_size) u32 attack_queue_travel_size(attack_queue_test::Fixture* f,u32 index){return f->snapshot(index)?f->state_size:0;}
API(attack_queue_players) Vec3* attack_queue_players(attack_queue_test::Fixture* f){return f->players;}
API(attack_queue_settings) void attack_queue_settings(attack_queue_test::Fixture* f,i32 rank,i32 difficulty,i32 level){f->rank=rank;f->difficulty=difficulty;for(u32 side=0;side<2;++side){f->attack_levels[side][0]=level+i32(side)*3;f->attack_levels[side][1]=level*2+i32(side);}}
API(attack_queue_bullets) const void* attack_queue_bullets(attack_queue_test::Fixture* f){return f->bullet_events.data();}
API(attack_queue_bullet_count) u32 attack_queue_bullet_count(attack_queue_test::Fixture* f){return f->bullet_events.size();}
API(attack_queue_effect_scales) Vec2* attack_queue_effect_scales(attack_queue_test::Fixture* f){return f->effect_scales;}
API(attack_queue_bullet) Bullet* attack_queue_bullet(attack_queue_test::Fixture* f,u32 side,u32 index){return &f->bullet_managers[side].pool[index];}
API(attack_queue_emitted) Bullet* attack_queue_emitted(attack_queue_test::Fixture* f){return &f->emitted;}
API(attack_queue_render) void attack_queue_render(attack_queue_test::Fixture* f,u32 index){f->clear_events();auto& a=f->queue.actors[index];if(a.behavior&&a.behavior->draw)a.behavior->draw(a,*f);}
API(attack_queue_draw_bytes) const void* attack_queue_draw_bytes(attack_queue_test::Fixture* f){return f->draw_bytes.data();}
API(attack_queue_draw_size) u32 attack_queue_draw_size(attack_queue_test::Fixture* f){return f->draw_bytes.size();}
API(attack_queue_render_all) void attack_queue_render_all(attack_queue_test::Fixture* f){f->clear_events();draw_character_attacks(f->queue,*f,false);draw_character_attacks(f->queue,*f,true);}
API(attack_areas_fixture) attack_areas_test::Fixture* attack_areas_fixture(){return new attack_areas_test::Fixture;}
API(attack_areas_delete) void attack_areas_delete(attack_areas_test::Fixture* f){delete f;}
API(attack_areas_part) void* attack_areas_part(attack_areas_test::Fixture* f,u32 which){switch(which){case 0:return f->areas.pool.data();case 1:return &f->rng;case 2:return f->context.geometry;case 3:return &f->context.side;case 4:return &f->bullet;case 5:return &f->graze;case 6:return &f->transfer;}return nullptr;}
API(attack_areas_list) i32 attack_areas_list(attack_areas_test::Fixture* f,u32 which,u32 index){auto p=which?f->areas.free[index]:f->areas.active[index];return p?i32(p-f->areas.pool.data()):-1;}
API(attack_areas_count) i32 attack_areas_count(attack_areas_test::Fixture* f,u32 which){return which?f->areas.free_count:f->areas.count;}
API(attack_areas_create) i32 attack_areas_create(attack_areas_test::Fixture* f,const Vec3* p,i32 lifetime,i32 delay){return i32(&f->areas.create(*p,lifetime,delay)-f->areas.pool.data());}
API(attack_areas_update) void attack_areas_update(attack_areas_test::Fixture* f){f->areas.update();}
API(attack_areas_cancel) i32 attack_areas_cancel(attack_areas_test::Fixture* f,const Vec3* p,const Vec3* extent,i32 kind){f->events.clear();return kind<2?f->areas.cancel(*p,kind?&f->bullet:nullptr,f->context):f->areas.probe(*p,*extent,f->graze,kind==3?&f->bullet:nullptr,f->context);}
API(attack_areas_damage) i32 attack_areas_damage(attack_areas_test::Fixture* f,const Vec3* p,const Vec3* extent,i32* special){return f->areas.damage(*p,*extent,*special);}
API(attack_areas_events) const void* attack_areas_events(attack_areas_test::Fixture* f){return f->events.data();}
API(attack_areas_event_count) u32 attack_areas_event_count(attack_areas_test::Fixture* f){return f->events.size();}
API(player_shots_fixture) player_shots_test::Fixture* player_shots_fixture(){return new player_shots_test::Fixture;}
API(player_shots_delete) void player_shots_delete(player_shots_test::Fixture* f){delete f;}
API(player_shots_load) u32 player_shots_load(player_shots_test::Fixture* f,const u8* p,u32 size){return f->resource.load(p,size);}
API(player_shots_part) void* player_shots_part(player_shots_test::Fixture* f,u32 which){switch(which){case 0:return &f->player.player_position;case 1:return f->player.option_positions.data();case 2:return &f->player.target;case 3:return &f->player.player_scale;case 4:return &f->player.beam_time;case 5:return &f->geometry;case 6:return &f->effect_pos;case 7:return &f->resource.movement;}return nullptr;}
API(player_shots_shot) PlayerShot* player_shots_shot(player_shots_test::Fixture* f,u32 index){return &f->player.shots[index];}
API(player_shots_fire) void player_shots_fire(player_shots_test::Fixture* f,u32 set,i32 frame,u32 effect_active){f->clear_events();f->effect_enabled=effect_active;f->player.fire(f->resource,set,frame);}
API(player_shots_update) void player_shots_update(player_shots_test::Fixture* f,float rate,u32 frozen,i32 frame){f->clear_events();f->timing={rate,false};f->frame=frame;f->player.update(f->timing,frozen);}
API(player_shots_draw) void player_shots_draw(player_shots_test::Fixture* f,u32 fading){f->clear_events();f->player.draw(f->geometry,fading);}
API(player_shots_hit) i32 player_shots_hit(player_shots_test::Fixture* f,const Vec3* pos,const Vec3* extent,const Timer* protection,i32* result){f->clear_events();return f->player.hit(*pos,*extent,*protection,result[0],reinterpret_cast<u32*>(result+1),result[2]);}
API(player_shots_beam) i32 player_shots_beam(player_shots_test::Fixture* f){return f->player.beam?i32(f->player.beam-f->player.shots.data()):-1;}
API(player_shots_side) void player_shots_side(player_shots_test::Fixture* f,i32 side){f->player.side=side;}
API(player_shots_event_count) u32 player_shots_event_count(player_shots_test::Fixture* f){return f->events.size();}
API(player_shots_events) const void* player_shots_events(player_shots_test::Fixture* f){return f->events.data();}
API(player_shots_draw_count) u32 player_shots_draw_count(player_shots_test::Fixture* f){return f->drawn.size();}
API(player_shots_draw_data) const void* player_shots_draw_data(player_shots_test::Fixture* f){return f->drawn.data();}
API(player_shots_area) AttackArea* player_shots_area(player_shots_test::Fixture* f,u32 index){return &f->player.areas.pool[index];}
API(player_shots_area_count) i32 player_shots_area_count(player_shots_test::Fixture* f){return f->player.areas.count;}
API(player_shots_area_update) void player_shots_area_update(player_shots_test::Fixture* f){f->player.areas.update();}
API(shot_control_fixture) shot_control_test::Fixture* shot_control_fixture(){return new shot_control_test::Fixture;}
API(shot_control_delete) void shot_control_delete(shot_control_test::Fixture* f){delete f;}
API(shot_control_state) ShotControlState* shot_control_state(shot_control_test::Fixture* f){return &f->state;}
API(shot_control_position) Vec3* shot_control_position(shot_control_test::Fixture* f){return &f->position;}
API(shot_control_fields) const void* shot_control_fields(){return shot_control_test::fields;}
API(shot_control_field_count) u32 shot_control_field_count(){return std::size(shot_control_test::fields);}
API(shot_control_resource) u32 shot_control_resource(shot_control_test::Fixture* f,const u8* data,u32 size){return f->resource.load(data,size);}
API(shot_control_step) void shot_control_step(shot_control_test::Fixture* f,u32 held,u32 pressed,u32 focus,u32 side,float rate,u32 boss,u32 bomb,u32 locked){f->events.clear();f->input.held=u16(held);f->input.pressed=u16(pressed);f->boss_available=boss;ShotControl control(f->state,f->areas,*f);if(bomb)control.bomb(f->input,locked,f->resource,f->position);else control.update(f->input,focus,side,f->resource,f->position,{rate,false});}
API(shot_control_events) const void* shot_control_events(shot_control_test::Fixture* f){return f->events.data();}
API(shot_control_event_count) u32 shot_control_event_count(shot_control_test::Fixture* f){return f->events.size();}
API(shot_control_areas) AttackArea* shot_control_areas(shot_control_test::Fixture* f){return f->areas.pool.data();}
API(shot_control_area_count) i32 shot_control_area_count(shot_control_test::Fixture* f){return f->areas.count;}
API(shot_control_areas_reset) void shot_control_areas_reset(shot_control_test::Fixture* f){f->areas.~AttackAreas();new(&f->areas)AttackAreas;}
API(player_hazards_create) PlayerHazards* player_hazards_create(){return new PlayerHazards;}
API(player_hazards_delete) void player_hazards_delete(PlayerHazards* p){delete p;}
API(player_hazards_entries) PlayerHazard* player_hazards_entries(PlayerHazards* p){return p->entries.data();}
API(player_hazards_count) u32 player_hazards_count(PlayerHazards* p){return p->count;}
API(player_hazards_clear) void player_hazards_clear(PlayerHazards* p){p->clear();}
#include "../../cpp/game/HazardTrace.hpp"
API(hazard_trace_enable) void hazard_trace_enable(u32 enabled){hazard_trace.enabled=enabled!=0;if(!enabled){hazard_trace.count=0;hazard_trace.next=0;}}
API(hazard_trace_ptr) void* hazard_trace_ptr(){return &hazard_trace;}
API(cpu_decisions_ptr) void* cpu_decisions_ptr(){return cpu_decisions.data();}
API(player_hazards_add) void player_hazards_add(PlayerHazards* p,u32 kind,const Vec3* position,const Vec3* extent,const Vec3* pivot,float radius,float angle,u32 owner){auto bullet=reinterpret_cast<Bullet*>(owner);if(kind==0)p->circle(*position,radius,bullet);else if(kind==1)p->box(*position,*extent,bullet);else p->rotated_box(*position,*extent,*pivot,angle,bullet);}
API(player_hazards_hit) i32 player_hazards_hit(PlayerHazards* p,HazardPlayer* player,const Vec3* position,const Vec3* extent,float radius){auto result=position?p->sample(*player,*position,*extent,radius):p->first_hit(*player);return result?i32(result-p->entries.data()):-1;}
API(player_hazard_state_size) u32 player_hazard_state_size(){return sizeof(HazardPlayer);}
API(player_items_fixture) player_items_test::Fixture* player_items_fixture(){return new player_items_test::Fixture;}
API(player_items_delete) void player_items_delete(player_items_test::Fixture* f){delete f;}
API(player_items_part) void* player_items_part(player_items_test::Fixture* f,u32 which){return which==0?static_cast<void*>(f->manager.items.data()):which==1?static_cast<void*>(&f->random):which==2?static_cast<void*>(&f->context):static_cast<void*>(&f->manager.attraction);}
API(player_items_spawn) void player_items_spawn(player_items_test::Fixture* f,i32 kind,const Vec3* p,u32 locked){f->clear();f->manager.spawn(kind,*p,locked);}
API(player_items_step) void player_items_step(player_items_test::Fixture* f){f->clear();f->manager.update(f->context);}
API(player_items_draw) void player_items_draw(player_items_test::Fixture* f){f->clear();f->manager.draw(f->context.geometry[f->context.side]);}
API(player_items_events) const void* player_items_events(player_items_test::Fixture* f){return f->events.data();}
API(player_items_event_count) u32 player_items_event_count(player_items_test::Fixture* f){return f->events.size();}
API(player_items_transfers) const void* player_items_transfers(player_items_test::Fixture* f){return f->transfers.data();}
API(player_items_transfer_count) u32 player_items_transfer_count(player_items_test::Fixture* f){return f->transfers.size();}
API(player_life_fixture) player_life_test::Fixture* player_life_fixture(){return new player_life_test::Fixture;}
API(player_life_delete) void player_life_delete(player_life_test::Fixture* f){delete f;}
API(player_life_part) void* player_life_part(player_life_test::Fixture* f,u32 which){switch(which){case 0:return &f->motion;case 1:return &f->control;case 2:return &f->body;case 3:return &f->rules;case 4:return &f->limits;case 5:return &f->random;case 6:return f->areas.pool.data();case 7:return &f->life.shield_position;case 8:return f->flash_duration;case 9:return f->flash_color;}return nullptr;}
API(player_life_resource) u32 player_life_resource(player_life_test::Fixture* f,const u8* data,u32 size){return f->resource.load(data,size);}
API(player_life_settings) void player_life_settings(player_life_test::Fixture* f,u32 character,i32 controller,u32 focus,i32 display,i32 hidden,u32 shield,u32 boss,float z){f->motion.character=character;f->motion.focus_effects=focus;f->life.input_controller=controller;f->life.display_frames=display;f->life.hidden=hidden;f->life.shield_active=shield;f->boss_available=boss;f->life.knockback_z=z;}
API(player_life_value) i32 player_life_value(player_life_test::Fixture* f,u32 which){return which==0?f->motion.focus_effects:which==1?f->life.display_frames:which==2?f->life.hidden:f->life.shield_active;}
API(player_life_step) void player_life_step(player_life_test::Fixture* f,u32 which,float rate){f->events.clear();if(which==0)f->life.damage(f->rules,f->resource);else if(which==1)f->life.recover(f->limits);else if(which==2)f->life.enter();else f->life.update_timers({rate,false});}
API(player_life_events) const void* player_life_events(player_life_test::Fixture* f){return f->events.data();}
API(player_life_event_count) u32 player_life_event_count(player_life_test::Fixture* f){return f->events.size();}
API(player_life_areas_reset) void player_life_areas_reset(player_life_test::Fixture* f){f->areas.~AttackAreas();new(&f->areas)AttackAreas;}
API(player_life_area_count) i32 player_life_area_count(player_life_test::Fixture* f){return f->areas.count;}
API(cpu_player_fixture) cpu_player_test::Fixture* cpu_player_fixture(){return new cpu_player_test::Fixture;}
API(cpu_player_delete) void cpu_player_delete(cpu_player_test::Fixture* f){delete f;}
API(cpu_player_fields) const void* cpu_player_fields(){return cpu_player_test::fields;}
API(cpu_player_field_count) u32 cpu_player_field_count(){return std::size(cpu_player_test::fields);}
API(cpu_player_part) void* cpu_player_part(cpu_player_test::Fixture* f,u32 which){switch(which){case 0:return &f->state;case 1:return &f->context;case 2:return &f->shot;case 3:return &f->input;case 4:return &f->random;case 5:return &f->hazards;case 6:return &f->context.player;case 7:return &f->context.speeds;case 8:return &f->context.limits;}return nullptr;}
API(cpu_player_modes) void cpu_player_modes(cpu_player_test::Fixture* f,u32 mode){f->context.scene_locked=mode&1;f->context.dialogue=mode&2;f->context.extra_mode=mode&4;f->context.has_priority=mode&8;f->context.has_first=mode&16;}
API(cpu_player_radius) void cpu_player_radius(cpu_player_test::Fixture* f,float radius){f->context.player.hit_radius=radius;}
API(cpu_player_step) void cpu_player_step(cpu_player_test::Fixture* f,float rate){f->events.clear();CpuPlayer(f->state,f->random,f->hazards,*f).update(f->input,f->context,f->shot,{rate,false});}
API(cpu_player_events) const void* cpu_player_events(cpu_player_test::Fixture* f){return f->events.data();}
API(cpu_player_event_count) u32 cpu_player_event_count(cpu_player_test::Fixture* f){return f->events.size();}
API(player_frame_fixture) player_frame_test::Fixture* player_frame_fixture(){return new player_frame_test::Fixture;}
API(player_frame_delete) void player_frame_delete(player_frame_test::Fixture* f){delete f;}
API(player_frame_part) void* player_frame_part(player_frame_test::Fixture* f,u32 which){auto& p=f->player;switch(which){case 0:return &p.motion;case 1:return &p.control;case 2:return &p.body;case 3:return &p.input;case 4:return &p.cpu;case 5:return &p.combo_state;case 6:return &f->world;case 7:return &f->damage;case 8:return &p.hazards;case 9:return p.shots.areas.pool.data();case 10:return p.items.items.data();case 11:return &p.shots.target;case 12:return &p.secondary_target;case 13:return &p.motion.base_scale;case 14:return &p.items.attraction;case 15:return &p.shots.beam_time;}return nullptr;}
API(player_frame_load) u32 player_frame_load(player_frame_test::Fixture* f,const u8* data,u32 size,u32 side,u32 character,i32 cpu){return f->player.initialize(data,size,side,character,cpu);}
API(player_frame_reset) void player_frame_reset(player_frame_test::Fixture* f,i32 health,i32 difficulty,i32 round){f->events.clear();f->player.reset_round(health,difficulty,round);}
API(player_frame_setup) void player_frame_setup(player_frame_test::Fixture* f,i32 difficulty,i32 rank){f->world.difficulty=difficulty;f->world.rank=rank;f->context.limits={{-184,32},{368,416}};for(u32 side=0;side<2;++side)f->context.geometry[side]={32+304*side,16,{0,0},288};}
API(player_frame_step) void player_frame_step(player_frame_test::Fixture* f,u32 game_flags,u32 field_flags,u32 dialogue,u32 automatic_focus,i32 scene,float rate,u32 number){f->events.clear();auto& c=f->context;c.game_flags=game_flags;c.field_flags=field_flags;c.dialogue=dialogue;c.automatic_focus=automatic_focus;c.scene_state=scene;c.timing={rate,false};f->frame_number=number;f->player.update(c,f->damage);}
API(player_frame_events) const void* player_frame_events(player_frame_test::Fixture* f){return f->events.data();}
API(player_frame_event_count) u32 player_frame_event_count(player_frame_test::Fixture* f){return f->events.size();}
API(player_frame_shot) PlayerShot* player_frame_shot(player_frame_test::Fixture* f,u32 index){return &f->player.shots.shots[index];}
API(player_frame_area_count) i32 player_frame_area_count(player_frame_test::Fixture* f){return f->player.shots.areas.count;}
API(player_frame_beam) i32 player_frame_beam(player_frame_test::Fixture* f){auto& s=f->player.shots;return s.beam?i32(s.beam-s.shots.data()):-1;}
API(player_frame_life_value) i32 player_frame_life_value(player_frame_test::Fixture* f,u32 which){return which==0?f->player.life.display_frames:which==1?f->player.life.hidden:f->player.motion.focus_effects;}
API(player_frame_draw) void player_frame_draw(player_frame_test::Fixture* f,u32 fading){f->events.clear();f->player.draw(fading);}
API(effects_fixture) effects_test::Fixture* effects_fixture(i32 side,u32 size){return new effects_test::Fixture(side,size);}
API(effects_delete) void effects_delete(effects_test::Fixture* f){delete f;}
API(effects_fields) const void* effects_fields(){return effects_test::fields;}
API(effects_field_count) u32 effects_field_count(){return std::size(effects_test::fields);}
API(effects_part) void* effects_part(effects_test::Fixture* f,u32 which){switch(which){case 0:return &f->rng;case 1:return f->geometry;case 2:return f->players;case 3:return f->player_angles;case 4:return f->player_steps;case 5:return f->characters;case 6:return f->spirit_counts;case 7:return &f->enemy.values;}return nullptr;}
API(effects_actor) EffectActor* effects_actor(effects_test::Fixture* f,u32 index){return &f->manager.actors[index];}
API(effects_animation) AnmVm* effects_animation(effects_test::Fixture* f,u32 index){return f->manager.actors[index].animation.get();}
API(effects_create) i32 effects_create(effects_test::Fixture* f,u32 kind,const Vec3* pos,const Vec3* extra,i32 slot,u32 coordinate){f->clear();f->coordinate_side=coordinate;auto p=slot<0?f->manager.create(EffectKind(kind),*pos,*extra):f->manager.slotted(EffectKind(kind),*pos,slot);return p?i32(p-f->manager.actors.data()):-1;}
API(effects_create_many) i32 effects_create_many(effects_test::Fixture* f,u32 kind,const Vec3* pos,const Vec3* extra,i32 count,u32 color){f->clear();auto p=f->manager.create(EffectKind(kind),*pos,*extra,count,color);return p?i32(p-f->manager.actors.data()):-1;}
API(effects_step) void effects_step(effects_test::Fixture* f,float rate,u32 flags,u32 frozen,i32 frame,i32 rank,u32 blocked){f->clear();f->timing={rate,false};f->rank=rank;f->rewards_blocked=blocked;f->sequence=frame;f->manager.update(flags,frozen);}
API(effects_values) u32 effects_values(effects_test::Fixture* f,u32 which){return which==0?f->manager.cursor:which==1?f->manager.count:f->manager.frame;}
API(effects_draw_index) i32 effects_draw_index(effects_test::Fixture* f,u32 layer,u32 index){auto& list=f->manager.draw_lists[layer];return index>=list.size()?-1:i32(list[index]-f->manager.actors.data());}
API(effects_render) void effects_render(effects_test::Fixture* f,u32 index){f->clear();draw_effect(f->manager.actors[index],*f);}
API(effects_draw_layer) void effects_draw_layer(effects_test::Fixture* f,u32 layer){f->clear();f->manager.draw(layer);}
API(effects_event_count) u32 effects_event_count(effects_test::Fixture* f){return f->events.size();}
API(effects_events) const void* effects_events(effects_test::Fixture* f){return f->events.data();}
API(effects_bullet_count) u32 effects_bullet_count(effects_test::Fixture* f){return f->bullets.size();}
API(effects_bullets) const void* effects_bullets(effects_test::Fixture* f){return f->bullets.data();}
API(effects_vertex_count) u32 effects_vertex_count(effects_test::Fixture* f){return f->vertices.size();}
API(effects_vertices) const void* effects_vertices(effects_test::Fixture* f){return f->vertices.data();}
API(effects_burst) const void* effects_burst(effects_test::Fixture* f,u32 index){return f->manager.actors[index].burst.get();}
API(effects_burst_jitter) const void* effects_burst_jitter(effects_test::Fixture* f,u32 index){auto p=f->manager.actors[index].burst.get();return p?p->jitter.data():nullptr;}
API(effects_reset) void effects_reset(effects_test::Fixture* f){f->clear();f->manager.clear();}
API(match_rules_fixture) match_rules_test::Fixture* match_rules_fixture(){return new match_rules_test::Fixture;}
API(match_rules_delete) void match_rules_delete(match_rules_test::Fixture* f){delete f;}
API(match_rules_fields) const void* match_rules_fields(){return match_rules_test::fields;}
API(match_rules_field_count) u32 match_rules_field_count(){return std::size(match_rules_test::fields);}
API(match_rules_part) void* match_rules_part(match_rules_test::Fixture* f,u32 n){switch(n){case 0:return &f->world;case 1:return &f->rules.progress;case 2:return f->rules.scores.data();case 3:return f->rules.attack_levels;}return nullptr;}
API(match_rules_initialize) void match_rules_initialize(match_rules_test::Fixture* f,i32 difficulty,i32 mode,i32 stage){f->events.clear();f->rules.initialize(difficulty,GameMode(mode),stage);}
API(match_rules_restart) void match_rules_restart(match_rules_test::Fixture* f,i32 round){f->events.clear();f->rules.restart_round(round);}
API(match_rules_step) void match_rules_step(match_rules_test::Fixture* f,u32 dialogue,u32 flags,u32 field_flags){f->events.clear();f->rules.update(dialogue,flags,field_flags);}
API(match_rules_event_count) u32 match_rules_event_count(match_rules_test::Fixture* f){return f->events.size();}
API(match_rules_events) const void* match_rules_events(match_rules_test::Fixture* f){return f->events.data();}
API(attack_controller_fixture) attack_controller_test::Fixture* attack_controller_fixture(i32 side){return new attack_controller_test::Fixture(side);}
API(attack_controller_delete) void attack_controller_delete(attack_controller_test::Fixture* f){delete f;}
API(attack_controller_fields) const void* attack_controller_fields(){return attack_controller_test::fields;}
API(attack_controller_field_count) u32 attack_controller_field_count(){return std::size(attack_controller_test::fields);}
API(attack_controller_part) void* attack_controller_part(attack_controller_test::Fixture* f,u32 n){switch(n){case 0:return &f->controller;case 1:return f->geometry;case 2:return &f->game_flags;case 3:return &f->reward_time;case 4:return f->players;case 5:return f->has_boss;case 6:return f->background;case 7:return &f->bosses[0].behavior_flags;case 8:return &f->bosses[1].behavior_flags;case 9:return f->controller.active_variants;}return nullptr;}
API(attack_controller_begin) u32 attack_controller_begin(attack_controller_test::Fixture* f,i32 variant,i32 level,i32 param,const char* name,u32 notify){f->events.clear();f->announce=notify;return f->controller.begin(variant,level,param,name);}
API(attack_controller_step) void attack_controller_step(attack_controller_test::Fixture* f,float rate,i32 frame){f->events.clear();f->timing={rate,false};f->sequence=frame;f->controller.update();}
API(attack_controller_draw) void attack_controller_draw(attack_controller_test::Fixture* f){f->events.clear();f->controller.draw();}
API(attack_controller_notify) void attack_controller_notify(attack_controller_test::Fixture* f,u32 kind){f->events.clear();f->controller.notify_pattern(kind);}
API(attack_controller_event_count) u32 attack_controller_event_count(attack_controller_test::Fixture* f){return f->events.size();}
API(attack_controller_events) const void* attack_controller_events(attack_controller_test::Fixture* f){return f->events.data();}
API(dialogue_fixture) dialogue_test::Fixture* dialogue_fixture(){return new dialogue_test::Fixture;}
API(dialogue_delete) void dialogue_delete(dialogue_test::Fixture* f){delete f;}
API(dialogue_load) u32 dialogue_load(dialogue_test::Fixture* f,const u8* p,u32 n){return f->resources[0].load(p,n)&&f->resources[1].load(p,n);}
API(dialogue_fields) const void* dialogue_fields(){return dialogue_test::fields;}
API(dialogue_field_count) u32 dialogue_field_count(){return std::size(dialogue_test::fields);}
API(dialogue_part) void* dialogue_part(dialogue_test::Fixture* f,u32 n){switch(n){case 0:return &f->dialogue;case 1:return f->characters;case 2:return f->backgrounds;case 3:return &f->game_over;case 4:return &f->match_complete;case 5:return &f->transition_code;case 6:return &f->random;}return nullptr;}
API(dialogue_begin) u32 dialogue_begin(dialogue_test::Fixture* f,i32 id,i32 flip,u32 victory,i32 opponent){f->clear();return victory?f->dialogue.begin_victory(flip,opponent,f->random):f->dialogue.begin(id,flip);}
API(dialogue_step) i32 dialogue_step(dialogue_test::Fixture* f,u32 held,u32 pressed,float rate,i32 mode,i32 music){f->clear();f->timing={rate,false};f->mode=GameMode(mode);f->stage_music=music;GameInput input;input.held=u16(held);input.pressed=u16(pressed);return f->dialogue.update(input);}
API(dialogue_draw) void dialogue_draw(dialogue_test::Fixture* f){f->clear();f->dialogue.draw();}
API(dialogue_cursor) u32 dialogue_cursor(dialogue_test::Fixture* f){return f->dialogue.cursor;}
API(dialogue_invalid) u32 dialogue_invalid(dialogue_test::Fixture* f){return f->dialogue.invalid;}
API(dialogue_event_count) u32 dialogue_event_count(dialogue_test::Fixture* f){return f->events.size();}
API(dialogue_events) const void* dialogue_events(dialogue_test::Fixture* f){return f->events.data();}
API(dialogue_text_size) u32 dialogue_text_size(dialogue_test::Fixture* f){return f->text_bytes.size();}
API(dialogue_text) const void* dialogue_text(dialogue_test::Fixture* f){return f->text_bytes.data();}
API(dialogue_panel_size) u32 dialogue_panel_size(dialogue_test::Fixture* f){return f->panel.size()*sizeof(AttackColorVertex);}
API(dialogue_panel) const void* dialogue_panel(dialogue_test::Fixture* f){return f->panel.data();}
API(stage_selection_fixture) stage_selection_test::Fixture* stage_selection_fixture(){return new stage_selection_test::Fixture;}
API(stage_selection_delete) void stage_selection_delete(stage_selection_test::Fixture* f){delete f;}
API(stage_selection_fields) const void* stage_selection_fields(){return stage_selection_test::fields;}
API(stage_selection_field_count) u32 stage_selection_field_count(){return std::size(stage_selection_test::fields);}
API(stage_selection_part) void* stage_selection_part(stage_selection_test::Fixture* f,u32 n){switch(n){case 0:return &f->state;case 1:return &f->random;case 2:return &f->state.lives;case 3:return &f->state.continued;case 4:return &f->state.all_unlocked;case 5:return &f->state.route;}return nullptr;}
API(stage_selection_step) u32 stage_selection_step(stage_selection_test::Fixture* f){f->encounters.clear();return StageSelection::select(f->state,f->random,*f);}
API(stage_selection_encounter) i32 stage_selection_encounter(stage_selection_test::Fixture* f,u32 n){return n<f->encounters.size()?f->encounters[n]:-1;}
API(match_scene_fixture) match_scene_test::Fixture* match_scene_fixture(){return new match_scene_test::Fixture;}
API(match_scene_delete) void match_scene_delete(match_scene_test::Fixture* f){delete f;}
API(match_scene_fields) const void* match_scene_fields(){return match_scene_test::fields;}
API(match_scene_field_count) u32 match_scene_field_count(){return std::size(match_scene_test::fields);}
API(match_scene_part) void* match_scene_part(match_scene_test::Fixture* f,u32 n){switch(n){case 0:return &f->scene;case 1:return f->scene.animations.data();case 2:return &f->rules.progress;case 3:return &f->rules.mode;case 4:return f->rules.scores.data();case 5:return &f->game_flags;case 6:return &f->wins_required;case 7:return f->wins;case 8:return &f->starting_extra_lives;case 9:return &f->opponent_character;case 10:return &f->music_track;case 11:return &f->cpu_level;case 12:return &f->route;case 13:return f->players;case 14:return f->geometry;case 15:return &f->dialogue_id;case 16:return f->background;case 17:return &f->transition_code;case 18:return &f->game_over;case 19:return &f->match_complete;case 20:return &f->world.difficulty;}return nullptr;}
API(match_scene_op) void match_scene_op(match_scene_test::Fixture* f,u32 op,i32 arg,float x){f->events.clear();switch(op){case 0:f->scene.initialize();break;case 1:f->scene.reset_round();break;case 2:f->scene.end_round(arg);break;case 3:f->scene.show_results();break;case 4:f->scene.boss_position(arg,{x,81,3});break;case 5:f->scene.update();break;case 6:f->scene.draw();break;}}
API(match_scene_event_count) u32 match_scene_event_count(match_scene_test::Fixture* f){return f->events.size();}
API(match_scene_events) const void* match_scene_events(match_scene_test::Fixture* f){return f->events.data();}
API(background_fixture) background_test::Fixture* background_fixture(){return new background_test::Fixture;}
API(background_delete) void background_delete(background_test::Fixture* f){delete f;}
API(background_fields) const void* background_fields(){return background_test::fields;}
API(background_field_count) u32 background_field_count(){return std::size(background_test::fields);}
API(background_load) u32 background_load(background_test::Fixture* f,const u8* data,u32 size,i32 background){f->events.clear();return f->bg.load(data,size,background);}
API(background_part) void* background_part(background_test::Fixture* f,u32 n){switch(n){case 0:return &f->bg;case 1:return &f->bg.camera;case 2:return f->bg.primitives.data();}return nullptr;}
API(background_count) u32 background_count(background_test::Fixture* f,u32 n){switch(n){case 0:return f->bg.primitives.size();case 1:return f->bg.resource.objects.size();case 2:return f->bg.resource.instances.size();case 3:return f->bg.resource.instructions.size();}return 0;}
API(background_object_flags) u32 background_object_flags(background_test::Fixture* f,u32 n){return f->bg.resource.objects[n].flags;}
API(background_step) void background_step(background_test::Fixture* f,u32 game_flags,u32 field_flags,float rate,u32 force){f->events.clear();f->timing={rate,bool(force)};f->bg.update(game_flags,field_flags);}
API(background_events) const void* background_events(background_test::Fixture* f){return f->events.data();}
API(background_event_count) u32 background_event_count(background_test::Fixture* f){return f->events.size();}
API(background_invalid) u32 background_invalid(background_test::Fixture* f){return f->bg.invalid;}
API(renderer_fixture) renderer_test::Fixture* renderer_fixture(){return new renderer_test::Fixture;}
API(renderer_delete) void renderer_delete(renderer_test::Fixture* f){delete f;}
API(renderer_part) void* renderer_part(renderer_test::Fixture* f,u32 n){switch(n){case 0:return &f->vm;case 1:return &f->sprite;case 2:return &f->renderer.view;case 3:return &f->renderer.shake;case 4:return &f->renderer.tint;case 5:return &f->renderer.tint_enabled;case 6:return f->renderer.quad.data();case 7:return &f->renderer.camera;case 8:return &f->renderer.last_world;case 9:return &f->renderer.texture_matrix;case 10:return &f->renderer.background_camera_data;case 11:return f->matrices;}return nullptr;}
API(renderer_draw) i32 renderer_draw(renderer_test::Fixture* f,u32 kind){f->submitted.clear();f->calls=0;f->vm.loadedSprite=&f->sprite;i32 result=0;switch(kind){case 0:result=f->renderer.draw_no_rotation(f->vm);break;case 1:result=f->renderer.draw_no_rotation(f->vm,true,true);break;case 2:result=f->renderer.draw_2d(f->vm);break;case 3:result=f->renderer.draw_2d(f->vm,true);break;case 4:result=f->renderer.draw_no_rotation(f->vm,false);break;case 5:result=f->renderer.draw_world(f->vm);break;case 6:result=f->renderer.draw_facing_camera(f->vm);break;case 7:result=f->renderer.draw_3d(f->vm);break;}f->renderer.flush();return result;}
API(renderer_data) const void* renderer_data(renderer_test::Fixture* f){return f->submitted.data();}
API(renderer_size) u32 renderer_size(renderer_test::Fixture* f){return f->submitted.size();}
API(renderer_state) u32 renderer_state(renderer_test::Fixture* f,u32 n){switch(n){case 0:return f->renderer.texture;case 1:return u32(f->renderer.state.destinationBlend);case 2:return f->renderer.state.depthWrite;case 3:return f->renderer.blend_mode;case 4:return f->renderer.depth_disabled;}return 0;}
API(graphics_math_op) void graphics_math_op(u32 op,float* out,const float* a,const float* b,const float* d){switch(op){case 0:*reinterpret_cast<Vec3*>(out)=GraphicsMath::normalize(*reinterpret_cast<const Vec3*>(a));break;case 1:*reinterpret_cast<Vec3*>(out)=GraphicsMath::cross(*reinterpret_cast<const Vec3*>(a),*reinterpret_cast<const Vec3*>(b));break;case 2:*reinterpret_cast<Matrix4*>(out)=GraphicsMath::multiply(*reinterpret_cast<const Matrix4*>(a),*reinterpret_cast<const Matrix4*>(b));break;case 3:*reinterpret_cast<Matrix4*>(out)=GraphicsMath::rotation(u32(a[0]),a[1]);break;case 4:*reinterpret_cast<Matrix4*>(out)=GraphicsMath::look_at(*reinterpret_cast<const Vec3*>(a),*reinterpret_cast<const Vec3*>(b),*reinterpret_cast<const Vec3*>(d));break;case 5:*reinterpret_cast<Matrix4*>(out)=GraphicsMath::perspective(a[0],a[1],a[2],a[3]);break;}}
API(graphics_project) void graphics_project(Vec3* out,const Vec3* p,const Viewport* v,const Matrix4* projection,const Matrix4* view,const Matrix4* world){*out=GraphicsMath::project(*p,*v,*projection,*view,*world);}
API(graphics_camera) void graphics_camera(Camera* out,const Viewport* v,const BackgroundCamera* bg,u32 scene){if(scene)out->scene(*v,*bg);else out->screen(*v);}
API(stage_renderer_fixture) stage_renderer_test::Fixture* stage_renderer_fixture(){return new stage_renderer_test::Fixture;}
API(stage_renderer_base) background_test::Fixture* stage_renderer_base(stage_renderer_test::Fixture* f){return f;}
API(stage_renderer_delete) void stage_renderer_delete(stage_renderer_test::Fixture* f){delete f;}
API(stage_renderer_prepare) void stage_renderer_prepare(stage_renderer_test::Fixture* f){f->prepare();}
API(stage_renderer_part) void* stage_renderer_part(stage_renderer_test::Fixture* f,u32 n){switch(n){case 0:return &f->viewport;case 1:return &f->first_camera;case 2:return f->sprites.data();case 3:return &f->camera;case 4:return &f->capture_positions;}return nullptr;}
API(stage_renderer_draw) void stage_renderer_draw(stage_renderer_test::Fixture* f,i32 layer){f->draw(layer);}
API(stage_renderer_data) const void* stage_renderer_data(stage_renderer_test::Fixture* f,u32 n){switch(n){case 0:return f->draws.data();case 1:return f->drawn_vms.data();case 2:return f->quads.data();case 3:return f->captured.data();}return nullptr;}
API(stage_renderer_size) u32 stage_renderer_size(stage_renderer_test::Fixture* f,u32 n){switch(n){case 0:return f->draws.size()*8;case 1:return f->drawn_vms.size()*sizeof(AnmVm);case 2:return f->quads.size()*sizeof(SpriteVertex);case 3:return f->captured.size()*sizeof(Vec3);}return 0;}
API(bg_draw_fixture) background_draw_test::Fixture* bg_draw_fixture(){return new background_draw_test::Fixture;}
API(bg_draw_base) background_test::Fixture* bg_draw_base(background_draw_test::Fixture* f){return f;}
API(bg_draw_delete) void bg_draw_delete(background_draw_test::Fixture* f){delete f;}
API(bg_draw_part) void* bg_draw_part(background_draw_test::Fixture* f,u32 n){switch(n){case 0:return &f->geometry;case 1:return &f->state;case 2:return &f->tint;case 3:return &f->tint_enabled;}return nullptr;}
API(bg_draw_configure) void bg_draw_configure(background_draw_test::Fixture* f,i32 side,u32 fog,u32 custom){f->side=side;f->fog_supported=fog;f->custom=custom;}
API(bg_draw_run) void bg_draw_run(background_draw_test::Fixture* f,u32 phase){f->draw(phase);}
API(bg_draw_count) u32 bg_draw_count(background_draw_test::Fixture* f,u32 n){return n?f->animations.size():f->calls.size();}
API(bg_draw_data) const void* bg_draw_data(background_draw_test::Fixture* f,u32 n){return n?static_cast<const void*>(f->animations.data()):static_cast<const void*>(f->calls.data());}
API(game_resources_fixture) game_resources_test::Fixture* game_resources_fixture(){return new game_resources_test::Fixture;}
API(game_resources_delete) void game_resources_delete(game_resources_test::Fixture* f){delete f;}
API(game_resources_load) u32 game_resources_load(game_resources_test::Fixture* f,const char* name,const u8* bytes,u32 size){return f->load(name,bytes,size);}
API(game_resources_part) void* game_resources_part(game_resources_test::Fixture* f,u32 n){auto* file=f->resources.animation(AnimationFile::menu);switch(n){case 0:return &f->vm;case 1:return &f->random;case 2:return file;case 3:return file?file->rawData:nullptr;case 4:return file?file->sprites:nullptr;}return nullptr;}
API(game_resources_value) u32 game_resources_value(game_resources_test::Fixture* f,u32 n){switch(n){case 0:return f->created;case 1:return f->destroyed;case 2:return f->live.size();case 3:return f->resources.animation(AnimationFile::menu)->scriptCount;}return 0;}
API(game_resources_fail) void game_resources_fail(game_resources_test::Fixture* f,u32 after){f->fail_after=after;}
API(game_resources_start) u32 game_resources_start(game_resources_test::Fixture* f,u32 script,u32 reset){f->executor.invalid=false;return f->resources.start(AnimationFile::menu,f->vm,script,reset);}
API(bullet_draw_fixture) bullet_draw_test::Fixture* bullet_draw_fixture(){return new bullet_draw_test::Fixture;}
API(bullet_draw_delete) void bullet_draw_delete(bullet_draw_test::Fixture* f){delete f;}
API(bullet_draw_part) void* bullet_draw_part(bullet_draw_test::Fixture* f,u32 n,u32 index){switch(n){case 0:return &f->geometry;case 1:return &f->bullets.pool[index];case 2:return f->visuals.instances[index].data();case 3:return &f->lasers.pool[index];case 4:return f->bullets.draw_heads.data();}return nullptr;}
API(bullet_draw_link) void bullet_draw_link(bullet_draw_test::Fixture* f,u32 index,i32 next){f->bullets.pool[index].draw_next=next;}
API(bullet_draw_run) void bullet_draw_run(bullet_draw_test::Fixture* f){f->draw();}
API(bullet_draw_count) u32 bullet_draw_count(bullet_draw_test::Fixture* f,u32 n){return n?f->animations.size():f->order.size();}
API(bullet_draw_data) const void* bullet_draw_data(bullet_draw_test::Fixture* f,u32 n){return n?static_cast<const void*>(f->animations.data()):static_cast<const void*>(f->order.data());}
API(enemy_draw_fixture) enemy_draw_test::Fixture* enemy_draw_fixture(){return new enemy_draw_test::Fixture;}
API(enemy_draw_delete) void enemy_draw_delete(enemy_draw_test::Fixture* f){delete f;}
API(enemy_draw_part) void* enemy_draw_part(enemy_draw_test::Fixture* f,u32 n){switch(n){case 0:return &f->geometry;case 1:return f->value.animation.layers;case 2:return &f->value.values.resolved_position;case 3:return &f->value.values.direction;case 4:return &f->value.values.flags;case 5:return &f->value.behavior_flags;case 6:return f->value.trail.history.data();case 7:return f->value.trail.vertices.data();case 8:return &f->value.trail.flags;case 9:return &f->value.trail.length;case 10:return &f->value.trail.interval;case 11:return &f->sprite;}return nullptr;}
API(enemy_draw_run) void enemy_draw_run(enemy_draw_test::Fixture* f){f->prepare();f->draw();}
API(enemy_draw_count) u32 enemy_draw_count(enemy_draw_test::Fixture* f,u32 n){return n==0?f->order.size():n==1?f->animations.size():f->vertices.size();}
API(enemy_draw_data) const void* enemy_draw_data(enemy_draw_test::Fixture* f,u32 n){return n==0?static_cast<const void*>(f->order.data()):n==1?static_cast<const void*>(f->animations.data()):static_cast<const void*>(f->vertices.data());}
API(battle_fixture) battle_test::Fixture* battle_fixture(){return new battle_test::Fixture;}
API(battle_delete) void battle_delete(battle_test::Fixture* f){delete f;}
API(battle_file) void battle_file(battle_test::Fixture* f,const char* name,const u8* data,u32 size){f->files[name]=std::vector<u8>(data,data+size);}
API(battle_initialize) u32 battle_initialize(battle_test::Fixture* f,i32 a,i32 b,i32 seed,i32 difficulty,i32 controller){return f->initialize(a,b,seed,difficulty,controller);}
API(battle_step) u32 battle_step(battle_test::Fixture* f,u32 left,u32 right,u32 mask){return f->step(u16(left),u16(right),mask);}
API(battle_part) void* battle_part(battle_test::Fixture* f,u32 side,u32 kind){return f->part(side,kind);}
API(battle_values) const u32* battle_values(battle_test::Fixture* f){return f->values();}
API(battle_error) const char* battle_error(battle_test::Fixture* f){return f->battle?f->battle->error.c_str():f->resources.error.c_str();}
API(battle_resource) void* battle_resource(battle_test::Fixture* f,u32 slot,u32 which){auto* a=f->resources.animation(AnimationFile(slot));if(!a)return nullptr;return which==0?static_cast<void*>(a):which==1?a->rawData:static_cast<void*>(a->sprites);}
API(battle_bullet) void* battle_bullet(battle_test::Fixture* f,u32 side,u32 index,u32 which){auto& field=f->battle->fields[side];return which?static_cast<void*>(field.bullet_visuals->instances[index].data()):static_cast<void*>(&field.bullets->pool[index]);}
API(battle_enemy) EclVm* battle_enemy(battle_test::Fixture* f,u32 side,u32 index){return &f->battle->fields[side].enemies->enemies[index];}
API(battle_shot) PlayerShot* battle_shot(battle_test::Fixture* f,u32 side,u32 index){return &f->battle->fields[side].player->shots.shots[index];}
API(hud_fixture) hud_test::Fixture* hud_fixture(){return new hud_test::Fixture;}
API(hud_delete) void hud_delete(hud_test::Fixture* f){delete f;}
API(hud_initialize) u32 hud_initialize(hud_test::Fixture* f,const u8* data,u32 size,i32 side,u32 versus){return f->initialize(data,size,side,versus);}
API(hud_operation) void hud_operation(hud_test::Fixture* f,i32 type,i32 value){f->operation(type,value);}
API(hud_input) void hud_input(hud_test::Fixture* f,const u32* values){f->input(values);}
API(hud_update) void hud_update(hud_test::Fixture* f){f->hud->update(f->frame);}
API(hud_draw) void hud_draw(hud_test::Fixture* f){f->hud->draw(f->frame);}
API(hud_animation) AnmVm* hud_animation(hud_test::Fixture* f,u32 index){return index<63?&f->hud->animations[index]:&f->hud->portraits[index-63];}
API(hud_resource) void* hud_resource(hud_test::Fixture* f,u32 which){auto* a=f->resources.animation(AnimationFile::front);return which==0?static_cast<void*>(a):which==1?a->rawData:static_cast<void*>(a->sprites);}
API(hud_part) void* hud_part(hud_test::Fixture* f,u32 which){return which==0?static_cast<void*>(&f->hud->wipe):which==1?static_cast<void*>(&f->hud->blink):static_cast<void*>(&f->random);}
API(hud_value) i32 hud_value(hud_test::Fixture* f,u32 which){return which?f->hud->wipe_state:f->hud->charge_active;}
API(hud_draws) const void* hud_draws(hud_test::Fixture* f){return f->draws.data();}
API(hud_draw_count) u32 hud_draw_count(hud_test::Fixture* f){return f->draws.size();}
API(hud_vertices) const void* hud_vertices(hud_test::Fixture* f){return f->vertices.data();}
API(hud_vertex_count) u32 hud_vertex_count(hud_test::Fixture* f){return f->vertices.size();}
API(screen_fixture) screen_effects_test::Fixture* screen_fixture(){return new screen_effects_test::Fixture;}
API(screen_delete) void screen_delete(screen_effects_test::Fixture* f){delete f;}
API(screen_create) u32 screen_create(screen_effects_test::Fixture* f,const SceneFade* r){return f->effects.create(*r);}
API(screen_effect) ScreenEffect* screen_effect(screen_effects_test::Fixture* f,u32 n){return &f->effects.effects[n];}
API(screen_part) void* screen_part(screen_effects_test::Fixture* f,u32 n){return n==0?static_cast<void*>(&f->random):n==1?static_cast<void*>(&f->context):static_cast<void*>(f->offsets);}
API(screen_step) u32 screen_step(screen_effects_test::Fixture* f,u32 n){auto& e=f->effects.effects[n];return e.active=f->effects.update_one(e,f->context);}
API(screen_draw) void screen_draw(screen_effects_test::Fixture* f,u32 n){f->rectangles.clear();f->effects.draw_one(f->effects.effects[n]);}
API(screen_rectangles) const void* screen_rectangles(screen_effects_test::Fixture* f){return f->rectangles.data();}
API(screen_rectangle_count) u32 screen_rectangle_count(screen_effects_test::Fixture* f){return f->rectangles.size();}
API(screen_field) u32 screen_field(u32 n){const u32 offsets[]{offsetof(ScreenEffect,time),offsetof(ScreenEffect,opacity),offsetof(ScreenEffect,release),offsetof(ScreenEffectContext,timing),offsetof(ScreenEffectContext,game_flags),offsetof(ScreenEffectContext,field_flags),offsetof(ScreenEffectContext,active_frames),offsetof(ScreenEffectContext,application_transition),offsetof(ScreenEffectContext,paused),offsetof(ScreenEffectContext,game_over)};return offsets[n];}
API(texture_image_fixture) texture_image_test::Fixture* texture_image_fixture(){return new texture_image_test::Fixture;}
API(texture_image_delete) void texture_image_delete(texture_image_test::Fixture* f){delete f;}
API(texture_image_load) u32 texture_image_load(texture_image_test::Fixture* f,const u8* bytes,u32 size,u32 index,u32 low_color){return f->load(bytes,size,index,low_color);}
API(texture_image_value) u32 texture_image_value(texture_image_test::Fixture* f,u32 n){const auto& p=f->image;return n==0?p.width:n==1?p.height:n==2?p.pitch:n==3?u32(p.format):p.pixels.size();}
API(texture_image_pixels) const u8* texture_image_pixels(texture_image_test::Fixture* f){return f->image.pixels.data();}
#include "overlay-fixture.hpp"
#include "sound-fixture.hpp"
API(sound_fixture) sound_test::Fixture* sound_fixture(){return new sound_test::Fixture;}
API(sound_delete) void sound_delete(sound_test::Fixture* f){delete f;}
API(sound_queue) SoundQueue* sound_queue(sound_test::Fixture* f){return &f->sound.state;}
API(sound_enqueue) void sound_enqueue(sound_test::Fixture* f,i32 id,i32 pan){f->sound.enqueue(id,pan);}
API(sound_positioned) void sound_positioned(sound_test::Fixture* f,i32 id,float x){f->sound.positioned(id,x);}
API(sound_process) void sound_process(sound_test::Fixture* f,i32 master,u32 initialized,u32 enabled){f->calls.clear();f->sound.master_volume=master;f->sound.initialized=initialized;f->sound.enabled=enabled;f->sound.process();}
API(sound_definitions_data) const SoundDefinition* sound_definitions_data(){return sound_definitions;}
API(sound_sample) const char* sound_sample(u32 n){return n<39?sound_samples[n]:nullptr;}
API(sound_calls) const void* sound_calls(sound_test::Fixture* f){return f->calls.data();}
API(sound_call_count) u32 sound_call_count(sound_test::Fixture* f){return f->calls.size();}
API(overlay_fixture) overlay_test::Fixture* overlay_fixture(){return new overlay_test::Fixture;}
API(overlay_delete) void overlay_delete(overlay_test::Fixture* f){delete f;}
API(overlay_initialize) u32 overlay_initialize(overlay_test::Fixture* f,const u8* p,u32 size){return f->initialize(p,size);}
API(overlay_part) void* overlay_part(overlay_test::Fixture* f,u32 n){switch(n){case 0:return &f->ascii.glyph;case 1:return &f->ascii.digit;case 2:return f->ascii.popups.data();case 3:return f->ascii.queue.data();case 4:return &f->frame;case 5:return &f->menus.pause;case 6:return &f->menus.game_over;case 7:return &f->menus.match_end;case 8:return &f->random;}return nullptr;}
API(overlay_resource) void* overlay_resource(overlay_test::Fixture* f,u32 n){auto* a=f->resources.animation(AnimationFile::ascii);return n==0?static_cast<void*>(a):n==1?a->rawData:static_cast<void*>(a->sprites);}
API(overlay_popup) void overlay_popup(overlay_test::Fixture* f,i32 side,const Vec3* p,i32 value,u32 color){f->ascii.popup(side,*p,value,color);}
API(overlay_popup_update) void overlay_popup_update(overlay_test::Fixture* f){f->ascii.update(f->frame);}
API(overlay_popup_draw) void overlay_popup_draw(overlay_test::Fixture* f,i32 side,const Vec3* player){f->clear();f->ascii.draw_popups(side,{u32(16+side*320),16,{-144,0},288},*player);}
API(overlay_text) void overlay_text(overlay_test::Fixture* f,const Vec3* p,const char* text,u32 color,float x,float y,i32 view){f->ascii.color=color;f->ascii.scale={x,y};f->ascii.field_view=view;f->ascii.text(*p,text);}
API(overlay_text_draw) void overlay_text_draw(overlay_test::Fixture* f){f->clear();f->ascii.draw_text();}
API(overlay_value) i32 overlay_value(overlay_test::Fixture* f,u32 n){return n==0?f->ascii.popup_cursor:n==1?f->ascii.count:f->continues;}
API(overlay_menu_step) i32 overlay_menu_step(overlay_test::Fixture* f,i32 type,u32 keys){return f->menu_step(type,u16(keys));}
API(overlay_configure) void overlay_configure(overlay_test::Fixture* f,i32 mode,i32 difficulty,i32 continued,u32 flags,u32 capture){f->configure(mode,difficulty,continued,flags,capture);}
API(overlay_drawing_count) u32 overlay_drawing_count(overlay_test::Fixture* f){return f->drawings.size();}
API(overlay_drawings) const AnmVm* overlay_drawings(overlay_test::Fixture* f){return f->drawings.data();}
API(overlay_event_count) u32 overlay_event_count(overlay_test::Fixture* f){return f->events.size();}
API(overlay_events) const i32* overlay_events(overlay_test::Fixture* f){return f->events.data();}
API(world_fixture) world_test::Fixture* world_fixture(){return new world_test::Fixture;}
API(world_delete) void world_delete(world_test::Fixture* f){delete f;}
API(world_file) void world_file(world_test::Fixture* f,const char* name,const u8* data,u32 size){f->files[name]=std::vector<u8>(data,data+size);}
API(world_initialize) u32 world_initialize(world_test::Fixture* f,i32 a,i32 b,i32 seed,i32 mode,i32 controller){return f->initialize(a,b,seed,mode,controller);}
API(world_step) u32 world_step(world_test::Fixture* f,u32 a,u32 b,u32 menu){f->clear();return f->game->update(u16(a),u16(b),u16(menu));}
API(world_part) void* world_part(world_test::Fixture* f,u32 side,u32 kind){return f->part(side,kind);}
API(world_error) const char* world_error(world_test::Fixture* f){return f->game->error.c_str();}
API(world_resource) void* world_resource(world_test::Fixture* f,u32 slot,u32 which){auto* a=f->resources.animation(AnimationFile(slot));if(!a)return nullptr;return which==0?static_cast<void*>(a):which==1?a->rawData:static_cast<void*>(a->sprites);}
API(world_hud) AnmVm* world_hud(world_test::Fixture* f,u32 side,u32 n){auto& h=*f->game->huds[side];return n<63?&h.animations[n]:&h.portraits[n-63];}
API(world_background_animation) AnmVm* world_background_animation(world_test::Fixture* f,u32 side,u32 n){return &f->game->backgrounds[side]->primitives[n];}
API(world_background_count) u32 world_background_count(world_test::Fixture* f,u32 side){return f->game->backgrounds[side]->primitives.size();}
API(world_end_round) void world_end_round(world_test::Fixture* f,u32 winner){f->game->scene->end_round(winner);}
API(world_enemy) EclVm* world_enemy(world_test::Fixture* f,u32 side,u32 n){return &f->game->battle->fields[side].enemies->enemies[n];}
API(world_value) i32 world_value(world_test::Fixture* f,u32 n){auto& g=*f->game;switch(n){case 0:return g.battle->state.flags;case 1:return g.battle->state.scene_phase;case 2:return g.music_track;case 3:return g.transition_pending;case 4:return i32(g.transition);case 5:return g.battle->fields[0].cpu_level;case 6:return g.battle->fields[1].cpu_level;case 7:return g.battle->state.extra_damage;}return 0;}
API(title_fixture) title_test::Fixture* title_fixture(){return new title_test::Fixture;}
API(title_delete) void title_delete(title_test::Fixture* f){delete f;}
API(title_file) void title_file(title_test::Fixture* f,const char* n,const u8* b,u32 size){f->files[n]={b,b+size};}
API(title_initialize) u32 title_initialize(title_test::Fixture* f){return f->menu.initialize();}
API(title_configure) void title_configure(title_test::Fixture* f,i32 difficulty,i32 versus,u32 unlocked,u32 extra){f->configure(difficulty,versus,unlocked,extra);}
API(title_step) u32 title_step(title_test::Fixture* f,u32 a,u32 b,u32 menu){InputFrame input[3];input[0].held=input[0].pressed=u16(a);input[1].held=input[1].pressed=u16(b);input[2].held=input[2].pressed=u16(menu);f->clear();return f->menu.update(input[0],input[1],input[2]);}
API(title_part) void* title_part(title_test::Fixture* f,u32 n){switch(n){case 0:return &f->menu.state;case 1:return f->menu.animations.data();case 2:return f->menu.descriptions.data();case 3:return f->menu.music_comments.data();case 4:return &f->settings;case 5:return &f->random;case 6:return &f->launched;}return nullptr;}
API(title_change) void title_change(title_test::Fixture* f,i32 screen,i32 selection){f->menu.change(TitleScreen(screen));f->menu.state.selection=selection;}
API(title_value) i32 title_value(title_test::Fixture* f,u32 n){return n==0?f->menu.state.description:n==1?i32(f->menu.animations.size()):n==2?f->settings.difficulty:n==3?f->settings.versus:n==4?f->settings.characters[0]:n==5?f->settings.characters[1]:i32(f->menu.leaving);}
API(title_fields) const void* title_fields(){return title_test::fields;}
API(title_field_count) u32 title_field_count(){return sizeof(title_test::fields)/sizeof(title_test::fields[0]);}
API(title_resource) void* title_resource(title_test::Fixture* f,u32 slot,u32 n){auto* a=f->resources.animation(AnimationFile(slot));return !a?nullptr:n==0?static_cast<void*>(a):n==1?a->rawData:static_cast<void*>(a->sprites);}
API(title_error) const char* title_error(title_test::Fixture* f){return f->menu.error.c_str();}
API(title_event_count) u32 title_event_count(title_test::Fixture* f){return f->events.size();}
API(title_events) const i32* title_events(title_test::Fixture* f){return f->events.data();}
API(title_text_size) u32 title_text_size(title_test::Fixture* f){return f->text.size();}
API(title_text) const u8* title_text(title_test::Fixture* f){return f->text.data();}

#include "../../cpp/game/PlayerRecords.hpp"
API(records_create) PlayerRecords* records_create(){return new PlayerRecords;}
API(records_delete) void records_delete(PlayerRecords* p){delete p;}
API(records_load) u32 records_load(PlayerRecords* p,const u8* bytes,u32 size,u32 plain){return plain?p->load_plain(bytes,size):p->load(bytes,size);}
API(records_write) u32 records_write(PlayerRecords* p,u8* out,u32 kind,Rng* rng){if(kind==1){p->write_profile(out);return 508;}if(kind==2){p->write_last_name(out);return 24;}if(kind==3){u32 at=0;for(const auto& c:p->scores)for(const auto& d:c)for(const auto& s:d){s.write(out+at);at+=44;}return at;}const auto bytes=kind==4?p->save(*rng):p->serialize();std::memcpy(out,bytes.data(),bytes.size());return u32(bytes.size());}
API(records_insert) i32 records_insert(PlayerRecords* p,const u8* bytes){ScoreEntry entry;entry.read(bytes);return p->insert(entry);}
API(records_clear_count) u32 records_clear_count(PlayerRecords* p,i32 c,i32 d){return p->clear_count(c,d);}
API(records_action) void records_action(PlayerRecords* p,u32 action,i32 a,i32 b){if(action==0)p->count_clear(a,b);if(action==1)p->count_encounter(a);if(action==2)p->unlock_after_ending(a,b);if(action==3)p->update_application_clock(u32(a));if(action==4)p->update_game_clock(u32(a));}

API(title_records) PlayerRecords* title_records(title_test::Fixture* f){return &f->records;}
API(title_binding_part) void* title_binding_part(title_test::Fixture* f,u32 part){return part==0?static_cast<void*>(f->settings.bindings.data()):part==1?static_cast<void*>(f->menu.edited_bindings.data()):part==2?static_cast<void*>(f->settings.devices.data()):static_cast<void*>(f->settings.auto_focus.data());}
API(title_joy_button) void title_joy_button(title_test::Fixture* f,u32 device,i32 button){if(device<2)f->joy_button[device]=button;}
API(title_score_candidate) void title_score_candidate(title_test::Fixture* f,const u8* bytes,u32 mode){f->menu.score_candidate.read(bytes);f->menu.result_mode=GameMode(mode);}
API(title_ascii_draw) u32 title_ascii_draw(title_test::Fixture* f){f->ascii.clear();f->menu.draw();return f->ascii.size();}
API(title_ascii_value) const void* title_ascii_value(title_test::Fixture* f,u32 n,u32 text){return n<f->ascii.size()?(text?static_cast<const void*>(f->ascii[n].text.c_str()):static_cast<const void*>(&f->ascii[n])):nullptr;}

API(ending_create) ending_test::Fixture* ending_create(){return new ending_test::Fixture;}
API(ending_delete) void ending_delete(ending_test::Fixture* f){delete f;}
API(ending_file) void ending_file(ending_test::Fixture* f,const char* n,const u8* p,u32 size){f->files[n]={p,p+size};}
API(ending_initialize) u32 ending_initialize(ending_test::Fixture* f,i32 character){return f->ending.initialize(character);}
API(ending_step) u32 ending_step(ending_test::Fixture* f,u32 held,u32 pressed,float rate){f->clear();f->ending.timing.rate=f->executor.timing.rate=rate;InputFrame input;input.held=u16(held);input.pressed=u16(pressed);return f->ending.update(input);}
API(ending_part) void* ending_part(ending_test::Fixture* f,u32 n){return n==0?static_cast<void*>(&f->ending.state):n==1?static_cast<void*>(f->ending.animations.data()):static_cast<void*>(&f->random);}
API(ending_value) u32 ending_value(ending_test::Fixture* f,u32 n){return n==0?f->ending.cursor:n==1?f->ending.finished:u32(f->ending.filename=="endstaff.end");}
API(ending_error) const char* ending_error(ending_test::Fixture* f){return f->ending.error.c_str();}
API(ending_fields) const void* ending_fields(){return ending_test::fields;}
API(ending_field_count) u32 ending_field_count(){return sizeof(ending_test::fields)/sizeof(ending_test::fields[0]);}
API(ending_resource) void* ending_resource(ending_test::Fixture* f,u32 slot,u32 n){auto* a=f->resources.animation(AnimationFile(slot));return !a?nullptr:n==0?static_cast<void*>(a):n==1?a->rawData:static_cast<void*>(a->sprites);}
API(ending_events) const i32* ending_events(ending_test::Fixture* f){return f->events.data();}
API(ending_event_count) u32 ending_event_count(ending_test::Fixture* f){return f->events.size();}
API(ending_text) const u8* ending_text(ending_test::Fixture* f){return f->text.data();}
API(ending_text_size) u32 ending_text_size(ending_test::Fixture* f){return f->text.size();}
API(ending_draw) void ending_draw(ending_test::Fixture* f){f->clear();f->ending.draw();}

#include "../../cpp/game/ReplayArchive.hpp"
namespace replay_replay_archive_test {struct Fixture {ReplayArchive archive;ReplayFile file;ReplayMetadata metadata;ReplayRoundSettings round;};}
API(replay_archive_create) replay_replay_archive_test::Fixture* replay_archive_create(){return new replay_replay_archive_test::Fixture;}
API(replay_archive_delete) void replay_archive_delete(replay_replay_archive_test::Fixture* f){delete f;}
API(replay_archive_metadata) ReplayMetadata* replay_archive_metadata(replay_replay_archive_test::Fixture* f){return &f->metadata;}
API(replay_archive_round) ReplayRoundSettings* replay_archive_round(replay_replay_archive_test::Fixture* f){return &f->round;}
API(replay_archive_begin) void replay_archive_begin(replay_replay_archive_test::Fixture* f){f->archive.begin(f->metadata);}
API(replay_archive_stage) u32 replay_archive_stage(replay_replay_archive_test::Fixture* f,u32 stage){return f->archive.begin_stage(stage,f->round);}
API(replay_archive_record) u32 replay_archive_record(replay_replay_archive_test::Fixture* f,u32 flags,u32 paused,const GameInput (*inputs)[3],u32 cpu,u32 rate){const bool modes[2]{bool(cpu&1),bool(cpu&2)};return f->archive.record(flags,paused,*inputs,modes,u8(rate));}
API(replay_archive_finish) u32 replay_archive_finish(replay_replay_archive_test::Fixture* f,Rng* rng,const char* name,const char* date,i32 left,i32 right,u8* output){f->file=f->archive.finish(*rng,name,date,left,right);const auto bytes=f->file.encode();std::memcpy(output,bytes.data(),bytes.size());return bytes.size();}
API(replay_archive_error) const char* replay_archive_error(replay_replay_archive_test::Fixture* f){return f->archive.error.c_str();}

#include "session-fixture.hpp"
API(session_create) session_test::Fixture* session_create(){return new session_test::Fixture;}
API(session_delete) void session_delete(session_test::Fixture* f){delete f;}
API(session_file) void session_file(session_test::Fixture* f,const char* n,const u8* p,u32 size){f->files[n]={p,p+size};}
API(session_play) u32 session_play(session_test::Fixture* f,const u8* p,u32 size,u32 stage){ReplayFile replay;if(!replay.decode(p,size))return 0;f->world.random={};return f->session.play(replay,stage);}
API(session_begin) u32 session_begin(session_test::Fixture* f,i32 character,i32 mode,i32 difficulty,i32 seed){f->world.random={u16(seed),0,0};WorldConfiguration cfg;cfg.selection.mode=GameMode(mode);cfg.selection.characters[0]=character;cfg.selection.difficulty=difficulty;cfg.selection.unlocked.fill(1);cfg.selection.selector=character;ReplayMetadata meta;meta.mode=u8(mode);meta.difficulty=u8(difficulty);meta.versus=1;return f->session.begin(cfg,meta,"26/09/20");}
API(session_step) u32 session_step(session_test::Fixture* f,u32 a,u32 b,u32 menu){f->clear();return f->session.update(u16(a),u16(b),u16(menu));}
API(session_part) void* session_part(session_test::Fixture* f,u32 side,u32 kind){return f->session.world?f->part_of(*f->session.world,side,kind):nullptr;}
API(session_laser) const Laser* session_laser(session_test::Fixture* f,u32 side,u32 index){return &f->session.world->battle->fields[side].lasers->pool[index];}
API(session_error) const char* session_error(session_test::Fixture* f){return f->session.error.c_str();}
API(session_resource) void* session_resource(session_test::Fixture* f,u32 slot,u32 which){auto* a=f->resources.animation(AnimationFile(slot));return !a?nullptr:which==0?static_cast<void*>(a):which==1?a->rawData:static_cast<void*>(a->sprites);}
API(session_hud) AnmVm* session_hud(session_test::Fixture* f,u32 side,u32 n){auto& h=*f->session.world->huds[side];return n<63?&h.animations[n]:&h.portraits[n-63];}
API(session_value) i32 session_value(session_test::Fixture* f,u32 n){auto& s=f->session;if(n==0)return i32(s.phase);if(n==1)return s.frames;if(n==2)return s.replay_frame();if(n==3)return s.replay_length();if(n==4)return s.continues;return s.world?s.world->configuration.selection.stage:-1;}
API(session_end_round) void session_end_round(session_test::Fixture* f,i32 winner){auto& g=*f->session.world;g.scene->end_round(winner);}
API(session_continue) u32 session_continue(session_test::Fixture* f){return f->session.continue_game();}
API(session_retry) u32 session_retry(session_test::Fixture* f){return f->session.retry();}
API(session_save) u32 session_save(session_test::Fixture* f,const char* name,u8* out,u32 capacity){auto file=f->session.save_replay(name);auto bytes=file.encode();if(bytes.size()>capacity)return 0;std::memcpy(out,bytes.data(),bytes.size());return bytes.size();}


#include "../../cpp/game/GameConfiguration.hpp"
API(configuration_create) GameConfiguration* configuration_create(){return new GameConfiguration;}
API(configuration_delete) void configuration_delete(GameConfiguration* p){delete p;}
API(configuration_data) const u8* configuration_data(GameConfiguration* p){return p->data().data();}
API(configuration_load) u32 configuration_load(GameConfiguration* p,const u8* data,u32 size){return p->load(data,size);}
API(configuration_roundtrip) u32 configuration_roundtrip(GameConfiguration* p){TitleSettings s;p->apply(s);return p->capture(s);}

#include "../../cpp/game/KeyboardInput.hpp"
API(keyboard_input_mask) u32 keyboard_input_mask(const bool (*keys)[256],u32 device){return keyboard_input(*keys,device);}

API(session_demo) u32 session_demo(session_test::Fixture* f,const u8* bytes,u32 size){ReplayFile replay;if(!replay.decode(bytes,size))return 0;return f->session.play(replay,9,true);}
API(session_result) void session_result(session_test::Fixture* f,u8* bytes){f->session.result.write(bytes);}

#include "../../cpp/game/NetworkInput.hpp"
API(network_create) NetworkInput* network_create(u32 side,u32 delay){auto* p=new NetworkInput;p->begin(i32(side),delay);return p;}
API(network_delete) void network_delete(NetworkInput* p){delete p;}
API(network_submit) u32 network_submit(NetworkInput* p,u32 side,u32 frame,u32 keys){return p->submit(i32(side),frame,u16(keys));}
API(network_take) u32 network_take(NetworkInput* p,u16 (*keys)[3]){return p->take(*keys);}
API(network_frame) u32 network_frame(NetworkInput* p){return p->frame();}
API(network_wants) u32 network_wants(NetworkInput* p){return p->wants_input();}
API(network_sending) u32 network_sending(NetworkInput* p){return p->sending_frame();}

API(session_warm) void session_warm(session_test::Fixture* f){f->session.warm_resources();}
API(game_resources_preload) void game_resources_preload(game_resources_test::Fixture* f,const char* name,const u8* bytes,u32 size){f->name=name;f->bytes.assign(bytes,bytes+size);f->resources.preload(AnimationFile::menu,name);}
API(game_resources_warm) u32 game_resources_warm(game_resources_test::Fixture* f){return f->resources.warm_one();}
API(game_resources_cancel) void game_resources_cancel(game_resources_test::Fixture* f){f->resources.cancel_preload();}

API(lzss_stream_create) LzssStream* lzss_stream_create(){return new LzssStream;}
API(lzss_stream_delete) void lzss_stream_delete(LzssStream* p){delete p;}
API(lzss_stream_step) u32 lzss_stream_step(LzssStream* p,const u8* in,u32 size,u8* out,u32 capacity,u32 budget){return p->step(in,size,out,capacity,budget);}
API(lzss_stream_done) u32 lzss_stream_done(LzssStream* p){return p->done();}
API(lzss_stream_size) u32 lzss_stream_size(LzssStream* p){return p->size();}
API(archive_packed) u32 archive_packed(Archive* p,const char* name,u8* out,u32* size){std::vector<u8> bytes;if(!p->packed(name,bytes,*size))return 0;std::memcpy(out,bytes.data(),bytes.size());return bytes.size();}

#include "../world-snapshot.hpp"
API(session_snapshot) const i32* session_snapshot(session_test::Fixture* f){return audit::snapshot(f->session.world.get(),f->world);}

API(session_begin_human) u32 session_begin_human(session_test::Fixture* f,i32 seed){f->world.random={u16(seed),0,0};WorldConfiguration cfg;cfg.selection.mode=GameMode::versus;cfg.selection.characters[0]=0;cfg.selection.characters[1]=1;cfg.selection.difficulty=1;cfg.selection.unlocked.fill(1);cfg.controllers[0]=cfg.controllers[1]=0;ReplayMetadata meta;meta.mode=2;meta.difficulty=1;meta.versus=0;return f->session.begin(cfg,meta,"26/09/20");}
API(session_motion) void session_motion(session_test::Fixture* f,u32 side,u32 enabled,float x,float y){if(side<2)f->session.motion_input[side]={enabled!=0,x,y};}
