// Hardware oracle for explicitly selected isolated routines. The original PE's
// entry point is never called. Two D3DX matrix imports and four GDI/User32
// imports are resolved for isolated checks; no game runtime is initialized.
// This executable is linked at 0x10000000, leaving 0x400000 for the verified PE.
#define NOMINMAX
#include <Windows.h>
#include <bcrypt.h>
#include <intrin.h>
#include "platform_window.hpp"
#include "fonts.hpp"
#include "../runtime_core/worker.hpp"
#include <future>
#include "th20/binary.hpp"
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <sstream>

namespace p = th20::source::program_entry;
namespace w = th20::source::platform_window;
namespace th20::source::program_entry {
WindowStatePrefix window_state;
GraphicsStatePrefix graphics_state;
SpriteController* sprite_controller;
}
// Globals above are test fixtures only, not application construction.
static_assert(sizeof(void*) == 4, "Original code must execute in a 32-bit process");
namespace {
const char* expected_sha = "a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897";
std::uint32_t mapped_image_base = 0x400000;

std::string sha256(const th20::Bytes& bytes) {
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0)
        throw std::runtime_error("Cannot open SHA-256 provider");
    ULONG size = 0, written = 0;
    if (BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&size), sizeof(size), &written, 0) < 0) {
        BCryptCloseAlgorithmProvider(algorithm, 0); throw std::runtime_error("Cannot query SHA-256 object");
    }
    th20::Bytes object(size), result(32);
    const auto created = BCryptCreateHash(algorithm, &hash, object.data(), size, nullptr, 0, 0);
    const auto updated = created < 0 ? created : BCryptHashData(hash, const_cast<PUCHAR>(bytes.data()), static_cast<ULONG>(bytes.size()), 0);
    const auto finished = updated < 0 ? updated : BCryptFinishHash(hash, result.data(), static_cast<ULONG>(result.size()), 0);
    if (hash) BCryptDestroyHash(hash);
    BCryptCloseAlgorithmProvider(algorithm, 0);
    if (finished < 0) throw std::runtime_error("Cannot compute SHA-256");
    return th20::bytes_hex(result);
}

