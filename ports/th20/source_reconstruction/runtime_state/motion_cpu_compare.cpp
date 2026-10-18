#define wmain unused_motion_native_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "motion.hpp"
#include <bit>
namespace st=th20::source::state;
namespace {template<class R,class... A>R cpu(std::uint32_t va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}}
int wmain(int argc,wchar_t** argv){try{
    if(argc!=3)throw std::runtime_error("Usage: motion_cpu_compare ORIGINAL.exe REPORT.json");
    const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original hash mismatch");const auto pe=th20::parse_pe(bytes);Mapping image(bytes,pe);mapped_image_base=image.address();
    int capabilities[4];__cpuid(capabilities,1);if(!(capabilities[2]&(1<<19)))throw std::runtime_error("Motion oracle needs SSE4.1 floor path");
    // Original CRT feature detection normally selects this available CPU path.
    *reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1e5288)=2;
    std::mt19937 random(0x453e40);unsigned checks=0,failed=0;std::vector<std::string> failures;FloatingEnvironment restore;
    auto check=[&](const char* label,const void* expected,const void* actual,std::size_t size){++checks;if(std::memcmp(expected,actual,size)){++failed;if(failures.size()<20){std::size_t offset=0;while(offset<size&&static_cast<const std::uint8_t*>(expected)[offset]==static_cast<const std::uint8_t*>(actual)[offset])++offset;std::ostringstream message;message<<label<<" +0x"<<std::hex<<offset;failures.push_back(message.str());std::cerr<<message.str()<<'\n';}}};
    for(unsigned test=0;test<8192;++test){
        st::Motion initial;auto* words=reinterpret_cast<float*>(&initial);for(unsigned i=0;i<17;++i)words[i]=static_cast<float>(static_cast<int>(random()%40001)-20000)/(test<4096?997.f:19.f);
        initial.field_34=static_cast<float>(static_cast<int>(test%11)-2)*.25f;initial.field_44=(random()&~63u)|(test%64);
        const float clock_scale=static_cast<float>(test%9)*.25f;*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=clock_scale;
        constexpr std::uint32_t addresses[]{0x453e40,0x453ac0,0x47a1f0,0x4543d0};
        for(unsigned kind=0;kind<4;++kind){
            auto expected=initial,actual=initial;FloatingEnvironment::prepare();cpu<void>(addresses[kind],&expected);FloatingEnvironment::prepare();
            if(kind==0)st::update_motion_velocity(actual,clock_scale);else if(kind==1)st::update_motion_position(actual,clock_scale);else if(kind==2)st::update_motion(actual,clock_scale);else st::snap_motion_position(actual);
            const std::string label="motion mode"+std::to_string(initial.field_44&63u)+" function"+std::to_string(kind);check(label.c_str(),&expected,&actual,sizeof(actual));
        }
        if(test<1024){auto expected=initial;cpu<void>(0x478530,&expected);const st::Motion actual{};check("complete shared Motion constructor",&expected,&actual,sizeof(actual));}
        // Bound tests include unordered operands and exact equality, independently
        // of finite trajectories used for the movement comparisons above.
        auto bounds_subject=initial;float bounds[4];for(auto& value:bounds)value=std::bit_cast<float>(random());
        if(test%2){bounds[0]=initial.position.x;bounds[1]=initial.position.y;bounds[2]=bounds[3]=0;}
        if(test%7==0)bounds_subject.position.x=std::bit_cast<float>(0x7fc01234u);
        const auto expected=cpu<int>(0x47a400,&bounds_subject,bounds[0],bounds[1],bounds[2],bounds[3]);const int actual=st::outside_motion_bounds(bounds_subject,bounds[0],bounds[1],bounds[2],bounds[3]);check("strict bounds comparisons including NaN",&expected,&actual,4);
    }
    std::ofstream report(argv[2]);report<<"{\n\"status\":\""<<(failed?"failed":"passed")<<"\",\n\"checks\":"<<checks<<",\n\"failed\":"<<failed<<",\n\"motion_cpp_sha256\":\""<<TH20_MOTION_CPP_SHA<<"\",\n\"motion_hpp_sha256\":\""<<TH20_MOTION_HPP_SHA<<"\",\n\"scope\":\"Complete72-byte original453e40 velocity,453ac0 position,47a1f0 combined,4543d0 floor100 on8192 finite randomized states each; all16 mode nibbles and flags4/5;8192 strict bounds cases including NaNs;1024 constructor cases\",\n\"limitations\":[\"Trajectory tests use finite inputs and round-to-nearest; original CRT floor uses verified host SSE4.1 path\"],\n\"failure_examples\":[";
    for(std::size_t i=0;i<failures.size();++i){if(i)report<<',';report<<th20::json_string(failures[i]);}report<<"]\n}\n";if(!report)throw std::runtime_error("Report write failed");std::cout<<"Motion: "<<checks<<" checks, "<<failed<<" failures\n";return failed?1:0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}}
