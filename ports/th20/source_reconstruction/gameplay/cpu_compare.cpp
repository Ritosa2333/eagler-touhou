// Isolated original CPU oracle; never linked into the reconstructed game.
#define wmain unused_original_oracle_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "test_services.hpp"

namespace {
template<class R,class... A> R original(std::uint32_t rva,void* self,A... args) {
    return reinterpret_cast<R(__thiscall*)(void*,A...)>(mapped_image_base+rva)(self,args...);
}
}
int wmain(int argc,wchar_t** argv) {
    try {
        if(argc!=3) throw std::runtime_error("Usage: th20_gameplay_cpu_compare VERIFIED_TH20.exe REPORT.json");
        const std::filesystem::path input(argv[1]),report(argv[2]);
        if(std::filesystem::weakly_canonical(input)==std::filesystem::weakly_canonical(report)) throw std::runtime_error("Report cannot replace original EXE");
        const auto bytes=th20::read_file(input);if(sha256(bytes)!=expected_sha) throw std::runtime_error("Original hash mismatch");
        const auto pe=th20::parse_pe(bytes);Mapping mapped(bytes,pe);mapped_image_base=mapped.address();
        std::map<std::string,unsigned> counts;std::vector<std::string> errors;unsigned failed=0;
        const auto check=[&](const char* name,bool condition) {++counts[name];if(!condition) {++failed;if(errors.size()<20)errors.push_back(name);}};
        RecordedServices env;auto* game=new gp::GameController(env);gp::controller=game;
        std::array<std::uint8_t,0x110> raw{};
        auto* returned=original<void*>(0xb9df0,raw.data());
        const auto vptr=*reinterpret_cast<std::uintptr_t*>(game);std::memcpy(raw.data(),&vptr,4);
        check("factory_zeroed_constructor_all_272_bytes",returned==raw.data() && std::memcmp(raw.data(),game,0x110)==0);
        std::mt19937 rng(0x20c05ba8);
        for(unsigned n=0;n<20000;++n) {
            game->game_flags=n<256?n:rng();game->restart_mode=static_cast<std::int32_t>(rng());
            std::memcpy(raw.data(),game,0x110);
            const auto suppress=original<unsigned>(0x24030,raw.data());
            const auto freeze=original<unsigned>(0x24060,raw.data());
            check("suppression_flags_424030",suppress==static_cast<unsigned>(game->update_suppressed()));
            check("animation_freeze_flag_424060",freeze==static_cast<unsigned>(game->animation_frozen()));
            check("restart_mode_488830",original<int>(0x88830,raw.data())==game->restart());
            check("nonnull_pause_predicates",gp::animation_frozen()==(freeze!=0) && gp::suppress_primary_update()==(freeze!=0&&suppress!=0));
            *reinterpret_cast<void**>(mapped_image_base+0x1ba828)=raw.data();
            if(suppress && freeze) {
                using Cdecl=unsigned(__cdecl*)(void*);
                const auto result=reinterpret_cast<Cdecl>(mapped_image_base+0x50880)(nullptr);
                check("controller_wrapper_450880_paused_branch",result==1 && gp::suppress_primary_update());
            }
            original<void>(0xbd920,raw.data());game->clear_flag_6();
            check("clear_flag6_preserves_all_other_bytes",std::memcmp(raw.data(),game,0x110)==0);
        }
        gp::controller=nullptr;check("nullable_pause_predicates",!gp::animation_frozen()&&!gp::suppress_primary_update());
        delete game;
        unsigned total=0;for(auto [name,count]:counts)total+=count;
        std::ofstream out(report);out<<"{\"status\":"<<th20::json_string(failed?"failed":"passed")<<",\"cases\":"<<total<<",\"failed\":"<<failed<<",\"original_sha256\":"<<th20::json_string(expected_sha)<<",\"coverage\":{";
        bool first=true;for(auto [name,count]:counts){if(!first)out<<',';first=false;out<<th20::json_string(name)<<':'<<count;}out<<"},\"failures\":[";
        first=true;for(auto& error:errors){if(!first)out<<',';first=false;out<<th20::json_string(error);}out<<"],\"source_sha256\":{\"gameplay.hpp\":"<<th20::json_string(TH20_GAMEPLAY_HPP_SHA)<<",\"gameplay.cpp\":"<<th20::json_string(TH20_GAMEPLAY_CPP_SHA)<<"},\"scope\":\"CPU constructor and getter comparisons; whole teardown is separately tested with recorded required subsystem boundaries, not claimed as an original-CPU teardown comparison.\"}\n";
        std::cout<<total<<" GameController CPU comparisons, "<<failed<<" failed\n";return failed?1:0;
    } catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 2;}
}
