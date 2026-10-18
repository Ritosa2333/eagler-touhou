// Test-only complete Controller construction oracle. Original instructions are
// unchanged; only their real Win32 imports and heap/lock globals are initialized.
#define wmain unused_controller_native_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "controller.hpp"
#include "pool.hpp"
#include "anm_vm.hpp"
#include "binding.hpp"
#include "quad.hpp"
#include "draw.hpp"
#include "dispatch.hpp"
#include "../runtime_core/runtime_core.hpp"
#include "controller_source_hashes.hpp"
#include <memory>
namespace s=th20::source::sprite;namespace q=th20::source::scheduler;
namespace {
s::Controller* subject=nullptr;float clock_scale=1.f;
[[noreturn]]void outside(const char* name){throw std::runtime_error(std::string("Unexpected gameplay dependency in constructor test: ")+name);}
template<class R,class... A>R cpu(std::uint32_t va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
template<class T>T& global(std::uint32_t va){return *reinterpret_cast<T*>(mapped_image_base+va-0x400000);}
LONG WINAPI fault(EXCEPTION_POINTERS* p){std::fprintf(stderr,"Controller oracle exception %08lx at %08lx original base %08x\n",p->ExceptionRecord->ExceptionCode,p->ContextRecord->Eip,mapped_image_base);return EXCEPTION_EXECUTE_HANDLER;}
LONG WINAPI first_fault(EXCEPTION_POINTERS* p){fault(p);const auto* stack=reinterpret_cast<std::uint32_t*>(p->ContextRecord->Esp);for(unsigned i=0;i<12;++i)std::fprintf(stderr,"stack%u %08x va%08x\n",i,stack[i],stack[i]-mapped_image_base+0x400000);return EXCEPTION_CONTINUE_SEARCH;}
unsigned declarations=0;
HRESULT WINAPI declaration(void*,IDirect3DVertexShader9* value){if(value)outside("non-null shader");++declarations;return S_OK;}
void* device_table[119]{};struct Device {void** vtable=device_table;} device;
using NodeState=std::array<std::uint32_t,7>;
std::vector<NodeState> registrations(q::State& state){
    std::vector<NodeState> result;
    for(auto* list:{&state.update,&state.draw})for(auto* link=list->sentinel.next;link;link=link->next){
        if(result.size()>52)outside("scheduler list cycle");const auto& node=*link->value;
        result.push_back({list==&state.draw,std::uint32_t(node.priority),node.flags,node.callback?1u:0u,node.before_insert?1u:0u,node.on_shutdown?1u:0u,node.userdata==subject});
    }return result;
}
void clear_original_nodes(q::State& state){
    // Original new uses its CRT heap, initialized below to the process heap.
    for(auto* list:{&state.update,&state.draw})for(auto* link=list->sentinel.next;link;){auto* node=link->value;link=link->next;if(!HeapFree(GetProcessHeap(),0,node))outside("original node HeapFree");}
    q::initialize_state(state);
}
void populate(bool original,unsigned count){
    for(unsigned i=0;i<count;++i){
        auto* animation=original?cpu<s::Animation*>(0x44c9b0,subject):s::allocate_animation(*subject);
        const bool secondary=(i&2)!=0,front=(i&1)!=0;std::uint32_t handle=0;
        if(original){const std::uint32_t addresses[4]={0x44b7f0,0x44b840,0x44b8e0,0x44b890};cpu<void>(addresses[i&3],subject->lists,&handle,animation);}
        else handle=s::register_animation(*subject,subject->lists,*animation,secondary,front);
        if(handle!=animation->handle)outside("active animation handle");
    }
}
}
// These fixtures are intentionally fail-fast. Full construction/destruction
// with no active scripts must not query the game, run ANM or draw a primitive.
namespace th20::source::sprite::controller_environment {bool suppress_primary_update(){outside("primary callback");}}
namespace th20::source::sprite::anm_environment {
float& clock_scale(){return ::clock_scale;}const float* timer_rate(){return &::clock_scale;}
AnmInstruction* script(Animation&){outside("script");}bool gameplay_frozen(){outside("gameplay_frozen");}
std::uint32_t random_next(){outside("random_next");}std::uint32_t random_bounded(std::uint32_t){outside("random_bounded");}
float random_unit(){outside("random_unit");}float random_signed_unit(){outside("random_signed_unit");}float camera_component(std::int32_t){outside("camera_component");}
void assign_sprite(Animation&,std::int32_t){outside("assign_sprite");}void set_layer(Animation&,std::int32_t){outside("set_layer");}
void add_camera_offset(Vec3&){outside("camera_offset");}void calculate_corners(Animation&,Vec3(&)[4]){outside("calculate_corners");}
float screen_scale(){outside("screen_scale");}std::int32_t screen_offset(unsigned,unsigned){outside("screen_offset");}
void* allocate_geometry(std::uint32_t){outside("allocate_geometry");}
std::uint32_t spawn_child(Animation&,std::int32_t,std::uint32_t){outside("spawn_child");}std::uint32_t spawn_detached(Animation&,std::int32_t,std::uint32_t){outside("spawn_detached");}
Animation& lookup_animation(std::uint32_t){outside("lookup_animation");}void spawn_effect(Animation&,std::int32_t){outside("spawn_effect");}
}
namespace th20::source::sprite::dispatch_environment {
Controller& controller(){return *subject;}void select_camera(std::int32_t){outside("select_camera");}void select_layer_camera(std::int32_t){outside("select_layer_camera");}
void select_viewport_camera(std::int32_t){outside("select_viewport_camera");}void disable_fog(){outside("fog");}void disable_depth_write(){outside("depth");}void set_render_state(std::uint32_t,std::uint32_t){outside("render_state");}
}
namespace th20::source::sprite {void draw_animation(Controller&,Animation&){outside("draw_animation");}}
namespace th20::source::sprite::draw_environment {
Controller& controller(){return *subject;}IDirect3DDevice9& device(){return *reinterpret_cast<IDirect3DDevice9*>(&::device);}const float* viewport_bounds(){outside("viewport_bounds");}
void enable_fog(){outside("enable_fog");}void disable_fog(){outside("disable_fog");}void enable_depth_write(){outside("enable_depth");}void disable_depth_write(){outside("disable_depth");}
}
int wmain(int argc,wchar_t** argv){
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);SetUnhandledExceptionFilter(fault);
    AddVectoredExceptionHandler(1,first_fault);
    try{
        if(argc!=3)throw std::runtime_error("Usage: controller_cpu_compare ORIGINAL.exe REPORT.json");
        auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original hash mismatch");
        auto pe=th20::parse_pe(bytes);Mapping mapping(bytes,pe);mapped_image_base=mapping.address();
        for(const auto& item:pe.imports)if(item.name=="GetCurrentThreadId"||item.name=="AcquireSRWLockExclusive"||item.name=="ReleaseSRWLockExclusive"||item.name=="HeapAlloc"||item.name=="HeapFree"||item.name=="GetLastError"){
            auto* address=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),item.name.c_str());if(!address)outside("Win32 import");
            *reinterpret_cast<std::uintptr_t*>(mapped_image_base+item.iat_rva)=reinterpret_cast<std::uintptr_t>(address);
        }
        global<HANDLE>(0x5e5990)=GetProcessHeap();cpu<void>(0x452e00,reinterpret_cast<void*>(mapped_image_base+0x1c0240));
        device_table[92]=reinterpret_cast<void*>(&declaration);global<void*>(0x5c4d48)=&device;
        // Allocate once: every self-pointer has exactly the same address in
        // original and source snapshots, so no heuristic pointer normalization.
        auto* allocation=::operator new(sizeof(s::Controller));subject=new(allocation)s::Controller;
        global<s::Controller*>(0x5c0028)=subject;
        q::State original_state{},source_state{};q::Environment environment;
        std::vector<std::uint8_t> original_after(sizeof(s::Controller));
        std::vector<std::uint8_t> original_dead(sizeof(s::Controller));
        unsigned checks=0,failed=0;std::vector<std::string> failures;
        auto check=[&](const char* name,bool same){++checks;if(!same){++failed;failures.emplace_back(name);}};
        for(unsigned pattern:{0u,0xa5u}){
            std::fprintf(stderr,"Controller original construct pattern%x\n",pattern);
            std::memset(subject,pattern,sizeof(*subject));q::initialize_state(original_state);q::initialize_state(source_state);
            std::memset(s::animation_quad,pattern,sizeof(s::animation_quad));std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1aef10),s::animation_quad,sizeof(s::animation_quad));
            std::memset(s::uncolored_quad,pattern,sizeof(s::uncolored_quad));std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1c0030),s::uncolored_quad,sizeof(s::uncolored_quad));
            global<q::State*>(0x5b66d8)=&original_state;declarations=0;FloatingEnvironment::prepare();
            const auto* returned=cpu<s::Controller*>(0x447e00,subject);check("original returns this and clears shader",returned==subject&&declarations==1);
            std::memcpy(original_after.data(),subject,sizeof(*subject));const auto original_callbacks=registrations(original_state);
            populate(true,pattern?32u:0u);
            std::fprintf(stderr,"Controller original destroy pattern%x\n",pattern);
            cpu<void>(0x449260,subject);std::memcpy(original_dead.data(),subject,sizeof(*subject));clear_original_nodes(original_state);
            std::memset(subject,pattern,sizeof(*subject));declarations=0;FloatingEnvironment::prepare();
            s::construct_controller(*subject,source_state,environment,*reinterpret_cast<IDirect3DDevice9*>(&device));
            std::fprintf(stderr,"Controller source constructed pattern%x\n",pattern);
            check("52 scheduler registrations",registrations(source_state)==original_callbacks&&original_callbacks.size()==52);
            check("one null vertex shader",declarations==1);
            const bool same=std::memcmp(subject,original_after.data(),sizeof(*subject))==0;
            if(!same){unsigned reported=0;for(std::size_t i=0;i<sizeof(*subject)&&reported<16;++i)if(reinterpret_cast<unsigned char*>(subject)[i]!=original_after[i]){std::fprintf(stderr,"Constructor pattern%x offset%x original%02x source%02x\n",pattern,unsigned(i),unsigned(original_after[i]),unsigned(reinterpret_cast<unsigned char*>(subject)[i]));++reported;}}
            check("all object bytes including pool/self links/padding",same);
            check("textured global all bytes",std::memcmp(s::animation_quad,reinterpret_cast<void*>(mapped_image_base+0x1aef10),sizeof(s::animation_quad))==0);
            check("uncolored global all bytes",std::memcmp(s::uncolored_quad,reinterpret_cast<void*>(mapped_image_base+0x1c0030),sizeof(s::uncolored_quad))==0);
            populate(false,pattern?32u:0u);s::destroy_controller_contents(*subject);
            check("destructor all bytes with empty or 32 active pooled animations",std::memcmp(subject,original_dead.data(),sizeof(*subject))==0);
            q::shutdown_chains(source_state,environment);
        }
        ::operator delete(allocation);subject=nullptr;
        std::ofstream report(argv[2]);report<<"{\n\"status\":\""<<(failed?"failed":"passed")<<"\",\n\"checks\":"<<checks<<",\n\"failed\":"<<failed<<",\n\"controller_bytes\":"<<sizeof(s::Controller)<<",\n\"controller_cpp_sha256\":\""<<TH20_CONTROLLER_SHA<<"\",\n\"scope\":\"Unmodified original447e00 full constructor and449260 destructor versus source; zero and A5 object/global inputs, exact whole-object bytes,52 scheduler registrations, SetVertexShader(null), empty/32 active pooled-animation teardown\",\n\"limitations\":[\"No active scripts or callbacks are dispatched\",\"No active worker/geometry destruction in this report\"],\n\"failure_examples\":[";
        for(std::size_t i=0;i<failures.size();++i){if(i)report<<',';report<<th20::json_string(failures[i]);}report<<"],\n\"source_hashes\":{";bool first=true;for(const auto& item:controller_source_hashes){if(!first)report<<',';first=false;report<<th20::json_string(item.path)<<':'<<th20::json_string(item.sha);}report<<"}\n}\n";if(!report)throw std::runtime_error("Report write failed");
        std::cout<<"Controller whole-object: "<<checks<<" checks, "<<failed<<" failures\n";return failed?1:0;
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}
}
