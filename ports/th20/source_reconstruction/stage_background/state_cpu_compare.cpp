#define wmain unused_background_native_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "background.hpp"
#include "fog.hpp"
namespace b=th20::source::background;
namespace {
template<class R,class... A>R cpu(std::uint32_t va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
}
int wmain(int argc,wchar_t** argv){try{
    if(argc!=3)throw std::runtime_error("Usage: background_state_cpu_compare ORIGINAL.exe OUTPUT.json");
    const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original hash mismatch");const auto pe=th20::parse_pe(bytes);Mapping image(bytes,pe);mapped_image_base=image.address();
    std::mt19937 random(0x4715c0);unsigned checks=0,failed=0;std::vector<std::string> failures;
    auto check=[&](const char* label,bool same){++checks;if(!same){++failed;if(failures.size()<16)failures.emplace_back(label);}};
    for(unsigned test=0;test<1024;++test){
        b::ScriptState state;std::array<std::uint8_t,sizeof(state)> before,expected;
        for(auto& value:before)value=static_cast<std::uint8_t>(random());std::memcpy(&state,before.data(),sizeof(state));
        auto* result=cpu<b::ScriptState*>(0x4715c0,&state);std::memcpy(expected.data(),&state,sizeof(state));std::memcpy(&state,before.data(),sizeof(state));
        b::construct_script_state(state);const bool same=std::memcmp(expected.data(),&state,sizeof(state))==0;
        if(!same&&test==0)for(std::size_t i=0;i<sizeof(state);++i)if(expected[i]!=reinterpret_cast<std::uint8_t*>(&state)[i]){std::cerr<<"first state difference +"<<std::hex<<i<<'\n';break;}
        check("full 0x3310 script state, matrices, eight animation self-links and padding",same&&result==&state);
        auto camera=state.camera;auto original=camera;for(auto* value=reinterpret_cast<std::uint8_t*>(&original);value!=reinterpret_cast<std::uint8_t*>(&original)+sizeof(original);++value)*value=static_cast<std::uint8_t>(random());camera=original;
        result=reinterpret_cast<b::ScriptState*>(cpu<void*>(0x471790,&original));b::construct_camera(camera);check("camera constructor preserves matrices",result==reinterpret_cast<b::ScriptState*>(&original)&&std::memcmp(&camera,&original,sizeof(camera))==0);
    }
    auto random_fog=[&](){b::FogState value;for(auto* byte=reinterpret_cast<std::uint8_t*>(&value);byte!=reinterpret_cast<std::uint8_t*>(&value)+sizeof(value);++byte)*byte=static_cast<std::uint8_t>(random());return value;};
    FloatingEnvironment restore;
    for(unsigned test=0;test<12000;++test){
        auto x=random_fog(),y=x,z=random_fog();b::FogState expected,actual;float factor;const auto factor_bits=random();std::memcpy(&factor,&factor_bits,4);
        FloatingEnvironment::prepare();cpu<void>(0x473400,&x);FloatingEnvironment::prepare();b::pack_fog_color(y);check("fog CVTT packed channels all bit patterns",std::memcmp(&x,&y,sizeof(x))==0);
        FloatingEnvironment::prepare();auto* result=cpu<void*>(0x471db0,&x,&expected,factor);FloatingEnvironment::prepare();actual=b::scale_fog(x,factor);check("fog scale all bytes and return",result==&expected&&std::memcmp(&expected,&actual,sizeof(actual))==0);
        FloatingEnvironment::prepare();result=cpu<void*>(0x471e60,&x,&expected,&z);FloatingEnvironment::prepare();actual=b::subtract_fog(x,z);check("fog subtraction all bytes and return",result==&expected&&std::memcmp(&expected,&actual,sizeof(actual))==0);
        FloatingEnvironment::prepare();result=cpu<void*>(0x471f50,&x,&expected,&z);FloatingEnvironment::prepare();actual=b::add_fog(x,z);check("fog addition all bytes and return",result==&expected&&std::memcmp(&expected,&actual,sizeof(actual))==0);
        th20::source::sprite::Interpolation<b::FogState> a{},c{};
        a.start=random_fog();a.end=random_fog();a.tangent_start=random_fog();a.tangent_end=random_fog();a.current=random_fog();
        a.mode=test%32;a.duration=static_cast<int>(test%101)-2;a.timer={-7,static_cast<int>(test%90),float(test%90)+.25f,test%8};c=a;
        float rate=float(test%5)*.5f;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=test%6?&rate:nullptr;
        FloatingEnvironment::prepare();result=cpu<void*>(0x473590,&a,&expected);FloatingEnvironment::prepare();actual=b::sample_fog(c,test%6?&rate:nullptr);
        const bool same=result==&expected&&std::memcmp(&a,&c,sizeof(a))==0&&std::memcmp(&expected,&actual,sizeof(actual))==0;
        if(!same&&failures.size()<16){std::cerr<<"Fog sample mode"<<a.mode<<" test"<<test<<'\n';}
        check("fog interpolation whole164 bytes and returned color",same);
    }
    std::ofstream report(argv[2]);report<<"{\n\"status\":\""<<(failed?"failed":"passed")<<"\",\n\"checks\":"<<checks<<",\n\"failed\":"<<failed<<",\n\"state_cpp_sha256\":\""<<TH20_BACKGROUND_STATE_SHA<<"\",\n\"background_hpp_sha256\":\""<<TH20_BACKGROUND_HPP_SHA<<"\",\n\"fog_cpp_sha256\":\""<<TH20_FOG_CPP_SHA<<"\",\n\"scope\":\"Unmodified original4715c0 ScriptState and471790 camera constructors,1024 randomized full-storage inputs each, exact return and bytes;12000 cases each of fog color pack,scale,subtract,add and full164-byte interpolation across32 modes\",\n\"limitations\":[\"Background factory, archive loader, STD VM and rendering are not exercised here\"],\n\"failure_examples\":[";
    for(std::size_t i=0;i<failures.size();++i){if(i)report<<',';report<<th20::json_string(failures[i]);}report<<"]\n}\n";if(!report)throw std::runtime_error("Report write failed");std::cout<<"Background state: "<<checks<<" checks, "<<failed<<" failures\n";return failed?1:0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}}
