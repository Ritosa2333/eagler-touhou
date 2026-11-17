#include "../../cpp/game/Rng.hpp"
#include "../../cpp/game/Archive.hpp"
#include "../../cpp/game/ResourceCrypt.hpp"
#include "../../cpp/game/AnmResource.hpp"
#include "../../cpp/game/EclResource.hpp"
#include "../../cpp/game/StageResource.hpp"
#include "../../cpp/game/Replay.hpp"
#include "../../cpp/game/Timer.hpp"
#include "../../cpp/game/MenuCursor.hpp"
#include "../../cpp/game/AnmVm.hpp"
#include "../../cpp/game/AnmRegistry.hpp"
#include "../../cpp/game/AnmManager.hpp"
#include "../../cpp/game/AnmRenderer.hpp"
#include "../../cpp/game/TitleMainMenu.hpp"
#include "../../cpp/game/TitleSelectionMenu.hpp"
#include "../../cpp/game/TitleAnimalMenu.hpp"
#include "../../cpp/game/TextureImage.hpp"
#include "../../cpp/game/EclContext.hpp"
#include "../../cpp/game/EclThreads.hpp"
#include "../../cpp/game/ShtResource.hpp"
#include "../../cpp/game/GameInput.hpp"
#include "../../cpp/game/FrameScheduler.hpp"
#include "../../cpp/game/PlayerAnmHost.hpp"
#include "../../cpp/game/PlayerOptions.hpp"
#include "../../cpp/game/MotionState.hpp"
#include "../../cpp/game/PositionInterpolation.hpp"
#include "../../cpp/game/PlayerCollision.hpp"
#include "../../cpp/game/PlayerShooting.hpp"
#include "../../cpp/game/DamageSources.hpp"
#include "../../cpp/game/PlayerShots.hpp"
#include "../../cpp/game/PlayerLife.hpp"
#include "../../cpp/game/PlayerBounds.hpp"
#include <algorithm>
#include <cstdlib>
using namespace th17;
#define API(name) extern "C" __attribute__((export_name(#name)))
struct TitleMainFixture final:TitleGameStartServices {
    TitleState state;GameSelection selection;TitleSelectionSettings settings;std::vector<i32> sounds;
    TitleRecords records;AnmManager& animations;TitleMainMenu menu;TitleDifficultyMenu difficulty;TitleCharacterMenu character;TitleAnimalMenu animal;TitleStageMenu stage;
    std::vector<i32> events;i32 checkpoint=0;
    bool sound(i32 id)override{sounds.push_back(id);return true;}
    bool fade_menu_music()override{events.push_back(1);return true;}
    bool show_loading_indicator()override{events.push_back(2);return true;}
    bool begin_transition(u32& animation)override{events.push_back(3);animation=0;return true;}
    bool start_game(i32 number)override{events.push_back(4);events.push_back(number);return true;}
    i32 checkpoint_key()const override{return checkpoint;}
    explicit TitleMainFixture(AnmManager& a):animations(a),menu(state,selection,settings,a),difficulty(state,selection,settings,a),character(state,selection,settings,a),animal(state,selection,settings,a,*this),stage(state,selection,settings,a,*this){auto output=[&](i32 id){return sound(id);};menu.play_sound=output;difficulty.play_sound=output;character.play_sound=output;}
};
API(allocate) void* allocate(u32 n){return std::malloc(n?n:1);}
API(deallocate) void deallocate(void* p){std::free(p);}
API(rng_create) Rng* rng_create(){return new Rng;}
API(rng_delete) void rng_delete(Rng* p){delete p;}
API(rng_next) u32 rng_next(Rng* p,u32 kind){if(kind==0)return p->next16();if(kind==1)return p->next32();return float_to_bits(kind==2?p->unit():p->signed_unit());}
API(crypt) u32 crypt(const u8* in,u8* out,u32 n,u32 key,u32 step,u32 block,u32 limit,u32 encode){return resource_crypt(in,out,n,{u8(key),u8(step),block,limit},encode!=0);}
API(codec_create) Lzss* codec_create(){return new Lzss;}
API(codec_delete) void codec_delete(Lzss* p){delete p;}
API(codec_decode) i32 codec_decode(Lzss* p,const u8* in,u32 n,u8* out,u32 capacity){u32 written=0;return p->decode(in,n,out,capacity,written)?i32(written):-1;}
API(codec_encode) i32 codec_encode(Lzss* p,const u8* in,u32 n,u8* out,u32 capacity){auto b=p->encode(in,n);if(b.size()>capacity)return -1;std::memcpy(out,b.data(),b.size());return b.size();}
API(archive_create) Archive* archive_create(){return new Archive;}
API(archive_delete) void archive_delete(Archive* p){delete p;}
API(archive_open) u32 archive_open(Archive* p,const u8* in,u32 n){return p->open(in,n);}
API(archive_count) u32 archive_count(Archive* p){return p->entries.size();}
API(archive_name) const char* archive_name(Archive* p,u32 n){return n<p->entries.size()?p->entries[n].name.c_str():nullptr;}
API(archive_field) u32 archive_field(Archive* p,u32 n,u32 field){if(n>=p->entries.size())return 0;const auto& e=p->entries[n];return field==0?e.offset:field==1?e.size:field==2?e.compressed:e.reserved;}
API(archive_read) i32 archive_read(Archive* p,u32 n,u8* out,u32 capacity){std::vector<u8> data;if(!p->read(n,data)||data.size()>capacity)return -1;std::memcpy(out,data.data(),data.size());return data.size();}
API(anm_create) AnmResource* anm_create(){return new AnmResource;}
API(anm_delete) void anm_delete(AnmResource* p){delete p;}
API(anm_open) u32 anm_open(AnmResource* p,const u8* in,u32 n){return p->open(in,n);}
API(anm_count) u32 anm_count(AnmResource* p,u32 kind){return kind==0?p->textures.size():kind==1?p->sprites.size():p->scripts.size();}
API(anm_sprite) const AnmSprite* anm_sprite(AnmResource* p,u32 n){return n<p->sprites.size()?&p->sprites[n]:nullptr;}
API(anm_script) const u8* anm_script(AnmResource* p,u32 n){return n<p->scripts.size()?p->scripts[n].bytes.data():nullptr;}
API(anm_script_size) u32 anm_script_size(AnmResource* p,u32 n){return n<p->scripts.size()?p->scripts[n].bytes.size():0;}
API(ecl_create) EclResource* ecl_create(){return new EclResource;}
API(ecl_delete) void ecl_delete(EclResource* p){delete p;}
API(ecl_open) u32 ecl_open(EclResource* p,const u8* in,u32 n){return p->open(in,n);}
API(ecl_count) u32 ecl_count(EclResource* p,u32 kind){return kind==0?p->subroutines.size():kind==1?p->animations.size():p->includes.size();}
API(ecl_name) const char* ecl_name(EclResource* p,u32 kind,u32 n){return kind==0?(n<p->subroutines.size()?p->subroutines[n].name.c_str():nullptr):kind==1?(n<p->animations.size()?p->animations[n].c_str():nullptr):(n<p->includes.size()?p->includes[n].c_str():nullptr);}
API(program_create) EclProgram* program_create(){return new EclProgram;}
API(program_delete) void program_delete(EclProgram* p){delete p;}
API(program_attach) i32 program_attach(EclProgram* p,const u8* data,u32 n){return p->attach(data,n);}
API(program_count) u32 program_count(EclProgram* p){return p->definitions.size();}
API(program_name) const char* program_name(EclProgram* p,u32 n){return n<p->definitions.size()?p->subroutine(n).name.c_str():nullptr;}
API(program_instruction) const EclInstruction* program_instruction(EclProgram* p,u32 n){if(n>=p->definitions.size())return nullptr;const auto& def=p->definitions[n];return reinterpret_cast<const EclInstruction*>(def.file->bytes.data()+p->subroutine(n).offset+16);}
API(program_find) const EclInstruction* program_find(EclProgram* p,const char* name){return p->find(name);}
API(anm_vm_interpolation) const void* anm_vm_interpolation(AnmVm* p,u32 kind){auto& i=p->interpolators;switch(kind){case 0:return &i.position;case 1:return &i.color;case 2:return &i.alpha;case 3:return &i.rotation;case 4:return &i.angle;case 5:return &i.scale;case 6:return &i.secondary_scale;case 7:return &i.uv_scale;case 8:return &i.secondary_color;case 9:return &i.secondary_alpha;case 10:return &i.uv_x;default:return &i.uv_y;}}
API(std_create) StageResource* std_create(){return new StageResource;}
API(std_delete) void std_delete(StageResource* p){delete p;}
API(std_open) u32 std_open(StageResource* p,const u8* in,u32 n){return p->open(in,n);}
API(std_count) u32 std_count(StageResource* p,u32 kind){return kind==0?p->objects.size():kind==1?p->instances.size():p->instructions.size();}
API(replay_create) Replay* replay_create(){return new Replay;}
API(replay_delete) void replay_delete(Replay* p){delete p;}
API(replay_open) u32 replay_open(Replay* p,const u8* in,u32 n){return p->open(in,n);}
API(replay_error) const char* replay_error(Replay* p){return p->error().c_str();}
API(replay_data) const u8* replay_data(Replay* p){return p->decoded().data();}
API(replay_size) u32 replay_size(Replay* p){return p->decoded().size();}
API(replay_field) u32 replay_field(Replay* p,u32 kind){return kind==0?p->stage_count():kind==1?p->character():kind==2?p->animal():kind==3?p->difficulty():p->mode_flags();}
API(replay_frame) u32 replay_frame(Replay* p){return p->frame();}
API(replay_web_rules) u32 replay_web_rules(Replay* p){return p->historical_web_recording()?1:p->web_rules_revision();}
API(replay_select) u32 replay_select(Replay* p,u32 number){return p->select(number);}
API(replay_tick) const ReplayInput* replay_tick(Replay* p,u32 active){static ReplayInput out;out=p->tick(active!=0);return &out;}
API(timer_tick) void timer_tick(Timer* p,float speed){p->tick(&speed);}
API(timer_add) void timer_add(Timer* p,float frames){p->add(frames);}
API(cursor_operation) i32 cursor_operation(MenuCursor* p,u32 kind,i32 value){if(kind==0)return p->step(value);if(kind==1)return p->select(value);if(kind==2)p->push();else p->pop();return p->selected;}
API(variables_create) AnmVariables* variables_create(){return new AnmVariables;}
API(variables_delete) void variables_delete(AnmVariables* p){delete p;}
API(variables_integer) i32 variables_integer(AnmVariables* p,Rng* rng,i32 value){return p->integer(value,*rng);}
API(variables_float) u32 variables_float(AnmVariables* p,Rng* rng,float value){return float_to_bits(p->floating(value,*rng));}
API(variables_destination) const void* variables_destination(AnmVariables* p,void* argument,u32 floating){return floating?static_cast<void*>(p->float_destination(static_cast<float*>(argument))):static_cast<void*>(p->integer_destination(static_cast<i32*>(argument)));}
API(anm_vm_create) AnmVm* anm_vm_create(){return new AnmVm;}
API(anm_vm_delete) void anm_vm_delete(AnmVm* p){delete p;}
API(anm_vm_bind) u32 anm_vm_bind(AnmVm* p,AnmResource* resource,u32 index){return p->bind(*resource,index);}
API(anm_vm_tick) i32 anm_vm_tick(AnmVm* p,Rng* rng,float rate){return p->tick(*rng,rate);}
API(anm_vm_error) const char* anm_vm_error(AnmVm* p){return p->error.c_str();}
API(anm_vm_pending_interrupt) i32 anm_vm_pending_interrupt(AnmVm* p){return p->pending_interrupt;}
API(anm_vm_set_interrupt) void anm_vm_set_interrupt(AnmVm* p,i32 value){p->pending_interrupt=value;}
API(anm_vm_translation) Vec3* anm_vm_translation(AnmVm* p){return &p->visual.translation;}
API(anm_vm_set_layer) void anm_vm_set_layer(AnmVm* p,i32 value){p->visual.set_layer(value);}
API(anm_vm_offset) i32 anm_vm_offset(AnmVm* p){return p->instruction_offset;}
API(anm_vm_source_script) i32 anm_vm_source_script(AnmVm* p){return p->source_script;}
API(anm_vm_timer) const Timer* anm_vm_timer(AnmVm* p){return &p->timer;}
API(anm_vm_variables) AnmVariables* anm_vm_variables(AnmVm* p){return &p->variables;}
API(anm_vm_visual) AnmVisualState* anm_vm_visual(AnmVm* p){return &p->visual;}
API(anm_fixture_create) AnmResource* anm_fixture_create(const u8* in,u32 n){auto* p=new AnmResource;AnmScript script;script.bytes.assign(in,in+n);p->scripts.push_back(std::move(script));return p;}
API(anm_vm_resource) void anm_vm_resource(AnmVm* p,AnmResource* resource){p->resource=resource;}
API(anm_vm_select_sprite) u32 anm_vm_select_sprite(AnmVm* p,i32 n){return p->select_sprite(n);}
API(anm_vm_sprite_values) const u8* anm_vm_sprite_values(AnmVm* p){static u8 out[232];u8* at=out;auto copy=[&](const void* in,u32 n){std::memcpy(at,in,n);at+=n;};copy(p->visual.uv,32);copy(&p->visual.sprite_size,8);copy(&p->visual.sprite_matrix,64);copy(&p->visual.transform_matrix,64);copy(&p->visual.uv_matrix,64);return out;}
struct AnmRegistryFixture {AnmRegistry registry;Rng random;};
API(anm_registry_create) AnmRegistryFixture* anm_registry_create(){return new AnmRegistryFixture;}
API(anm_registry_delete) void anm_registry_delete(AnmRegistryFixture* p){delete p;}
API(anm_registry_allocate) AnmVm* anm_registry_allocate(AnmRegistryFixture* p){return p->registry.allocate();}
API(anm_registry_submit) u32 anm_registry_submit(AnmRegistryFixture* p,AnmVm* vm,u32 ordering){return p->registry.submit(*vm,ordering);}
API(anm_registry_find) AnmVm* anm_registry_find(AnmRegistryFixture* p,u32 handle){return p->registry.find(handle);}
API(anm_registry_generation) u32* anm_registry_generation(AnmRegistryFixture* p){return &p->registry.generation_counter();}
API(anm_registry_slot) u32 anm_registry_slot(AnmRegistryFixture* p,AnmVm* vm){return p->registry.slot(*vm);}
API(anm_registry_release) u32 anm_registry_release(AnmRegistryFixture* p,AnmVm* vm,u32 recursive){return recursive?p->registry.destroy_tree(*vm):p->registry.discard(*vm);}
API(anm_registry_update) u32 anm_registry_update(AnmRegistryFixture* p,u32 alternate,float rate){return p->registry.update(alternate!=0,p->random,rate);}
API(anm_registry_attach) u32 anm_registry_attach(AnmRegistryFixture* p,AnmVm* child,AnmVm* parent){return p->registry.attach(*child,*parent);}
API(anm_registry_pause) u32 anm_registry_pause(AnmRegistryFixture* p,AnmVm* vm,u32 paused){return p->registry.pause_tree(*vm,paused!=0);}
API(anm_registry_retire) u32 anm_registry_retire(AnmRegistryFixture* p,AnmVm* vm){vm->visual.render_flags=(vm->visual.render_flags&~64u)|32;return p->registry.retire_children(*vm);}
API(anm_registry_interrupt) u32 anm_registry_interrupt(AnmRegistryFixture* p,AnmVm* vm,i32 label,u32 immediate,float rate){return p->registry.interrupt(*vm,label,p->random,rate,immediate!=0);}
API(anm_registry_count) u32 anm_registry_count(AnmRegistryFixture* p,u32 alternate){return alternate<2?p->registry.ordered_count(alternate!=0):p->registry.count();}
API(anm_registry_order) u32 anm_registry_order(AnmRegistryFixture* p,u32 alternate,u32 index){return p->registry.ordered_handle(alternate!=0,index);}
API(anm_registry_layer_count) u32 anm_registry_layer_count(AnmRegistryFixture* p,u32 layer){return p->registry.layer(layer).size();}
API(anm_registry_layer_handle) u32 anm_registry_layer_handle(AnmRegistryFixture* p,u32 layer,u32 index){return p->registry.handle(*p->registry.layer(layer)[index]);}
API(anm_registry_error) const char* anm_registry_error(AnmRegistryFixture* p){return p->registry.error.c_str();}
struct AnmManagerFixture {Rng random;AnmEnvironment environment;AnmManager manager{random,environment};};
API(anm_manager_create) AnmManagerFixture* anm_manager_create(){return new AnmManagerFixture;}
API(anm_manager_delete) void anm_manager_delete(AnmManagerFixture* p){delete p;}
API(anm_manager_load) u32 anm_manager_load(AnmManagerFixture* p,i32 id,const u8* bytes,u32 size){return p->manager.load(id,bytes,size);}
API(anm_manager_template) AnmVm* anm_manager_template(AnmManagerFixture* p,i32 id,u32 script){return p->manager.script_template(id,script);}
API(anm_manager_bind) u32 anm_manager_bind(AnmManagerFixture* p,AnmVm* vm,i32 id,i32 script){return p->manager.bind_template(*vm,id,script);}
API(anm_manager_spawn) u32 anm_manager_spawn(AnmManagerFixture* p,i32 id,i32 script,i32 layer,u32 ordering){return p->manager.create(id,script,layer,ordering);}
API(anm_manager_find) AnmVm* anm_manager_find(AnmManagerFixture* p,u32 handle){return p->manager.registry.find(handle);}
API(anm_manager_destroy_tree) u32 anm_manager_destroy_tree(AnmManagerFixture* p,AnmVm* vm){return p->manager.registry.destroy_tree(*vm);}
API(anm_manager_retire) u32 anm_manager_retire(AnmManagerFixture* p,u32 handle){return p->manager.retire(handle);}
API(anm_manager_child) AnmVm* anm_manager_child(AnmManagerFixture* p,AnmVm* source,i32 script,u32 ordering,u32 detached){return detached?p->manager.spawn_detached(*source,script):p->manager.spawn_child(*source,script,ordering);}
API(anm_manager_rebind) u32 anm_manager_rebind(AnmManagerFixture* p,u32 handle,i32 script){return p->manager.rebind(handle,script)?handle:0;}
API(anm_manager_registry) AnmRegistry* anm_manager_registry(AnmManagerFixture* p){return &p->manager.registry;}
API(anm_manager_random) Rng* anm_manager_random(AnmManagerFixture* p){return &p->random;}
API(anm_manager_environment) AnmEnvironment* anm_manager_environment(AnmManagerFixture* p){return &p->environment;}
API(anm_manager_fallback) u32 anm_manager_fallback(AnmManagerFixture* p,i32 id){p->environment.fallback_sprite_resource=p->manager.resource(id);return p->environment.fallback_sprite_resource!=nullptr;}
API(anm_manager_update) u32 anm_manager_update(AnmManagerFixture* p,u32 alternate,float rate){p->manager.rate=rate;return p->manager.update(alternate!=0);}
API(anm_manager_order) u32 anm_manager_order(AnmManagerFixture* p,u32 alternate,u32 index){return p->manager.registry.ordered_handle(alternate!=0,index);}
API(anm_manager_count) u32 anm_manager_count(AnmManagerFixture* p,u32 alternate){return alternate<2?p->manager.registry.ordered_count(alternate!=0):p->manager.registry.count();}
API(anm_manager_error) const char* anm_manager_error(AnmManagerFixture* p){return p->manager.error.c_str();}
API(anm_manager_unload) void anm_manager_unload(AnmManagerFixture* p,i32 id){p->manager.unload(id);}
API(anm_manager_collect_resources) void anm_manager_collect_resources(AnmManagerFixture* p){p->manager.collect_resources();}
API(anm_manager_resource_count) u32 anm_manager_resource_count(AnmManagerFixture* p){return p->manager.resource_count();}
API(anm_manager_resource) AnmResource* anm_manager_resource(AnmManagerFixture* p,i32 id){return p->manager.resource(id);}
API(anm_manager_retired_resources) u32 anm_manager_retired_resources(AnmManagerFixture* p){return p->manager.retired_resource_count();}
API(anm_manager_name) u32 anm_manager_name(AnmManagerFixture* p,i32 id,const char* name){return p->manager.name_resource(id,name);}

