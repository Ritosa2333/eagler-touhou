#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../special.hpp"
namespace ss=th20::source::special_state;namespace sc=th20::source::scheduler;namespace gs=th20::source::game_session;
template<class R,class... A>R cpu(unsigned va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
gs::Player player;
gs::Player* __cdecl player_boundary(int index){if(index)throw std::logic_error("Unexpected player");return &player;}
#include "frame_fixture.hpp"
int wmain(int argc,wchar_t** argv){try{
    if(argc!=3)throw std::runtime_error("Usage: th20_special_state_cpu_compare ORIGINAL.exe REPORT.json");
    const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");const auto info=th20::parse_pe(bytes);Mapping image(bytes,info);mapped_image_base=image.address();
    AddVectoredExceptionHandler(1,[](PEXCEPTION_POINTERS p)->LONG{if(p->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION)std::cerr<<"access violation VA="<<std::hex<<p->ContextRecord->Eip-mapped_image_base+0x400000<<" address="<<p->ExceptionRecord->ExceptionInformation[1]<<std::dec<<std::endl;return EXCEPTION_CONTINUE_SEARCH;});
    auto hook=[&](unsigned va,void* target){auto* p=reinterpret_cast<unsigned char*>(mapped_image_base+va-0x400000);p[0]=0xe9;*reinterpret_cast<unsigned*>(p+1)=reinterpret_cast<unsigned>(target)-reinterpret_cast<unsigned>(p+5);FlushInstructionCache(GetCurrentProcess(),p,5);};
    hook(0x464080,reinterpret_cast<void*>(&player_boundary));
    for(const auto& import:info.imports)if(auto address=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),import.name.c_str()))*reinterpret_cast<FARPROC*>(mapped_image_base+import.iat_rva)=address;
    *reinterpret_cast<HANDLE*>(mapped_image_base+0x1e5990)=GetProcessHeap();cpu<void>(0x452e00,reinterpret_cast<void*>(mapped_image_base+0x1c0240));
    // Match runtime_state's isolated single-thread recursive lock fixture.
    auto* random_lock=reinterpret_cast<unsigned*>(mapped_image_base+0x1c0240+10*0x30);std::memset(random_lock,0,0x30);random_lock[0]=0x101;random_lock[10]=GetCurrentThreadId();random_lock[11]=1;
    std::mt19937 random(0x511980);unsigned passed=0,failed=0;std::vector<std::string> failures;
    auto check=[&](const char* label,unsigned test,const void* expected,const void* actual,std::size_t count){if(!std::memcmp(expected,actual,count)){++passed;return;}++failed;if(failures.size()<40){std::ostringstream s;s<<label<<" test="<<test;for(std::size_t i=0;i<count;++i)if(static_cast<const unsigned char*>(expected)[i]!=static_cast<const unsigned char*>(actual)[i]){s<<" +0x"<<std::hex<<i<<" expected="<<unsigned(static_cast<const unsigned char*>(expected)[i])<<" actual="<<unsigned(static_cast<const unsigned char*>(actual)[i]);break;}failures.push_back(s.str());}};
    auto fill=[&](auto& p){auto* at=reinterpret_cast<unsigned char*>(&p);for(unsigned i=0;i<sizeof(p);++i)at[i]=std::uint8_t(random());};
    std::cerr<<"constructors and byte interpolation\n";
    for(unsigned test=0;test<2048;++test){ss::Controller a,b;fill(a);b=a;cpu<void>(0x511980,&a);ss::construct(b);a.entries.tail=b.entries.tail;check("controller_constructor",test,&a,&b,sizeof(a));ss::Entry e,f;fill(e);f=e;cpu<void>(0x5119e0,&e);ss::construct(f);e.link.value=f.link.value;check("entry_constructor",test,&e,&f,sizeof(e));ss::ByteInterpolation x,y;fill(x);y=x;cpu<void>(0x511930,&x);ss::construct(y);check("byte_constructor",test,&x,&y,sizeof(x));}
    for(unsigned test=0;test<8192;++test){
        ss::ByteInterpolation a,b;fill(a);a.mode=int(test%32);a.duration=int(random()%300)-30;r::timer_set(a.timer,int(random()%350)-20);b=a;
        float rate=float(random()%13)*.25f;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=&rate;
        const bool tick=(test/32)%2;FloatingEnvironment::prepare();const auto er=cpu<unsigned char>(tick?0x513710:0x511af0,&a);FloatingEnvironment::prepare();const auto ar=tick?ss::sample(b,&rate):ss::evaluate(b);check("byte_interpolation_object",test,&a,&b,sizeof(a));check("byte_interpolation_return",test,&er,&ar,1);
        fill(a);b=a;const int duration=int(random()%500)-10,mode=int(random()%32);const auto from=std::uint8_t(random()),to=std::uint8_t(random());cpu<void>(0x5141e0,&a,duration,mode,&from,&to);ss::start(b,duration,mode,from,to);check("byte_start",test,&a,&b,sizeof(a));
    }
    std::cerr<<"float interpolation and color selection\n";
    for(unsigned test=0;test<8192;++test){ss::FloatInterpolation a,b;a.start=float(int(random()%200000)-100000)/79.f;a.end=float(int(random()%200000)-100000)/73.f;a.tangent_start=float(int(random()%200000)-100000)/97.f;a.tangent_end=float(int(random()%200000)-100000)/59.f;a.current=float(int(random()));a.mode=int(test%32);a.duration=int(random()%300)-30;r::timer_set(a.timer,int(random()%350)-20);b=a;float rate=float(random()%13)*.25f;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=&rate;
        const bool tick=(test/32)%2;FloatingEnvironment::prepare();const float er=cpu<float>(tick?0x42a110:0x511cf0,&a);FloatingEnvironment::prepare();const float ar=tick?ss::sample(b,&rate):ss::evaluate(b);check("float_interpolation_object",test,&a,&b,sizeof(a));check("float_interpolation_return",test,&er,&ar,4);
    }
    for(unsigned test=0;test<8192;++test){ss::Controller owner;ss::construct(owner);ss::Entry entry;ss::construct(entry);entry.color=int(random()%4);if(test%3)sc::append(owner.entries,entry.link);owner.active=std::uint8_t(test%2);fill(player);for(unsigned i=0;i<4;++i)player.fields_30[(0x64-0x30)/4+i]=unsigned(int(random()%3000)-1000);const auto before=player;const auto er=cpu<unsigned>(0x513f70,&owner);const auto ep=player;player=before;const auto ar=ss::color(owner,player);check("color_return",test,&er,&ar,4);check("color_player_clamps",test,&ep,&player,sizeof(player));}
    #include "frame_cases.inc"
    std::ofstream out(argv[2]);out<<"{\n  \"module\": \"special_state\",\n  \"original_sha256\": "<<th20::json_string(expected_sha)<<",\n  \"passed\": "<<passed<<",\n  \"failed\": "<<failed<<",\n  \"failures\": [";for(std::size_t i=0;i<failures.size();++i){if(i)out<<",";out<<th20::json_string(failures[i]);}out<<"]\n}\n";
    for(const auto& f:failures)std::cerr<<f<<'\n';std::cout<<"passed="<<passed<<" failed="<<failed<<'\n';return failed?1:0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 2;}}
