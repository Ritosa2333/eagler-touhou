// Original executable bytes are mapped only in this isolated oracle target.
#define wmain unused_original_oracle_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "test_host.hpp"
#include <random>

namespace input=th20::source::input;
namespace platform=th20::source::platform;
namespace rt=th20::source::runtime;
namespace scheduler=th20::source::scheduler;
namespace {
RecordedHost host;
BOOL WINAPI recorded_keyboard(PBYTE output) {return host.keyboard(output);}
BOOL WINAPI recorded_set_keyboard(PBYTE output) {return host.set_keyboard(output);}
DWORD WINAPI recorded_xinput(DWORD index,XINPUT_STATE* output) {return host.xinput(index,output);}
MMRESULT WINAPI recorded_joystick(UINT index,JOYINFOEX* output) {return host.joystick(index,output);}
MMRESULT WINAPI recorded_caps(UINT_PTR index,JOYCAPSW* output,UINT size) {
    if(size!=sizeof *output) std::abort();return host.joystick_caps(static_cast<UINT>(index),output);
}
HRESULT WINAPI recorded_poll(IDirectInputDevice8W* device) {return host.poll(device);}
HRESULT WINAPI recorded_acquire(IDirectInputDevice8W* device) {return host.acquire(device);}
HRESULT WINAPI recorded_device_state(IDirectInputDevice8W* device,DWORD size,void* output) {
    if(size!=sizeof(DIJOYSTATE2)) std::abort();return host.device_state(device,static_cast<DIJOYSTATE2*>(output));
}
std::uintptr_t test_vtable[33]{};
struct RecordedDevice {std::uintptr_t* vtable=test_vtable;} recorded_device;
LONG WINAPI diagnostic(EXCEPTION_POINTERS* error) {
    std::fprintf(stderr,"Input oracle exception %08lx at %08lx, original base %08x\n",
      error->ExceptionRecord->ExceptionCode,error->ContextRecord->Eip,mapped_image_base);
    std::fflush(stderr);return EXCEPTION_CONTINUE_SEARCH;
}
template<class R,class... Args> R original(std::uint32_t rva,void* self,Args... args) {
    using Function=R(__thiscall*)(void*,Args...);
    return reinterpret_cast<Function>(mapped_image_base+rva)(self,args...);
}
template<class R,class... Args> R original_cdecl(std::uint32_t rva,Args... args) {
    using Function=R(__cdecl*)(Args...);
    return reinterpret_cast<Function>(mapped_image_base+rva)(args...);
}
void copy_to_original(const input::LegacyState& value) {
    std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1b88b0),value.slots,sizeof value.slots);
    std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1b93b0),value.previous_slots,sizeof value.previous_slots);
    std::memcpy(reinterpret_cast<void*>(mapped_image_base+0x1b9ef0),value.capabilities,sizeof value.capabilities);
}
bool compare_original(const input::LegacyState& value) {
    return std::memcmp(reinterpret_cast<void*>(mapped_image_base+0x1b88b0),value.slots,sizeof value.slots)==0
      && std::memcmp(reinterpret_cast<void*>(mapped_image_base+0x1b93b0),value.previous_slots,sizeof value.previous_slots)==0
      && std::memcmp(reinterpret_cast<void*>(mapped_image_base+0x1b9ef0),value.capabilities,sizeof value.capabilities)==0;
}
}
int wmain(int argc,wchar_t** argv) {
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);SetUnhandledExceptionFilter(diagnostic);
    AddVectoredExceptionHandler(1,diagnostic);
    try {
        if(argc!=3) throw std::runtime_error("Usage: th20_input_cpu_compare VERIFIED_TH20.exe REPORT.json");
        const std::filesystem::path source(argv[1]),report(argv[2]);
        if(std::filesystem::weakly_canonical(source)==std::filesystem::weakly_canonical(report))
            throw std::runtime_error("Report must not replace original EXE");
        const auto bytes=th20::read_file(source);const auto digest=sha256(bytes);
        if(digest!=expected_sha) throw std::runtime_error("Original hash mismatch");
        const auto pe=th20::parse_pe(bytes);Mapping mapped(bytes,pe);mapped_image_base=mapped.address();
        for(const auto& item:pe.imports) {
            std::uintptr_t address=0;
            if(item.name=="GetKeyboardState") address=reinterpret_cast<std::uintptr_t>(recorded_keyboard);
            else if(item.name=="SetKeyboardState") address=reinterpret_cast<std::uintptr_t>(recorded_set_keyboard);
            else if(item.iat_rva==0x16c2f0) address=reinterpret_cast<std::uintptr_t>(recorded_xinput); // XINPUT1_4 ordinal 2
            else if(item.name=="joyGetPosEx") address=reinterpret_cast<std::uintptr_t>(recorded_joystick);
            else if(item.name=="joyGetDevCapsW") address=reinterpret_cast<std::uintptr_t>(recorded_caps);
            else if(item.name=="GetCurrentThreadId" || item.name=="AcquireSRWLockExclusive" || item.name=="ReleaseSRWLockExclusive")
                address=reinterpret_cast<std::uintptr_t>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"),item.name.c_str()));
            if(address) *reinterpret_cast<std::uintptr_t*>(mapped_image_base+item.iat_rva)=address;
        }
        original<void>(0x52e00,reinterpret_cast<void*>(mapped_image_base+0x1c0240));
        test_vtable[25]=reinterpret_cast<std::uintptr_t>(recorded_poll);
        test_vtable[7]=reinterpret_cast<std::uintptr_t>(recorded_acquire);
        test_vtable[9]=reinterpret_cast<std::uintptr_t>(recorded_device_state);
        auto* fake_pad=reinterpret_cast<IDirectInputDevice8W*>(&recorded_device);
        std::map<std::string,unsigned> counts;std::vector<std::string> failures;unsigned failed=0;
        const auto check=[&](const char* name,bool value) {
            ++counts[name];if(!value) {++failed;if(failures.size()<20) failures.push_back(name);}
        };
        std::mt19937 rng(0x20c04228);
        const auto fill=[&](void* pointer,std::size_t size) {
            auto* out=static_cast<unsigned char*>(pointer);
            for(std::size_t i=0;i<size;++i) out[i]=static_cast<unsigned char>(rng());
        };
        check("xinput_table",std::memcmp(reinterpret_cast<void*>(mapped_image_base+0x1ae220),input::xinput_button_masks,sizeof input::xinput_button_masks)==0);
        for(unsigned n=0;n<10000;++n) {
            input::ButtonState a,b;fill(&a,sizeof a);b=a;
            if(n<256) {
                a.current=b.current=0xffffffff;
                for(unsigned i=0;i<32;++i) {
                    a.repeat8_count[i]=b.repeat8_count[i]=n;
                    a.repeat12_count[i]=b.repeat12_count[i]=n;
                }
            }
            if(n==256) {
                a.current=b.current=0xffffffff;
                for(unsigned i=0;i<32;++i) {
                    a.repeat8_count[i]=b.repeat8_count[i]=0xffffffff;
                    a.repeat12_count[i]=b.repeat12_count[i]=0xfffffffe;
                    a.held_frames[i]=b.held_frames[i]=0xffffffff;
                }
            }
            original<void>(0x228b0,&a);input::update_buttons(b);
            check("button_update_all_704_bytes",std::memcmp(&a,&b,sizeof a)==0);
            if(n<100) {
                fill(&a,sizeof a);b=a;original<void>(0x1f9b0,&a);input::initialize(b);
                check("button_constructor",std::memcmp(&a,&b,sizeof a)==0);
                input::Device da,db;fill(&da,sizeof da);db=da;
                original<void>(0x1fcb0,&da);input::initialize(db);
                check("device_constructor",std::memcmp(&da,&db,sizeof da)==0);
                fill(&da,sizeof da);db=da;original<void>(0x21720,&da);input::reset_device_header(db);
                check("device_header_reset_preserves_history",std::memcmp(&da,&db,sizeof da)==0);
                const auto logical=static_cast<int>(rng()),physical=static_cast<int>(rng());
                original<void>(0x21ab0,&da,logical);input::initialize_keyboard(db,logical);
                check("keyboard_device_initialize",std::memcmp(&da,&db,sizeof da)==0);
                original<void>(0x21ad0,&da,physical,logical);input::initialize_xinput(db,physical,logical);
                check("xinput_device_initialize",std::memcmp(&da,&db,sizeof da)==0);
            }
            std::uint32_t output1=rng(),output2=output1,bit=rng(),raw=rng();
            const auto index=static_cast<std::int16_t>(rng());
            const auto result1=original_cdecl<std::uint32_t>(0x204a0,&output1,index,bit,raw);
            const auto result2=input::map_bit_button(output2,index,bit,raw);
            check("bit_mapping_shift_and_return",result1==result2 && output1==output2);
            std::uint8_t buttons[128];fill(buttons,sizeof buttons);
            const auto byteindex=static_cast<std::int16_t>(static_cast<int>(rng()%160)-32);
            const auto byte1=original_cdecl<std::uint32_t>(0x20510,&output1,byteindex,bit,buttons);
            const auto byte2=input::map_byte_button(output2,byteindex,bit,buttons);
            check("byte_mapping_and_return",byte1==byte2 && output1==output2);
        }
        platform::Configuration configuration=platform::default_configuration();
        std::array<std::uint8_t,0x2ef8> original_manager{};
        *reinterpret_cast<std::uintptr_t*>(mapped_image_base+0x1b8898)=reinterpret_cast<std::uintptr_t>(original_manager.data());
        for(unsigned n=0;n<6000;++n) {
            input::Device a,b;fill(&a,sizeof a);
            a.kind=n%4;a.logical_index=(n%7==0)?-1:static_cast<int>(n%12);a.direct_input=fake_pad;a.xinput_index=n%4;b=a;
            for(auto& binding:configuration.bindings) {
                for(unsigned i=0;i<8;++i) {
                    binding.pad[i]=static_cast<std::uint16_t>((n+i)%130==129?0xffff:(n+i)%128);
                    binding.alternate_pad[i]=static_cast<std::uint16_t>((n+i)%12);
                    binding.keyboard[i]=static_cast<std::uint16_t>((n*17+i*31)%256);
                }
            }
            std::memcpy(original_manager.data()+0x2e38,configuration.bindings,sizeof configuration.bindings);
            std::int32_t kind=static_cast<int>(rng());
            std::memcpy(original_manager.data()+0x18,&kind,4);
            const bool active=n%5!=0;*reinterpret_cast<std::uint8_t*>(mapped_image_base+0x1b67c0)=active;
            const std::int16_t dx=static_cast<std::int16_t>(rng()),dy=static_cast<std::int16_t>(rng());
            *reinterpret_cast<std::int16_t*>(mapped_image_base+0x1c4f78)=dx;
            *reinterpret_cast<std::int16_t*>(mapped_image_base+0x1c4f7a)=dy;
            fill(host.keys,sizeof host.keys);fill(host.xbox,sizeof host.xbox);fill(&host.pad,sizeof host.pad);
            host.keyboard_status=n%11?TRUE:FALSE;
            for(auto& status:host.xbox_status) status=n%9?ERROR_SUCCESS:ERROR_DEVICE_NOT_CONNECTED;
            host.poll_status=(n/4)%6?S_OK:E_FAIL;host.state_status=n%11?S_OK:E_FAIL;
            host.inputlost_count=n%407;host.acquire_status=n%3?S_OK:E_FAIL;
            host.reset_trace();original<void>(0x21b00,&a);const auto trace=host.trace;
            host.reset_trace();input::PollContext context{host,active,dx,dy,configuration.bindings,kind};input::poll_device(b,context);
            check("device_poll_all_bytes_and_host_order",std::memcmp(&a,&b,sizeof a)==0 && trace==host.trace
                && std::memcmp(original_manager.data()+0x18,&kind,4)==0);
        }
        // Explicit threshold and one-key cases supplement random snapshots.
        for(auto& binding:configuration.bindings) platform::initialize_bindings(binding);
        std::memcpy(original_manager.data()+0x2e38,configuration.bindings,sizeof configuration.bindings);
        for(unsigned n=0;n<768;++n) {
            input::Device a{},b{};a.kind=n<512?0:2;a.logical_index=0;a.xinput_index=0;b=a;
            std::int32_t kind=7;std::memcpy(original_manager.data()+0x18,&kind,4);
            *reinterpret_cast<std::uint8_t*>(mapped_image_base+0x1b67c0)=1;
            std::memset(host.keys,0,sizeof host.keys);host.keyboard_status=TRUE;
            host.keys[n%256]=n<256?0x80:0x7f;
            host.xbox_status[0]=ERROR_SUCCESS;host.xbox[0]={};
            constexpr SHORT edges[]={-32768,-7849,-7848,-7847,0,7847,7848,7849,32767};
            host.xbox[0].Gamepad.sThumbLX=edges[n%9];host.xbox[0].Gamepad.sThumbLY=edges[(n/9)%9];
            host.xbox[0].Gamepad.bLeftTrigger=static_cast<BYTE>(28+n%4);
            host.xbox[0].Gamepad.bRightTrigger=static_cast<BYTE>(28+(n/4)%4);
            host.xbox[0].Gamepad.wButtons=static_cast<WORD>(1u<<(n%16));
            host.reset_trace();original<void>(0x21b00,&a);const auto trace=host.trace;
            host.reset_trace();input::PollContext context{host,true,600,600,configuration.bindings,kind};input::poll_device(b,context);
            check("single_keys_and_xinput_threshold_edges",std::memcmp(&a,&b,sizeof a)==0 && trace==host.trace
                && std::memcmp(original_manager.data()+0x18,&kind,4)==0);
        }
        for(unsigned n=0;n<2000;++n) {
            input::LegacyState states;fill(&states,sizeof states);copy_to_original(states);
            fill(host.joysticks,sizeof host.joysticks);fill(host.caps,sizeof host.caps);fill(host.xbox,sizeof host.xbox);
            for(unsigned i=0;i<2;++i) {host.joystick_status[i]=(n>>i)&1?JOYERR_NOERROR:JOYERR_UNPLUGGED;host.caps_status[i]=n%3?JOYERR_NOERROR:JOYERR_PARMS;}
            for(unsigned i=0;i<4;++i) host.xbox_status[i]=(n>>i)&1?ERROR_SUCCESS:ERROR_DEVICE_NOT_CONNECTED;
            host.reset_trace();original_cdecl<void>(0x20580);auto trace=host.trace;
            host.reset_trace();input::sample_startup_input(states,host);
            check("startup_legacy_aggregate_and_host_order",compare_original(states) && trace==host.trace);
            auto flags=rng();*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c5888)=flags;
            copy_to_original(states);host.reset_trace();const auto result1=original_cdecl<int>(0x20f80);trace=host.trace;
            host.reset_trace();const auto result2=input::probe_legacy_devices(states,host,flags);
            check("legacy_probe_caps_flags_return_order",result1==result2 && compare_original(states) && trace==host.trace
                && *reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c5888)==flags);
            host.keyboard_status=TRUE;fill(host.keys,sizeof host.keys);host.reset_trace();original_cdecl<void>(0x21040);
            std::array<std::uint8_t,256> cleared;std::memcpy(cleared.data(),host.last_set,256);trace=host.trace;
            host.reset_trace();input::clear_keyboard_high_bits(host);
            check("clear_keyboard_state_and_host_order",std::memcmp(cleared.data(),host.last_set,256)==0 && trace==host.trace);
        }
        {
            input::LegacyState states{};rt::Log log;DIDEVCAPS caps{};std::uint8_t active=1;
            scheduler::State chains;scheduler::initialize_state(chains);
            auto environment=rt::shared_locks().scheduler_environment();
            input::ControllerContext context{nullptr,nullptr,active,configuration,caps,log,states,host,chains,environment};
            input::Controller manager(context);
            original<void>(0x1f8a0,original_manager.data());
            check("controller_constructor_all_recovered_fields",std::memcmp(original_manager.data()+4,
                reinterpret_cast<const std::uint8_t*>(&manager)+4,0x2ef8-4)==0);
            for(unsigned n=0;n<1000;++n) {
                manager.device_count=1+n%9;manager.frame=rng();manager.last_input_kind=static_cast<int>(rng());
                manager.selected[0]=n%manager.device_count;
                for(int i=0;i<manager.device_count;++i) {
                    fill(&manager.devices[i],sizeof(input::Device));auto& d=manager.devices[i];
                    d.kind=i%3;d.logical_index=i;d.direct_input=fake_pad;d.xinput_index=i%4;
                }
                fill(&states,sizeof states);copy_to_original(states);
                std::memcpy(original_manager.data(),&manager,0x2ef8);
                active=n%5!=0;*reinterpret_cast<std::uint8_t*>(mapped_image_base+0x1b67c0)=active;
                configuration.value_70=n%1001;configuration.value_72=(n*7)%1001;
                *reinterpret_cast<std::int16_t*>(mapped_image_base+0x1c4f78)=static_cast<std::int16_t>(configuration.value_70);
                *reinterpret_cast<std::int16_t*>(mapped_image_base+0x1c4f7a)=static_cast<std::int16_t>(configuration.value_72);
                host.keyboard_status=TRUE;host.poll_status=S_OK;host.state_status=S_OK;
                fill(host.keys,sizeof host.keys);fill(host.xbox,sizeof host.xbox);fill(&host.pad,sizeof host.pad);
                host.reset_trace();original<void>(0x1fe80,original_manager.data());auto trace=host.trace;
                host.reset_trace();manager.sample_frame();
                check("manager_frame_aggregate_history_and_host_order",compare_original(states) && trace==host.trace
                    && std::memcmp(original_manager.data()+4,reinterpret_cast<const std::uint8_t*>(&manager)+4,0x2ef8-4)==0);
            }
            original<void>(0x202c0,original_manager.data());manager.shutdown_devices();
            check("shutdown_without_com_preserves_history",std::memcmp(original_manager.data()+4,
                reinterpret_cast<const std::uint8_t*>(&manager)+4,0x2ef8-4)==0);
        }
        unsigned total=0;for(const auto& count:counts) total+=count.second;
        std::ofstream out(report,std::ios::binary|std::ios::trunc);
        out<<"{\n\"status\":\""<<(failed?"failed":"passed")<<"\",\n\"source_sha256\":\""<<digest
          <<"\",\n\"input_hpp_sha256\":\""<<TH20_SHA_input_hpp<<"\",\n\"input_state_cpp_sha256\":\""<<TH20_SHA_input_state_cpp
          <<"\",\n\"input_win32_cpp_sha256\":\""<<TH20_SHA_input_win32_cpp
          <<"\",\n\"callback_owner_hpp_sha256\":\""<<TH20_SHA_callback_owner_hpp<<"\",\n\"total\":"<<total<<",\n\"failed\":"<<failed
          <<",\n\"scope\":\"Full recovered input-state bytes, API observation order, edge/repeat/counter behavior, controller aggregate frames\",\n"
          <<"\"limitations\":[\"OS input supplied by isolated deterministic host; no live user controls\",\"Original instructions unchanged; only OS IAT calls and external COM device are fixtures\",\"DirectInput/WMI discovery and allocation/lifecycle are production implementations but not CPU-compared\",\"Invalid mappings, failed GetKeyboardState in clear operation, concurrent reconfiguration outside equivalence domain\"],\n\"comparisons\":{";
        bool first=true;for(const auto& item:counts) {if(!first) out<<',';first=false;out<<'\n'<<th20::json_string(item.first)<<':'<<item.second;}
        out<<"\n},\n\"failure_examples\":[";first=true;for(const auto& value:failures) {if(!first) out<<',';first=false;out<<th20::json_string(value);}out<<"]\n}\n";
        if(!out) throw std::runtime_error("Cannot finish input report");
        std::cout<<"Input CPU comparisons: "<<total<<"; failed: "<<failed<<'\n';
        for(const auto& item:failures) std::cout<<item<<'\n';return failed?1:0;
    } catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 2;}
}