struct GraphicsCaptureState {std::array<Matrix4,4> matrices;std::vector<u32> states;u32 binding;std::array<u32,4> alpha;};
struct GraphicsCapture final:ZunGraphics {
    touhou::graphics::PipelineState state;std::vector<AnmGeometryVertex> vertices;std::vector<u8> bytes;std::vector<std::vector<u8>> history;std::array<Matrix4,4> matrices;std::vector<u32> states;std::vector<GraphicsCaptureState> submissions;u32 calls=0,binding=0,layout=0,topology=0,primitive_count=0,vertex_stride=0;
    bool targets_enabled=false;u32 target=0;std::vector<u32> target_events;
    void target_event(u32 kind,u32 color,const GraphicsViewport* rect){target_events.insert(target_events.end(),{kind,target,color,rect?1u:0u,rect?rect->x:0u,rect?rect->y:0u,rect?rect->width:0u,rect?rect->height:0u});}
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource&,u32 index)override{return index+1;}void bind_texture(u32 handle)override{binding=handle;}
    void set_layout(touhou::graphics::VertexLayout value)override{layout=u32(value);}void set_matrix(touhou::graphics::MatrixKind kind,const Matrix4& value)override{matrices[u32(kind)]=value;}
    void capture(u32 n,const void* data,u32 stride){calls++;vertex_stride=stride;bytes.assign(static_cast<const u8*>(data),static_cast<const u8*>(data)+n*stride);history.push_back(bytes);vertices.resize(n);for(u32 i=0;i<n;i++)std::memcpy(&vertices[i],static_cast<const u8*>(data)+i*stride,std::min<u32>(sizeof(AnmGeometryVertex),stride));states={state.alphaTest,u32(state.sourceBlend),u32(state.destinationBlend),u32(state.blendEquation),u32(state.minFilter),u32(state.addressU),u32(state.addressV),state.depthWrite,state.textureTransform,state.textureFactor,state.fog,layout,topology,primitive_count,vertex_stride};submissions.push_back({matrices,states,binding,{state.separateAlphaBlend,u32(state.sourceAlphaBlend),u32(state.destinationAlphaBlend),u32(state.alphaBlendEquation)}});}
    void triangles(u32 count,const void* data,u32 stride)override{topology=u32(touhou::graphics::Topology::Triangles);primitive_count=count;capture(count*3,data,stride);}void primitives(touhou::graphics::Topology kind,u32 count,const void* data,u32 stride)override{topology=u32(kind);primitive_count=count;capture(touhou::graphics::vertex_count(kind,count),data,stride);}
    bool select_target(const AnmResource* file,u32 index)override{if(!targets_enabled)return false;target=file?index+1:0;target_event(0,0,nullptr);return true;}
    bool clear_target(u32 color,const GraphicsViewport* rect)override{if(!targets_enabled)return false;target_event(1,color,rect);return true;}
    bool clear_depth(const GraphicsViewport* rect)override{if(!targets_enabled)return false;target_event(2,0,rect);return true;}void set_viewport(const GraphicsViewport&)override{}
};
struct AnmRendererFixture {GraphicsCapture graphics;AnmRenderer renderer{graphics};AnmRendererFixture(){graphics.configure_game(1);}};
API(anm_renderer_create) AnmRendererFixture* anm_renderer_create(){return new AnmRendererFixture;}
API(anm_renderer_delete) void anm_renderer_delete(AnmRendererFixture* p){delete p;}
API(anm_renderer_draw) i32 anm_renderer_draw(AnmRendererFixture* p,AnmVm* vm,u32 flush){const int result=p->renderer.draw(*vm);if(flush)p->renderer.flush();return result;}
API(anm_renderer_offset) Vec2* anm_renderer_offset(AnmRendererFixture* p){return &p->renderer.offset;}
API(anm_renderer_tint) void anm_renderer_tint(AnmRendererFixture* p,u32 enabled,u32 color){p->renderer.tint_enabled=enabled!=0;p->renderer.tint=color;}
API(anm_renderer_reset) void anm_renderer_reset(AnmRendererFixture* p){p->renderer.invalidate();p->graphics.vertices.clear();p->graphics.bytes.clear();p->graphics.history.clear();p->graphics.submissions.clear();p->graphics.states.clear();p->graphics.calls=0;p->renderer.error.clear();}
API(anm_renderer_vertices) const AnmGeometryVertex* anm_renderer_vertices(AnmRendererFixture* p){return p->graphics.vertices.data();}
API(anm_renderer_count) u32 anm_renderer_count(AnmRendererFixture* p){return p->graphics.vertices.size();}
API(anm_renderer_calls) u32 anm_renderer_calls(AnmRendererFixture* p){return p->graphics.calls;}
API(anm_renderer_states) const u32* anm_renderer_states(AnmRendererFixture* p){return p->graphics.states.data();}
API(anm_renderer_bytes) const u8* anm_renderer_bytes(AnmRendererFixture* p){return p->graphics.bytes.data();}
API(anm_renderer_history_state) const u32* anm_renderer_history_state(AnmRendererFixture* p,u32 index){return p->graphics.submissions[index].states.data();}
API(anm_renderer_history) const u8* anm_renderer_history(AnmRendererFixture* p,u32 index){return p->graphics.history[index].data();}
API(anm_renderer_matrix) const Matrix4* anm_renderer_matrix(AnmRendererFixture* p,u32 kind){return &p->graphics.matrices[kind];}
API(anm_renderer_camera) AnmCamera* anm_renderer_camera(AnmRendererFixture* p){return &p->renderer.camera;}
API(anm_renderer_error) const char* anm_renderer_error(AnmRendererFixture* p){return p->renderer.error.c_str();}
API(anm_vm_inherited_color) u32* anm_vm_inherited_color(AnmVm* p){return &p->visual.inherited_color;}
API(anm_vm_quad) const Vec3* anm_vm_quad(AnmVm* p){return p->visual.quad.data();}
API(anm_vm_creation_parent) void anm_vm_creation_parent(AnmVm* p,AnmVm* parent){p->creation_parent=parent;}
API(anm_environment_scale) float* anm_environment_scale(AnmEnvironment* p){return &p->resolution_scale;}
API(anm_environment_offsets) i32* anm_environment_offsets(AnmEnvironment* p){return p->screen_offsets.data();}
API(anm_environment_delta) Vec3* anm_environment_delta(AnmEnvironment* p){return &p->background_delta;}
API(anm_environment_pause) void anm_environment_pause(AnmEnvironment* p,u32 paused){p->paused=paused!=0;}
API(anm_environment_create) AnmEnvironment* anm_environment_create(){return new AnmEnvironment;}
API(anm_environment_delete) void anm_environment_delete(AnmEnvironment* p){delete p;}
API(anm_vm_environment) void anm_vm_environment(AnmVm* p,AnmEnvironment* environment){p->environment=environment;}
API(anm_vm_parent) void anm_vm_parent(AnmVm* p,AnmVm* parent){p->rotation_parent=parent;}
API(anm_renderer_history_size) u32 anm_renderer_history_size(AnmRendererFixture* p,u32 i){return i<p->graphics.history.size()?p->graphics.history[i].size():0;}
API(anm_renderer_submission_matrix) const Matrix4* anm_renderer_submission_matrix(AnmRendererFixture* p,u32 draw,u32 kind){return &p->graphics.submissions[draw].matrices[kind];}
API(anm_renderer_submission_states) const u32* anm_renderer_submission_states(AnmRendererFixture* p,u32 draw){return p->graphics.submissions[draw].states.data();}
API(anm_environment_continue_prompt) void anm_environment_continue_prompt(AnmEnvironment* p,u32 value){p->render_target_has_alpha=value;}
API(anm_renderer_submission_alpha) const u32* anm_renderer_submission_alpha(AnmRendererFixture* p,u32 draw){return p->graphics.submissions[draw].alpha.data();}
API(anm_environment_dimensions) u32* anm_environment_dimensions(AnmEnvironment* value){return &value->screen_width;}
API(anm_renderer_flush) void anm_renderer_flush(AnmRendererFixture* p){p->renderer.flush();}
API(anm_renderer_target_events) const u32* anm_renderer_target_events(AnmRendererFixture* p){return p->graphics.target_events.data();}
API(anm_renderer_target_event_count) u32 anm_renderer_target_event_count(AnmRendererFixture* p){return p->graphics.target_events.size();}
API(anm_renderer_clear_target_events) void anm_renderer_clear_target_events(AnmRendererFixture* p){p->graphics.target_events.clear();}

