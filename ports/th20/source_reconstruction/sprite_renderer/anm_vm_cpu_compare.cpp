// Isolated original-CPU oracle. None of this PE mapping is linked into the VM.
#define wmain unused_native_cpu_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "anm_vm.hpp"
#include <functional>
namespace s=th20::source::sprite;
namespace {
float scale=1.f;float* rate=&scale;float screen_scale_value=1.f;s::AnmInstruction* current_code=nullptr;
std::uint32_t seed=1,last=0,maximum=0x7fffffff;
template<class R,class... Args>R original(std::uint32_t va,void* object,Args...args){using F=R(__thiscall*)(void*,Args...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(object,args...);}
[[noreturn]]void unexpected(const char* dependency){throw std::runtime_error(std::string("Unexpected test dependency: ")+dependency);}
float bits_float(std::uint32_t bits){float value;std::memcpy(&value,&bits,4);return value;}
std::vector<std::uint32_t> instruction(std::int16_t op,const std::vector<std::uint32_t>& args,unsigned mask=0,int time=0){
    std::vector<std::uint32_t> code(2+args.size());auto* ins=reinterpret_cast<s::AnmInstruction*>(code.data());ins->opcode=op;ins->size=static_cast<std::uint16_t>(code.size()*4);ins->time=static_cast<std::int16_t>(time);ins->mask=static_cast<std::uint16_t>(mask);std::copy(args.begin(),args.end(),code.begin()+2);return code;
}
}
namespace th20::source::sprite::anm_environment {
float& clock_scale(){return scale;}const float* timer_rate(){return rate;}AnmInstruction* script(Animation&){return current_code;}
bool gameplay_frozen(){return false;}
std::uint32_t random_next(){last=th20::recovered::lcg_next(seed);return last%maximum;}
std::uint32_t random_bounded(std::uint32_t n){return random_next()%n;}
float random_unit(){return static_cast<float>(static_cast<double>(random_next()))/(static_cast<float>(static_cast<double>(maximum))-1.f);}
float random_signed_unit(){
    const float numerator=static_cast<float>(static_cast<double>(random_next()));const float limit=static_cast<float>(static_cast<double>(maximum));
    const auto denominator=_mm_sub_ss(_mm_div_ss(_mm_set_ss(limit),_mm_set_ss(2.f)),_mm_set_ss(1.f));
    return _mm_cvtss_f32(_mm_sub_ss(_mm_div_ss(_mm_set_ss(numerator),denominator),_mm_set_ss(1.f)));
}
float camera_component(std::int32_t){unexpected("camera_component");}
void assign_sprite(Animation&,std::int32_t){unexpected("assign_sprite");}void set_layer(Animation&,std::int32_t){unexpected("set_layer");}
void add_camera_offset(Vec3&){unexpected("add_camera_offset");}
void calculate_corners(Animation&,Vec3(&)[4]){unexpected("calculate_corners");}
float screen_scale(){return screen_scale_value;}std::int32_t screen_offset(unsigned preset,unsigned axis){return 5+int(preset)*20+int(axis)*10;}
void* allocate_geometry(std::uint32_t){unexpected("allocate_geometry");}
std::uint32_t spawn_child(Animation&,std::int32_t,std::uint32_t){unexpected("spawn_child");}
std::uint32_t spawn_detached(Animation&,std::int32_t,std::uint32_t){unexpected("spawn_detached");}
Animation& lookup_animation(std::uint32_t){unexpected("lookup_animation");}void spawn_effect(Animation&,std::int32_t){unexpected("spawn_effect");}
}
int wmain(int argc,wchar_t** argv){try{
    if(argc!=3)throw std::runtime_error("Usage: th20_anm_vm_cpu_compare ORIGINAL.exe OUTPUT.json");
    auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");
    const auto pe=th20::parse_pe(bytes);Mapping mapped(bytes,pe);mapped_image_base=mapped.address();
    std::vector<std::uint32_t> controller(0x60007d8/4);std::uint32_t file[0x5c/4]{};void* script_array[1]{};
    controller[0x6000730/4]=reinterpret_cast<std::uint32_t>(file);file[0x54/4]=reinterpret_cast<std::uint32_t>(script_array);
    *reinterpret_cast<void**>(mapped_image_base+0x1c0028)=controller.data();
    *reinterpret_cast<void**>(mapped_image_base+0x1ba828)=nullptr;
    *reinterpret_cast<float**>(mapped_image_base+0x1aefe0)=reinterpret_cast<float*>(mapped_image_base+0x1aefe4);
    for(unsigned i=0;i<4;++i)*reinterpret_cast<std::int32_t*>(mapped_image_base+0x1b67b0+i*4)=5+int(i/2)*20+int(i%2)*10;
    for(const auto& item:pe.imports)if(item.name=="GetCurrentThreadId")*reinterpret_cast<std::uintptr_t*>(mapped_image_base+item.iat_rva)=reinterpret_cast<std::uintptr_t>(&GetCurrentThreadId);
    auto* lock=reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c0240+10*0x30);std::memset(lock,0,0x30);lock[0]=0x101;lock[10]=GetCurrentThreadId();lock[11]=1;
    std::mt19937 rng(0x42b5d0);std::map<std::string,unsigned> counts;unsigned failed=0;std::vector<std::string> failures;
    auto check=[&](const std::string& name,std::vector<std::uint32_t> code,const std::function<void(s::Animation&)>& prepare,unsigned ticks=1){
        s::Animation a;s::construct_animation(a);s::reset_animation_state(a);a.base.fields_10_28[6]=0;
        for(auto& v:a.base.fields_444)v=rng();for(unsigned i=4;i<15;++i)a.base.fields_444[i]=float_bits(static_cast<float>(int(rng()%20000)-10000)/37.f);
        a.base.flags[0]=rng();a.base.flags[1]=rng()&0x1ffefe7f;a.base.flags[1]&=~0x2000000u;
        a.base.flags[2]=rng()&~3u;a.base.flags[7]=0;
        for(unsigned i=3;i<7;++i)a.base.flags[i]=rng();
        th20::recovered::timer_set(a.timer_4c8,0);th20::recovered::timer_set(a.timer_4d8,0);prepare(a);
        s::Animation b=a;auto source_code=code;script_array[0]=code.data();current_code=reinterpret_cast<s::AnmInstruction*>(source_code.data());
        const float initial_scale=0.5f+float(rng()%4)*0.5f;scale=initial_scale;*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=initial_scale;
        const auto initial_seed=rng();seed=initial_seed;last=0;auto* random=reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1ba4c4);random[1]=seed;random[4]=maximum;random[5]=last;
        for(unsigned tick=0;tick<ticks;++tick){FloatingEnvironment::prepare();const auto ra=original<std::int32_t>(0x42b5d0,&a);FloatingEnvironment::prepare();const auto rb=s::execute_animation(b);
        ++counts[name];std::ostringstream why;
        if(ra!=rb)why<<" return "<<ra<<'/'<<rb;
        for(unsigned i=0;i<sizeof(a);++i)if(reinterpret_cast<unsigned char*>(&a)[i]!=reinterpret_cast<unsigned char*>(&b)[i]){why<<" byte+"<<std::hex<<i<<' '<<unsigned(reinterpret_cast<unsigned char*>(&a)[i])<<'/'<<unsigned(reinterpret_cast<unsigned char*>(&b)[i]);break;}
        if(code!=source_code)why<<" script mutation";
        if(float_bits(scale)!=float_bits(initial_scale))why<<" clock restore";
        if(random[1]!=seed||random[5]!=last)why<<" RNG state";
        if(!why.str().empty()){++failed;if(failures.size()<30)failures.push_back(name+" tick="+std::to_string(tick)+why.str());}if(ra||rb)break;}
    };
    auto terminal=instruction(2,{});
    auto single=[&](unsigned op,std::vector<std::uint32_t> args,unsigned mask,const std::function<void(s::Animation&)>& prepare=[](s::Animation&) {}){auto code=instruction(static_cast<std::int16_t>(op),args,mask);code.insert(code.end(),terminal.begin(),terminal.end());check("opcode_"+std::to_string(op),code,prepare);};
    for(unsigned op=100;op<=131;++op)for(unsigned k=0;k<160;++k){
        const bool floatop=op%2==1||op>=124;
        const auto dest=floatop?float_bits(10004.f):10000u;
        std::vector<std::uint32_t> args(4);args[0]=dest;
        for(unsigned i=1;i<4;++i)args[i]=floatop?float_bits(float(int(rng()%1000)-500)/503.f):(rng()%1000+1);
        if(op==129)args[0]=float_bits(10004.f);
        if(op==130||op==131){args[0]=float_bits(10004.f);args[1]=float_bits(10005.f);}
        unsigned mask=(k&1)?1:0;if(op==130||op==131)mask=(k&1)?3:0;
        single(op,args,mask);
    }
    for(unsigned op=202;op<=213;++op)for(unsigned k=0;k<200;++k){
        auto code=instruction(static_cast<std::int16_t>(op),{op%2?float_bits(float(int(rng()%30)-15)):rng()%30,op%2?float_bits(float(int(rng()%30)-15)):rng()%30,24,0});
        code.insert(code.end(),terminal.begin(),terminal.end());check("opcode_"+std::to_string(op),code,[](s::Animation&){});
    }
    for(unsigned op:{0u,5u,6u,200u,201u,302u,303u,305u,306u,307u,308u,309u,310u,311u,312u,313u,314u,315u,316u,317u,318u,400u,401u,402u,403u,404u,405u,406u,415u,416u,419u,421u,422u,423u,424u,425u,426u,431u,432u,436u,437u,438u,439u,440u,441u})for(unsigned k=0;k<160;++k){
        std::vector<std::uint32_t> args{rng(),rng(),rng()};
        if(op==6)args[0]=0u-(rng()%1000);
        if(op==200)args={16,0};
        if(op==201)args={10000,20,0};
        if(op>=400&&op<=402||op==415||op==416||op==425||op==426||op==436||op==441)for(auto& v:args)v=float_bits(float(int(rng()%100000)-50000)/17.f);
        single(op,args,op==201?1:0,[&](s::Animation& a){if(op==201)a.base.fields_444[0]=static_cast<std::uint32_t>(int(k%8)-3);});
    }
    for(auto op:{-1,1,2})for(unsigned k=0;k<40;++k){auto code=instruction(static_cast<std::int16_t>(op),{});check("terminal_"+std::to_string(op),code,[](s::Animation&){});}
    for(unsigned op:{407u,408u,409u,410u,411u,412u,413u,414u,417u,420u,427u,428u,429u,430u,433u,434u,435u})for(unsigned k=0;k<200;++k){
        std::vector<std::uint32_t> args(10);for(auto& v:args)v=float_bits(float(int(rng()%100000)-50000)/173.f);args[0]=static_cast<std::uint32_t>(int(rng()%70)-10);args[1]=rng()%34;
        if(op==408||op==413)for(unsigned i=2;i<5;++i)args[i]=rng();if(op==409||op==414)args[2]=rng();if(op==417){args[0]=rng();args[1]=rng()%200;}
        single(op,args,0,[&](s::Animation& a){for(unsigned offset=0x8c;offset<0x378;offset+=4)*reinterpret_cast<std::uint32_t*>(reinterpret_cast<unsigned char*>(&a)+offset)=rng();});
    }
    for(unsigned op=603;op<=632;++op)if(op!=609&&op!=610)for(unsigned k=0;k<100;++k){std::vector<std::uint32_t> args(5);for(auto& v:args)v=float_bits(float(int(rng()%10000)-5000)/101.f);single(op,args,0);}
    for(unsigned scenario=0;scenario<7;++scenario)for(unsigned k=0;k<30;++k){
        auto code=instruction(100,{10000,50},1,4);auto next=instruction(102,{10000,3},1,8);code.insert(code.end(),next.begin(),next.end());next=instruction(1,{},0,12);code.insert(code.end(),next.begin(),next.end());
        if(scenario==1||scenario==2){code=instruction(scenario==1?3:4,{},0,0);code.insert(code.end(),terminal.begin(),terminal.end());}
        check("frame_scenario_"+std::to_string(scenario),code,[&](s::Animation& a){a.base.flags[0]=0;a.base.flags[1]=0;a.base.flags[2]=0;a.base.flags[3]=0;if(scenario==3)a.base.flags[7]=1;if(scenario==4)a.base.fields_10_28[6]=0xffffffffu;if(scenario==5)a.base.flags[1]|=0x2000000;if(scenario==6){a.fields_550[4]=float_bits(float(k%6)/4.f);a.base.flags[1]|=0x100;}},24);
    }
    for(unsigned request:{17u,29u,0x100u})for(unsigned k=0;k<30;++k){
        auto code=instruction(3,{});auto label=instruction(5,{17},0,3);code.insert(code.end(),label.begin(),label.end());auto action=instruction(100,{10000,9},1,3);code.insert(code.end(),action.begin(),action.end());code.insert(code.end(),terminal.begin(),terminal.end());
        label=instruction(5,{0xffffffffu},0,5);code.insert(code.end(),label.begin(),label.end());action=instruction(100,{10000,19},1,5);code.insert(code.end(),action.begin(),action.end());code.insert(code.end(),terminal.begin(),terminal.end());auto end=instruction(-1,{});code.insert(code.end(),end.begin(),end.end());
        check("interrupt_"+std::to_string(request),code,[&](s::Animation& a){a.base.flags[0]=0;a.base.flags[1]=0;a.base.flags[2]=0;a.base.flags[3]=0;a.base.field_438=request;},4);
    }
    auto compare_buffers=[&](const std::string& name,const void* original,const void* source,unsigned size){++counts[name];if(std::memcmp(original,source,size)){++failed;if(failures.size()<30){std::ostringstream why;why<<name;auto* x=static_cast<const std::uint32_t*>(original);auto* y=static_cast<const std::uint32_t*>(source);for(unsigned i=0;i<size/4;++i)if(x[i]!=y[i]){why<<" word+"<<std::hex<<i*4<<' '<<x[i]<<'/'<<y[i];break;}failures.push_back(why.str());}}};
    for(unsigned test=0;test<1000;++test){s::Animation a{};for(unsigned off:{0x38u,0x3cu,0x40u,0x44u,0x48u,0x4cu,0x50u,0x54u,0x60u,0x64u,0x78u,0x7cu,0x3a0u,0x3a4u})*reinterpret_cast<float*>(reinterpret_cast<unsigned char*>(&a)+off)=float(int(rng()%100000)-50000)/217.f;a.base.flags[1]=rng();s::Animation b=a;scale=float(rng()%100)/31.f;*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=scale;FloatingEnvironment::prepare();original<void>(0x435520,&a);FloatingEnvironment::prepare();s::update_animation_motion(b);compare_buffers("motion_435520",&a,&b,sizeof(a));}
    for(unsigned type:{9u,13u,14u,24u,25u,47u,48u})for(unsigned test=0;test<200;++test){
        s::Animation a{},parent{};a.base.flags[0]=type;a.base.flags[1]=rng();a.base.flags[2]=rng();a.base.flags[3]=test%5;
        for(unsigned off:{0x2cu,0x30u,0x34u,0x38u,0x3cu,0x40u,0x50u,0x54u,0x78u,0x7cu,0x378u,0x37cu,0x380u,0x384u,0x388u,0x38cu,0x390u,0x394u,0x454u,0x458u,0x45cu,0x460u,0x484u,0x488u,0x48cu,0x5bcu,0x5c0u,0x5c4u})*reinterpret_cast<float*>(reinterpret_cast<unsigned char*>(&a)+off)=float(int(rng()%10000)-5000)/311.f;
        a.base.fields_444[0]=2+rng()%15;a.base.fields_444[1]=rng()%12;a.base.field_490=rng();a.base.field_494=rng();parent.base.vector_50={.7f,1.3f};if(test%2)a.fields_550[3]=reinterpret_cast<std::uint32_t>(&parent);
        s::Animation b=a;std::vector<std::uint32_t> va(600),vb;for(auto& word:va)word=rng();vb=va;a.fields_550[5]=reinterpret_cast<std::uint32_t>(va.data());b.fields_550[5]=reinterpret_cast<std::uint32_t>(vb.data());screen_scale_value=.5f+float(test%4)*.5f;*reinterpret_cast<float*>(mapped_image_base+0x1b8818)=screen_scale_value;
        FloatingEnvironment::prepare();original<void>(0x435c80,&a);FloatingEnvironment::prepare();s::update_animation_geometry(b);b.fields_550[5]=a.fields_550[5];compare_buffers("geometry_state_"+std::to_string(type),&a,&b,sizeof(a));compare_buffers("geometry_vertices_"+std::to_string(type),va.data(),vb.data(),static_cast<unsigned>(va.size()*4));
    }
    for(unsigned test=0;test<1000;++test){
        s::Animation aa[3]{},bb[3]{};for(unsigned i=0;i<3;++i){auto& a=aa[i];a.base.flags[1]=rng();a.base.flags[2]=rng();a.base.flags[3]=rng()%5;for(unsigned off:{0x2cu,0x30u,0x34u,0x38u,0x3cu,0x40u,0x50u,0x54u,0x484u,0x488u,0x48cu,0x5bcu,0x5c0u,0x5c4u})*reinterpret_cast<float*>(reinterpret_cast<unsigned char*>(&a)+off)=float(int(rng()%100000)-50000)/137.f;bb[i]=a;if(i<2){aa[i].fields_550[2]=reinterpret_cast<std::uint32_t>(&aa[i+1]);bb[i].fields_550[2]=reinterpret_cast<std::uint32_t>(&bb[i+1]);}}
        screen_scale_value=.5f+float(test%4)*.5f;*reinterpret_cast<float*>(mapped_image_base+0x1b8818)=screen_scale_value;
        s::Vec3 pa{};FloatingEnvironment::prepare();original<s::Vec3*>(0x4376e0,&aa[0],&pa);FloatingEnvironment::prepare();const auto pb=s::animation_position(bb[0]);compare_buffers("position_parent_chain",&pa,&pb,sizeof(pa));
        FloatingEnvironment::prepare();const auto* ra=original<s::Vec3*>(0x437770,&aa[0]);FloatingEnvironment::prepare();const auto& rb=s::inherited_animation_rotation(bb[0]);compare_buffers("rotation_parent_chain",ra,&rb,sizeof(rb));
        s::Vec3 xa{1.5f,-3.2f,7.f},xb=xa;FloatingEnvironment::prepare();original<s::Vec3*>(0x437840,&aa[0],&xa,int(test&1),int((test>>1)&1));FloatingEnvironment::prepare();s::transform_animation_offset(bb[0],xb,(test&1)!=0,(test&2)!=0);compare_buffers("offset_parent_chain",&xa,&xb,sizeof(xa));
        for(unsigned i=0;i<3;++i){bb[i].fields_550[2]=aa[i].fields_550[2];compare_buffers("parent_chain_mutations",&aa[i],&bb[i],sizeof(s::Animation));}
    }
    for(unsigned test=0;test<10000;++test){s::Animation a{};for(auto& word:a.base.fields_444)word=rng();for(unsigned off=0x2c;off<0x44;off+=4)*reinterpret_cast<std::uint32_t*>(reinterpret_cast<unsigned char*>(&a)+off)=rng();s::Animation b=a;
        const int variables[]={10000,10001,10002,10003,10004,10005,10006,10007,10008,10009,10013,10014,10015,10023,10024,10025,10026,10027,10028,10029,10033,10034,10035,-1,0,9999,20000};const auto variable=variables[test%(sizeof(variables)/sizeof(variables[0]))];
        FloatingEnvironment::prepare();const auto ia=original<std::int32_t>(0x437d00,&a,variable);FloatingEnvironment::prepare();const auto ib=s::anm_integer_variable(b,variable);compare_buffers("integer_variable_reads",&ia,&ib,4);
        const float argument=float(variable)+(test%3==0?.25f:0.f);FloatingEnvironment::prepare();const auto fa=original<float>(0x437eb0,&a,argument);FloatingEnvironment::prepare();const auto fb=s::anm_float_variable(b,argument);compare_buffers("float_variable_reads",&fa,&fb,4);compare_buffers("variable_read_mutations",&a,&b,sizeof(a));
    }
    for(unsigned kind=0;kind<6;++kind)for(int mode=-1;mode<=32;++mode)for(int test=0;test<30;++test){
        const bool integer=kind==3||kind==4,angle=kind==5;const unsigned count=kind==1?2:(kind==2||kind==4?3:1);const std::uint32_t va=kind==0?0x42a110:(kind==1?0x42a980:(kind==2?0x42ad80:(kind==3?0x429e90:(kind==4?0x42a3a0:0x42b1f0))));
        std::vector<std::uint32_t> a(count*5+6),b;
        for(unsigned i=0;i<count*5;++i)a[i]=integer?rng()%10000:float_bits(float(int(rng()%10000)-5000)/173.f);
        auto& timer=*reinterpret_cast<th20::recovered::Timer*>(a.data()+count*5);th20::recovered::timer_set(timer,test%3?0:-5);a[count*5+4]=static_cast<std::uint32_t>(test%3==0?-3:(test%3==1?0:12));a[count*5+5]=static_cast<std::uint32_t>(mode);b=a;
        for(unsigned tick=0;tick<16;++tick){scale=float((test%5)+1)/2.f;*reinterpret_cast<float*>(mapped_image_base+0x1aefe4)=scale;std::uint32_t out_a[3]{},out_b[3]{};FloatingEnvironment::prepare();
            if(kind==0)out_a[0]=float_bits(original<float>(va,a.data()));else if(kind==3)out_a[0]=static_cast<std::uint32_t>(original<std::int32_t>(va,a.data()));else original<void*>(va,a.data(),out_a);
            FloatingEnvironment::prepare();s::sample_animation_interpolation(b.data(),count,integer,angle,out_b,&scale);
            compare_buffers("interpolation_"+std::to_string(kind)+"_mode_"+std::to_string(mode),a.data(),b.data(),static_cast<unsigned>(a.size()*4));compare_buffers("interpolation_result_"+std::to_string(kind),out_a,out_b,count*4);
        }
    }
    unsigned total=0;for(auto& row:counts)total+=row.second;
    std::ofstream out(argv[2]);out<<"{\n\"status\":"<<th20::json_string(failed?"failed":"passed")<<",\"original_sha256\":"<<th20::json_string(expected_sha)<<",\"total\":"<<total<<",\"failed\":"<<failed<<",\"source_hashes\":{\"anm_vm.cpp\":\""<<TH20_ANM_VM_CPP_SHA<<"\",\"anm_vm.hpp\":\""<<TH20_ANM_VM_HPP_SHA<<"\",\"animation.cpp\":\""<<TH20_ANM_ANIMATION_CPP_SHA<<"\",\"animation.hpp\":\""<<TH20_ANM_ANIMATION_HPP_SHA<<"\",\"../ecl_vm/math.cpp\":\""<<TH20_ANM_MATH_CPP_SHA<<"\",\"../ecl_vm/math.hpp\":\""<<TH20_ANM_MATH_HPP_SHA<<"\",\"../../native_recovered/native_core.hpp\":\""<<TH20_ANM_CORE_SHA<<"\",\"anm_vm_cpu_compare.cpp\":\""<<TH20_ANM_ORACLE_SHA<<"\"},\"comparisons\":{";
    bool first=true;for(auto& row:counts){if(!first)out<<',';first=false;out<<'\n'<<th20::json_string(row.first)<<':'<<row.second;}out<<"},\"failure_examples\":[";first=true;for(auto& f:failures){if(!first)out<<',';first=false;out<<th20::json_string(f);}out<<"]}\n";
    std::cout<<"ANM CPU comparisons "<<total<<" failed "<<failed<<'\n';for(auto& f:failures)std::cout<<f<<'\n';return failed?1:0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}}
