// Test-only mapping of hash-verified original code. Never linked into gameplay.
#define wmain unused_original_oracle_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "player_state.hpp"
#include "stage_data.hpp"
#include "gameplay_source_hashes.hpp"
namespace ps=th20::source::gameplay::player_state;
namespace gp=th20::source::gameplay;
namespace {
template<class R,class... A> R original(std::uint32_t address,void* self,A... args) {
    return reinterpret_cast<R(__thiscall*)(void*,A...)>(mapped_image_base+address-0x400000)(self,args...);
}
}
int wmain(int argc,wchar_t** argv) {
    try {
        if(argc!=3)throw std::runtime_error("Usage: player_cpu_compare VERIFIED_TH20.exe REPORT.json");
        const std::filesystem::path input(argv[1]),report(argv[2]);
        if(std::filesystem::weakly_canonical(input)==std::filesystem::weakly_canonical(report))throw std::runtime_error("Report must not replace original");
        const auto bytes=th20::read_file(input);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original hash mismatch");
        Mapping mapped(bytes,th20::parse_pe(bytes));mapped_image_base=mapped.address();
        *reinterpret_cast<void**>(mapped_image_base+0x1c06a4)=nullptr;
        std::mt19937 rng(0x4bbde020);std::map<std::string,unsigned> counts;unsigned failed=0;
        const auto check=[&](const std::string& name,bool condition) {++counts[name];if(!condition){if(failed<20)std::cerr<<"FAIL "<<name<<'\n';++failed;}};
        ps::Player a,b;ps::PlayerTable ta,tb;
        auto randomize=[&](auto& object){auto* p=reinterpret_cast<unsigned char*>(&object);for(std::size_t j=0;j<sizeof(object);++j)p[j]=static_cast<unsigned char>(rng());};
        for(const auto& field:ps::player_setters) {
            for(unsigned n=0;n<400;++n) {
                randomize(a);b=a;
                const std::int32_t edges[]={INT_MIN,-1,0,1,field.lower-1,field.lower,field.upper,field.upper+1,INT_MAX};
                const auto value=n<std::size(edges)?edges[n]:static_cast<std::int32_t>(rng());
                original<void>(field.address,&a,value);ps::set(b,field,value);
                check("setter_"+std::to_string(field.address),std::memcmp(&a,&b,sizeof(a))==0);
            }
        }
        for(unsigned n=0;n<5000;++n) {
            randomize(a);b=a;const auto value=static_cast<std::int32_t>(rng());
            original<void>(0x4e15e0,&a,value);ps::set_bombs(b,value,nullptr);
            check("set_bombs_no_HUD_all_bytes",std::memcmp(&a,&b,sizeof(a))==0);
            randomize(a);b=a;original<void>(0x4bbde0,&a,value);ps::reset_player(b,value,nullptr);
            check("reset_player_no_HUD_all_bytes",std::memcmp(&a,&b,sizeof(a))==0);
            randomize(ta);tb=ta;original<void>(0x4bbd80,&ta);ps::reset_players(tb,nullptr);
            check("reset_two_players_no_HUD_all_bytes",std::memcmp(&ta,&tb,sizeof(ta))==0);
            randomize(ta);tb=ta;std::uint32_t bits=rng();float amount;std::memcpy(&amount,&bits,4);
            original<void>(0x4bdfd0,&ta,amount);ps::set_meter(tb,amount);
            check("meter_float_bits_clamp_all_bytes",std::memcmp(&ta,&tb,sizeof(ta))==0);
            randomize(ta);tb=ta;const auto stage=original<int>(0x474d80,&ta);
            check("mutating_stage_clamp",stage==ps::stage(tb)&&std::memcmp(&ta,&tb,sizeof(ta))==0);
            randomize(ta);tb=ta;const auto previous=original<int>(0x4bd5c0,&ta);
            check("mutating_previous_stage_clamp",previous==ps::previous_stage(tb)&&std::memcmp(&ta,&tb,sizeof(ta))==0);
            randomize(ta);tb=ta;const auto spell=original<int>(0x499480,&ta);
            check("mutating_spell_clamp",spell==ps::spell(tb)&&std::memcmp(&ta,&tb,sizeof(ta))==0);
            randomize(a);b=a;const auto power=original<int>(0x4b81d0,&a);
            check("mutating_starting_power_clamp",power==ps::starting_power(b)&&std::memcmp(&a,&b,sizeof(a))==0);
        }
        for(int i=0;i<8;++i) {
            auto* row=reinterpret_cast<const std::uint32_t*>(mapped_image_base+0x1b0038+i*0x134);
            const auto* source=reinterpret_cast<const std::uint32_t*>(&gp::stages[i]);
            bool match=row[0]==source[0]&&std::memcmp(row+22,source+22,220)==0;
            for(int j=1;j<22;++j) {
                if(row[j]==0)match&=source[j]==0;
                else match&=source[j]!=0&&std::strcmp(reinterpret_cast<const char*>(row[j]),reinterpret_cast<const char*>(source[j]))==0;
            }
            check("all_stage_fields_and_pointed_strings",match);
            randomize(ta);tb=ta;original<void>(0x4be390,&ta,i);gp::select_stage(tb,i);
            check("select_stage_all_bytes_and_global",std::memcmp(&ta,&tb,sizeof(ta))==0&&gp::selected_stage==&gp::stages[i]&&*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c6110)==mapped_image_base+0x1b0038+static_cast<unsigned>(i)*0x134);
            randomize(ta);ps::write(ta,0x1f8,i);tb=ta;original<void>(0x4dd8d0,&ta);gp::restore_stage(tb);
            check("restore_stage_all_bytes_and_global",std::memcmp(&ta,&tb,sizeof(ta))==0&&gp::selected_stage==&gp::stages[i]&&*reinterpret_cast<std::uint32_t*>(mapped_image_base+0x1c6110)==mapped_image_base+0x1b0038+static_cast<unsigned>(i)*0x134);
        }
        check("difficulty_constants",std::memcmp(gp::initial_stage_parameter,reinterpret_cast<void*>(mapped_image_base+0x1afd08),24)==0&&std::memcmp(gp::meter_minimum,reinterpret_cast<void*>(mapped_image_base+0x1afd20),24)==0&&std::memcmp(gp::meter_maximum,reinterpret_cast<void*>(mapped_image_base+0x1afd38),24)==0);
        unsigned total=0;for(auto [name,count]:counts)total+=count;
        std::ofstream out(report);out<<"{\"status\":"<<th20::json_string(failed?"failed":"passed")<<",\"cases\":"<<total<<",\"failed\":"<<failed<<",\"original_sha256\":"<<th20::json_string(expected_sha)<<",\"coverage\":{";
        bool first=true;for(auto [name,count]:counts){if(!first)out<<',';first=false;out<<th20::json_string(name)<<':'<<count;}
        out<<"},\"source_sha256\":{";first=true;for(auto file:gameplay_source_hashes){if(!first)out<<',';first=false;out<<th20::json_string(file.path)<<':'<<th20::json_string(file.sha);}
        out<<"},\"scope\":\"45 setters, player/table reset with original nullable HUD absent, meter random IEEE754 bits, mutating getters, all eight static stage rows and selection. Load flow is tested separately; not whole game equivalence.\"}\n";
        std::cout<<total<<" player/stage CPU comparisons, "<<failed<<" failed\n";return failed?1:0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 2;}
}