API(title_main_create) TitleMainFixture* title_main_create(AnmManagerFixture* p){return new TitleMainFixture(p->manager);}
API(title_main_delete) void title_main_delete(TitleMainFixture* p){delete p;}
API(title_main_step) u32 title_main_step(TitleMainFixture* p,u32 pressed,u32 repeated,u32 extra,float rate){if(!p->menu.update(pressed,repeated,extra!=0))return 0;p->state.age.tick(&rate);return 1;}
API(title_main_field) void* title_main_field(TitleMainFixture* p,u32 field){switch(field){case 0:return &p->state.menu;case 1:return &p->state.screen;case 2:return &p->state.substate;case 3:return &p->state.age;case 4:return &p->state.flags;case 5:return p->state.handles.data();case 6:return &p->state.portrait;case 7:return &p->selection;case 8:return &p->settings;case 9:return &p->state.spell_character;case 10:return &p->records;case 11:return &p->state.return_reason;default:return nullptr;}}
API(title_main_error) const char* title_main_error(TitleMainFixture* p){return p->menu.error.c_str();}
API(title_main_sounds) const i32* title_main_sounds(TitleMainFixture* p){return p->sounds.data();}
API(title_main_sound_count) u32 title_main_sound_count(TitleMainFixture* p){return p->sounds.size();}
API(title_selection_step) u32 title_selection_step(TitleMainFixture* p,u32 pressed,u32 repeated){if(p->state.screen==TitleScreen::Main)return p->menu.update(pressed,repeated,p->records.extra(0)||p->records.extra(1)||p->records.extra(2));if(p->state.screen==TitleScreen::Difficulty)return p->difficulty.update(pressed,repeated,p->records);if(p->state.screen==TitleScreen::Character)return p->character.update(pressed,repeated,p->records);if(p->state.screen==TitleScreen::Animal)return p->animal.update(pressed,repeated,p->records);if(p->state.screen==TitleScreen::Stage)return p->stage.update(pressed,repeated,p->records);return 0;}
API(title_selection_error) const char* title_selection_error(TitleMainFixture* p){for(const auto* error:{&p->menu.error,&p->difficulty.error,&p->character.error,&p->animal.error,&p->stage.error})if(!error->empty())return error->c_str();return "";}
API(title_tick) void title_tick(TitleMainFixture* p,float rate){p->state.age.tick(&rate);}
API(title_start_events) const i32* title_start_events(TitleMainFixture* p){return p->events.data();}
API(title_start_event_count) u32 title_start_event_count(TitleMainFixture* p){return p->events.size();}
API(title_checkpoint) void title_checkpoint(TitleMainFixture* p,i32 value){p->checkpoint=value;}
API(anm_vm_allocate_geometry) u32 anm_vm_allocate_geometry(AnmVm* p,u32 count,u32 world){return p->geometry.allocate(count,world!=0);}
API(anm_vm_world_vertices) AnmWorldVertex* anm_vm_world_vertices(AnmVm* p){return p->geometry.world_vertices.data();}
API(anm_vm_screen_vertices) AnmGeometryVertex* anm_vm_screen_vertices(AnmVm* p){return p->geometry.screen_vertices.data();}
API(texture_decode) u32 texture_decode(u32 format,u32 width,u32 height,const u8* pixels,u32 size,u8* out){AnmTexture t;t.pixel_format=format;t.pixel_width=width;t.pixel_height=height;t.pixels.assign(pixels,pixels+size);std::vector<u8> rgba;if(!t.rgba(rgba))return 0;std::memcpy(out,rgba.data(),rgba.size());return 1;}
API(texture_image) u32 texture_image(u32 input,u32 output,u32 width,u32 height,u32 texture_width,u32 texture_height,const u8* bytes,u32 size,u8* out){AnmTexture t;t.format=output;t.width=texture_width;t.height=texture_height;t.pixel_width=width;t.pixel_height=height;t.pixel_format=input;t.pixels.assign(bytes,bytes+size);TextureImage image;if(!image.load(t))return 0;std::memcpy(out,image.pixels.data(),image.pixels.size());return image.pixels.size();}
API(anm_camera_reference) Vec3* anm_camera_reference(AnmCamera* p){return &p->reference;}
API(anm_camera_eye) Vec3* anm_camera_eye(AnmCamera* p){return &p->eye;}
API(anm_camera_matrix) Matrix4* anm_camera_matrix(AnmCamera* p,u32 kind){return kind?&p->projection:&p->view;}
API(anm_camera_fog_range) float* anm_camera_fog_range(AnmCamera* p){return &p->fog_near;}
API(anm_camera_fog_rgb) Vec3* anm_camera_fog_rgb(AnmCamera* p){return &p->fog_rgb;}
API(ecl_context_create) EclContext* ecl_context_create(){return new EclContext;}
API(ecl_context_delete) void ecl_context_delete(EclContext* p){delete p;}
API(ecl_context_stack) u8* ecl_context_stack(EclContext* p){return p->stack.data();}
API(ecl_context_bind) void ecl_context_bind(EclContext* p,const EclInstruction* instruction,i32 stack_pointer,i32 base){p->instruction=instruction;p->stack_pointer=stack_pointer;p->local_base=base;p->error.clear();}
API(ecl_context_field) i32 ecl_context_field(EclContext* p,u32 kind){return kind?p->local_base:p->stack_pointer;}
API(ecl_context_integer) i32 ecl_context_integer(EclContext* p,u32 argument,i32 literal,u32 pop){i32 value=0;p->integer(argument,literal,value,pop);return value;}
API(ecl_context_float) u32 ecl_context_float(EclContext* p,u32 argument,float literal,u32 pop){float value=0;p->floating(argument,literal,value,pop);u32 bits;std::memcpy(&bits,&value,4);return bits;}
API(ecl_context_destination) const void* ecl_context_destination(EclContext* p,u32 argument,u32 floating){return floating?static_cast<void*>(p->float_destination(argument)):static_cast<void*>(p->integer_destination(argument));}
API(ecl_context_error) const char* ecl_context_error(EclContext* p){return p->error.c_str();}
API(ecl_context_push) u32 ecl_context_push(EclContext* p,u32 value,u32 floating){if(floating){float f;std::memcpy(&f,&value,4);return p->push(f);}return p->push(signed_bits(value));}
API(ecl_context_code) u32 ecl_context_code(EclContext* p,const u8* bytes,u32 n,u32 mask,float time){const bool result=p->bind_code(bytes,n);p->difficulty=u8(mask);p->time=time;return result;}
API(ecl_context_step) i32 ecl_context_step(EclContext* p,float elapsed){return p->step(elapsed);}
API(ecl_context_state) const void* ecl_context_state(EclContext* p){return &p->time;}
API(ecl_context_interpolation) ScalarInterpolation* ecl_context_interpolation(EclContext* p,u32 slot){return &p->interpolators[slot];}
API(ecl_context_rng) void ecl_context_rng(EclContext* p,Rng* rng){p->game_rng=rng;}
API(ecl_context_rate) void ecl_context_rate(EclContext* p,float rate){p->interpolation_rate=rate;}
API(ecl_context_program) u32 ecl_context_program(EclContext* p,EclProgram* program,u32 id){return p->bind(*program,id);}
API(ecl_threads_create) EclThreads* ecl_threads_create(EclProgram* program){return new EclThreads(*program);}
API(ecl_threads_delete) void ecl_threads_delete(EclThreads* p){delete p;}
API(ecl_threads_start) u32 ecl_threads_start(EclThreads* p,const char* name,u32 mask){return p->start(name,u8(mask));}
API(ecl_threads_step) i32 ecl_threads_step(EclThreads* p,float elapsed){return p->step(elapsed);}
API(ecl_threads_count) u32 ecl_threads_count(EclThreads* p){return p->count();}
API(ecl_threads_at) EclContext* ecl_threads_at(EclThreads* p,u32 index){return p->at(index);}
API(ecl_thread_field) i32 ecl_thread_field(EclContext* p,u32 field){return field==0?p->thread_id:field==1?p->thread_state:i32(p->thread_flags);}
API(sht_create) ShtResource* sht_create(){return new ShtResource;}
API(sht_delete) void sht_delete(ShtResource* p){delete p;}
API(sht_open) u32 sht_open(ShtResource* p,const u8* data,u32 n){return p->open(data,n);}
API(sht_header) const ShtHeader* sht_header(ShtResource* p){return &p->header;}
API(sht_group_count) u32 sht_group_count(ShtResource* p){return p->groups.size();}
API(sht_count) u32 sht_count(ShtResource* p,u32 group){return p->groups[group].size();}
API(sht_shot) const ShotSpec* sht_shot(ShtResource* p,u32 group,u32 n){return &p->groups[group][n];}
API(game_input_create) GameInput* game_input_create(){return new GameInput;}
API(game_input_delete) void game_input_delete(GameInput* p){delete p;}
API(game_input_calculate) void game_input_calculate(GameInput* p){p->calculate();}
API(game_input_update) void game_input_update(GameInput* p,u32 held){p->update(held);}
API(game_input_recording) void game_input_recording(GameInput* p,u32 held,u32 focus){p->recording_update(u16(held),focus);}
struct FrameSchedulerFixture {
    struct Entry {FrameCallback callback;FrameSchedulerFixture* fixture=nullptr;u32 id=0;i32 first=1,later=1,operation=0,target=0,priority=0,pass=0,auxiliary=0,calls=0;};
    std::array<Entry,16> entries;FrameScheduler scheduler;std::vector<u32> events;
    FrameSchedulerFixture(){for(u32 i=0;i<entries.size();++i){auto& e=entries[i];e.fixture=this;e.id=i;e.callback.owner=&e;}}
    void record(u32 kind,u32 id){events.push_back(kind);events.push_back(id);}
    void mutate(Entry& e){auto& target=entries[u32(e.target)&15];switch(e.operation){
        case 1:scheduler.add(target.callback,e.pass?FramePass::Draw:FramePass::Update,e.priority);break;
        case 2:scheduler.remove(target.callback);break;
        case 3:target.callback.enabled=e.auxiliary!=0;break;
        case 4:scheduler.closing=e.auxiliary!=0;break;
        case 5:scheduler.clear(e.pass?FramePass::Draw:FramePass::Update);break;
        case 6:target.callback.run=nullptr;break;
    }}
    static i32 run(void* p){auto& e=*static_cast<Entry*>(p);auto& f=*e.fixture;f.record(1,e.id);++e.calls;if(e.calls==1)f.mutate(e);return e.calls==1?e.first:e.later;}
    static i32 initialize(void* p){auto& e=*static_cast<Entry*>(p);e.fixture->record(2,e.id);return e.auxiliary;}
    static i32 cleanup(void* p){auto& e=*static_cast<Entry*>(p);e.fixture->record(3,e.id);return 0;}
};
API(frame_scheduler_create) FrameSchedulerFixture* frame_scheduler_create(){return new FrameSchedulerFixture;}
API(frame_scheduler_delete) void frame_scheduler_delete(FrameSchedulerFixture* p){delete p;}
API(frame_scheduler_configure) void frame_scheduler_configure(FrameSchedulerFixture* p,u32 id,i32 first,i32 later,i32 operation,i32 target,i32 priority,i32 pass,i32 auxiliary,u32 flags){auto& e=p->entries[id];e.first=first;e.later=later;e.operation=operation;e.target=target;e.priority=priority;e.pass=pass;e.auxiliary=auxiliary;e.callback.run=flags&1?&FrameSchedulerFixture::run:nullptr;e.callback.initialize=flags&2?&FrameSchedulerFixture::initialize:nullptr;e.callback.cleanup=flags&4?&FrameSchedulerFixture::cleanup:nullptr;e.callback.enabled=flags&8;}
API(frame_scheduler_add) i32 frame_scheduler_add(FrameSchedulerFixture* p,u32 id,u32 pass,i32 priority){return p->scheduler.add(p->entries[id].callback,pass?FramePass::Draw:FramePass::Update,priority);}
API(frame_scheduler_remove) void frame_scheduler_remove(FrameSchedulerFixture* p,u32 id){p->scheduler.remove(p->entries[id].callback);}
API(frame_scheduler_clear) void frame_scheduler_clear(FrameSchedulerFixture* p,u32 pass){p->scheduler.clear(pass?FramePass::Draw:FramePass::Update);}
API(frame_scheduler_closing) void frame_scheduler_closing(FrameSchedulerFixture* p,u32 closing){p->scheduler.closing=closing;}
API(frame_scheduler_execute) i32 frame_scheduler_execute(FrameSchedulerFixture* p,u32 pass){return pass?p->scheduler.draw():p->scheduler.update();}
API(frame_scheduler_events) u32* frame_scheduler_events(FrameSchedulerFixture* p){return p->events.data();}
API(frame_scheduler_event_count) u32 frame_scheduler_event_count(FrameSchedulerFixture* p){return p->events.size()/2;}
API(frame_scheduler_event_reset) void frame_scheduler_event_reset(FrameSchedulerFixture* p){p->events.clear();}
API(frame_scheduler_state) u32 frame_scheduler_state(FrameSchedulerFixture* p,u32 id){const auto& e=p->entries[id];return u32(p->scheduler.contains(e.callback))|(u32(e.callback.run!=nullptr)<<1)|(u32(e.callback.initialize!=nullptr)<<2)|(u32(e.callback.enabled)<<3);}
struct PlayerMotionFixture {PlayerMotion motion;PlayerAnmHost visuals;AnmManager& manager;PlayerMotionFixture(AnmManager& manager,i32 player,i32 effects):visuals(manager,player,effects),manager(manager){visuals.initialize();}};
API(player_motion_create) PlayerMotionFixture* player_motion_create(AnmManagerFixture* p,i32 player,i32 effects){return new PlayerMotionFixture(p->manager,player,effects);}
API(player_motion_delete) void player_motion_delete(PlayerMotionFixture* p){delete p;}
API(player_motion_configure) void player_motion_configure(PlayerMotionFixture* p,ShtHeader* header){p->motion.configure(*header);}
API(player_motion_step) u32 player_motion_step(PlayerMotionFixture* p,u32 input,u32 allowed,float rate){p->manager.rate=rate;return p->motion.update(input,allowed!=0,rate,p->visuals);}
API(player_motion_error) const char* player_motion_error(PlayerMotionFixture* p){return p->motion.error.empty()?p->manager.error.c_str():p->motion.error.c_str();}
API(player_motion_root) AnmVm* player_motion_root(PlayerMotionFixture* p){return &p->visuals.root;}
API(player_motion_handles) u32* player_motion_handles(PlayerMotionFixture* p,u32 kind){return kind==0?&p->visuals.focus_handle:kind==1?&p->visuals.barrier_handle:p->visuals.option_handles.data()->data();}
API(player_motion_field) void* player_motion_field(PlayerMotionFixture* p,u32 field){auto& v=p->motion;switch(field){case 0:return &v.position_fixed;case 1:return &v.position;case 2:return &v.last_step;case 3:return &v.step;case 4:return &v.velocity;case 5:return &v.last_direction;case 6:return &v.normal_speed;case 7:return &v.direction;case 8:return &v.focus;case 9:return &v.input_frame;case 10:return &v.option_follow_percentage;case 11:return &v.collapse_frame;case 12:return &v.option_count;case 13:return &v.behavior_flags;case 14:return &v.enlargement;case 15:return &v.movement_scale;case 16:return &v.external_velocity;case 17:return &v.barrier_timer;case 18:return v.history.data();default:return nullptr;}}
API(player_motion_option) void* player_motion_option(PlayerMotionFixture* p,u32 index,u32 field){auto& v=p->motion.options[index];switch(field){case 0:return &v.active;case 1:return &v.target;case 2:return &v.position;case 3:return &v.normal_offset;case 4:return &v.focus_offset;case 5:return &v.snap;case 6:return &v.index;case 7:return &v.previous_focus;default:return nullptr;}}
API(player_motion_touch) void player_motion_touch(PlayerMotionFixture* p,i32 mode,float x,float y){p->motion.touch={mode,x,y};}
API(player_options_create) PlayerOptions* player_options_create(ShtResource* s,PlayerMotionFixture* m,i32 resource,i32 character){return new PlayerOptions(*s,m->motion,m->visuals,m->manager,resource,character);}
API(player_options_delete) void player_options_delete(PlayerOptions* p){delete p;}
API(player_options_configure) i32 player_options_configure(PlayerOptions* p,i32 power,i32 step,i32 max){return p->configure(power,step,max);}
API(player_options_state) i32* player_options_state(PlayerOptions* p){return &p->power_level;}
API(player_options_error) const char* player_options_error(PlayerOptions* p){return p->error.c_str();}
API(motion_create) MotionState* motion_create(){return new MotionState;}
API(motion_delete) void motion_delete(MotionState* p){delete p;}
API(motion_step) void motion_step(MotionState* p,float rate){p->advance(rate);}
API(motion_integrate) void motion_integrate(MotionState* p,float rate){p->integrate(rate);}
API(position_interpolation_create) PositionInterpolation* position_interpolation_create(){return new PositionInterpolation;}
API(position_interpolation_delete) void position_interpolation_delete(PositionInterpolation* p){delete p;}
API(position_interpolation_step) const float* position_interpolation_step(PositionInterpolation* p,float rate){static std::array<float,3> out;out=p->step(rate);return out.data();}
struct PlayerCollisionFixture:PlayerDamageHost{PlayerCollision state;u32 hits=0;void hit()override{hits++;}};
API(player_collision_create) PlayerCollisionFixture* player_collision_create(){return new PlayerCollisionFixture;}
API(player_collision_delete) void player_collision_delete(PlayerCollisionFixture* p){delete p;}
API(player_collision_field) void* player_collision_field(PlayerCollisionFixture* p,u32 field){auto& s=p->state;switch(field){case 0:return &s.position;case 1:return &s.hitbox_min;case 2:return &s.hitbox_max;case 3:return &s.radius;case 4:return &s.size_multiplier;case 5:return &s.state;case 7:return &s.laser_half_size;default:return &s.invulnerability;}}
API(player_collision_options) void player_collision_options(PlayerCollisionFixture* p,u32 enlarged,u32 bomb){p->state.enlarged=enlarged;p->state.bomb_active=bomb;}
API(player_collision_check) i32 player_collision_check(PlayerCollisionFixture* p,u32 circle,float x,float y,float a,float b,u32 graze){return i32(circle?p->state.circle({x,y},a,graze,p):p->state.rectangle({x,y},{a,b},graze,p));}
API(player_collision_hits) u32 player_collision_hits(PlayerCollisionFixture* p){return p->hits;}
API(player_collision_laser) i32 player_collision_laser(PlayerCollisionFixture* p,float x,float y,float angle,float length,float width,u32 graze){return i32(p->state.laser({x,y},angle,length,width,graze,p));}
struct PlayerShootingFixture final:PlayerShootingServices {
    PlayerMotion motion;PlayerShooting shooting{motion,*this};std::vector<u32> events;u32 next_handle=1000;
    void event(std::array<u32,8> e){events.insert(events.end(),e.begin(),e.end());}
    bool fire(i32 frame,i32 continuous)override{event({1,u32(frame),u32(continuous),float_to_bits(motion.position.x),float_to_bits(motion.position.y),float_to_bits(motion.position.z),u32(motion.focus),0});return true;}
    bool sound(i32 id,ShotSoundAction action,float x)override{event({2,u32(id),u32(action),float_to_bits(x),0,0,0,0});return true;}
    u32 charge_indicator(i32 script,const Vec3& p)override{const u32 handle=++next_handle;event({3,u32(script),float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),handle,0,0});return handle;}
    bool retire(u32 handle)override{event({4,handle,0,0,0,0,0,0});return true;}
    bool interrupt(u32 handle,i32 label)override{event({5,handle,u32(label),0,0,0,0,0});return true;}
};
API(player_shooting_create) PlayerShootingFixture* player_shooting_create(){return new PlayerShootingFixture;}
API(player_shooting_delete) void player_shooting_delete(PlayerShootingFixture* p){delete p;}
API(player_shooting_step) u32 player_shooting_step(PlayerShootingFixture* p,i32 state,u32 held,float rate,i32 character,i32 animal,u32 flags,i32 kind){p->events.clear();return p->shooting.update(state,held!=0,rate,character,animal,flags,kind);}
API(player_shooting_field) void* player_shooting_field(PlayerShootingFixture* p,u32 field){auto& s=p->shooting;switch(field){case 0:return &s.shot;case 1:return &s.continuous;case 2:return &s.charge;case 3:return &s.charge_origin;case 4:return &s.charge_handle;case 5:return &s.auxiliary;case 6:return &s.auxiliary_active;case 7:return &p->motion.position;case 8:return &p->motion.position_fixed;case 9:return &p->motion.focus;default:return nullptr;}}
API(player_shooting_snap) i32* player_shooting_snap(PlayerShootingFixture* p,u32 option){return &p->motion.options[option].snap;}
API(player_shooting_events) const u32* player_shooting_events(PlayerShootingFixture* p){return p->events.data();}
API(player_shooting_event_count) u32 player_shooting_event_count(PlayerShootingFixture* p){return p->events.size()/8;}
API(player_shooting_error) const char* player_shooting_error(PlayerShootingFixture* p){return p->shooting.error.c_str();}
API(damage_sources_create) DamageSources* damage_sources_create(){return new DamageSources;}
API(damage_sources_delete) void damage_sources_delete(DamageSources* p){delete p;}
API(damage_sources_data) DamageSource* damage_sources_data(DamageSources* p){return p->sources.data();}
API(damage_sources_cursor) i32* damage_sources_cursor(DamageSources* p){return &p->cursor;}
API(damage_sources_score) i32* damage_sources_score(DamageSources* p){return &p->score;}
API(damage_sources_maximum) i32* damage_sources_maximum(DamageSources* p){return &p->maximum_damage;}
API(damage_sources_circle) i32 damage_sources_circle(DamageSources* p,const Vec3* position,float radius,float growth,i32 lifetime,i32 damage){return p->circle(*position,radius,growth,lifetime,damage);}
API(damage_sources_rectangle) i32 damage_sources_rectangle(DamageSources* p,const Vec3* position,float angle,const Vec2* size,i32 lifetime,i32 damage){return p->rectangle(*position,angle,*size,lifetime,damage);}
API(damage_sources_tick) void damage_sources_tick(DamageSources* p,float rate){p->tick(rate);}
API(damage_query_create) DamageQuery* damage_query_create(){return new DamageQuery;}
API(damage_query_delete) void damage_query_delete(DamageQuery* p){delete p;}
API(damage_query_field) void* damage_query_field(DamageQuery* p,u32 field){switch(field){case 0:return &p->position;case 1:return &p->size;case 2:return &p->radius;case 3:return &p->angle;case 4:return &p->target;default:return nullptr;}}
API(damage_sources_query) const DamageResult* damage_sources_query(DamageSources* p,DamageQuery* q,u32 rectangle,u32 preview,u32 changed,i32 bomb){static DamageResult value;q->rectangle=rectangle!=0;q->preview=preview!=0;value=p->query(*q,changed!=0,bomb);return &value;}
struct PlayerShotItemEvent {i32 kind;Vec3 position;float angle,speed;};
struct PlayerShotsFixture {
    PlayerMotion motion;PlayerShotContext context{motion};DamageSources damage;Rng& random;Rng& visual_random;PlayerShots shots;
    std::vector<i32> sounds;std::vector<PlayerShotItemEvent> items;std::array<PlayerShotTarget,8> targets;
    PlayerShotsFixture(ShtResource& s,AnmManager& a,Rng& game,Rng& visual,i32 id):random(game),visual_random(visual),shots(s,context,damage,a,random,visual_random,id,8){
        shots.sound=[this](i32 id,float,PlayerShots::SoundAction action){sounds.push_back(id);sounds.push_back(i32(action));return true;};
        shots.item=[this](i32 kind,const Vec3& p,float angle,float speed){items.push_back({kind,p,angle,speed});return true;};
    }
};
API(player_shots_create) PlayerShotsFixture* player_shots_create(ShtResource* s,AnmManagerFixture* a,i32 id){return new PlayerShotsFixture(*s,a->manager,a->environment.game_rng,a->random,id);}
API(player_shots_delete) void player_shots_delete(PlayerShotsFixture* p){delete p;}
API(player_shots_data) PlayerShot* player_shots_data(PlayerShotsFixture* p){return p->shots.shots.data();}
API(player_shots_damage) DamageSource* player_shots_damage(PlayerShotsFixture* p){return p->damage.sources.data();}
API(player_shots_random) Rng* player_shots_random(PlayerShotsFixture* p){return &p->random;}
API(player_shots_visual_random) Rng* player_shots_visual_random(PlayerShotsFixture* p){return &p->visual_random;}
API(player_shots_rewards) i32* player_shots_rewards(PlayerShotsFixture* p){return &p->shots.reward_counter;}
API(player_shots_items) PlayerShotItemEvent* player_shots_items(PlayerShotsFixture* p){return p->items.data();}
API(player_shots_item_count) u32 player_shots_item_count(PlayerShotsFixture* p){return p->items.size();}
API(player_shots_cursor) i32* player_shots_cursor(PlayerShotsFixture* p){return &p->damage.cursor;}
API(damage_sources_tick_for_shots) void damage_sources_tick_for_shots(PlayerShotsFixture* p,float rate){p->damage.tick(rate);}
API(player_shots_position) Vec3* player_shots_position(PlayerShotsFixture* p){return &p->motion.position;}
API(player_shots_option) PlayerFixedPosition* player_shots_option(PlayerShotsFixture* p,u32 i){return &p->motion.options[i].position;}
API(player_shots_angles) float* player_shots_angles(PlayerShotsFixture* p){return p->context.option_angles.data();}
API(player_shots_locks) i32* player_shots_locks(PlayerShotsFixture* p){return p->context.laser_power.data();}
API(player_shots_option_count) i32* player_shots_option_count(PlayerShotsFixture* p){return &p->motion.option_count;}
API(player_shots_options) void player_shots_options(PlayerShotsFixture* p,i32 focus,i32 power,i32 step,float rate){p->motion.focus=focus;p->context.power=power;p->context.power_step=step;p->shots.set_rate(rate);}
API(player_shots_roaring) void player_shots_roaring(PlayerShotsFixture* p,u32 flags,i32 kind){p->context.roaring_flags=flags;p->context.roaring_kind=kind;}
API(player_shots_fire) u32 player_shots_fire(PlayerShotsFixture* p,i32 frame,i32 sound_frame){return p->shots.fire(frame,sound_frame);}
API(player_shots_update) u32 player_shots_update(PlayerShotsFixture* p){return p->shots.update();}
API(player_shots_hit) i32 player_shots_hit(PlayerShotsFixture* p,u32 source,DamageQuery* q,u32 rectangle){q->rectangle=rectangle!=0;return p->shots.hit(p->damage.sources[source],*q);}
API(player_shots_targets) void player_shots_targets(PlayerShotsFixture* p,u32 count){p->context.targets.clear();for(u32 i=0;i<count&&i<8;i++)p->context.targets.push_back(&p->targets[i]);}
API(player_shots_target) void* player_shots_target(PlayerShotsFixture* p,u32 i,u32 field){auto& target=p->targets[i];return field==0?static_cast<void*>(&target.position):field==1?static_cast<void*>(&target.flags):static_cast<void*>(&target.id);}
API(player_shots_context) void player_shots_context(PlayerShotsFixture* p,i32 frame,u32 enemy,u32 bomb,float x,float y){p->context.shoot_frame=frame;p->context.enemy_scene=enemy!=0;p->context.bomb_active=bomb!=0;p->context.screen_origin={x,y};}
API(player_shots_spawn) i32 player_shots_spawn(PlayerShotsFixture* p,u32 id){return p->shots.spawn(id,p->motion.position);}
API(player_shots_error) const char* player_shots_error(PlayerShotsFixture* p){return p->shots.error.c_str();}
API(player_shots_sound_count) u32 player_shots_sound_count(PlayerShotsFixture* p){return p->sounds.size()/2;}
API(player_shots_sound) i32* player_shots_sound(PlayerShotsFixture* p,u32 i){return p->sounds.data()+i*2;}
struct PlayerLifeFixture final:PlayerLifecycleHost {
    PlayerMotion motion;PlayerAnmHost visuals;EffectManager effects;PlayerLifeSession session;PlayerSpellStatus spell;PlayerLife life;
    std::vector<i32> life_events;std::vector<u32> events;bool can_bomb=false;float rate=1;
    PlayerLifeFixture(AnmManager& a,i32 id):visuals(a,id,8),effects(a,8),life(motion,visuals,effects,session,spell){
        visuals.initialize();life.sound=[this](i32 id){life_events.insert(life_events.end(),{1,id,0});return true;};
        life.life_hud=[this](i32 a,i32 b){life_events.insert(life_events.end(),{2,a,b});return true;};life.bomb_hud=[this](i32 a,i32 b){life_events.insert(life_events.end(),{3,a,b});return true;};
    }
    void event(std::initializer_list<u32> values){events.insert(events.end(),values);}
    bool allow_bomb()override{event({1});return can_bomb;}bool begin_bomb()override{event({2});return true;}bool move()override{event({3});return true;}
    bool end_roaring(bool death)override{event({12,u32(death)});return true;}
    bool cancel_lasers_near(const Vec3& p,float r,i32 kind,bool flag)override{event({4,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(r),u32(kind),u32(flag)});return true;}
    bool cancel_bullets_near(const Vec3& p,float r,i32 kind,bool all)override{event({5,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(r),u32(kind),u32(all)});return true;}
    bool cancel_lasers(i32 kind,bool flag)override{event({6,u32(kind),u32(flag)});return true;}
    bool item(i32 kind,const Vec3& p,float angle,float speed)override{event({7,u32(kind),float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(angle),float_to_bits(speed)});return true;}
    bool options_changed()override{event({8});return true;}bool game_over()override{event({9});return true;}
    bool respawn_damage(const Vec3& p,float radius,float growth,i32 frames,i32 damage)override{event({10,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(radius),float_to_bits(growth),u32(frames),u32(damage)});return true;}
};
API(player_life_create) PlayerLifeFixture* player_life_create(AnmManagerFixture* p,i32 id){return new PlayerLifeFixture(p->manager,id);}
API(player_life_delete) void player_life_delete(PlayerLifeFixture* p){delete p;}
API(player_life_transition) u32 player_life_transition(PlayerLifeFixture* p,u32 kind){if(kind==2){p->life.cancel_death();return 1;}return kind?p->life.commit_death():p->life.hit();}
API(player_life_root) AnmVm* player_life_root(PlayerLifeFixture* p){return &p->visuals.root;}
API(player_life_field) void* player_life_field(PlayerLifeFixture* p,u32 field){switch(field){case 0:return &p->life.state;case 1:return &p->life.age;case 2:return &p->life.invulnerability;case 3:return &p->session;case 4:return &p->spell;case 5:return &p->motion.behavior_flags;case 6:return &p->motion.position;case 7:return p->effects.handles.data();case 8:return &p->effects.cursor;case 9:return &p->motion.option_count;case 10:return &p->session.roaring_countdown;case 11:return &p->session.roaring_flags;case 12:return &p->session.bomb_available;default:return nullptr;}}
API(player_life_option_active) i32* player_life_option_active(PlayerLifeFixture* p,u32 index){return &p->motion.options[index].active;}
API(player_life_option_snap) i32* player_life_option_snap(PlayerLifeFixture* p,u32 index){return &p->motion.options[index].snap;}
API(player_life_option_handles) u32* player_life_option_handles(PlayerLifeFixture* p){return p->visuals.option_handles.data()->data();}
API(player_life_count) u32 player_life_count(PlayerLifeFixture* p){return p->life_events.size()/3;}
API(player_life_events) i32* player_life_events(PlayerLifeFixture* p){return p->life_events.data();}
API(player_life_error) const char* player_life_error(PlayerLifeFixture* p){return p->life.error.c_str();}
API(player_lifecycle_step) u32 player_lifecycle_step(PlayerLifeFixture* p,u32 pressed,float rate,u32 hud,u32 can){p->rate=rate;p->can_bomb=can!=0;return p->life.advance_state(pressed,p->rate,hud!=0,*p);}
API(player_lifecycle_field) void* player_lifecycle_field(PlayerLifeFixture* p,u32 field){switch(field){case 0:return &p->life.last_death_position;case 1:return &p->motion.position_fixed;case 2:return &p->rate;case 3:return p->motion.history.data();default:return nullptr;}}
API(player_lifecycle_count) u32 player_lifecycle_count(PlayerLifeFixture* p){return p->events.size();}
API(player_lifecycle_events) u32* player_lifecycle_events(PlayerLifeFixture* p){return p->events.data();}
struct PlayerBoundsFixture {PlayerMotion motion;PlayerBounds bounds;Timer age;AnmVm vm;};
API(player_bounds_create) PlayerBoundsFixture* player_bounds_create(){return new PlayerBoundsFixture;}
API(player_bounds_delete) void player_bounds_delete(PlayerBoundsFixture* p){delete p;}
API(player_bounds_step) void player_bounds_step(PlayerBoundsFixture* p,float rate){p->bounds.update(p->motion,p->age,p->vm,rate);}
API(player_bounds_field) void* player_bounds_field(PlayerBoundsFixture* p,u32 f){switch(f){case 0:return &p->motion.position;case 1:return &p->bounds.hit_half_size;case 2:return &p->bounds.item_half_size;case 3:return &p->bounds.graze_half_size;case 4:return &p->bounds.hit;case 5:return &p->bounds.item_near;case 6:return &p->bounds.graze;case 7:return &p->bounds.item_full;case 8:return &p->bounds.enlargement;case 9:return &p->motion.enlargement;case 10:return &p->motion.behavior_flags;case 11:return &p->age;case 12:return &p->vm.visual.flags;case 13:return &p->vm.visual.scale;default:return nullptr;}}
#include "PlayerFixture.hpp"
#include "EnemyFixture.hpp"
#include "EnemyRuntimeFixture.hpp"
#include "EnemyManagerFixture.hpp"
#include "EnemyDeathFixture.hpp"
#include "BulletFixture.hpp"
#include "BulletRuntimeFixture.hpp"
#include "BulletSceneFixture.hpp"

