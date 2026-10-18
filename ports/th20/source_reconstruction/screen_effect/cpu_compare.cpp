#define wmain unused_screen_native_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "effect.hpp"
#include "../program_entry/program_entry.hpp"
#include "../gameplay/gameplay.hpp"
#include "../runtime_state/state.hpp"
#include <bit>
namespace se=th20::source::screen;namespace pe=th20::source::program_entry;namespace st=th20::source::state;namespace gp=th20::source::gameplay;
// The tested environment.cpp uses these actual engine layouts; no game entry point or
// background code is linked into this isolated state comparison executable.
namespace th20::source::program_entry {GraphicsStatePrefix graphics_state{};WindowStatePrefix window_state{};FunctionController* function_controller=nullptr;scheduler::Environment scheduler_environment;SpriteController* sprite_controller=nullptr;}
namespace th20::source::gameplay {GameController* controller=nullptr;std::uint32_t slowdown_frames=0;}
namespace {
LONG WINAPI diagnose_fault(EXCEPTION_POINTERS* exception){std::cerr<<"Screen oracle exception "<<std::hex<<exception->ExceptionRecord->ExceptionCode<<" EIP "<<exception->ContextRecord->Eip<<" VA "<<exception->ContextRecord->Eip-mapped_image_base+0x400000<<'\n';const auto* stack=reinterpret_cast<std::uint32_t*>(exception->ContextRecord->Esp);for(unsigned i=0;i<10;++i)std::cerr<<"stack "<<i<<' '<<std::hex<<stack[i]<<'\n';return EXCEPTION_CONTINUE_SEARCH;}
template<class R,class... A>R cpu(std::uint32_t va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
using ScreenRecord=std::vector<std::uint32_t>;std::vector<ScreenRecord> calls;
HRESULT WINAPI texture_state(void*,DWORD stage,D3DTEXTURESTAGESTATETYPE type,DWORD value){calls.push_back({67,stage,std::uint32_t(type),value});return S_OK;}
HRESULT WINAPI render_state(void*,D3DRENDERSTATETYPE type,DWORD value){calls.push_back({57,std::uint32_t(type),value});return S_OK;}
HRESULT WINAPI vertex_format(void*,DWORD value){calls.push_back({89,value});return S_OK;}
HRESULT WINAPI primitive(void*,D3DPRIMITIVETYPE type,UINT count,const void* vertices,UINT stride){
    ScreenRecord call{83,std::uint32_t(type),count,stride};const auto vertex_count=type==D3DPT_TRIANGLELIST?count*3u:count+2u;
    const auto* first=static_cast<const std::uint32_t*>(vertices);call.insert(call.end(),first,first+vertex_count*stride/4);calls.push_back(std::move(call));return S_OK;
}
HRESULT WINAPI transform(void*,D3DTRANSFORMSTATETYPE type,const D3DMATRIX* matrix){ScreenRecord call{44,std::uint32_t(type)};const auto* words=reinterpret_cast<const std::uint32_t*>(matrix);call.insert(call.end(),words,words+16);calls.push_back(std::move(call));return S_OK;}
HRESULT WINAPI viewport(void*,const D3DVIEWPORT9* viewport){ScreenRecord call{47};const auto* words=reinterpret_cast<const std::uint32_t*>(viewport);call.insert(call.end(),words,words+6);calls.push_back(std::move(call));return S_OK;}
void* device_table[119]{};struct TestDevice{void** table=device_table;} device;
ScreenRecord controller_state(){const auto& c=*pe::sprite_controller;ScreenRecord result{c.quad_count,c.draw_calls,std::uint32_t(c.textured_write),std::uint32_t(c.textured_batch_start)};result.insert(result.end(),c.fields_c8,c.fields_c8+5);const auto* first=reinterpret_cast<const std::uint32_t*>(&c.cached_texture);result.insert(result.end(),first,first+5);return result;}
std::vector<ScreenRecord> scheduler_state(th20::source::scheduler::State& state,void* subject){
    std::vector<ScreenRecord> records;for(auto* list:{&state.update,&state.draw})for(auto* link=list->sentinel.next;link;link=link->next){const auto& node=*link->value;records.push_back({list==&state.draw,std::uint32_t(node.priority),node.flags,node.callback?1u:0u,node.before_insert?1u:0u,node.on_shutdown?1u:0u,node.userdata==subject});}return records;
}
std::array<std::uint32_t,17> effect_state(const se::Effect& effect){std::array<std::uint32_t,17> value;std::memcpy(value.data(),&effect,sizeof(effect));value[0]=0;value[2]=effect.update_node?1u:0u;value[3]=effect.draw_node?1u:0u;return value;}
}
int wmain(int argc,wchar_t** argv){try{
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);AddVectoredExceptionHandler(1,diagnose_fault);
    if(argc!=3)throw std::runtime_error("Usage: screen_effect_cpu_compare ORIGINAL.exe REPORT.json");
    const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original hash mismatch");const auto pe_image=th20::parse_pe(bytes);Mapping image(bytes,pe_image);mapped_image_base=image.address();
    for(const auto& item:pe_image.imports)if(item.name=="GetCurrentThreadId")*reinterpret_cast<std::uintptr_t*>(mapped_image_base+item.iat_rva)=reinterpret_cast<std::uintptr_t>(&GetCurrentThreadId);
    auto* lock=reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c0240+10*0x30);std::memset(lock,0,0x30);lock[0]=0x101;lock[10]=GetCurrentThreadId();lock[11]=1;
    std::mt19937 random(0x4240c0);std::map<std::string,unsigned> groups;unsigned checks=0,failed=0;std::vector<std::string> failures;
    const std::pair<std::uint32_t,int(*)(se::Effect&)> functions[]{{0x4240c0,&se::update_fade_out},{0x424260,&se::update_fade_in},{0x424180,&se::update_hold},{0x424350,&se::update_flashes},{0x424460,&se::update_solid},{0x4249f0,&se::update_linear_shake},{0x4244c0,&se::update_envelope_shake}};
    auto check=[&](const std::string& label,const void* a,const void* b,std::size_t size){++checks;++groups[label];if(std::memcmp(a,b,size)){++failed;if(failures.size()<30){std::size_t offset=0;while(offset<size&&static_cast<const std::uint8_t*>(a)[offset]==static_cast<const std::uint8_t*>(b)[offset])++offset;std::ostringstream msg;msg<<label<<" +0x"<<std::hex<<offset;failures.push_back(msg.str());std::cerr<<msg.str()<<'\n';}}};
    for(const auto& [va,function]:functions)for(unsigned test=0;test<4096;++test){
        alignas(se::Effect) std::uint8_t storage[sizeof(se::Effect)];auto& effect=*reinterpret_cast<se::Effect*>(storage);
        for(auto& byte:storage)byte=static_cast<std::uint8_t>(random());effect.duration=test%4?int(random()%100)-3:std::bit_cast<int>(random());
        effect.argument_20=test%3?random()%100:random();effect.argument_24=test%3?random()%100:random();effect.argument_28=test%3?random()%100:random();effect.phase=test%3;effect.view_index=test%3;
        effect.timer={-1,int(random()%200)-3,float(int(random()%200)-3)+float(test%4)*.25f,test%8};
        std::array<std::uint8_t,sizeof(effect)> before,expected;std::memcpy(before.data(),&effect,sizeof(effect));
        for(auto* p=reinterpret_cast<std::uint8_t*>(&pe::graphics_state);p!=reinterpret_cast<std::uint8_t*>(&pe::graphics_state)+sizeof(pe::graphics_state);++p)*p=static_cast<std::uint8_t>(random());
        std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1c4d40),&pe::graphics_state,sizeof(pe::graphics_state));
        const auto graphics_before=pe::graphics_state;
        pe::window_state.scale=float(test%7)*.5f;*reinterpret_cast<float*>(mapped_image_base+0x1b8818)=pe::window_state.scale;
        gp::slowdown_frames=test%11==0;*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1ba518)=gp::slowdown_frames;
        alignas(gp::GameController) std::uint8_t game[sizeof(gp::GameController)]{};
        gp::controller=test%5?reinterpret_cast<gp::GameController*>(game):nullptr;if(gp::controller)gp::controller->game_flags=test%3?0:random()%128;
        *reinterpret_cast<void**>(mapped_image_base+0x1ba828)=gp::controller;
        float clock_rate=float(test%5)*.5f;st::timer_rate=test%6?&clock_rate:nullptr;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=st::timer_rate;
        st::Random rng(1);rng.state=random();rng.last=random();rng.modulus=0x7fffffff;
        std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1ba4c4),&rng,sizeof(rng));st::random_streams[1]=rng;
        using F=int(__cdecl*)(void*);FloatingEnvironment::prepare();const auto expected_return=reinterpret_cast<F>(mapped_image_base+va-0x400000)(&effect);
        std::memcpy(expected.data(),&effect,sizeof(effect));pe::GraphicsStatePrefix graphics_expected;std::memcpy(&graphics_expected,reinterpret_cast<void*>(mapped_image_base+0x1c4d40),sizeof(graphics_expected));
        st::Random random_expected(1);std::memcpy(&random_expected,reinterpret_cast<void*>(mapped_image_base+0x1ba4c4),sizeof(random_expected));
        std::memcpy(&effect,before.data(),sizeof(effect));pe::graphics_state=graphics_before;FloatingEnvironment::prepare();const auto actual_return=function(effect);
        const auto label=std::to_string(va);check(label+" state44",expected.data(),&effect,sizeof(effect));check(label+" return",&expected_return,&actual_return,4);
        check(label+" graphics de8",&graphics_expected,&pe::graphics_state,sizeof(graphics_expected));check(label+" RNG28",&random_expected,&st::random_streams[1],sizeof(random_expected));
    }
    std::cerr<<"Screen update comparisons complete; initializing lifecycle imports\n";
    for(const auto& item:pe_image.imports)if(item.name=="AcquireSRWLockExclusive"||item.name=="ReleaseSRWLockExclusive"||item.name=="HeapAlloc"||item.name=="HeapFree"||item.name=="GetLastError")*reinterpret_cast<std::uintptr_t*>(mapped_image_base+item.iat_rva)=reinterpret_cast<std::uintptr_t>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"),item.name.c_str()));
    *reinterpret_cast<HANDLE*>(mapped_image_base+0x1e5990)=GetProcessHeap();cpu<void>(0x452e00,reinterpret_cast<void*>(mapped_image_base+0x1c0240));
    for(int mode=0;mode<10;++mode)for(unsigned test=0;test<64;++test){
        if(!test)std::cerr<<"Screen lifecycle mode"<<mode<<'\n';
        alignas(se::Effect) std::uint8_t storage[sizeof(se::Effect)];auto* effect=reinterpret_cast<se::Effect*>(storage);
        const auto pattern=static_cast<unsigned char>(test%2?0xa5:0);std::memset(storage,pattern,sizeof(storage));
        cpu<void>(0x4233b0,effect);const auto constructor_expected=effect_state(*effect);
        std::memset(storage,pattern,sizeof(storage));new(storage)se::Effect;const auto constructor_actual=effect_state(*effect);check("constructor44 except C++ vtable",constructor_expected.data(),constructor_actual.data(),sizeof(constructor_expected));
        th20::source::scheduler::State native_scheduler{},source_scheduler{};th20::source::scheduler::initialize_state(native_scheduler);th20::source::scheduler::initialize_state(source_scheduler);
        pe::function_controller=&source_scheduler;*reinterpret_cast<void**>(mapped_image_base+0x1b66d8)=&native_scheduler;
        const auto duration=std::bit_cast<int>(random());const auto a=random(),b=random(),c=random();const int priority=int(random()%160)-80;
        std::memset(storage,pattern,sizeof(storage));cpu<void>(0x4233b0,effect);cpu<void>(0x423bf0,effect,mode,duration,a,b,c,priority);
        const auto initialized_expected=effect_state(*effect);const auto callbacks_expected=scheduler_state(native_scheduler,effect);
        cpu<void>(0x4234a0,effect);const auto destroyed_expected=effect_state(*effect);
        if(native_scheduler.update.sentinel.next||native_scheduler.draw.sentinel.next)throw std::runtime_error("Original Screen destructor left callback nodes");
        std::memset(storage,pattern,sizeof(storage));new(storage)se::Effect;effect->initialize(mode,duration,a,b,c,priority);
        const auto initialized_actual=effect_state(*effect);const auto callbacks_actual=scheduler_state(source_scheduler,effect);
        check("initialize44 except two node identities",initialized_expected.data(),initialized_actual.data(),sizeof(initialized_expected));const bool same=callbacks_expected==callbacks_actual,yes=true;check("scheduler callback metadata and insertion",&same,&yes,sizeof(bool));
        effect->~Effect();const auto destroyed_actual=effect_state(*effect);check("destroy44 except vtable and node identities",destroyed_expected.data(),destroyed_actual.data(),sizeof(destroyed_expected));
        const bool empty=!source_scheduler.update.sentinel.next&&!source_scheduler.draw.sentinel.next;check("destructor unlinks both callbacks",&empty,&yes,sizeof(bool));
    }
    std::cerr<<"Screen lifecycle comparisons complete; testing drawing\n";
    const auto d3dx=LoadLibraryW(L"d3dx9_43.dll");if(!d3dx)throw std::runtime_error("D3DX9 SDK is unavailable");
    for(const auto& item:pe_image.imports)if(item.name=="D3DXMatrixLookAtLH"||item.name=="D3DXMatrixPerspectiveFovLH")*reinterpret_cast<std::uintptr_t*>(mapped_image_base+item.iat_rva)=reinterpret_cast<std::uintptr_t>(GetProcAddress(d3dx,item.name.c_str()));
    device_table[57]=reinterpret_cast<void*>(&render_state);device_table[67]=reinterpret_cast<void*>(&texture_state);device_table[89]=reinterpret_cast<void*>(&vertex_format);device_table[83]=reinterpret_cast<void*>(&primitive);device_table[44]=reinterpret_cast<void*>(&transform);device_table[47]=reinterpret_cast<void*>(&viewport);
    auto* controller_storage=VirtualAlloc(nullptr,sizeof(th20::source::sprite::Controller),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);if(!controller_storage)throw std::bad_alloc();pe::sprite_controller=static_cast<th20::source::sprite::Controller*>(controller_storage);
    *reinterpret_cast<void**>(mapped_image_base+0x1c0028)=pe::sprite_controller;
    const std::pair<std::uint32_t,int(*)(se::Effect&)> draw_functions[]{{0x424e00,se::draw_display},{0x424e80,se::draw_playfield},{0x424f30,se::draw_current_view},{0x424fa0,se::draw_offset_playfield}};
    for(unsigned kind=0;kind<5;++kind)for(unsigned test=0;test<320;++test){if(test<2)std::cerr<<"Screen draw kind"<<kind<<" case"<<test<<" mapped"<<std::hex<<mapped_image_base<<" device"<<&device<<std::dec<<'\n';
        std::memset(&pe::graphics_state,0,sizeof(pe::graphics_state));pe::graphics_state.device=reinterpret_cast<IDirect3DDevice9*>(&device);
        for(auto& camera:pe::graphics_state.viewports){camera.viewport={DWORD(test%13),DWORD(test%17),640+test,480+test,0,1};camera.view._11=camera.view._22=camera.view._33=camera.view._44=1;camera.projection=camera.view;camera.field_of_view=float(test%9+1)/6.f;camera.offset_x=test%53;camera.offset_y=-int(test%47);for(auto& point:camera.points)for(float& value:point)value=float(int(random()%1000)-500)/37.f;}
        std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1c4d40),&pe::graphics_state,sizeof(pe::graphics_state));
        pe::window_state.scaled_width=640+test;pe::window_state.scaled_height=480+test;pe::window_state.offset_x=test;pe::window_state.offset_y=-int(test);pe::window_state.playfield_width=384+test;pe::window_state.playfield_height=448+test;
        std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1b6758),&pe::window_state,sizeof(pe::window_state));
        auto& c=*pe::sprite_controller;for(auto& value:c.fields_c8)value=random();for(auto* byte=reinterpret_cast<std::uint8_t*>(&c.cached_texture);byte!=reinterpret_cast<std::uint8_t*>(&c.cached_texture)+20;++byte)*byte=static_cast<std::uint8_t>(random());
        c.fields_c8[2]=std::bit_cast<std::uint32_t>(float(int(test)-160)/13.f);c.fields_c8[3]=std::bit_cast<std::uint32_t>(float(int(test)-160)/17.f);c.quad_count=test%2;c.draw_calls=test;c.textured_batch_start=c.textured_vertices;c.textured_write=c.textured_vertices+c.quad_count*6;
        for(unsigned i=0;i<6;++i)c.textured_vertices[i]={float(i),float(i*2),0,1,random(),float(i)/6.f,float(i)/3.f};
        const auto before=controller_state();const auto graphics_before=pe::graphics_state;
        alignas(se::Effect) std::uint8_t effect_storage[sizeof(se::Effect)]{};auto& effect=*reinterpret_cast<se::Effect*>(effect_storage);effect.alpha=std::bit_cast<int>(random());effect.argument_20=random();effect.argument_24=random();
        float bounds[4];for(float& value:bounds)value=float(int(random()%10000)-5000)/13.f;const auto color=random();
        calls.clear();FloatingEnvironment::prepare();int expected_return=0;if(kind==0){using F=void(__cdecl*)(const float*,std::uint32_t);reinterpret_cast<F>(mapped_image_base+0x23650)(bounds,color);}else{using F=int(__cdecl*)(void*);expected_return=reinterpret_cast<F>(mapped_image_base+draw_functions[kind-1].first-0x400000)(&effect);}
        const auto expected_calls=calls;const auto expected_controller=controller_state();pe::GraphicsStatePrefix graphics_expected;std::memcpy(&graphics_expected,reinterpret_cast<void*>(mapped_image_base+0x1c4d40),sizeof(graphics_expected));if(kind==1)graphics_expected.current_viewport=&pe::graphics_state.viewports[2];
        c.quad_count=before[0];c.draw_calls=before[1];c.textured_write=reinterpret_cast<th20::source::sprite::Vertex28*>(before[2]);c.textured_batch_start=reinterpret_cast<th20::source::sprite::Vertex28*>(before[3]);std::copy(before.begin()+4,before.begin()+9,c.fields_c8);std::memcpy(&c.cached_texture,before.data()+9,20);pe::graphics_state=graphics_before;
        calls.clear();FloatingEnvironment::prepare();int actual_return=0;if(kind==0)se::draw_rectangle(c,*pe::graphics_state.device,bounds,color);else actual_return=draw_functions[kind-1].second(effect);
        const auto actual_controller=controller_state();const bool same=expected_calls==calls,yes=true;const auto label="drawing"+std::to_string(kind);
        check(label+" COM order arguments and all vertex bytes",&same,&yes,sizeof(bool));check(label+" full touched Controller regions",expected_controller.data(),actual_controller.data(),expected_controller.size()*4);check(label+" entire graphics de8",&graphics_expected,&pe::graphics_state,sizeof(graphics_expected));check(label+" return",&expected_return,&actual_return,4);
    }
    VirtualFree(controller_storage,0,MEM_RELEASE);pe::sprite_controller=nullptr;
    std::ofstream report(argv[2]);report<<"{\n\"status\":\""<<(failed?"failed":"passed")<<"\",\n\"checks\":"<<checks<<",\n\"failed\":"<<failed<<",\n\"source_hashes\":{";
    bool comma=false;for(const auto& pair:std::initializer_list<std::pair<const char*,const char*>>{{"update.cpp",TH20_SCREEN_UPDATE_SHA},{"environment.cpp",TH20_SCREEN_ENV_SHA},{"effect.hpp",TH20_SCREEN_HPP_SHA},{"native_core.hpp",TH20_SCREEN_NATIVE_SHA},{"runtime_state.cpp",TH20_SCREEN_RNG_SHA},{"lifecycle.cpp",TH20_SCREEN_LIFECYCLE_SHA},{"draw.cpp",TH20_SCREEN_DRAW_SHA}}){if(comma)report<<',';comma=true;report<<th20::json_string(pair.first)<<':'<<th20::json_string(pair.second);}
    report<<"},\n\"scope\":\"4096 cases for each of seven original update callbacks, whole44-byte owner state, return codes, entire de8-byte graphics state and RNG28 including draw-direction call counts. Production environment.cpp used directly, three viewport indexes and fractional/null timer rates;640 constructor/initialize/destructor cases across10 modes with node/vtable identity normalization, scheduler metadata/unlink checks;320 cases each of rectangle core and4 draw callbacks compare COM order,all vertex bytes,touched controller state,entire graphics and returns\",\n\"limitations\":[\"Original executable entry point is never called; DrawPrimitiveUP is a COM recording boundary, not a GPU pixel comparison;allocation-failure factory paths and full-game scene transitions are outside this oracle\"],\n\"failure_examples\":[";for(unsigned i=0;i<failures.size();++i){if(i)report<<',';report<<th20::json_string(failures[i]);}report<<"]\n}\n";
    std::cout<<"Screen effect CPU: "<<checks<<" checks, "<<failed<<" failures\n";return failed?1:0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}}