struct Mapping {
    void* base = nullptr;
    std::size_t relocation_count = 0;
    std::uint32_t address() const { return reinterpret_cast<std::uint32_t>(base); }
    ~Mapping() { if (base) VirtualFree(base, 0, MEM_RELEASE); }
    Mapping(const th20::Bytes& bytes, const th20::PeImage& pe) {
        if (pe.image_base != 0x400000 || pe.machine != 0x14c || pe.pe32_plus)
            throw std::runtime_error("Expected verified i386 PE at 0x400000");
        base = VirtualAlloc(reinterpret_cast<void*>(0x400000), pe.image_size,
            MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (!base) base = VirtualAlloc(nullptr, pe.image_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (!base) throw std::runtime_error("Cannot allocate original PE test image");
        auto fail = [&](const char* message) {
            VirtualFree(base, 0, MEM_RELEASE); base = nullptr;
            throw std::runtime_error(message);
        };
        if (pe.headers_size > bytes.size() || pe.headers_size > pe.image_size)
            fail("Invalid original headers mapping size");
        std::memcpy(base, bytes.data(), pe.headers_size);
        for (const auto& section : pe.sections) {
            if (std::uint64_t(section.virtual_address) + section.raw_size > pe.image_size)
                fail("Section exceeds mapped image");
            std::memcpy(static_cast<unsigned char*>(base) + section.virtual_address,
                bytes.data() + section.raw_offset, section.raw_size);
        }
        if (address() != 0x400000) {
            const th20::Reader reader(bytes);
            const auto directory = std::size_t(pe.pe_offset) + 24 + 96 + 5 * 8;
            const auto reloc_rva = reader.u32(directory), reloc_size = reader.u32(directory + 4);
            if (!reloc_rva || !reloc_size) fail("Preferred base unavailable and PE has no relocation table");
            auto at = pe.rva_to_file(reloc_rva, reloc_size);
            const auto end = at + reloc_size;
            const auto delta = address() - 0x400000u;
            while (at < end) {
                if (end - at < 8) fail("Truncated relocation block");
                const auto page = reader.u32(at), block_size = reader.u32(at + 4);
                if (block_size < 8 || block_size > end - at || block_size % 2) fail("Invalid relocation block size");
                for (std::size_t entry = at + 8; entry < at + block_size; entry += 2) {
                    const auto word = reader.u16(entry);
                    const auto type = word >> 12;
                    if (!type) continue;
                    if (type != IMAGE_REL_BASED_HIGHLOW) fail("Unsupported original PE relocation type");
                    const auto target = std::uint64_t(page) + (word & 0xfff);
                    if (target + 4 > pe.image_size) fail("Relocation target exceeds image");
                    auto* slot = static_cast<unsigned char*>(base) + static_cast<std::size_t>(target);
                    std::uint32_t value;
                    std::memcpy(&value, slot, 4); value += delta; std::memcpy(slot, &value, 4);
                    ++relocation_count;
                }
                at += block_size;
            }
        }
        DWORD previous = 0;
        if (!VirtualProtect(base, pe.image_size, PAGE_EXECUTE_READWRITE, &previous))
            fail("Cannot enable mapped selected machine code");
        FlushInstructionCache(GetCurrentProcess(), base, pe.image_size);
    }
    Mapping(const Mapping&) = delete;
    Mapping& operator=(const Mapping&) = delete;
};

struct FloatingEnvironment {
    unsigned int mxcsr;
    unsigned short x87_control;
    FloatingEnvironment() : mxcsr(_mm_getcsr()) {
        unsigned short saved;
        __asm fnstcw saved
        x87_control = saved;
    }
    ~FloatingEnvironment() {
        const auto saved = x87_control;
        __asm fldcw saved
        _mm_setcsr(mxcsr);
    }
    static void prepare() {
        // Empty x87 stack and masked exceptions, round-to-nearest, extended
        // precision. Status flags are intentionally outside the comparison.
        __asm fninit
        _mm_setcsr(0x1f80);
    }
};
// Test-only COM endpoint: captures calls made by both original selected
// routines and source. It does not stand in for a device in production code.
struct DeviceTrace {
    void** vtable;
    std::vector<std::uint32_t> calls;
};
HRESULT __stdcall record_transform(DeviceTrace* self,D3DTRANSFORMSTATETYPE state,const D3DMATRIX* matrix) {
    self->calls.push_back(44);self->calls.push_back(state);
    std::uint32_t values[16];std::memcpy(values,matrix,sizeof(values));
    self->calls.insert(self->calls.end(),values,values+16);return D3D_OK;
}
HRESULT __stdcall record_viewport(DeviceTrace* self,const D3DVIEWPORT9* viewport) {
    self->calls.push_back(47);std::uint32_t values[6];std::memcpy(values,viewport,sizeof(values));
    self->calls.insert(self->calls.end(),values,values+6);return D3D_OK;
}
}

int wmain(int argc,wchar_t** argv) {
    try {
        if(argc!=3) throw std::runtime_error("usage: th20_window_cpu_compare original.exe report.json");
        const auto bytes=th20::read_file(argv[1]);
        if(sha256(bytes)!=expected_sha) throw std::runtime_error("Specimen hash mismatch");
        Mapping mapping(bytes,th20::parse_pe(bytes));
        auto* original_window=reinterpret_cast<p::WindowStatePrefix*>(mapping.address()+0x1b6758);
        auto* original_graphics=reinterpret_cast<p::GraphicsStatePrefix*>(mapping.address()+0x1c4d40);
        using OriginalLayout=void(__thiscall*)(p::WindowStatePrefix*,int);
        auto original_layout=reinterpret_cast<OriginalLayout>(mapping.address()+0x1e050);
        using OriginalQuery=std::uint32_t(__thiscall*)(w::InputPrefix*,std::uint32_t);
        auto original_pressed=reinterpret_cast<OriginalQuery>(mapping.address()+0x19c00);
        auto original_repeat=reinterpret_cast<OriginalQuery>(mapping.address()+0x1a280);
        FloatingEnvironment restore;
        std::mt19937 random(0x41e050);
        unsigned passed=0,failed=0,layouts=0,queries=0,worker_constructors=0,worker_behaviors=0,viewports=0,font_cases=0,viewport_initialization=0,viewport_selection=0;
        auto check=[&](bool good,const char* kind) {
            if(good) ++passed;
            else {++failed;if(failed<=8) std::cerr<<"Mismatch in "<<kind<<" after "<<passed<<" passes\n";}
        };
        const int dimensions[][2]={{320,240},{640,480},{959,719},{960,720},{1024,768},
            {1280,960},{1920,1080},{1920,1440},{2560,1920},{3840,2160},{1080,1920},{768,1024}};
        for(int mode=-1;mode<=10;++mode) for(auto& dims:dimensions) for(int choose=0;choose<2;++choose)
        for(float scale:{1.0f,1.5f,2.0f}) for(unsigned rounding=0;rounding<4;++rounding) for(bool separate:{false,true}) {
            // Initialized randomized storage detects unexpected writes in every byte.
            p::WindowStatePrefix initial,other;
            for(auto* begin=reinterpret_cast<unsigned char*>(&initial);begin!=reinterpret_cast<unsigned char*>(&initial)+sizeof(initial);++begin)
                *begin=static_cast<unsigned char>(random());
            for(auto* begin=reinterpret_cast<unsigned char*>(&other);begin!=reinterpret_cast<unsigned char*>(&other)+sizeof(other);++begin)
                *begin=static_cast<unsigned char>(random());
            initial.display_mode=mode;initial.display_width=dims[0];initial.display_height=dims[1];
            initial.scale=scale;initial.viewport_height=static_cast<int>(random()%3000);
            p::graphics_state={};p::graphics_state.configuration.scale_choice=static_cast<std::uint8_t>(random());
            *original_graphics=p::graphics_state;*original_window=initial;p::window_state=initial;
            p::WindowStatePrefix native_other=other,original_other=other;
            FloatingEnvironment::prepare();_mm_setcsr(0x1f80|(rounding<<13));
            original_layout(separate?&original_other:original_window,choose);
            FloatingEnvironment::prepare();_mm_setcsr(0x1f80|(rounding<<13));
            w::calculate_layout(separate?native_other:p::window_state,choose);
            check(std::memcmp(original_window,&p::window_state,sizeof(initial))==0 &&
                std::memcmp(original_graphics,&p::graphics_state,sizeof(p::graphics_state))==0 &&
                std::memcmp(&original_other,&native_other,sizeof(other))==0,"layout");
            ++layouts;
        }
        for(unsigned i=0;i<10000;++i) {
            w::InputPrefix input{random(),random(),random(),random(),random()};
            const auto mask=i<32?1u<<i:random();
            check(original_pressed(&input,mask)==w::pressed(input,mask),"pressed");
            check(original_repeat(&input,mask)==static_cast<unsigned>(w::repeated_or_pressed(input,mask)),"repeat");
            queries+=2;
        }
        using Worker=th20::source::runtime::Worker;
        using WorkerCtor=void*(__thiscall*)(void*);
        auto original_worker_ctor=reinterpret_cast<WorkerCtor>(mapping.address()+0xb780);
        for(unsigned i=0;i<256;++i) {
            alignas(Worker) unsigned char original[sizeof(Worker)],recovered[sizeof(Worker)];
            for(auto& byte:original) byte=static_cast<unsigned char>(random());
            std::memcpy(recovered,original,sizeof(original));
            void* original_result=original_worker_ctor(original);
            auto* native=::new(static_cast<void*>(recovered)) Worker;
            check(original_result==original && std::memcmp(original,recovered,sizeof(original))==0,"worker constructor");
            native->~Worker();++worker_constructors;
        }
        {
            Worker worker;std::atomic<bool> observed=false;
            worker.thread=std::jthread([&]{while(!worker.close_requested.load()) std::this_thread::yield();observed=true;});
            th20::source::runtime::join_worker(worker);
            check(observed && !worker.thread.joinable(),"worker join behavior");++worker_behaviors;
        }
        {
            Worker worker;std::promise<void> stopped;auto completed=stopped.get_future();
            worker.thread=std::jthread([&]{while(!worker.close_requested.load()) std::this_thread::yield();stopped.set_value();});
            th20::source::runtime::sync_close_worker(worker);
            completed.get();
            check(worker.close_requested && !worker.thread.joinable(),"worker detach behavior");++worker_behaviors;
        }
        {
            std::atomic<bool> observed=false;
            {Worker worker;worker.thread=std::jthread([&]{while(!worker.close_requested.load()) std::this_thread::yield();observed=true;});}
            check(observed,"worker destructor behavior");++worker_behaviors;
        }
        HMODULE d3dx=LoadLibraryW(L"d3dx9_43.dll");
        if(!d3dx) throw std::runtime_error("D3DX9_43 unavailable for viewport comparison");
        auto look=GetProcAddress(d3dx,"D3DXMatrixLookAtLH");
        auto perspective=GetProcAddress(d3dx,"D3DXMatrixPerspectiveFovLH");
        if(!look || !perspective) throw std::runtime_error("Required D3DX matrix exports unavailable");
        *reinterpret_cast<FARPROC*>(mapping.address()+0x16c310)=look;
        *reinterpret_cast<FARPROC*>(mapping.address()+0x16c330)=perspective;
        using OriginalOffsets=void(__thiscall*)(p::GraphicsStatePrefix*,int,int);
        auto original_offsets=reinterpret_cast<OriginalOffsets>(mapping.address()+0xddb20);
        for(unsigned i=0;i<2000;++i) {
            auto& g=p::graphics_state;
            for(auto* begin=reinterpret_cast<unsigned char*>(&g);begin!=reinterpret_cast<unsigned char*>(&g)+sizeof(g);++begin)
                *begin=static_cast<unsigned char>(random());
            g.viewports[5].field_of_view=i%3==0?0.5235988f:i%3==1?0.7853982f:1.0471976f;
            p::window_state={};p::window_state.scale=i%3==0?1.0f:i%3==1?1.5f:2.0f;
            p::window_state.scaled_width=static_cast<int>(640*p::window_state.scale);
            p::window_state.scaled_height=static_cast<int>(480*p::window_state.scale);
            *original_window=p::window_state;*original_graphics=g;
            const int x=static_cast<int>(random()%2000)-500,y=static_cast<int>(random()%1200)-300;
            FloatingEnvironment::prepare();original_offsets(original_graphics,x,y);
            FloatingEnvironment::prepare();w::set_render_offsets(g,x,y);
            const bool equal=std::memcmp(original_graphics,&g,sizeof(g))==0;
            if(!equal && failed<8) {
                auto* a=reinterpret_cast<unsigned char*>(original_graphics);auto* b=reinterpret_cast<unsigned char*>(&g);
                for(unsigned offset=0;offset<sizeof(g);++offset) if(a[offset]!=b[offset]) {std::cerr<<"viewport first differing byte +"<<std::hex<<offset<<std::dec<<'\n';break;}
            }
            check(equal,"viewport offsets and camera");++viewports;
        }
        auto original_initialize_viewports=reinterpret_cast<void(__fastcall*)(p::GraphicsStatePrefix*)>(mapping.address()+0xdaba0);
        for(unsigned i=0;i<1000;++i) {
            auto& g=p::graphics_state;
            for(auto* begin=reinterpret_cast<unsigned char*>(&g);begin!=reinterpret_cast<unsigned char*>(&g)+sizeof(g);++begin)
                *begin=static_cast<unsigned char>(random());
            p::window_state={};p::window_state.scale=i%3==0?1.0f:i%3==1?1.5f:2.0f;
            p::window_state.scaled_width=static_cast<int>(random()%2000)+300;
            p::window_state.scaled_height=static_cast<int>(random()%1200)+200;
            *original_window=p::window_state;*original_graphics=g;
            FloatingEnvironment::prepare();original_initialize_viewports(original_graphics);
            FloatingEnvironment::prepare();w::initialize_render_viewports(g);
            check(std::memcmp(original_graphics,&g,sizeof(g))==0,"six viewport initialization");++viewport_initialization;
        }
        void* device_vtable[119]{};device_vtable[44]=reinterpret_cast<void*>(record_transform);device_vtable[47]=reinterpret_cast<void*>(record_viewport);
        DeviceTrace traced_device{device_vtable,{}};
        auto* controller=static_cast<p::SpriteController*>(VirtualAlloc(nullptr,sizeof(p::SpriteController),MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));
        if(!controller) throw std::runtime_error("Cannot allocate isolated controller fixture");
        p::sprite_controller=controller;*reinterpret_cast<p::SpriteController**>(mapping.address()+0x1c0028)=controller;
        auto original_select_viewport=reinterpret_cast<void(__thiscall*)(p::GraphicsStatePrefix*,int)>(mapping.address()+0x1dce0);
        for(unsigned i=0;i<512;++i) {
            auto& g=p::graphics_state;
            for(auto* begin=reinterpret_cast<unsigned char*>(&g);begin!=reinterpret_cast<unsigned char*>(&g)+sizeof(g);++begin)
                *begin=static_cast<unsigned char>(random());
            const unsigned index=i%6;auto& viewport=g.viewports[index];
            viewport.field_of_view=0.5235988f;viewport.viewport={10,20,640,480,0.0f,1.0f};
            viewport.offset_x=static_cast<int>(random()%2000)-500;viewport.offset_y=static_cast<int>(random()%1200)-300;
            g.device=reinterpret_cast<IDirect3DDevice9*>(&traced_device);*original_graphics=g;
            std::uint32_t initial_fields[5];for(auto& field:initial_fields) field=random();
            std::memcpy(controller->fields_c8,initial_fields,sizeof(initial_fields));traced_device.calls.clear();
            FloatingEnvironment::prepare();original_select_viewport(original_graphics,index);
            const auto original_calls=traced_device.calls;std::uint32_t original_fields[5];
            std::memcpy(original_fields,controller->fields_c8,sizeof(original_fields));
            const bool original_selected=original_graphics->current_viewport==&original_graphics->viewports[index];
            std::memcpy(controller->fields_c8,initial_fields,sizeof(initial_fields));traced_device.calls.clear();
            FloatingEnvironment::prepare();w::select_viewport(g,index);
            original_graphics->current_viewport=&g.viewports[index]; // normalize known self pointer for byte comparison
            check(original_selected && original_calls==traced_device.calls &&
                std::memcmp(original_fields,controller->fields_c8,sizeof(original_fields))==0 &&
                std::memcmp(original_graphics,&g,sizeof(g))==0,"viewport selection and COM trace");++viewport_selection;
        }
        VirtualFree(controller,0,MEM_RELEASE);p::sprite_controller=nullptr;
        FreeLibrary(d3dx);
        // Real GDI is used by both implementations. Compare logical font
        // descriptions, not allocator-specific HFONT values or glyph pixels.
        *reinterpret_cast<decltype(&GetDC)*>(mapping.address()+0x16c2ac)=GetDC;
        *reinterpret_cast<decltype(&ReleaseDC)*>(mapping.address()+0x16c2b0)=ReleaseDC;
        *reinterpret_cast<decltype(&EnumFontFamiliesExW)*>(mapping.address()+0x16c034)=EnumFontFamiliesExW;
        *reinterpret_cast<decltype(&CreateFontW)*>(mapping.address()+0x16c040)=CreateFontW;
        auto original_fonts=reinterpret_cast<HFONT*>(mapping.address()+0x1b66f0);
        auto original_available=reinterpret_cast<std::uint8_t*>(mapping.address()+0x1b66ec);
        auto original_initialize_fonts=reinterpret_cast<void(__cdecl*)()>(mapping.address()+0x16d20);
        for(unsigned mask=0;mask<8;++mask) {
            for(unsigned i=0;i<3;++i) original_available[i]=w::font_available[i]=(mask>>i)&1;
            std::memset(original_fonts,0,sizeof(w::fonts));std::memset(w::fonts,0,sizeof(w::fonts));
            original_initialize_fonts();w::initialize_fonts();
            check(std::memcmp(original_available,w::font_available,3)==0,"font availability");++font_cases;
            for(unsigned i=0;i<22;++i) {
                LOGFONTW a{},b{};
                const int got_a=original_fonts[i]?GetObjectW(original_fonts[i],sizeof(a),&a):0;
                const int got_b=w::fonts[i]?GetObjectW(w::fonts[i],sizeof(b),&b):0;
                // GDI leaves unspecified bytes after lfFaceName's terminator;
                // compare all 28 scalar bytes and the full terminated name.
                check(got_a==got_b && std::memcmp(&a,&b,offsetof(LOGFONTW,lfFaceName))==0 &&
                    std::wcscmp(a.lfFaceName,b.lfFaceName)==0,"GDI logical fonts");++font_cases;
                if(original_fonts[i]) DeleteObject(original_fonts[i]);
                if(w::fonts[i]) DeleteObject(w::fonts[i]);
            }
        }
        std::ofstream out{std::filesystem::path(argv[2])};
        out<<"{\n  \"status\": \""<<(failed?"failed":"passed")<<"\",\n"
            <<"  \"specimen_sha256\": \""<<expected_sha<<"\",\n"
            <<"  \"mapped_image_base\": \""<<th20::hex(mapping.address())<<"\",\n"
            <<"  \"layout_cases\": "<<layouts<<",\n  \"input_query_cases\": "<<queries<<",\n"
            <<"  \"worker_constructor_cpu_cases\": "<<worker_constructors<<",\n  \"worker_source_behavior_cases\": "<<worker_behaviors<<",\n"
            <<"  \"viewport_and_camera_cpu_cases\": "<<viewports<<",\n"
            <<"  \"font_gdi_cpu_cases\": "<<font_cases<<",\n"
            <<"  \"six_viewport_initialization_cpu_cases\": "<<viewport_initialization<<",\n"
            <<"  \"viewport_selection_cpu_com_trace_cases\": "<<viewport_selection<<",\n"
            <<"  \"passed\": "<<passed<<",\n  \"failed\": "<<failed<<",\n"
            <<"  \"scope\": \"0x0041e050 full window/global bytes; 0x00419c00/0x0041a280 EAX; 0x0040b780 ctor/EAX; 0x004ddb20/0x004da1f0 and 0x004daba0 full graphics bytes; 0x0041dce0/0x004da120 self pointer, bytes and COM traces; 0x00416d20 GDI logical-font scalar fields and names\",\n"
            <<"  \"excluded\": \"Window messages, dialog behavior, Direct3D device/presentation calls, rendering output and complete gameplay have not been CPU-validated\"\n}\n";
        std::cout<<passed<<" passed; "<<failed<<" failed\n";
        return failed?1:0;
    } catch(const std::exception& e) {std::cerr<<e.what()<<'\n';return 2;}
}
