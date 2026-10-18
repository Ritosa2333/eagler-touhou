// Test-only PE mapping. The independent VM library contains no original bytes
// or addresses to execute; only this separately enabled test uses original CPU.
#define wmain unused_native_cpu_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "vm.hpp"
#include <algorithm>
#include <functional>
#include <limits>

namespace e = th20::source::ecl;
namespace {
template<class R,class... A> R original(std::uint32_t va,void* self,A... args) {
    using F=R(__thiscall*)(void*,A...);
    return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);
}
// No engine fallback in the fixture: an unexpected missing dependency fails.
struct NoEngine final:e::Engine {
    std::int32_t read_integer(std::int32_t) override {throw std::runtime_error("Unexpected engine variable");}
    std::uint32_t& integer_destination(std::int32_t) override {throw std::runtime_error("Unexpected engine destination");}
    float read_float(std::int32_t) override {throw std::runtime_error("Unexpected engine variable");}
    std::uint32_t& float_destination(std::int32_t) override {throw std::runtime_error("Unexpected engine destination");}
    std::int32_t execute_entity_opcode(e::Runtime&,e::Instruction) override {throw std::runtime_error("Unexpected entity opcode");}
};
void put32(std::vector<std::uint8_t>& d,std::size_t at,std::uint32_t v){std::memcpy(d.data()+at,&v,4);}
void put16(std::vector<std::uint8_t>& d,std::size_t at,std::uint16_t v){std::memcpy(d.data()+at,&v,2);}
std::vector<std::uint8_t> instruction(unsigned op,std::vector<std::uint32_t> arguments={},unsigned mask=0,unsigned rank=255,int time=0,unsigned drop=0) {
    std::vector<std::uint8_t> bytes(16+arguments.size()*4);
    put32(bytes,0,static_cast<std::uint32_t>(time));put16(bytes,4,static_cast<std::uint16_t>(op));put16(bytes,6,static_cast<std::uint16_t>(bytes.size()));
    put16(bytes,8,static_cast<std::uint16_t>(mask));bytes[10]=static_cast<std::uint8_t>(rank);bytes[11]=static_cast<std::uint8_t>(arguments.size());bytes[12]=static_cast<std::uint8_t>(drop);
    for(std::size_t i=0;i<arguments.size();++i)put32(bytes,16+i*4,arguments[i]);return bytes;
}
void append(std::vector<std::uint8_t>& a,const std::vector<std::uint8_t>& b){a.insert(a.end(),b.begin(),b.end());}
struct Raw {
    std::array<std::uint32_t,0x48/4> runtime{};
    std::array<std::uint32_t,1024> words{};
    std::array<std::uint32_t,0x70/4> manager{};
    std::array<std::uint32_t,0x220/4> resources{};
    std::vector<std::uint32_t> records;
    std::vector<std::vector<std::uint8_t>> code;
    std::vector<e::math::Interpolator> interpolators;
    explicit Raw(e::Runtime& source) {
        runtime[0]=e::float_bits(source.time);runtime[1]=static_cast<std::uint32_t>(source.subroutine);runtime[2]=static_cast<std::uint32_t>(source.instruction_offset);
        for(std::size_t i=0;i<source.stack.words.size();++i)words[i]=source.stack.words[i];
        runtime[4]=reinterpret_cast<std::uint32_t>(words.data());runtime[5]=runtime[4]+sizeof(words);runtime[6]=runtime[5];
        runtime[7]=static_cast<std::uint32_t>(source.stack.pointer);runtime[8]=static_cast<std::uint32_t>(source.stack.frame_base);
        runtime[9]=static_cast<std::uint32_t>(source.async_id);runtime[10]=reinterpret_cast<std::uint32_t>(manager.data());runtime[11]=static_cast<std::uint32_t>(source.async_field_2c);runtime[12]=source.rank_mask;
        runtime[17]=source.async_flags;
        manager[3]=reinterpret_cast<std::uint32_t>(runtime.data());
        manager[0x5c/4]=reinterpret_cast<std::uint32_t>(runtime.data());
        manager[0x58/4]=reinterpret_cast<std::uint32_t>(resources.data());
        for(auto& sub:source.program->subroutines){code.emplace_back(16,0);append(code.back(),sub.bytes);}
        for(std::size_t i=0;i<code.size();++i){records.push_back(reinterpret_cast<std::uint32_t>(source.program->subroutines[i].name.c_str()));records.push_back(reinterpret_cast<std::uint32_t>(code[i].data()));}
        resources[2]=static_cast<std::uint32_t>(code.size());
        resources[0x210/4]=reinterpret_cast<std::uint32_t>(records.data());
        interpolators=source.interpolators;
        if(!interpolators.empty()){
            runtime[14]=reinterpret_cast<std::uint32_t>(interpolators.data());runtime[15]=runtime[14]+static_cast<std::uint32_t>(interpolators.size()*sizeof(e::math::Interpolator));runtime[16]=runtime[15];
        }
        *reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=source.clock_rate;
        if(source.random){auto* random=reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1ba4a8);random[1]=source.random->state;random[4]=source.random->maximum;random[5]=source.random->last;}
    }
    std::int32_t tick(float delta){return original<std::int32_t>(0x53b5c0,runtime.data(),delta);}
};
}
int wmain(int argc,wchar_t** argv) {
    try {
        if(argc!=3)throw std::runtime_error("Usage: th20_ecl_cpu_compare VERIFIED_TH20.exe OUTPUT.json");
        const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA256 mismatch");
        const auto pe=th20::parse_pe(bytes);Mapping image(bytes,pe);mapped_image_base=image.address();
        // Only the RNG's nested single-thread lock path needs an OS import.
        for(const auto& item:pe.imports)if(item.name=="GetCurrentThreadId")
            *reinterpret_cast<std::uintptr_t*>(mapped_image_base+item.iat_rva)=reinterpret_cast<std::uintptr_t>(&GetCurrentThreadId);
        auto* rng_lock=reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c0240+10*0x30);
        std::memset(rng_lock,0,0x30);rng_lock[0]=0x101;rng_lock[10]=GetCurrentThreadId();rng_lock[11]=1;
        std::map<std::string,unsigned> counts;std::vector<std::string> failures;unsigned failed=0;
        NoEngine engine;std::mt19937 rng(0x53b5c0);
        auto check=[&](const std::string& name,e::Program& p,const std::function<void(e::Runtime&)>& prepare,float delta=1.f){
            e::Scheduler scheduler(p,engine);auto& r=scheduler.main;r.subroutine=0;r.instruction_offset=0;r.stack.words.resize(1024);prepare(r);
            Raw raw(r);FloatingEnvironment::prepare();const auto original_result=raw.tick(delta);FloatingEnvironment::prepare();const auto cpp_result=r.tick(delta);
            ++counts[name];std::ostringstream why;
            if(original_result!=cpp_result)why<<"return "<<original_result<<'/'<<cpp_result<<' ';
            if(raw.runtime[0]!=e::float_bits(r.time))why<<"time "<<std::hex<<raw.runtime[0]<<'/'<<e::float_bits(r.time)<<' ';
            if(raw.runtime[1]!=static_cast<std::uint32_t>(r.subroutine)||raw.runtime[2]!=static_cast<std::uint32_t>(r.instruction_offset))why<<"location ";
            if(raw.runtime[7]!=static_cast<std::uint32_t>(r.stack.pointer)||raw.runtime[8]!=static_cast<std::uint32_t>(r.stack.frame_base))why<<"stack pointers "<<raw.runtime[7]<<'/'<<r.stack.pointer<<' '<<raw.runtime[8]<<'/'<<r.stack.frame_base<<' ';
            if(raw.runtime[11]!=static_cast<std::uint32_t>(r.async_field_2c)||raw.runtime[17]!=r.async_flags)why<<" async fields";
            for(unsigned i=0;i<raw.words.size();++i)if(raw.words[i]!=r.stack.words[i]){why<<"word "<<i<<"="<<std::hex<<raw.words[i]<<'/'<<r.stack.words[i];break;}
            for(std::size_t i=0;i<raw.code.size();++i)if(!std::equal(p.subroutines[i].bytes.begin(),p.subroutines[i].bytes.end(),raw.code[i].begin()+16)){why<<" instruction mutation";break;}
            if((raw.runtime[15]-raw.runtime[14])/56!=r.interpolators.size())why<<" interpolation count";
            else if(!r.interpolators.empty()&&std::memcmp(raw.interpolators.data(),r.interpolators.data(),r.interpolators.size()*56))why<<" interpolation state";
            if(r.random){const auto* random=reinterpret_cast<const std::uint32_t*>(mapped_image_base+0x1ba4a8);if(random[1]!=r.random->state||random[5]!=r.random->last)why<<" RNG state";}
            if(!why.str().empty()){++failed;if(failures.size()<40)failures.push_back(name+" case "+std::to_string(counts[name])+": "+why.str());}
        };
        auto test=[&](unsigned op,const std::vector<std::uint32_t>& args,unsigned mask,const std::function<void(e::Runtime&)>& prepare,unsigned drop=0,unsigned rank=255){
            auto code=instruction(op,args,mask,rank,0,drop);append(code,instruction(0,{},0,255,1000000));
            e::Program program;program.subroutines.push_back({"test",code});check("opcode_"+std::to_string(op),program,prepare);
        };
        for(unsigned op=50;op<=77;++op) for(unsigned i=0;i<250;++i) {
            const bool floating=(op<59?(op%2==1):op<=70?(op%2==0):op==72);
            auto a=rng(),b=rng();if((op==56||op==58)&&(!b||(a==0x80000000&&b==0xffffffff)))b=1;
            // Use all raw float bit patterns; NaNs and signed zero are preserved.
            test(op,{},0,[&](e::Runtime& r){r.stack.push(a,floating?'f':'i');if(op!=71&&op!=72)r.stack.push(b,floating?'f':'i');},i%9);
        }
        for(unsigned i=0;i<300;++i){
            const auto a=rng();const auto f=static_cast<float>(static_cast<int>(rng()%100000)-50000)/32.f;
            test(42,{a},0,[](e::Runtime&){});test(44,{e::float_bits(f)},0,[](e::Runtime&){});
            test(42,{0},1,[&](e::Runtime& r){r.stack.frame_base=20;r.stack.absolute(20)=a;});
            test(44,{0},1,[&](e::Runtime& r){r.stack.frame_base=20;r.stack.absolute(20)=e::float_bits(f);});
            test(42,{0xffffffff},1,[&](e::Runtime& r){r.stack.push(e::float_bits(f),'f');});
            test(44,{e::float_bits(-1.f)},1,[&](e::Runtime& r){r.stack.push(a,'i');});
            test(43,{0},1,[&](e::Runtime& r){r.stack.frame_base=64;r.stack.push(e::float_bits(f),'f');});
            test(45,{0},1,[&](e::Runtime& r){r.stack.frame_base=64;r.stack.push(a,'i');});
            test(78,{0},1,[&](e::Runtime& r){r.stack.frame_base=64;r.stack.absolute(64)=a;});
            test(83,{},0,[&](e::Runtime& r){r.stack.push(a,'i');});test(84,{},0,[&](e::Runtime& r){r.stack.push(a,'f');});
            test(23,{i%100},0,[](e::Runtime&){});test(24,{e::float_bits(f)},0,[](e::Runtime&){});
            test(40,{i%40*4},0,[](e::Runtime& r){r.stack.push(0x11223344);});
            test(41,{},0,[&](e::Runtime& r){r.stack.enter_frame(i%40*4);});
            test(46,{0,1,a,0,a^0xdeadbeef,0},1,[&](e::Runtime& r){r.stack.frame_base=64;});
            test(47,{0,1,a,0,e::float_bits(f),0},1,[&](e::Runtime& r){r.stack.frame_base=64;});
            test(85,{0,e::float_bits(f),e::float_bits(f/4)},1,[&](e::Runtime& r){r.stack.frame_base=64;});
        }
        for(unsigned op:{0u,1u,22u,30u,31u})for(unsigned i=0;i<20;++i)
            test(op,{},0,[](e::Runtime& r){r.stack.pointer=32;},i%16,i%2?255:0);
        for(unsigned op:{12u,13u,14u})for(unsigned value:{0u,1u,0xffffffffu}) {
            auto code=instruction(op,{40,17});append(code,instruction(1));append(code,instruction(0,{},0,255,1000000));
            e::Program program;program.subroutines.push_back({"jump",code});
            check("opcode_"+std::to_string(op),program,[&](e::Runtime& r){if(op!=12)r.stack.push(value,'i');});
        }
        // Non-consuming typed stack references use top-relative 8-byte slots.
        for(unsigned i=0;i<200;++i)test(23,{0xfffffffe},1,[&](e::Runtime& r){r.stack.push(i,'i');r.stack.push(e::float_bits(99.f),'f');});
        for(unsigned variant=0;variant<4;++variant)for(unsigned i=0;i<100;++i){
            auto call=instruction(11,{4,static_cast<unsigned>('z'),variant%2?0x6966u:0x6669u,variant%2?e::float_bits(12.75f):i},variant>=2?2:0);
            call[11]=2;
            auto caller=call;append(caller,instruction(0,{},0,255,1000000));
            auto callee=instruction(40,{4});append(callee,instruction(10));
            e::Program program;program.subroutines={{"a",caller},{"z",callee}};
            check("synchronous_call_return",program,[&](e::Runtime& r){r.stack.enter_frame(8);if(variant>=2){
                // Numeric -1 means typed stack top in the source argument domain.
                put32(r.program->subroutines[0].bytes,28,variant%2?e::float_bits(-1.f):0xffffffffu);
                r.stack.push(variant%2?e::float_bits(12.75f):i,variant%2?'f':'i');
            }});
        }
        // A call from an empty stack returns through the -1 sentinel frame.
        {
            auto call=instruction(11,{4,static_cast<unsigned>('z')});call[11]=1;
            e::Program program;program.subroutines={{"a",call},{"z",instruction(40,{0})}};
            append(program.subroutines[1].bytes,instruction(10));
            check("root_call_return",program,[](e::Runtime&){});
        }
        for(unsigned i=0;i<500;++i){
            const auto a=static_cast<float>(static_cast<int>(rng()%100000)-50000)/1000.f;
            const auto b=static_cast<float>(static_cast<int>(rng()%100000)-50000)/1000.f;
            for(unsigned op:{79u,80u,88u})test(op,{},0,[&](e::Runtime& r){r.stack.push(e::float_bits(op==88?std::abs(a):a),'f');});
            test(81,{0,e::float_bits(4),e::float_bits(a),e::float_bits(b)},3,[](e::Runtime& r){r.stack.frame_base=64;});
            test(82,{0},1,[&](e::Runtime& r){r.stack.frame_base=64;r.stack.absolute(64)=e::float_bits(a);});
            test(86,{0,e::float_bits(a),e::float_bits(b)},1,[](e::Runtime& r){r.stack.frame_base=64;});
            test(87,{0,e::float_bits(a),e::float_bits(b),e::float_bits(b),e::float_bits(a)},1,[](e::Runtime& r){r.stack.frame_base=64;});
            test(89,{0,e::float_bits(a),e::float_bits(b)},1,[](e::Runtime& r){r.stack.frame_base=64;});
            test(90,{0,e::float_bits(4),e::float_bits(a),e::float_bits(b),e::float_bits(a)},3,[](e::Runtime& r){r.stack.frame_base=64;});
            test(94,{0,e::float_bits(4),e::float_bits(a),e::float_bits(b),e::float_bits(b),e::float_bits(2)},3,[](e::Runtime& r){r.stack.frame_base=64;});
            test(95,{0,e::float_bits(a),e::float_bits(b)},1,[](e::Runtime& r){r.stack.frame_base=64;});
            test(96,{0,e::float_bits(a)},1,[](e::Runtime& r){r.stack.frame_base=64;});
            test(97,{0,e::float_bits(a)},1,[](e::Runtime& r){r.stack.frame_base=64;});
        }
        for(unsigned mode=0;mode<34;++mode)for(unsigned i=0;i<25;++i){
            const auto begin=static_cast<float>(static_cast<int>(rng()%10000)-5000)/32.f;
            const auto end=static_cast<float>(static_cast<int>(rng()%10000)-5000)/32.f;
            test(91,{0,e::float_bits(64),i+1,mode,e::float_bits(begin),e::float_bits(end)},2,[](e::Runtime& r){r.interpolators.resize(1);});
            test(92,{0,e::float_bits(64),i+1,mode,e::float_bits(begin),e::float_bits(end),e::float_bits(4),e::float_bits(-2)},2,[](e::Runtime& r){r.interpolators.resize(1);});
        }
        for(unsigned op:{17u,18u,19u,20u,21u})for(unsigned id:{0u,777u})for(unsigned i=0;i<30;++i)
            test(op,{id,i},0,[&](e::Runtime& r){r.async_flags=op==19?1:0;});
        const std::uint32_t edge_values[]={0,0x80000000,1,0x80000001,0x3f800000,0xbf800000,0x7f7fffff,0xff7fffff,0x7f800000,0xff800000,0x7fc12345,0xffc12345};
        for(auto bits:edge_values){
            for(unsigned op:{51u,53u,55u,57u,60u,62u,64u,66u,68u,70u})for(auto second:edge_values)
                test(op,{},0,[&](e::Runtime& r){r.stack.push(bits,'f');r.stack.push(second,'f');});
            for(unsigned op:{82u,96u,97u})test(op,op==82?std::vector<std::uint32_t>{0}:std::vector<std::uint32_t>{0,bits},1,[&](e::Runtime& r){r.stack.frame_base=64;r.stack.absolute(64)=bits;});
            // Avoid domain-error cases that need original CRT thread-local errno.
            if((bits&0x7fffffff)<0x7f800000){for(unsigned op:{79u,80u})test(op,{},0,[&](e::Runtime& r){r.stack.push(bits,'f');});}
            if((bits&0x80000000)==0&&(bits&0x7fffffff)<=0x7f800000)test(88,{},0,[&](e::Runtime& r){r.stack.push(bits,'f');});
            // NaN CRT transcendental calls enter original CRT thread-local error
            // handling, which this isolated (uninitialized CRT) oracle cannot run.
        }
        for(unsigned skip=0;skip<=1;++skip)for(unsigned variant=0;variant<4;++variant)for(unsigned i=0;i<80;++i){
            std::vector<std::uint32_t> args{4,static_cast<unsigned>('z')};if(skip)args.push_back(123);
            args.push_back(variant%2?0x6966u:0x6669u);args.push_back(variant%2?e::float_bits(-1.f):0xffffffffu);
            auto call=instruction(skip?16:15,args,variant>=2?1u<<(skip+1):0);call[11]=static_cast<std::uint8_t>(skip+2);
            e::Program program;program.subroutines={{"a",call},{"z",instruction(0,{},0,255,1000000)}};
            e::Scheduler manager(program,engine);auto& source=manager.main;source.subroutine=0;source.instruction_offset=0;source.stack.words.resize(1024);
            source.stack.enter_frame(8);if(variant>=2)source.stack.push(variant%2?e::float_bits(12.75f):i,variant%2?'f':'i');
            e::Runtime target;target.program=&program;target.engine=&engine;target.stack.words.resize(1024);
            Raw raw_source(source),raw_target(target);raw_target.runtime[10]=reinterpret_cast<std::uint32_t>(raw_source.manager.data());
            FloatingEnvironment::prepare();const auto old_result=original<int>(0x53f3b0,raw_source.runtime.data(),raw_target.runtime.data(),skip,0);
            FloatingEnvironment::prepare();const auto new_result=source.call_into(source.current(),target,static_cast<int>(skip));
            ++counts["async_call_setup"];bool equal=old_result==0&&new_result;
            equal=equal&&raw_target.runtime[0]==e::float_bits(target.time)&&raw_target.runtime[1]==static_cast<std::uint32_t>(target.subroutine)&&raw_target.runtime[2]==static_cast<std::uint32_t>(target.instruction_offset);
            equal=equal&&raw_source.runtime[7]==static_cast<std::uint32_t>(source.stack.pointer)&&raw_target.runtime[7]==static_cast<std::uint32_t>(target.stack.pointer)&&raw_target.runtime[8]==static_cast<std::uint32_t>(target.stack.frame_base);
            equal=equal&&std::equal(raw_source.words.begin(),raw_source.words.end(),source.stack.words.begin())&&std::equal(raw_target.words.begin(),raw_target.words.end(),target.stack.words.begin());
            if(!equal){++failed;if(failures.size()<40)failures.push_back("async_call_setup "+std::to_string(skip)+"/"+std::to_string(variant)+"/"+std::to_string(i));}
        }
        const float rates[]={1.f,.5f,1.02f,2.f,-.5f};
        for(unsigned mode=0;mode<34;++mode)for(unsigned test_index=0;test_index<30;++test_index){
            e::math::Interpolator original_state,cpp_state;
            original_state.start=static_cast<float>(static_cast<int>(rng()%1000)-500)/16;
            original_state.end=static_cast<float>(static_cast<int>(rng()%1000)-500)/16;
            original_state.tangent_start=2;original_state.tangent_end=-3;original_state.duration=static_cast<int>(test_index%12)-2;original_state.mode=static_cast<int>(mode);
            cpp_state=original_state;const auto* rate=&rates[test_index%5];*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=rate;
            for(unsigned frame=0;frame<16;++frame){
                FloatingEnvironment::prepare();const auto before=original<float>(0x42a110,&original_state);
                FloatingEnvironment::prepare();const auto after=cpp_state.sample(rate);++counts["interpolator_update"];
                if(e::float_bits(before)!=e::float_bits(after)||std::memcmp(&original_state,&cpp_state,56)){++failed;if(failures.size()<40)failures.push_back("interpolator_update mode "+std::to_string(mode)+" test "+std::to_string(test_index)+" frame "+std::to_string(frame));}
            }
        }
        for(unsigned i=0;i<1000;++i){
            std::uint32_t state=rng(),last=0,maximum=i%3==0?0x7fffffffu:i%3==1?0xffffffffu:0x10000u;
            std::recursive_mutex mutex;e::RandomStream random{state,last,maximum,mutex};
            test(93,{0,e::float_bits(4),e::float_bits(3),e::float_bits(10)},3,[&](e::Runtime& r){r.random=&random;r.stack.frame_base=64;});
        }
        std::ofstream out(std::filesystem::path(argv[2]),std::ios::binary);
        out<<"{\n  \"status\":\""<<(failed?"failed":"passed")<<"\",\n  \"source_sha256\":\""<<expected_sha<<"\",\n  \"vm_cpp_sha256\":\""<<TH20_ECL_CPP_SHA<<"\",\n  \"vm_hpp_sha256\":\""<<TH20_ECL_HPP_SHA<<"\",\n  \"math_cpp_sha256\":\""<<TH20_ECL_MATH_CPP_SHA<<"\",\n  \"math_hpp_sha256\":\""<<TH20_ECL_MATH_HPP_SHA<<"\",\n  \"cpu_compare_sha256\":\""<<TH20_ECL_CPU_COMPARE_SHA<<"\",\n  \"failed\":"<<failed<<",\n  \"comparisons\":{";
        bool first=true;unsigned total=0;for(auto& [name,count]:counts){if(!first)out<<',';first=false;total+=count;out<<'"'<<name<<"\":"<<count;}
        out<<"},\n  \"total\":"<<total<<",\n  \"original_entry_point_executed\":false,\n  \"original_imports_resolved\":[\"GetCurrentThreadId\"],\n  \"scope\":\"Synthetic instruction states: return codes, time bits, sub/IP, SP/BP, 4096 stack bytes, mutable instruction headers, full 56-byte interpolation records, and RNG state. Includes synchronous return and asynchronous call setup; allocator/traversal ownership tested separately in source_tests.cpp\",\n  \"limitations\":[\"No entity opcode, game-variable or complete-game validation\",\"Async spawning allocation and task-list traversal are source-only tests, not original-CPU comparisons\",\"Original CRT NaN transcendental probe faulted in isolated image; only new-CRT NaN classification is tested for sin/cos/sqrt. Basic float/angle opcodes have explicit NaN comparisons\",\"Original RNG lock is exercised nested on one thread; cross-thread ordering not verified\",\"Invalid-stack behavior and CPU exception/errno delivery are outside equivalence\"],\n  \"failure_examples\":[";
        for(std::size_t i=0;i<failures.size();++i){if(i)out<<',';out<<th20::json_string(failures[i]);}out<<"]\n}\n";
        std::cout<<"ECL CPU checks "<<total<<", failed "<<failed<<'\n';for(auto& f:failures)std::cout<<f<<'\n';return failed?1:0;
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}
}
