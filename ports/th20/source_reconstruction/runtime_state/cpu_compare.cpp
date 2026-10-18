// Original instructions are isolated test evidence; no production target maps an EXE.
#define wmain unused_native_cpu_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "state.hpp"
#include <float.h>
namespace s=th20::source::state;
namespace {
template<class R,class... A> R original(std::uint32_t va,void* self,A... args) {
    using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);
}
void prepare(unsigned mode) {FloatingEnvironment::prepare();unsigned x87,sse;__control87_2(mode,_MCW_RC,&x87,&sse);}
std::uint32_t bits(float value){std::uint32_t result;std::memcpy(&result,&value,4);return result;}
}
int wmain(int argc,wchar_t** argv) {
    try {
        if(argc!=3)throw std::runtime_error("Usage: state_cpu_compare TH20.exe REPORT.json");
        const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA256 mismatch");
        const auto pe=th20::parse_pe(bytes);Mapping image(bytes,pe);mapped_image_base=image.address();FloatingEnvironment restore;
        for(const auto& item:pe.imports)if(item.name=="GetCurrentThreadId")*reinterpret_cast<std::uintptr_t*>(mapped_image_base+item.iat_rva)=reinterpret_cast<std::uintptr_t>(&GetCurrentThreadId);
        auto* lock=reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c0240+10*0x30);
        std::memset(lock,0,0x30);lock[0]=0x101;lock[10]=GetCurrentThreadId();lock[11]=1;
        std::mt19937 samples(0x4d9940);std::map<std::string,unsigned> counts;unsigned failed=0;
        std::vector<std::string> failures;
        auto check=[&](const char* name,bool ok){++counts[name];if(!ok){++failed;if(failures.size()<20)failures.emplace_back(name);}};
        for(unsigned i=0;i<2000;++i) {
            const auto id=samples();s::Random raw(id);std::memset(&raw,0xa5,sizeof(raw));s::Random cpp(id);
            auto* result=original<s::Random*>(0x422c50,&raw,id);
            check("constructor",result==&raw&&std::memcmp(&raw,&cpp,sizeof(raw))==0);
            const auto value=i<5?std::array<std::uint32_t,5>{0,1,0x7fffffff,0xfffffffe,0xffffffff}[i]:samples();
            original<void>(0x4d9940,&raw,value);s::seed(cpp,value);
            check("seed",std::memcmp(&raw,&cpp,sizeof(raw))==0);
        }
        for(const auto mode:{0u,0x100u,0x200u,0x300u})for(unsigned i=0;i<10000;++i) {
            s::Random start(samples());start.state=samples();start.last=samples();start.minimum=samples();start.upper=samples();
            start.modulus=i<4?std::array<std::uint32_t,4>{1,2,0x7fffffff,0xffffffff}[i]:(samples()|1);
            auto raw=start,cpp=start;prepare(mode);const auto a=original<std::uint32_t>(0x423ee0,&raw);prepare(mode);const auto b=s::next(cpp);
            check("next",a==b&&std::memcmp(&raw,&cpp,sizeof(raw))==0);
            raw=cpp=start;const auto count=i%7==0?0:samples();prepare(mode);const auto ab=original<std::uint32_t>(0x423ea0,&raw,count);prepare(mode);const auto bb=s::bounded(cpp,count);
            check("bounded",ab==bb&&std::memcmp(&raw,&cpp,sizeof(raw))==0);
            raw=cpp=start;prepare(mode);const auto af=original<float>(0x429830,&raw);prepare(mode);const auto bf=s::unit(cpp);
            check("unit",bits(af)==bits(bf)&&std::memcmp(&raw,&cpp,sizeof(raw))==0);
            raw=cpp=start;prepare(mode);const auto as=original<float>(0x4298e0,&raw);prepare(mode);const auto bs=s::signed_unit(cpp);
            check("signed_unit",bits(as)==bits(bs)&&std::memcmp(&raw,&cpp,sizeof(raw))==0);
        }
        unsigned total=0;for(auto& entry:counts)total+=entry.second;
        std::ofstream out(argv[2]);out<<"{\n  \"status\":\""<<(failed?"failed":"passed")<<"\",\n  \"total\":"<<total<<",\n  \"failed\":"<<failed<<",\n  \"state_cpp_sha256\":\""<<TH20_STATE_CPP_SHA<<"\",\n  \"state_hpp_sha256\":\""<<TH20_STATE_HPP_SHA<<"\",\n  \"original_sha256\":\""<<expected_sha<<"\",\n  \"comparisons\":{";
        bool comma=false;for(auto& [name,count]:counts){if(comma)out<<',';comma=true;out<<'"'<<name<<"\":"<<count;}
        out<<"},\n  \"scope\":\"Whole 28-byte RNG constructor/seed/state and return values for integer, bounded and both float wrappers under all four rounding modes\",\n  \"limitations\":[\"Original single-thread recursive lock path exercised; cross-thread ordering not compared\",\"Zero modulus original CPU fault is replaced by explicit domain_error\"],\n  \"failure_examples\":[";
        comma=false;for(auto& error:failures){if(comma)out<<',';comma=true;out<<'"'<<error<<'"';}out<<"]\n}\n";
        if(!out)throw std::runtime_error("Could not write state report");std::cout<<"Runtime state CPU: "<<total<<" checks, "<<failed<<" failures\n";return failed?1:0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