#include "LaserFixture.hpp"
#include "LaserSceneFixture.hpp"
#include "ItemFixture.hpp"
#include "AnimalSpiritsFixture.hpp"
#include "RoaringFixture.hpp"
#include "ItemSpiritsFixture.hpp"
#include "EnemySceneFixture.hpp"
#include "ScreenGridFixture.hpp"
#include "StageScriptFixture.hpp"
#include "EnemyCombatFixture.hpp"
#include "GameBattleFixture.hpp"
#include "BombFixture.hpp"
#include "ScreenShakeFixture.hpp"
#include "SpellFixture.hpp"
#include "HudFixture.hpp"
#include "MessageFixture.hpp"
#include "DialogueFixture.hpp"
#include "AsciiTextFixture.hpp"
#include "GameProgressionFixture.hpp"
#include "GameFrameFixture.hpp"
#include "StageAssetsFixture.hpp"
#include "StageGameplayFixture.hpp"
#include "RunGameplayFixture.hpp"
#include "AnmSceneEffectsFixture.hpp"
#include "GameStartFixture.hpp"
#include "PopupFixture.hpp"
#include "ReplayStateFixture.hpp"
#include "DrawScheduleFixture.hpp"

#include "ScreenCompositorFixture.hpp"
#include "SpellCaptureFixture.hpp"
#include "TitleOptionsFixture.hpp"
#include "TitleFrameFixture.hpp"
#include "TextRasterFixture.hpp"
#include "AudioFixture.hpp"
#include "ScoreFileFixture.hpp"
#include "PauseFixture.hpp"
#include "PauseActivationFixture.hpp"
#include "ReplayRecordingFixture.hpp"
#include "TitleReplayFixture.hpp"
#include "SceneCaptureFixture.hpp"

#include "TitleMusicFixture.hpp"

#include "TitleControllerFixture.hpp"
#include "PauseActionsFixture.hpp"
#include "PauseFrameFixture.hpp"

#include "ManualFixture.hpp"
#include "EndingFixture.hpp"
#include "TitleResultsFixture.hpp"
#include "TitleDataFixture.hpp"
#include "ApplicationFlowFixture.hpp"
#include "../../cpp/game/GameKeyboard.hpp"
#include "../../cpp/game/GameConfig.hpp"
API(keyboard_keys) u32 keyboard_test(const u8* input){bool keys[256]{};for(u32 i=0;i<256;i++)keys[i]=(input[i]&128)!=0;return keyboard_keys(keys);}
API(game_config_create) GameConfig* game_config_create(){return new GameConfig;}
API(game_config_delete) void game_config_delete(GameConfig* p){delete p;}
API(game_config_bytes) u8* game_config_bytes(GameConfig* p){return p->bytes.data();}
API(game_config_open) i32 game_config_open(GameConfig* p,const u8* data,u32 size){return p->open(data,size);}
API(game_config_reset) void game_config_reset(GameConfig* p,const u8* controller){p->reset(controller);}
