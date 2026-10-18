#define wmain unused_background_native_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "vm.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include <bit>
#include <functional>
namespace b=th20::source::background;namespace sp=th20::source::sprite;
namespace {
template<class R,class... A>R cpu(std::uint32_t va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
[[noreturn]]void outside_scope(){throw std::runtime_error("A dependency outside the STD state oracle scope was reached");}
std::uint32_t clear_color=0,rotation_flags=0;
std::vector<std::uint32_t> instruction(int opcode,std::vector<std::uint32_t> args={},int time=0){std::vector<std::uint32_t> code(2+args.size());auto* ins=reinterpret_cast<b::Instruction*>(code.data());ins->time=time;ins->opcode=static_cast<std::int16_t>(opcode);ins->size=static_cast<std::int16_t>(code.size()*4);std::copy(args.begin(),args.end(),code.begin()+2);return code;}
}
// Test-only engine boundaries. Unexercised calls fail instead of returning defaults.
namespace th20::source::sprite::anm_environment {
float& clock_scale(){outside_scope();}const float* timer_rate(){outside_scope();}AnmInstruction* script(Animation&){outside_scope();}
bool gameplay_frozen(){outside_scope();}std::uint32_t random_next(){outside_scope();}std::uint32_t random_bounded(std::uint32_t){outside_scope();}
float random_unit(){outside_scope();}float random_signed_unit(){outside_scope();}float camera_component(std::int32_t){outside_scope();}
void assign_sprite(Animation&,std::int32_t){outside_scope();}void set_layer(Animation&,std::int32_t){outside_scope();}void add_camera_offset(Vec3&){outside_scope();}
void calculate_corners(Animation&,Vec3(&)[4]){outside_scope();}float screen_scale(){outside_scope();}std::int32_t screen_offset(unsigned,unsigned){outside_scope();}
void* allocate_geometry(std::uint32_t){outside_scope();}std::uint32_t spawn_child(Animation&,std::int32_t,std::uint32_t){outside_scope();}
std::uint32_t spawn_detached(Animation&,std::int32_t,std::uint32_t){outside_scope();}Animation& lookup_animation(std::uint32_t){outside_scope();}void spawn_effect(Animation&,std::int32_t){outside_scope();}
}
namespace th20::source::background::vm_environment {
void set_clear_color(std::uint32_t value){clear_color=value;}
void set_primary_rotation_flag(std::uint32_t value){rotation_flags=(rotation_flags&~16u)|((value&1u)<<4);}
void assign_animation(ScriptState&,int,int){outside_scope();}void reset_meshes(ScriptState&){outside_scope();}void interrupt_animations(Background&,std::uint32_t){outside_scope();}
}
int wmain(int argc,wchar_t** argv){try{
    if(argc!=3)throw std::runtime_error("Usage: background_vm_cpu_compare ORIGINAL.exe OUTPUT.json");
    const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original hash mismatch");const auto pe=th20::parse_pe(bytes);Mapping image(bytes,pe);mapped_image_base=image.address();
    std::mt19937 random(0x4751b0);unsigned checks=0,failed=0;std::vector<std::string> failures;std::map<std::string,unsigned> groups;
    auto finite=[&](){return float(static_cast<int>(random()%200000)-100000)/127.f;};
    auto bits=[&](){return std::bit_cast<std::uint32_t>(finite());};
    auto prepare=[&](b::ScriptState& state){
        std::memset(&state,0xa5,sizeof(state));b::construct_script_state(state);
        for(auto* value=reinterpret_cast<float*>(&state.camera);value!=reinterpret_cast<float*>(&state.camera)+sizeof(state.camera)/4;++value)*value=finite();
        auto interpolation=[&](auto& p){for(auto* value=reinterpret_cast<float*>(&p);value!=reinterpret_cast<float*>(&p.timer);++value)*value=finite();p.mode=random()%34;p.duration=0;p.timer={-1,int(random()%100),finite(),random()%8};};
        interpolation(state.direction_interpolation);interpolation(state.position_interpolation);interpolation(state.up_interpolation);interpolation(state.fog_interpolation);interpolation(state.fov_interpolation);
        for(auto& animation:state.animations){animation.base.flags[0]=random();animation.base.fields_10_28[6]=random();}
        for(auto& value:state.fields_3294)value=random();
    };
    auto compare=[&](const std::string& label,std::vector<std::uint32_t> code,const std::function<void(b::ScriptState&)>& initialize,unsigned ticks=1){
        alignas(b::Background) std::uint8_t owner_storage[sizeof(b::Background)]{};auto& owner=*reinterpret_cast<b::Background*>(owner_storage);
        b::ScriptState state;prepare(state);state.owner=&owner;owner.instructions=reinterpret_cast<b::Instruction*>(code.data());initialize(state);
        float rate_value=float(random()%5)*.5f;const float* rate=random()%6?&rate_value:nullptr;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=rate;
        *reinterpret_cast<void**>(mapped_image_base+0x1c069c)=&owner;
        std::array<std::uint8_t,sizeof(state)> before,expected;
        for(unsigned tick=0;tick<ticks;++tick){
            std::memcpy(before.data(),&state,sizeof(state));const auto initial_flags=random(),initial_color=random();owner.state_flags=initial_flags;*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c5b20)=initial_color;
            FloatingEnvironment::prepare();cpu<void>(0x4751b0,&state);std::memcpy(expected.data(),&state,sizeof(state));const auto expected_flags=owner.state_flags,expected_color=*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c5b20);
            std::memcpy(&state,before.data(),sizeof(state));rotation_flags=initial_flags;clear_color=initial_color;
            FloatingEnvironment::prepare();b::execute_script(state,rate);
            ++checks;++groups[label];std::string why;
            for(unsigned i=0;i<sizeof(state);++i)if(expected[i]!=reinterpret_cast<std::uint8_t*>(&state)[i]){std::ostringstream error;error<<" +0x"<<std::hex<<i<<" original="<<unsigned(expected[i])<<" source="<<unsigned(reinterpret_cast<std::uint8_t*>(&state)[i]);why=error.str();break;}
            if(rotation_flags!=expected_flags)why+=" rotation global";if(clear_color!=expected_color)why+=" clear color global";
            if(!why.empty()){++failed;if(failures.size()<30){failures.push_back(label+" tick"+std::to_string(tick)+why);std::cerr<<failures.back()<<'\n';}}
        }
    };
    for(unsigned motion=0;motion<16;++motion)for(int time:{-1025,-1,0,1,255,510,511,512,799,800,1023,1024,1999,2000,2047,2048,3071,3072,4799,4800})for(unsigned flags=0;flags<8;++flags){
        compare("camera_motion_"+std::to_string(motion),instruction(0),[&](b::ScriptState& state){state.camera_motion=static_cast<std::uint8_t>(motion);state.motion_timer={time-3,time,float(time)+.25f,flags};state.secondary_motion_timer={time-1,time/2,float(time)/2.f,flags};},3);
    }
    for(unsigned test=0;test<3000;++test)compare("future_instruction_interpolation",instruction(0,{},100000),[&](b::ScriptState& state){
        state.camera_motion=static_cast<std::uint8_t>(test%16);state.motion_timer={-1,int(test%4800),float(test%4800)+.5f,test%8};
        for(auto* p:{&state.direction_interpolation,&state.position_interpolation,&state.up_interpolation})p->duration=static_cast<int>(random()%110)-2;
        state.fog_interpolation.duration=static_cast<int>(random()%110)-2;state.fov_interpolation.duration=static_cast<int>(random()%110)-2;
    },3);
    for(int opcode:{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,18,20,21,22,23,-1})for(unsigned test=0;test<400;++test){
        std::vector<std::uint32_t> args(11);for(auto& v:args)v=bits();
        if(opcode==1){args[0]=52;args[1]=random()%100;}
        if(opcode==3||opcode==5||opcode==9||opcode==10||opcode==11||opcode==18||opcode==21){args[0]=random()%110-2;args[1]=random()%34;}
        if(opcode==12)args[0]=random()%272;if(opcode==14){args[0]=random()%8;args[1]=0u-(test%4+1);args[2]=random();}
        auto code=instruction(opcode,args);const auto tail=instruction(0,{},test%2?100000:0);code.insert(code.end(),tail.begin(),tail.end());
        compare("opcode_"+std::to_string(opcode),code,[](b::ScriptState&){},3);
    }
    std::ofstream report(argv[2]);report<<"{\n\"status\":\""<<(failed?"failed":"passed")<<"\",\n\"checks\":"<<checks<<",\n\"failed\":"<<failed<<",\n\"source_hashes\":{";
    bool comma=false;for(const auto& pair:std::initializer_list<std::pair<const char*,const char*>>{{"vm.cpp",TH20_BG_VM_SHA},{"camera_motion.cpp",TH20_BG_MOTION_SHA},{"fog.cpp",TH20_BG_FOG_SHA},{"background.hpp",TH20_BG_HPP_SHA},{"anm_vm.cpp",TH20_BG_ANM_SHA},{"math.cpp",TH20_BG_MATH_SHA},{"native_core.hpp",TH20_BG_NATIVE_SHA}}){if(comma)report<<',';comma=true;report<<th20::json_string(pair.first)<<':'<<th20::json_string(pair.second);}report<<"},\n\"groups\":{";comma=false;for(auto& [label,count]:groups){if(comma)report<<',';comma=true;report<<th20::json_string(label)<<':'<<count;}
    report<<"},\n\"scope\":\"Unmodified4751b0, whole0x3310 state at identical address, camera modes0..15, timer boundaries and fractional rates, all five interpolation states, opcode0/1..16/18/20..22 and unknown opcodes.13 and22 compare original global state.14 covers negative script variants\",\n\"limitations\":[\"Positive ANM binding,mesh opcode17,animation interrupt19 and complete stage rendering are outside this state oracle\",\"Transcendental camera inputs are finite; original CRT exception delivery is not compared\"],\n\"failure_examples\":[";for(unsigned i=0;i<failures.size();++i){if(i)report<<',';report<<th20::json_string(failures[i]);}report<<"]\n}\n";
    std::cout<<"Background VM: "<<checks<<" checks, "<<failed<<" failures\n";return failed?1:0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}}
