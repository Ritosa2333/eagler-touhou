// Original executable mapping is restricted to this differential test program.
#define wmain unused_native_cpu_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "pool.hpp"
#include "dispatch.hpp"
#include "binding.hpp"
#include "anm_vm.hpp"
#include "quad.hpp"
#include "render_state.hpp"
#include "draw.hpp"
#include "file_tasks.hpp"
#include "projected_draw.hpp"
#include "loading_interrupt.hpp"
#include "render_mesh.hpp"
#include "../stage_background/object_draw.hpp"
#include "../stage_background/vm.hpp"
#include "../stage_background/update.hpp"
#include "../effect_system/rounded_panel.hpp"
#include "../effect_system/short_line.hpp"
#include "../effect_system/spiral_trail.hpp"
#include "../effect_system/wavering_trail.hpp"
#include "../effect_system/converging_particles.hpp"
#include "../effect_system/transition_panels.hpp"
#include "../effect_system/stone_selection.hpp"
#include "../hud_system/hud.hpp"
#include "../hud_system/scoring.hpp"
#include "../stage_clear/stage_clear.hpp"
#include "../card_system/card.hpp"
#include "../card_system/records.hpp"
#include "../pause_system/draw.hpp"
#include "../pause_system/draw_constants.hpp"
#include "../trophy_system/trophy.hpp"
#include "../ending_scene/ending.hpp"
#include "../notice_system/notice.hpp"
#include "../title_system/music.hpp"
#include "../title_system/music_data.hpp"
#include "../title_system/stage_select.hpp"
#include "../title_system/practice_pool_fixture.hpp"
#include "../title_system/practice_data.hpp"
#include "../title_system/replay_menu.hpp"
#include "../title_system/stones_pool_fixture.hpp"
#include "../title_system/stones_data.hpp"
#include "../platform_window/graphics_callbacks.hpp"
#include "../text_renderer/bitmap.hpp"
#include "../text_renderer/raster.hpp"
#include "../text_renderer/deferred_queue.hpp"
#include "../platform_window/fonts.hpp"
#include "../platform_services/clock.hpp"
#include "../stage_clear/data_constants.hpp"
#include "../input/input.hpp"
#include "../progress_state/manager.hpp"
#include "../startup_scene/startup.hpp"
#include "../stone_menu/update.hpp"
#include "../text_renderer/text.hpp"
#include "../player_entity/state_layout.hpp"
#include "../player_entity/owner.hpp"
#include "../effect_system/rings.hpp"
#include "../effect_system/burst_rings.hpp"
#include "../effect_system/radial_trails.hpp"
#include "../runtime_state/state.hpp"
#include "../bomb_system/bomb.hpp"
#include "../bomb_system/marisa.hpp"
#include "../bomb_system/reimu.hpp"
#include "../bomb_system/character_environment.hpp"
#include "../bullet_system/bullet.hpp"
#include "../bullet_system/movement.hpp"
#include "../bullet_system/player_cancellation.hpp"
#include "../bullet_system/command.hpp"
#include "../bullet_system/style.hpp"
#include "../bullet_system/shoot.hpp"
#include "../laser_system/laser.hpp"
#include "../laser_system/type3.hpp"
#include "../laser_system/type1.hpp"
#include "../laser_system/type0.hpp"
#include "../laser_system/type2.hpp"
#include "../player_entity/events.hpp"
#include "animation_file.hpp"
#include "../damage_regions/regions.hpp"
#include "../audio_runtime/audio.hpp"
#include "../screen_effect/effect.hpp"
#include "../gameplay/gameplay.hpp"
#include "../gameplay/frame.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../gameplay/player_state.hpp"
#include "../gameplay/stage_data.hpp"
#include "../gameplay/enemy_frame.hpp"
#include "../platform_window/directx_math.hpp"
#include "../core_scheduler/scheduler.hpp"
#include "../runtime_core/runtime_core.hpp"
#include <functional>
#include "source_hashes.hpp"
#include "../trophy_system/pool_fixture.hpp"
#include "../notice_system/pool_fixture.hpp"
namespace s=th20::source::sprite;
namespace q=th20::source::scheduler;
namespace {
unsigned diagnostic_case=0;const char* diagnostic_phase="initial";
LONG WINAPI diagnostic_exception(EXCEPTION_POINTERS* p){std::cerr<<"CPU fixture exception 0x"<<std::hex<<p->ExceptionRecord->ExceptionCode<<" EIP 0x"<<p->ContextRecord->Eip<<" operation "<<p->ExceptionRecord->ExceptionInformation[0]<<" mapped 0x"<<mapped_image_base<<" address 0x"<<p->ExceptionRecord->ExceptionInformation[1]<<std::dec<<" case "<<diagnostic_case<<" phase "<<diagnostic_phase<<std::endl;auto* stack=reinterpret_cast<const std::uint32_t*>(p->ContextRecord->Esp);std::cerr<<"CPU stack";for(unsigned i=0;i<16;++i)std::cerr<<" "<<std::hex<<stack[i];std::cerr<<" ECX "<<p->ContextRecord->Ecx<<" EDX "<<p->ContextRecord->Edx<<std::dec<<std::endl;return EXCEPTION_EXECUTE_HANDLER;}
LONG WINAPI first_chance_diagnostic(EXCEPTION_POINTERS* p){if(p->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION)diagnostic_exception(p);return EXCEPTION_CONTINUE_SEARCH;}
bool stone_fixture_updates=true,stone_fixture_buttons[4]{},stone_fixture_alternate[4]{};int stone_fixture_stones[4]{},stone_fixture_profiles[4]{};s::AnimationFile* stone_fixture_file=nullptr;
struct StoneNativeButton{void** vtable;bool value;};bool __fastcall stone_native_button(StoneNativeButton* self,void*){return self->value;}
th20::source::runtime::CallbackOwner* hud_pause_fixture=nullptr;
std::uint64_t card_counter_sample=0;
BOOL WINAPI sample_card_counter(LARGE_INTEGER* value){std::memcpy(value,&card_counter_sample,8);return TRUE;}
bool scene_camera_mode=false;
bool mesh_enabled=false;s::AnimationFile* mesh_file=nullptr;
s::Controller* controller=nullptr;float clock_value=1,screen_scale_value=1;const float* anm_clock_pointer=&clock_value;th20::source::program_entry::ViewportState camera{};
std::vector<std::array<std::uint32_t,4>> device_calls;
bool capture_primitive_vertices=false;std::vector<std::vector<std::uint8_t>> primitive_vertex_calls;
std::vector<std::array<std::uint32_t,17>> matrix_calls;std::uint32_t depth_value=0,fog_value=0;
std::vector<std::array<std::uint32_t,6>> viewport_calls;
HRESULT __stdcall viewport_call(void*,const D3DVIEWPORT9* viewport){std::array<std::uint32_t,6> value;std::memcpy(value.data(),viewport,24);device_calls.push_back({47,static_cast<std::uint32_t>(viewport_calls.size()),0,0});viewport_calls.push_back(value);return S_OK;}
HRESULT __stdcall clear_call(void*,DWORD count,const D3DRECT* rectangles,DWORD flags,D3DCOLOR color,float depth,DWORD stencil){std::array<std::uint32_t,17> value{};value[0]=43;value[1]=count;value[2]=flags;value[3]=color;std::memcpy(&value[4],&depth,4);value[5]=stencil;if(count)std::memcpy(value.data()+6,rectangles,16);device_calls.push_back({43,static_cast<std::uint32_t>(matrix_calls.size()),0,0});matrix_calls.push_back(value);return S_OK;}
HRESULT __stdcall transform_call(void*,DWORD state,const D3DMATRIX* matrix){std::array<std::uint32_t,17> item;item[0]=state;std::memcpy(item.data()+1,matrix,64);device_calls.push_back({44,state,static_cast<std::uint32_t>(matrix_calls.size()),0});matrix_calls.push_back(item);return S_OK;}
HRESULT __stdcall render_call(void*,DWORD state,DWORD value){device_calls.push_back({57,state,value,0});return S_OK;}
HRESULT __stdcall sampler_call(void*,DWORD slot,DWORD state,DWORD value){device_calls.push_back({69,slot,state,value});return S_OK;}
HRESULT __stdcall stage_call(void*,DWORD slot,DWORD state,DWORD value){device_calls.push_back({67,slot,state,value});return S_OK;}
HRESULT __stdcall texture_call(void*,DWORD slot,void* texture){device_calls.push_back({65,slot,reinterpret_cast<std::uint32_t>(texture),0});return S_OK;}
HRESULT __stdcall fvf_call(void*,DWORD value){device_calls.push_back({89,value,0,0});return S_OK;}
HRESULT __stdcall draw_call(void*,DWORD type,UINT count,const void* vertices,UINT stride){device_calls.push_back({83,type,count,stride});if(capture_primitive_vertices){const auto n=type==D3DPT_POINTLIST?count:type==D3DPT_LINELIST?count*2:type==D3DPT_LINESTRIP?count+1:type==D3DPT_TRIANGLELIST?count*3:count+2;const auto* p=static_cast<const std::uint8_t*>(vertices);primitive_vertex_calls.emplace_back(p,p+n*stride);}return S_OK;}
HRESULT __stdcall stream_call(void*,UINT slot,IDirect3DVertexBuffer9* buffer,UINT offset,UINT stride){device_calls.push_back({100,slot,reinterpret_cast<std::uint32_t>(buffer),stride});return S_OK;}
HRESULT __stdcall primitive_call(void*,DWORD type,UINT first,UINT count){device_calls.push_back({81,type,first,count});return S_OK;}
void* device_vtable[119]{};struct Device {void** vtable=device_vtable;} device;
float bounds[4]{-10000,-10000,10000,10000};
template<class R,class... A>R cpu(std::uint32_t va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
[[noreturn]]void unexpected(const char* dependency){throw std::runtime_error(std::string("Unexercised test dependency: ")+dependency);}
}
namespace th20::source::program_entry {GraphicsStatePrefix graphics_state{};WindowStatePrefix window_state{};SpriteController* sprite_controller=nullptr;}
namespace th20::source::program_entry {scheduler::State* function_controller=nullptr;scheduler::Environment scheduler_environment;}
namespace th20::source::program_entry {audio::SoundInf thread_registry;}
namespace th20::source::program_entry {LogBuffer log_buffer;std::uint32_t& graphics_event_flags=*reinterpret_cast<std::uint32_t*>(reinterpret_cast<unsigned char*>(&graphics_state)+0xb48);}
namespace th20::source::sprite {AnimationFile* preload_animation_file(Controller&,std::int32_t,const char*,runtime::Log&){unexpected("ANM file IO outside cached resource CPU fixture");}}
namespace th20::source::player_entity {CollisionServices& collision_services(){unexpected("Player collision services outside initial Type1 fixture");}EventServices& event_services(){unexpected("Player event services outside initial Type1 fixture");}void graze(void*,const sprite::Vec3&,std::uint32_t,EventServices&){unexpected("Player graze outside initial Type1 fixture");}}
namespace th20::source::bullet::unrecovered {
void spawn_extended_enemy(Bullet&,const Command&){unexpected("enemy ETEX spawn outside initial fixture");}
int player_hit_test_00484a20(Bullet&,std::int32_t){unexpected("player collision outside cancellation fixture");}
void item_spawn_004c45b0(void*,const sprite::Vec3&,std::int32_t,std::int32_t){unexpected("item spawn outside cancellation fixture");}
void item_spawn_004c3c90(void*,std::int32_t,const sprite::Vec3&,std::int32_t,float,float,std::int32_t,std::int32_t,std::int32_t){unexpected("item spawn outside cancellation fixture");}
}
namespace th20::source::game_session {Context& context(std::int32_t index) noexcept{return *reinterpret_cast<Context*>(mapped_image_base+0x1ba568+index*0x30);}}
namespace th20::source::gameplay {EnemyController& enemy_controller(int index){return *static_cast<EnemyController*>(game_session::context(index).objects_04[1]);}}
namespace th20::source::gameplay {GameController* controller=nullptr;std::uint32_t slowdown_frames=0;}
namespace th20::source::gameplay {int start_game_frame(GameController&){unexpected("Initial activation outside first Game frame fixture");}void complete_background_transition(GameController&){unexpected("Background activation outside first Game frame fixture");}}
namespace th20::source::gameplay::unrecovered {void finish_replay_004e5f30(){unexpected("Pause replay-finish owner outside first Game frame fixture");}}
namespace th20::source::gameplay::unrecovered {void* boss_hud_005c06a4(){return *reinterpret_cast<void**>(mapped_image_base+0x1c06a4);}}
namespace th20::source::background {Background* primary=nullptr;Background* secondary=nullptr;}
namespace th20::source::text {Renderer* renderer=nullptr;}
namespace th20::source::progress {SaveManager* manager=nullptr;}
namespace th20::source::hud {FrontInf* controller=nullptr;}
// Deterministic OS-clock boundary. The original still executes 41cb10 and its
// arithmetic; the source uses the same recovered arithmetic with the same sample.
namespace th20::source::platform {double read_clock(program_entry::WindowStatePrefix& window){return performance_clock(card_counter_sample,window.performance_origin,window.performance_frequency,window.clock_offset);}}
namespace th20::source::startup::unrecovered {runtime::CallbackOwner* owner_005c60fc=nullptr;}
namespace th20::source::trophy {Message* messages=nullptr;}
namespace th20::source::ending {
int initialize(EndingInf&){unexpected("Ending resource initialization outside VM fixture");}
void begin_resource_load(Script&){unexpected("Ending asynchronous resource load outside VM fixture");}
std::uint8_t* replace_script_data(EndingInf&,const char*){unexpected("Ending credits file replacement outside VM fixture");}
}
namespace th20::source::sprite {void clear_animation_file(Controller&,AnimationFile&){unexpected("Nonempty ANM file cleanup outside owner CPU fixture");}void destroy_animation_file(Controller&,AnimationFile*){unexpected("Nonempty ANM file destruction outside owner CPU fixture");}}
namespace th20::source::hud::unrecovered {void complete_stage_004bc570(){unexpected("Stage completion and self-delete outside StageClear fixture");}}
namespace th20::source::hud {bool update_dialogue(Dialogue&){unexpected("Active Dialogue execution outside HUD frame fixture");}void destroy_dialogue(Dialogue*){unexpected("Active Dialogue deletion outside HUD frame fixture");}}
namespace th20::source::hud::environment {sprite::Controller& sprites(){return *::controller;}sprite::AnimationFile& notice_file(){return *text::renderer->animation_file;}}
namespace th20::source::stone_menu {StoneMenuInf* controller=nullptr;void open(StoneMenuInf&,int){unexpected("StoneMenu open outside bounded HUD enable fixture");}}
namespace th20::source::gameplay::unrecovered {runtime::CallbackOwner*& owner(Owner which){if(which!=Owner::global_005c60bc)unexpected("Non-HUD owner outside HUD fixture");return hud_pause_fixture;}}
namespace th20::source::effects::environment {sprite::Controller& sprites(){return *::controller;}game_session::Context& context(std::int32_t index){return *reinterpret_cast<game_session::Context*>(mapped_image_base+0x1ba568+index*0x30);}}
namespace th20::source::effects::stone_selection_environment {bool updates_enabled(){return stone_fixture_updates;}bool button(unsigned i){return stone_fixture_buttons[i];}int stone(unsigned i){return stone_fixture_stones[i];}bool alternate(unsigned i){return stone_fixture_alternate[i];}int selected_profile(unsigned i){return stone_fixture_profiles[i];}sprite::AnimationFile& file(){return *stone_fixture_file;}}
namespace th20::source::effects::unrecovered {
#define OUTSIDE_EFFECT(number) void __cdecl initialize_##number(sprite::Animation*,const void*,std::int32_t){unexpected("Effect subtype " #number " is outside this test batch");}

#undef OUTSIDE_EFFECT
}
namespace th20::source::sprite::anm_environment {
float& clock_scale(){return clock_value;}const float* timer_rate(){return anm_clock_pointer;}
AnmInstruction* script(Animation& a){return script_start(*controller,a);}bool gameplay_frozen(){return gameplay::controller&&gameplay::controller->animation_frozen();}
std::uint32_t random_next(){unexpected("random_next");}std::uint32_t random_bounded(std::uint32_t){unexpected("random_bounded");}
float random_unit(){unexpected("random_unit");}float random_signed_unit(){unexpected("random_signed_unit");}float camera_component(std::int32_t){unexpected("camera_component");}
void assign_sprite(Animation&,std::int32_t){unexpected("assign_sprite");}void set_layer(Animation& a,std::int32_t layer){set_animation_layer(a,layer);}
void add_camera_offset(Vec3&){unexpected("add_camera_offset");}void calculate_corners(Animation&,Vec3(&)[4]){unexpected("calculate_corners");}
float screen_scale(){return screen_scale_value;}std::int32_t screen_offset(unsigned,unsigned){return 0;}
void* allocate_geometry(std::uint32_t){unexpected("allocate_geometry");}
std::uint32_t spawn_child(Animation& animation,std::int32_t script,std::uint32_t flags){return spawn_child_animation(*controller,script_file(*controller,animation),animation,script,flags);}
std::uint32_t spawn_detached(Animation&,std::int32_t,std::uint32_t){unexpected("spawn_detached");}
Animation& lookup_animation(std::uint32_t){unexpected("lookup_animation");}void spawn_effect(Animation&,std::int32_t){unexpected("spawn_effect");}
}
namespace th20::source::sprite::dispatch_environment {
Controller& controller(){return *::controller;}
void select_camera(std::int32_t){unexpected("select_camera");}void select_layer_camera(std::int32_t){unexpected("select_layer_camera");}
void select_viewport_camera(std::int32_t){unexpected("select_viewport_camera");}void disable_fog(){unexpected("disable_fog");}
void disable_depth_write(){unexpected("disable_depth_write");}void set_render_state(std::uint32_t state,std::uint32_t value){flush_textured_quads(*::controller,*reinterpret_cast<IDirect3DDevice9*>(&::device));reinterpret_cast<IDirect3DDevice9*>(&::device)->SetRenderState(static_cast<D3DRENDERSTATETYPE>(state),value);}
}

namespace th20::source::sprite::draw_environment {
Controller& controller(){return *::controller;}IDirect3DDevice9& device(){return *reinterpret_cast<IDirect3DDevice9*>(&::device);}const float* viewport_bounds(){return scene_camera_mode?program_entry::graphics_state.current_viewport->bounds:bounds;}
program_entry::ViewportState& current_camera(){return scene_camera_mode?*program_entry::graphics_state.current_viewport : ::camera;}std::int32_t scaled_dimension(unsigned axis){return axis?480:640;}
void enable_fog(){auto& value=scene_camera_mode?program_entry::graphics_state.render_value:fog_value;if(value!=1){flush_textured_quads(controller(),device());value=1;device().SetRenderState(D3DRS_FOGENABLE,1);}}
void disable_fog(){auto& value=scene_camera_mode?program_entry::graphics_state.render_value:fog_value;if(value){flush_textured_quads(controller(),device());value=0;device().SetRenderState(D3DRS_FOGENABLE,0);}}
void enable_depth_write(){auto& value=scene_camera_mode?program_entry::graphics_state.field_0dbc:depth_value;if(value!=1){flush_textured_quads(controller(),device());value=1;device().SetRenderState(D3DRS_ZWRITEENABLE,1);}}
void disable_depth_write(){auto& value=scene_camera_mode?program_entry::graphics_state.field_0dbc:depth_value;if(value){flush_textured_quads(controller(),device());value=0;device().SetRenderState(D3DRS_ZWRITEENABLE,0);}}
}
namespace th20::source::sprite::mesh_environment {
bool enabled(){return mesh_enabled;}Controller& controller(){return *::controller;}AnimationFile& surface_animation(){return *mesh_file;}game_session::Context& context(std::int32_t index){return *reinterpret_cast<game_session::Context*>(mapped_image_base+0x1ba568+index*0x30);}std::int32_t view_offset(std::int32_t index,unsigned axis){return axis?index*53-17:index*37-11;}std::int32_t display_offset(unsigned axis){return axis?-19:7;}std::int32_t scaled_dimension(unsigned axis){return axis?480:640;}
}
int wmain(int argc,wchar_t** argv){SetUnhandledExceptionFilter(diagnostic_exception);AddVectoredExceptionHandler(1,first_chance_diagnostic);try{
    if(argc!=3)throw std::runtime_error("Usage: th20_pool_cpu_compare ORIGINAL.exe OUTPUT.json");
    const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");
    const auto pe=th20::parse_pe(bytes);const auto& cpu_imports=pe.imports;Mapping image(bytes,pe);mapped_image_base=image.address();
    controller=static_cast<s::Controller*>(VirtualAlloc(nullptr,sizeof(s::Controller),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));if(!controller)throw std::bad_alloc();auto& c=*controller;
    *reinterpret_cast<void**>(mapped_image_base+0x1c0028)=controller;th20::source::program_entry::sprite_controller=controller;
    *reinterpret_cast<void**>(mapped_image_base+0x1ba828)=nullptr;
    *reinterpret_cast<float**>(mapped_image_base+0x1aefe0)=reinterpret_cast<float*>(mapped_image_base+0x1aefe4);
    *reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=1;
    *reinterpret_cast<float*>(mapped_image_base+0x1b8818)=1;
    for(unsigned i=0;i<4;++i)*reinterpret_cast<std::int32_t*>(mapped_image_base+0x1b67b0+i*4)=0;
    device_vtable[57]=reinterpret_cast<void*>(&render_call);device_vtable[69]=reinterpret_cast<void*>(&sampler_call);device_vtable[67]=reinterpret_cast<void*>(&stage_call);device_vtable[65]=reinterpret_cast<void*>(&texture_call);device_vtable[89]=reinterpret_cast<void*>(&fvf_call);device_vtable[83]=reinterpret_cast<void*>(&draw_call);
    *reinterpret_cast<void**>(mapped_image_base+0x1c4d48)=&device;
    device_vtable[44]=reinterpret_cast<void*>(&transform_call);device_vtable[100]=reinterpret_cast<void*>(&stream_call);device_vtable[81]=reinterpret_cast<void*>(&primitive_call);
    device_vtable[47]=reinterpret_cast<void*>(&viewport_call);
    device_vtable[43]=reinterpret_cast<void*>(&clear_call);
    const auto d3dx=LoadLibraryW(L"d3dx9_43.dll");if(!d3dx)throw std::runtime_error("D3DX SDK unavailable");
    for(const auto& item:pe.imports)if(item.name.rfind("D3DX",0)==0)*reinterpret_cast<std::uintptr_t*>(mapped_image_base+item.iat_rva)=reinterpret_cast<std::uintptr_t>(GetProcAddress(d3dx,item.name.c_str()));
    std::uint32_t viewport[0x16c/4]{};std::memcpy(reinterpret_cast<unsigned char*>(viewport)+0x11c,bounds,16);*reinterpret_cast<void**>(mapped_image_base+0x1c5840)=viewport;
    for(const auto& item:pe.imports)if(item.name=="GetCurrentThreadId")*reinterpret_cast<std::uintptr_t*>(mapped_image_base+item.iat_rva)=reinterpret_cast<std::uintptr_t>(&GetCurrentThreadId);
    for(unsigned index:{1u,9u,10u}){auto* lock=reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c0240+index*0x30);std::memset(lock,0,0x30);lock[0]=0x101;lock[10]=GetCurrentThreadId();lock[11]=1;}
    std::mt19937 rng(0x44c9b0);std::map<std::string,unsigned> counts,failure_counts;unsigned failed=0;std::vector<std::string> failures;
    auto check=[&](const std::string& name,const void* a,const void* b,std::size_t length){++counts[name];if(std::memcmp(a,b,length)){++failed;if(++failure_counts[name]<=3){std::ostringstream why;why<<name<<" case="<<diagnostic_case;for(std::size_t i=0;i<length;++i)if(static_cast<const unsigned char*>(a)[i]!=static_cast<const unsigned char*>(b)[i]){why<<" byte+"<<std::hex<<i<<' '<<unsigned(static_cast<const unsigned char*>(a)[i])<<'/'<<unsigned(static_cast<const unsigned char*>(b)[i]);break;}failures.push_back(why.str());}}};
    struct Snapshot {
        std::vector<std::uint8_t> list,pool,free;std::uint32_t generation;
        explicit Snapshot(s::Controller& c):list(sizeof(c.lists)),pool(sizeof(s::PooledAnimation)*64),free(sizeof(c.free_sentinel)),generation(c.field_7d40e88){std::memcpy(list.data(),c.lists,list.size());std::memcpy(pool.data(),c.pool,pool.size());std::memcpy(free.data(),&c.free_sentinel,free.size());}
        void restore(s::Controller& c)const{std::memcpy(c.lists,list.data(),list.size());std::memcpy(c.pool,pool.data(),pool.size());std::memcpy(&c.free_sentinel,free.data(),free.size());c.field_7d40e88=generation;}
    };
    auto compare=[&](const std::string& name,const Snapshot& a,const Snapshot& b){check(name+"_lists",a.list.data(),b.list.data(),a.list.size());check(name+"_pool",a.pool.data(),b.pool.data(),a.pool.size());check(name+"_free",a.free.data(),b.free.data(),a.free.size());check(name+"_generation",&a.generation,&b.generation,4);};
    auto init=[&]{for(auto& list:c.lists)s::initialize_animation_list(list);c.free_sentinel={};c.field_7d40e88=rng();for(unsigned i=0;i<64;++i){auto& p=c.pool[i];std::memset(&p,0,sizeof(p));s::construct_pooled_animation(p);p.index=i;p.animation.index=i;s::initialize_animation_link(p.free_link,&p.animation);}};
    for(unsigned test=0;test<2000;++test){s::Animation a{};for(auto& word:a.base.fields_444)word=rng();a.base.fields_10_28[6]=rng();a.handle=rng();a.fields_550[7]=rng();auto b=a;cpu<void>(0x44c240,&a);s::destroy_animation_contents(b);check("cleanup_empty_resources",&a,&b,sizeof(a));
        a.fields_550[7]=rng();b=a;cpu<void>(0x44c5f0,&a);s::release_animation_geometry(b);check("geometry_null_preserves_size",&a,&b,sizeof(a));
        const auto color=rng();cpu<void>(0x44fa10,&a,color);s::set_animation_color(b,color);check("color",&a,&b,sizeof(a));
    }
    for(unsigned test=0;test<2000;++test){init();const unsigned index=rng()%64;auto& a=c.pool[index].animation;a.index=rng();const auto initial=c.field_7d40e88;std::uint32_t output=0;cpu<std::uint32_t*>(0x44c8f0,&c,&output,&a);const auto gen=c.field_7d40e88,handle=a.handle;c.field_7d40e88=initial;const auto actual=s::assign_animation_handle(c,a);check("handle_return",&output,&actual,4);check("handle_field",&handle,&a.handle,4);check("handle_generation",&gen,&c.field_7d40e88,4);}
    for(unsigned test=0;test<400;++test){init();const unsigned available=1+rng()%63;for(unsigned i=0;i<available;++i){auto& p=c.pool[i];for(auto& word:p.animation.base.fields_444)word=rng();q::insert_after(reinterpret_cast<q::Link&>(c.free_sentinel),reinterpret_cast<q::Link&>(p.free_link));}
        const Snapshot before(c);auto* original=cpu<s::Animation*>(0x44c9b0,&c);const Snapshot expected(c);before.restore(c);auto* source=s::allocate_animation(c);compare("allocate_pool",expected,Snapshot(c));check("allocate_pointer",&original,&source,sizeof(source));
    }
    for(unsigned test=0;test<300;++test){init();const auto index=rng()%64;auto& a=c.pool[index].animation;c.pool[index].active=1;const bool secondary=(test&1)!=0,front=(test&2)!=0;const Snapshot before(c);std::uint32_t out=0;
        const auto va=secondary?(front?0x44b890u:0x44b8e0u):(front?0x44b840u:0x44b7f0u);cpu<std::uint32_t*>(va,c.lists,&out,&a);const Snapshot expected(c);before.restore(c);const auto source=s::register_animation(c,c.lists,a,secondary,front);compare("register",expected,Snapshot(c));check("register_return",&out,&source,4);
    }
    for(unsigned test=0;test<300;++test){init();for(unsigned i=0;i<30;++i){auto& p=c.pool[i];p.active=1;s::register_animation(c,c.lists,p.animation,i%2,false);p.animation.base.fields_10_28[1]=static_cast<std::uint32_t>(int(rng()%80)-10);p.animation.fields_550[8]=rng()%4==0;}
        const Snapshot before(c);const unsigned index=rng()%30;const auto original=cpu<std::int32_t>(0x44c2f0,&c,&c.pool[index].animation);const Snapshot expected(c);before.restore(c);const auto source=s::retire_animation(c,c.pool[index].animation);compare("retire_pool",expected,Snapshot(c));check("retire_return",&original,&source,4);
        before.restore(c);const auto layer=static_cast<std::int32_t>(rng()%65)-5;const bool secondary=(test&1)!=0;const auto count=cpu<std::uint32_t>(secondary?0x44a210:0x44a090,c.lists,layer);const Snapshot selection(c);before.restore(c);const auto actual=s::select_layer_animations(c.lists,layer,secondary);compare("layer_select",selection,Snapshot(c));check("layer_count",&count,&actual,4);
        before.restore(c);cpu<void>(0x44a3a0,c.lists);const Snapshot empty(c);before.restore(c);s::retire_animation_group(c,c.lists);compare("retire_group",empty,Snapshot(c));
    }
    for(unsigned test=0;test<300;++test){init();s::AnimationFile file{};file.id=rng()%5;for(unsigned i=0;i<40;++i){auto& a=c.pool[i].animation;s::register_animation(c,c.lists,a,i%2,false);a.base.fields_10_28[2]=rng()%5;a.base.fields_10_28[3]=rng()%5;a.base.flags[6]=rng();a.fields_550[8]=rng()%4;}
        const Snapshot before(c);const bool preserve=(test&1)!=0;cpu<void>(0x44fd30,&c,&file,preserve);const Snapshot expected(c);before.restore(c);s::mark_file_animations(c,&file,preserve);compare("file_mark",expected,Snapshot(c));
    }
    init();for(unsigned i=0;i<64;++i){auto& p=c.pool[i];p.active=i%3?1:0;s::register_animation(c,c.lists,p.animation,i%2,false);}
    for(unsigned test=0;test<4000;++test){std::uint32_t handle=test%3==0?0:(test%3==1?c.pool[rng()%64].animation.handle:((rng()&0xffff0000u)|(rng()%64)));const auto original=cpu<s::Animation*>(0x44cd00,&c,handle);const auto source=s::find_animation(c,handle);check("lookup_pool",&original,&source,sizeof(source));}
    for(unsigned i=0;i<64;++i)c.pool[i].animation.handle=((i+1)<<16)|0xffffu;
    for(unsigned test=0;test<200;++test){const auto handle=(rng()%80<<16)|0xffffu;const auto original=cpu<s::Animation*>(0x44cd00,&c,handle);const auto source=s::find_animation(c,handle);check("lookup_heap_lists",&original,&source,sizeof(source));}
    for(unsigned test=0;test<1000;++test){s::Animation chain[3]{};for(unsigned i=0;i<3;++i){auto& a=chain[i];a.base.flags[1]=rng();a.vector_5bc={float(int(rng()%1000)-500)/17.f,3.7f,2.f};a.base.vector_2c={3.4f,float(int(rng()%1000)-500)/31.f,1.f};a.base.vector_484={-1.5f,7.9f,11.f};a.base.vector_38.z=float(int(rng()%1000)-500)/21.f;if(i<2)a.fields_550[2]=reinterpret_cast<std::uint32_t>(&chain[i+1]);}
        s::Vec3 original{};FloatingEnvironment::prepare();cpu<void>(0x44cb80,&chain[0],&original);FloatingEnvironment::prepare();const auto source=s::detached_animation_position(chain[0]);check("detached_position",&original,&source,sizeof(original));}
    for(unsigned test=0;test<400;++test){init();s::AnmInstruction instruction{2,8,0,0};s::AnmInstruction* scripts[]{&instruction};s::Animation prototype{};s::construct_animation(prototype);s::reset_animation_state(prototype);prototype.base.fields_10_28[6]=0;prototype.base.flags[2]=1;prototype.base.flags[1]=0;prototype.base.fields_10_28[1]=rng()%54;
        s::AnimationFile file{};file.templates=&prototype;file.scripts=scripts;file.fields_5c[3]=rng();c.files[0]=&file;
        for(unsigned i=0;i<8;++i)q::insert_after(reinterpret_cast<q::Link&>(c.free_sentinel),reinterpret_cast<q::Link&>(c.pool[i].free_link));
        auto& parent=c.pool[31].animation;parent.field_4e8=test%3?reinterpret_cast<std::uint32_t>(c.lists):0;parent.base.flags[1]=rng();parent.base.fields_10_28[1]=rng()%54;parent.base.vector_38={.25f,-.13f,1.72f};parent.base.vector_2c={3.5f,10.2f,-1.4f};parent.vector_5bc={8.f,4.f,2.f};parent.base.vector_484={-2.f,3.f,4.f};
        auto& grand=c.pool[30].animation;grand.field_4e8=reinterpret_cast<std::uint32_t>(c.lists);grand.base.vector_38.z=-.9f;grand.base.vector_2c={3.f,1.f,4.f};if(test%4==0)parent.fields_550[2]=reinterpret_cast<std::uint32_t>(&grand);
        const auto counter=file.fields_5c[3];const auto flags=rng()%16;const Snapshot before(c);std::uint32_t expected_handle=0;FloatingEnvironment::prepare();cpu<void>(test&1?0x451050:0x4512b0,&file,&expected_handle,0,&parent,flags);const Snapshot expected(c);const auto expected_counter=file.fields_5c[3];
        before.restore(c);file.fields_5c[3]=counter;FloatingEnvironment::prepare();const auto source=test&1?s::spawn_child_animation(c,file,parent,0,flags):s::spawn_detached_animation(c,file,parent,0,flags);compare(test&1?"spawn_child":"spawn_detached",expected,Snapshot(c));check("spawn_handle",&expected_handle,&source,4);check("spawn_counter",&expected_counter,&file.fields_5c[3],4);
    }
    for(unsigned test=0;test<300;++test){init();s::AnmInstruction code[2]{{2,8,0,0},{1,8,0,0}};s::AnmInstruction* scripts[]{code,code+1};s::AnimationFile file{};file.scripts=scripts;c.files[0]=&file;
        for(unsigned i=0;i<30;++i){auto& p=c.pool[i];p.active=1;s::reset_animation_state(p.animation);s::register_animation(c,c.lists,p.animation,i%2,false);p.animation.base.fields_10_28[5]=i%3==0?1:0;p.animation.fields_550[8]=i%5==0?1:0;p.animation.fields_550[0]=rng();p.animation.fields_550[1]=rng();}
        for(unsigned i=2;i<15;i+=2){auto& parent=c.pool[0].animation;auto& child=c.pool[i].animation;q::insert_after(reinterpret_cast<q::Link&>(parent.links[3]),reinterpret_cast<q::Link&>(child.links[2]));child.fields_550[2]=reinterpret_cast<std::uint32_t>(&parent);}
        const bool secondary=(test&1)!=0;const Snapshot before(c);FloatingEnvironment::prepare();cpu<void>(secondary?0x449b20:0x449870,c.lists);const Snapshot expected(c);before.restore(c);FloatingEnvironment::prepare();s::update_animation_group(c,c.lists,secondary);compare("update_and_retire",expected,Snapshot(c));
    }
    for(unsigned test=0;test<4000;++test){s::Animation a{},parent{};s::SpriteData sprite{};s::AnimationFile file{};file.sprites=&sprite;c.files[0]=&file;
        a.base.flags[0]=test%4;a.base.flags[1]=rng();a.base.flags[2]=rng();a.base.flags[3]=rng()%7;a.base.flags[4]=rng()%3;a.base.flags[5]=rng()%3;
        for(unsigned offset:{0x2cu,0x30u,0x34u,0x38u,0x3cu,0x40u,0x50u,0x54u,0x58u,0x5cu,0x70u,0x74u,0x80u,0x84u,0x484u,0x488u,0x48cu,0x5bcu,0x5c0u,0x5c4u})*reinterpret_cast<float*>(reinterpret_cast<unsigned char*>(&a)+offset)=float(int(rng()%10000)-5000)/131.f;
        for(auto& word:sprite.fields_24)word=float_bits(float(int(rng()%1000)-500)/101.f);parent.base.vector_50={.8f,1.3f};parent.base.vector_58={1.2f,.5f};parent.base.vector_38.z=-.71f;parent.base.vector_2c={9.f,-7.f,3.f};if(test&1){a.fields_550[2]=reinterpret_cast<std::uint32_t>(&parent);a.fields_550[3]=reinterpret_cast<std::uint32_t>(&parent);}
        const auto initial=a;const auto parent_initial=parent;s::Vec3 points[4]{{1,2,3},{4,5,6},{7,8,9},{10,11,12}},source[4];std::memcpy(source,points,sizeof(points));
        FloatingEnvironment::prepare();cpu<void>(test&2?0x43ef20:0x43eb40,&c,&a,points,points+1,points+2,points+3);const auto expected=a;const auto parent_expected=parent;a=initial;parent=parent_initial;FloatingEnvironment::prepare();s::calculate_sprite_corners(a,sprite,source,source+1,source+2,source+3,(test&2)!=0);check("quad_corners",points,source,sizeof(points));check("quad_animation_mutation",&expected,&a,sizeof(a));check("quad_parent_mutation",&parent_expected,&parent,sizeof(parent));
    }
    for(unsigned test=0;test<2000;++test){s::Animation a{};a.base.flags[0]=rng();a.base.flags[2]=rng();c.blend_mode=rng()%15;c.field_e12=rng()%5;c.field_e13=rng()%5;c.field_e14=rng()%5;c.field_c0=rng();c.quad_count=0;
        const auto blend=c.blend_mode,filter=c.field_e12,u=c.field_e13,v=c.field_e14;const auto counter=c.field_c0;device_calls.clear();cpu<void>(0x445bc0,&c,&a);const auto expected=device_calls;std::uint32_t state[]{c.blend_mode,c.field_e12,c.field_e13,c.field_e14,c.field_c0};
        c.blend_mode=blend;c.field_e12=filter;c.field_e13=u;c.field_e14=v;c.field_c0=counter;device_calls.clear();s::apply_animation_render_state(c,a,*reinterpret_cast<IDirect3DDevice9*>(&device));const auto actual=device_calls;std::uint32_t final[]{c.blend_mode,c.field_e12,c.field_e13,c.field_e14,c.field_c0};check("render_state_cache",state,final,sizeof(state));const auto expected_count=expected.size(),actual_count=actual.size();check("render_state_call_count",&expected_count,&actual_count,sizeof(expected_count));if(expected_count==actual_count)check("render_state_call_trace",expected.data(),actual.data(),expected_count*sizeof(expected[0]));
        const auto mode=std::uint8_t(rng()%7),old=std::uint8_t(rng()%7);c.field_00=old;device_calls.clear();cpu<void>(0x446690,&c,mode);const auto combine=device_calls;const auto final_mode=c.field_00;c.field_00=old;device_calls.clear();s::select_texture_combine(c,*reinterpret_cast<IDirect3DDevice9*>(&device),mode);const auto count_a=combine.size(),count_b=device_calls.size();check("combine_mode",&final_mode,&c.field_00,1);check("combine_call_count",&count_a,&count_b,sizeof(count_a));if(count_a==count_b)check("combine_call_trace",combine.data(),device_calls.data(),count_a*sizeof(combine[0]));
    }
    for(unsigned test=0;test<3000;++test){const auto bits=rng();float value;std::memcpy(&value,&bits,4);using F=float(__cdecl*)(float);FloatingEnvironment::prepare();const auto a=reinterpret_cast<F>(mapped_image_base+0x45b80)(value);FloatingEnvironment::prepare();const auto b=s::round_sprite_coordinate(value);check("coordinate_round",&a,&b,4);}
    for(unsigned test=0;test<3000;++test){s::Animation a{},parent{};s::SpriteData sprite{};s::TextureRecord texture{};texture.texture=reinterpret_cast<IDirect3DTexture9*>(0x12345678);s::AnimationFile file{};file.sprites=&sprite;file.textures=&texture;c.files[0]=&file;
        a.base.flags[1]=rng();a.base.flags[2]=rng();a.base.flags[0]=rng();a.base.field_490=rng();a.base.field_494=rng();a.field_5cc=rng();parent.field_5cc=rng();if(test%2)a.fields_550[3]=reinterpret_cast<std::uint32_t>(&parent);
        a.base.vector_398={.125f,.25f};a.base.vector_68={.3f,.7f};a.base.field_78=float_bits(.1f);a.base.field_7c=float_bits(-.3f);for(auto& point:a.base.vectors_378)point={float(rng()%100)/41.f,float(rng()%100)/79.f};
        s::Vertex28 quad[4];for(auto& vertex:quad){auto* words=reinterpret_cast<std::uint32_t*>(&vertex);for(unsigned i=0;i<7;++i)words[i]=rng();vertex.x=float(int(rng()%40000)-20000)/11.f;vertex.y=float(int(rng()%40000)-20000)/7.f;}
        if(test%11==0)for(auto& vertex:quad)vertex.x=20000.f;if(test%13==0)for(auto& vertex:quad)vertex.y=-20000.f;
        const auto initial=a;const unsigned flags=rng()%4,initial_color=rng(),initial_blend=rng()%13,initial_filter=rng()%4,initial_u=rng()%4,initial_v=rng()%4,initial_combine=rng()%6;
        auto prepare=[&]{a=initial;c.field_00=static_cast<std::uint8_t>(initial_combine);c.field_c0=0;c.draw_calls=0;for(unsigned i=0;i<4;++i)c.fields_c8[i]=float_bits(float(i)*3.75f);c.cached_texture=test%3?0:0xffffffffu;c.blend_mode=static_cast<std::uint8_t>(initial_blend);c.unknown_cached_e0e=test%4==0?0:1;c.field_e12=static_cast<std::uint8_t>(initial_filter);c.field_e13=static_cast<std::uint8_t>(initial_u);c.field_e14=static_cast<std::uint8_t>(initial_v);c.field_7d40e8c=initial_color;c.field_7d40e90=test%3;c.quad_count=0;c.textured_write=c.textured_batch_start=c.textured_vertices;std::memset(c.textured_vertices,0xa7,6*sizeof(s::Vertex28));device_calls.clear();};
        prepare();std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1aef10),quad,sizeof(quad));FloatingEnvironment::prepare();cpu<void>(0x43f6a0,&c,&a,flags);const auto expected_a=a;const auto expected_calls=device_calls;const auto expected_count=c.quad_count;const auto expected_write=c.textured_write;std::uint32_t expected_state[8]{c.field_00,c.field_c0,c.draw_calls,c.cached_texture,c.blend_mode,c.unknown_cached_e0e,c.field_e12,std::uint32_t(c.field_e13)|(std::uint32_t(c.field_e14)<<8)};s::Vertex28 expected_output[6],expected_quad[4];std::memcpy(expected_output,c.textured_vertices,sizeof(expected_output));std::memcpy(expected_quad,reinterpret_cast<void*>(mapped_image_base+0x1aef10),sizeof(expected_quad));
        prepare();s::Vertex28 actual_quad[4];std::memcpy(actual_quad,quad,sizeof(quad));FloatingEnvironment::prepare();s::submit_animation_quad(c,a,actual_quad,flags);std::uint32_t actual_state[8]{c.field_00,c.field_c0,c.draw_calls,c.cached_texture,c.blend_mode,c.unknown_cached_e0e,c.field_e12,std::uint32_t(c.field_e13)|(std::uint32_t(c.field_e14)<<8)};
        check("quad_submit_animation",&expected_a,&a,sizeof(a));check("quad_submit_globals",expected_quad,actual_quad,sizeof(actual_quad));check("quad_submit_vertices",expected_output,c.textured_vertices,sizeof(expected_output));check("quad_submit_state",expected_state,actual_state,sizeof(actual_state));check("quad_submit_count",&expected_count,&c.quad_count,4);check("quad_submit_pointer",&expected_write,&c.textured_write,4);const auto count_a=expected_calls.size(),count_b=device_calls.size();check("quad_submit_call_count",&count_a,&count_b,sizeof(count_a));if(count_a==count_b)check("quad_submit_call_trace",expected_calls.data(),device_calls.data(),count_a*sizeof(expected_calls[0]));
    }
    for(unsigned test=0;test<200;++test){init();c.fields_c8[4]=0xabcdefu;const auto primary=cpu<int>(0x4497d0,&c);const auto source_primary=s::update_animations(c,false);check("update_primary_return",&primary,&source_primary,4);const auto secondary=cpu<int>(0x449a70,&c);const auto draw_count=c.fields_c8[4];c.fields_c8[4]=0xabcdefu;const auto source_secondary=s::update_animations(c,true);check("update_secondary_return",&secondary,&source_secondary,4);check("update_secondary_counter",&draw_count,&c.fields_c8[4],4);const auto layer=cpu<int>(0x449e40,&c,test%54);const auto source_layer=s::draw_animation_layer(c,test%54);check("layer_callback_return",&layer,&source_layer,4);}
    for(unsigned test=0;test<2400;++test){s::Animation a{};s::construct_animation(a);s::reset_animation_state(a);a.base.flags[0]=rng();a.base.flags[1]=rng();a.base.flags[2]=rng();a.base.field_490=rng();a.base.vector_50={.9f,-1.2f};a.base.vector_58={1.3f,.4f};a.base.vector_68={test%2?1.f:.7f,test%3?1.f:1.4f};a.base.vector_38={float(int(rng()%70)-35)/23.f,float(int(rng()%30)-15)/17.f,float(int(rng()%50)-25)/11.f};a.base.vector_2c={3.f,4.f,5.f};a.vector_5bc={5.f,-2.f,1.f};a.base.vector_484={.1f,.4f,.7f};a.base.field_78=float_bits(test%2?0.f:.13f);a.base.field_7c=float_bits(test%3?0.f:-.12f);a.base.matrix_3f8={{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
        s::SpriteData sprite{};s::TextureRecord texture{};texture.texture=reinterpret_cast<IDirect3DTexture9*>(0x12345678);s::AnimationFile file{};file.sprites=&sprite;file.textures=&texture;c.files[0]=&file;std::uint32_t input[7*15];for(auto& value:input)value=float_bits(float(int(rng()%1000)-500)/31.f);
        const auto initial=a;const auto old_blend=std::uint8_t(rng()%13),old_cache=std::uint8_t(rng()%7);const auto raw_count=std::uint32_t(int(rng()%17)-2),kind=test%3;const auto initial_depth=rng()%3;
        auto prepare=[&]{a=initial;c.quad_count=test%4?0:1;c.textured_write=c.textured_batch_start=c.textured_vertices;c.field_c0=c.draw_calls=0;c.field_00=test%4;c.blend_mode=old_blend;c.unknown_cached_e0e=old_cache;c.cached_texture=test%2?0:0xffffffffu;c.field_e12=c.field_e13=c.field_e14=0;c.field_e18=test%2?reinterpret_cast<std::uint32_t>(&sprite):0;c.fields_c8[2]=float_bits(7.3f);c.fields_c8[3]=float_bits(-8.5f);depth_value=initial_depth;*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c5afc)=initial_depth;device_calls.clear();matrix_calls.clear();};
        std::uint32_t original_vertices[7*15],source_vertices[7*15];std::memcpy(original_vertices,input,sizeof(input));std::memcpy(source_vertices,input,sizeof(input));prepare();FloatingEnvironment::prepare();int result=0;if(kind==2)cpu<void>(0x443020,&c,&a,original_vertices,raw_count);else result=cpu<int>(kind==0?0x445350:0x445130,&c,&a,original_vertices,raw_count);const auto expected_a=a;const auto expected_calls=device_calls;const auto expected_matrices=matrix_calls;std::uint32_t state[]{c.field_00,c.field_c0,c.draw_calls,c.cached_texture,c.unknown_cached_e0e,c.field_e18,c.quad_count,*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c5afc)};
        prepare();FloatingEnvironment::prepare();int actual=0;if(kind==2)s::primitive::p443020(c,a,source_vertices,raw_count);else actual=kind==0?s::primitive::p445350(c,a,reinterpret_cast<float*>(source_vertices),raw_count):s::primitive::p445130(c,a,reinterpret_cast<float*>(source_vertices),raw_count);std::uint32_t actual_state[]{c.field_00,c.field_c0,c.draw_calls,c.cached_texture,c.unknown_cached_e0e,c.field_e18,c.quad_count,depth_value};
        const auto name="geometry_draw_"+std::to_string(kind);check(name+"_animation",&expected_a,&a,sizeof(a));check(name+"_vertices",original_vertices,source_vertices,sizeof(input));check(name+"_state",state,actual_state,sizeof(state));if(kind!=2)check(name+"_return",&result,&actual,4);const auto count_a=expected_calls.size(),count_b=device_calls.size();check(name+"_calls",&count_a,&count_b,sizeof(count_a));if(count_a==count_b)check(name+"_trace",expected_calls.data(),device_calls.data(),count_a*sizeof(expected_calls[0]));const auto matrix_count=expected_matrices.size(),actual_count=matrix_calls.size();check(name+"_matrix_count",&matrix_count,&actual_count,sizeof(matrix_count));if(matrix_count==actual_count)check(name+"_matrices",expected_matrices.data(),matrix_calls.data(),matrix_count*sizeof(expected_matrices[0]));
    }
    for(unsigned test=0;test<8000;++test){std::uint64_t storage[0x108/8]{};const auto index=test%8;const std::uint64_t begin=(std::uint64_t(rng())<<32)|rng(),end=(std::uint64_t(rng())<<32)|rng();storage[0x50/8+index]=begin;storage[0x90/8+index]=end;std::int64_t signed_begin,signed_end;std::memcpy(&signed_begin,&begin,8);std::memcpy(&signed_end,&end,8);FloatingEnvironment::prepare();const auto original=cpu<double>(0x44cf50,storage,index);FloatingEnvironment::prepare();const auto source=s::elapsed_frame_interval(signed_begin,signed_end);check("elapsed_time_cast",&original,&source,8);}
    for(unsigned test=0;test<7200;++test){const unsigned type=test%18;const float x=float(int(rng()%10000)-5000)/137.f,y=float(int(rng()%10000)-5000)/137.f,width=float(int(rng()%10000)-5000)/137.f,height=float(int(rng()%10000)-5000)/137.f,angle=float(int(rng()%1000)-500)/107.f;const auto primary=rng(),secondary=rng(),count=3+rng()%24;const int ax=int(rng()%5)-1,ay=int(rng()%5)-1;const auto initial_combine=std::uint8_t(rng()%6);const auto initial_depth=rng()%3;
        auto prepare=[&]{c.quad_count=test%4?0:1;c.textured_write=c.textured_batch_start=c.textured_vertices;c.draw_calls=0;c.unknown_cached_e0e=0;c.field_00=initial_combine;c.colored_write=c.colored_vertices;std::memset(c.colored_vertices,0xa8,64*sizeof(s::Vertex20));c.fields_c8[2]=float_bits(2.25f);c.fields_c8[3]=float_bits(-3.75f);depth_value=initial_depth;*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c5afc)=initial_depth;device_calls.clear();};
        diagnostic_case=test;diagnostic_phase="original colored";prepare();FloatingEnvironment::prepare();int result=0;switch(type){case 0:cpu<void>(0x439a00,&c,x,y,width,height,angle,primary,secondary,ax,ay);break;case 1:result=cpu<int>(0x43a1c0,&c,x,y,width,height,angle,primary,secondary,ax,ay);break;case 2:result=cpu<int>(0x43d830,&c,x,y,primary);break;case 3:result=cpu<int>(0x43b960,&c,x,y,width,angle,count,primary,secondary);break;case 4:result=cpu<int>(0x43bc60,&c,x,y,width,angle,count,primary);break;case 5:result=cpu<int>(0x43be80,&c,x,y,width,height,angle,count,primary,secondary);break;case 6:cpu<void>(0x43c180,&c,x,y,width,height,angle,count,primary,secondary);break;case 7:cpu<void>(0x43c4e0,&c,x,y,width,height,angle,count,primary,secondary);break;case 8:cpu<void>(0x43c780,&c,x,y,width,height,x,angle,count,primary,secondary);break;case 9:result=cpu<int>(0x43d9b0,&c,x,y,width,height,angle,count,primary,secondary);break;case 10:result=cpu<int>(0x43dce0,&c,x,y,width,height,angle,count,primary);break;case 11:result=cpu<int>(0x43df30,&c,x,y,width,height,x,angle,count,primary,secondary);break;case 12:cpu<void>(0x43cd40,&c,x,y,width,angle,primary,secondary,ax,ay);break;case 13:cpu<void>(0x43e2c0,&c,x,y,width,height,angle,primary,secondary,ax,ay);break;case 14:cpu<void>(0x43b0e0,&c,x,y,width,height,angle,primary,secondary,ax,ay);break;case 15:result=cpu<int>(0x43a2b0,&c,x,y,width,height,x,angle,count,primary,secondary);break;case 16:result=cpu<int>(0x43a750,&c,x,y,width,height,x,angle,count,primary);break;case 17:result=cpu<int>(0x43ab20,&c,x,y,width,height,x,y,angle,count,primary,secondary);break;}
        s::Vertex20 expected_vertices[64];std::memcpy(expected_vertices,c.colored_vertices,sizeof(expected_vertices));const auto expected_calls=device_calls;std::uint32_t expected_state[]{c.draw_calls,c.unknown_cached_e0e,c.field_00,c.quad_count,reinterpret_cast<std::uint32_t>(c.colored_write),*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c5afc)};
        diagnostic_phase="source colored";prepare();FloatingEnvironment::prepare();int actual=0;switch(type){case 0:s::primitive::p439a00(c,x,y,width,height,angle,primary,secondary,ax,ay);break;case 1:actual=s::primitive::p43a1c0(c,x,y,width,height,angle,primary,secondary,ax,ay);break;case 2:actual=s::primitive::p43d830(c,x,y,primary);break;case 3:actual=s::primitive::p43b960(c,x,y,width,angle,count,primary,secondary);break;case 4:actual=s::primitive::p43bc60(c,x,y,width,angle,count,primary);break;case 5:actual=s::primitive::p43be80(c,x,y,width,height,angle,count,primary,secondary);break;case 6:s::primitive::p43c180(c,x,y,width,height,angle,count,primary,secondary);break;case 7:s::primitive::p43c4e0(c,x,y,width,height,angle,count,primary,secondary);break;case 8:s::primitive::p43c780(c,x,y,width,height,x,angle,count,primary,secondary);break;case 9:actual=s::primitive::p43d9b0(c,x,y,width,height,angle,count,primary,secondary);break;case 10:actual=s::primitive::p43dce0(c,x,y,width,height,angle,count,primary);break;case 11:actual=s::primitive::p43df30(c,x,y,width,height,x,angle,count,primary,secondary);break;case 12:s::primitive::p43cd40(c,x,y,width,angle,primary,secondary,ax,ay);break;case 13:s::primitive::p43e2c0(c,x,y,width,height,angle,primary,secondary,ax,ay);break;case 14:s::primitive::p43b0e0(c,x,y,width,height,angle,primary,secondary,ax,ay);break;case 15:actual=s::primitive::p43a2b0(c,x,y,width,height,x,angle,count,primary,secondary);break;case 16:actual=s::primitive::p43a750(c,x,y,width,height,x,angle,count,primary);break;case 17:actual=s::primitive::p43ab20(c,x,y,width,height,x,y,angle,count,primary,secondary);break;}
        std::uint32_t actual_state[]{c.draw_calls,c.unknown_cached_e0e,c.field_00,c.quad_count,reinterpret_cast<std::uint32_t>(c.colored_write),depth_value};const auto name="colored_primitive_"+std::to_string(type);check(name+"_vertices",expected_vertices,c.colored_vertices,sizeof(expected_vertices));check(name+"_state",expected_state,actual_state,sizeof(expected_state));if((type>=1&&type<=5)||(type>=9&&type<=11)||type>=15)check(name+"_return",&result,&actual,4);const auto n1=expected_calls.size(),n2=device_calls.size();check(name+"_count",&n1,&n2,sizeof(n1));if(n1==n2)check(name+"_trace",expected_calls.data(),device_calls.data(),n1*sizeof(expected_calls[0]));
    }

    for(unsigned test=0;test<4704;++test){const unsigned type=test%8;diagnostic_case=test;diagnostic_phase="projection preparation";
        s::Animation a{},parent{};s::construct_animation(a);s::construct_animation(parent);s::reset_animation_state(a);s::reset_animation_state(parent);
        auto finite=[&]{return float(int(rng()%1000)-500)/499.f;};
        const s::Matrix4 identity{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
        a.base.matrix_3b8=identity;a.base.matrix_3f8=identity;a.matrix_57c=identity;
        a.base.matrix_3b8.elements[12]=finite();a.base.matrix_3b8.elements[13]=finite();a.base.matrix_3b8.elements[14]=finite();
        a.base.flags[0]=rng()|0x10000;a.base.flags[1]=rng()|1;a.base.flags[2]=rng();a.base.flags[3]=rng()%7;a.base.flags[4]=rng()%3;a.base.flags[5]=rng()%3;if(type==7)a.base.flags[0]=(a.base.flags[0]&0xffffff00u)|((test/8)%49);
        a.base.vector_2c={finite(),finite(),test%3?.5f:2.f};a.vector_5bc={finite(),finite(),0};a.base.vector_484={finite(),finite(),0};a.base.vector_80={finite(),finite(),finite()};a.base.vector_38={finite(),finite(),finite()};a.base.vector_50={finite(),finite()};a.base.vector_58={finite(),finite()};a.base.vector_68={finite(),finite()};a.base.vector_70={finite(),finite()};a.base.field_490=rng();a.base.field_494=rng();a.base.field_4b8=float_bits(5.f);a.base.field_4bc=float_bits(.3f);
        parent.base.vector_2c={finite(),finite(),finite()};parent.base.vector_38={finite(),finite(),finite()};parent.base.vector_50={finite(),finite()};parent.base.vector_58={finite(),finite()};parent.base.flags[1]=0x1000;
        std::uint32_t geometry[80*7],initial_geometry[80*7];for(auto& word:initial_geometry)word=float_bits(finite());a.fields_550[5]=reinterpret_cast<std::uint32_t>(geometry);a.base.fields_444[0]=3+rng()%22;a.base.fields_444[4]=float_bits(finite());a.base.fields_444[5]=float_bits(finite());
        if(test%3==0)a.fields_550[2]=reinterpret_cast<std::uint32_t>(&parent);if(test%4==0)a.fields_550[3]=reinterpret_cast<std::uint32_t>(&parent);
        s::SpriteData sprite{};sprite.fields_24[2]=sprite.fields_24[3]=float_bits(1.f);s::TextureRecord texture{};texture.texture=reinterpret_cast<IDirect3DTexture9*>(0x12345678);s::AnimationFile file{};file.sprites=&sprite;file.textures=&texture;c.files[0]=&file;
        camera={};camera.view=*reinterpret_cast<const D3DMATRIX*>(&identity);camera.projection=camera.view;camera.viewport={0,0,640,480,0,1};camera.vectors[4][1]=1;camera.vectors[0][2]=finite();std::memcpy(camera.bounds,bounds,16);camera.final_state[0]=float_bits(.8f);camera.final_state[1]=float_bits(20.f);camera.final_state[2]=float_bits(128.5f);camera.final_state[3]=float_bits(64.3f);camera.final_state[4]=float_bits(32.1f);camera.final_state[6]=0x123456u;
        *reinterpret_cast<void**>(mapped_image_base+0x1c5840)=&camera;*reinterpret_cast<int*>(mapped_image_base+0x1b87f8)=640;*reinterpret_cast<int*>(mapped_image_base+0x1b87fc)=480;
        s::Vertex28 initial_quad[4];for(auto& v:initial_quad){v={finite()*100,finite()*100,finite(),finite(),rng(),finite(),finite()};}
        const auto initial=a,initial_parent=parent;const auto initial_depth=rng()%3,initial_color=rng(),initial_combine=rng()%4,initial_cache=rng()%6;const auto initial_tint=rng(),use_tint=rng()%2;
        auto prepare=[&]{a=initial;parent=initial_parent;std::memcpy(geometry,initial_geometry,sizeof(geometry));c.colored_write=c.colored_vertices;std::memset(c.colored_vertices,0xa8,64*sizeof(s::Vertex20));fog_value=0;*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c5af8)=0;c.quad_count=test%4?0:1;c.textured_write=c.textured_batch_start=c.textured_vertices;std::memset(c.textured_vertices,0,24*sizeof(s::Vertex28));c.field_c0=c.draw_calls=0;c.field_00=initial_combine;c.blend_mode=0;c.unknown_cached_e0e=initial_cache;c.cached_texture=0xffffffffu;c.field_e12=c.field_e13=c.field_e14=0;c.field_e18=0;c.field_e04=initial_color;c.field_7d40e8c=initial_tint;c.field_7d40e90=use_tint;c.corner_buffer=reinterpret_cast<IDirect3DVertexBuffer9*>(0x22334455);c.matrix_60007d8=identity;c.fields_c8[0]=c.fields_c8[1]=0;c.fields_c8[2]=float_bits(1.25f);c.fields_c8[3]=float_bits(-2.75f);for(unsigned i=0;i<4;++i)c.corners[i]={float(i&1),float(i>>1),.3f,0,0};std::memcpy(s::animation_quad,initial_quad,sizeof(initial_quad));std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1aef10),initial_quad,sizeof(initial_quad));depth_value=initial_depth;*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c5afc)=initial_depth;device_calls.clear();matrix_calls.clear();};
        prepare();FloatingEnvironment::prepare();diagnostic_phase="projection original";int result=0;switch(type){case 0:result=cpu<int>(0x440340,&c,&a);break;case 1:result=cpu<int>(0x441430,&c,&a);break;case 2:cpu<void>(0x440310,&c,&a);break;case 3:cpu<void>(0x4413a0,&c,&a);break;case 4:cpu<void>(0x4408b0,&c,&a);break;case 5:cpu<void>(0x441c00,&c,&a);break;case 6:cpu<void>(0x441f00,&c,&a);break;case 7:cpu<void>(0x443880,&c,&a);break;}
        std::uint32_t expected_geometry[80*7];s::Vertex20 expected_colored[64];std::memcpy(expected_geometry,geometry,sizeof(geometry));std::memcpy(expected_colored,c.colored_vertices,sizeof(expected_colored));const auto expected_a=a,expected_parent=parent;const auto expected_matrix=c.matrix_60007d8;const auto calls=device_calls;const auto matrices=matrix_calls;s::Vertex28 expected_quad[4],expected_vertices[24];std::memcpy(expected_quad,reinterpret_cast<void*>(mapped_image_base+0x1aef10),sizeof(expected_quad));std::memcpy(expected_vertices,c.textured_vertices,sizeof(expected_vertices));std::uint32_t expected_state[]{c.field_00,c.field_c0,c.draw_calls,c.cached_texture,c.unknown_cached_e0e,c.field_e18,c.quad_count,*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c5afc),c.field_e04,reinterpret_cast<std::uint32_t>(c.colored_write),*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c5af8)};
        prepare();FloatingEnvironment::prepare();diagnostic_phase="projection source";int actual=0;switch(type){case 0:actual=s::prepare_projected_billboard(a);break;case 1:actual=s::prepare_projected_matrix(c,a);break;case 2:s::primitive::p440310(c,a);break;case 3:s::primitive::p4413a0(c,a);break;case 4:s::primitive::p4408b0(c,a);break;case 5:s::primitive::p441c00(c,a);break;case 6:s::primitive::p441f00(c,a);break;case 7:s::draw_animation(c,a);break;}
        const auto name=(type==7?"full_dispatch_mode_"+std::to_string((test/8)%49):"projection_"+std::to_string(type));check(name+"_geometry",expected_geometry,geometry,sizeof(geometry));check(name+"_colored",expected_colored,c.colored_vertices,sizeof(expected_colored));check(name+"_animation",&expected_a,&a,sizeof(a));check(name+"_parent",&expected_parent,&parent,sizeof(parent));check(name+"_world",&expected_matrix,&c.matrix_60007d8,sizeof(expected_matrix));check(name+"_quad",expected_quad,s::animation_quad,sizeof(expected_quad));check(name+"_vertices",expected_vertices,c.textured_vertices,sizeof(expected_vertices));std::uint32_t actual_state[]{c.field_00,c.field_c0,c.draw_calls,c.cached_texture,c.unknown_cached_e0e,c.field_e18,c.quad_count,depth_value,c.field_e04,reinterpret_cast<std::uint32_t>(c.colored_write),fog_value};check(name+"_state",expected_state,actual_state,sizeof(expected_state));if(type<2)check(name+"_return",&result,&actual,4);const auto count=calls.size(),actual_count=device_calls.size();check(name+"_calls",&count,&actual_count,sizeof(count));if(count==actual_count)check(name+"_trace",calls.data(),device_calls.data(),count*sizeof(calls[0]));const auto n1=matrices.size(),n2=matrix_calls.size();check(name+"_matrix_count",&n1,&n2,sizeof(n1));if(n1==n2)check(name+"_matrices",matrices.data(),matrix_calls.data(),n1*sizeof(matrices[0]));
    }

    #include "loading_interrupt_cpu_cases.inc"
    #include "render_mesh_cpu_cases.inc"
    #include "../stage_background/object_projection_cpu_cases.inc"
    #include "../stage_background/object_draw_cpu_cases.inc"
    #include "../stage_background/vm_pool_cpu_cases.inc"
    #include "../effect_system/pool_cpu_cases.inc"
    #include "../effect_system/short_line_cpu_cases.inc"
    #include "../effect_system/spiral_trail_cpu_cases.inc"
    #include "../effect_system/rings_cpu_cases.inc"
    #include "../effect_system/converging_cpu_cases.inc"
    #include "../effect_system/transition_cpu_cases.inc"
    #include "../effect_system/stone_cpu_cases.inc"
    #include "../hud_system/feedback_cpu_cases.inc"
    #include "../hud_system/draw_cpu_cases.inc"
    #include "../hud_system/scoring_cpu_cases.inc"
    #include "../hud_system/icons_cpu_cases.inc"
    #include "../hud_system/notifications_cpu_cases.inc"
    #include "../bomb_system/pool_cpu_cases.inc"
    #include "../bomb_system/character_cpu_cases.inc"
    #include "../bullet_system/pool_cpu_cases.inc"
    #include "../laser_system/pool_cpu_cases.inc"
    unsigned total=0;for(const auto& item:counts)total+=item.second;
    std::ofstream out(argv[2]);out<<"{\n\"status\":"<<th20::json_string(failed?"failed":"passed")<<",\"original_sha256\":"<<th20::json_string(expected_sha)<<",\"total\":"<<total<<",\"failed\":"<<failed<<",\"source_hashes\":{";
    bool first_hash=true;for(const auto& source:bound_source_hashes){if(!first_hash)out<<',';first_hash=false;out<<th20::json_string(source.path)<<':'<<th20::json_string(source.hash);}out<<"},\"comparisons\":{";
    bool first=true;for(const auto& item:counts){if(!first)out<<',';first=false;out<<th20::json_string(item.first)<<':'<<item.second;}out<<"},\"failure_counts\":{";first=true;for(const auto& item:failure_counts){if(!first)out<<',';first=false;out<<th20::json_string(item.first)<<':'<<item.second;}out<<"},\"failure_examples\":[";first=true;for(const auto& failure:failures){if(!first)out<<',';first=false;out<<th20::json_string(failure);}out<<"]}\n";
    std::cout<<"Pool CPU comparisons "<<total<<" failed "<<failed<<'\n';for(const auto& failure:failures)std::cout<<failure<<'\n';VirtualFree(controller,0,MEM_RELEASE);return failed?1:0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}}





