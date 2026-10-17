#define wmain unused_bomb_native_main
#include "../../tests/native_cpu_compare.cpp"
#undef wmain
#include "bomb.hpp"
#include "../gameplay/enemy.hpp"
#include "../gameplay/player_state.hpp"
namespace b=th20::source::bomb;namespace gp=th20::source::gameplay;namespace q=th20::source::scheduler;
namespace {
template<class R,class... A>R cpu(std::uint32_t va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
}
int wmain(int argc,wchar_t** argv){try{
    if(argc!=3)throw std::runtime_error("Usage: bomb_state_cpu_compare ORIGINAL.exe REPORT.json");
    const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original hash mismatch");const auto pe=th20::parse_pe(bytes);Mapping image(bytes,pe);mapped_image_base=image.address();
    std::mt19937 random(0x477f60);unsigned checks=0,failed=0;std::vector<std::string> failures;
    auto check=[&](const char* label,const void* expected,const void* actual,std::size_t size){++checks;if(std::memcmp(expected,actual,size)){++failed;if(failures.size()<20)failures.emplace_back(label);}};
    for(unsigned test=0;test<12000;++test){
        th20::source::game_session::Player expected,actual;for(auto* p=reinterpret_cast<std::uint8_t*>(&actual);p!=reinterpret_cast<std::uint8_t*>(&actual)+sizeof(actual);++p)*p=static_cast<std::uint8_t>(random());
        const auto amount=th20::recovered::signed_bits(random());expected=actual;cpu<void>(0x477f60,&expected,amount);b::add_player_meter(actual,amount);check("meter wholePlayer + signed overflow clamp",&expected,&actual,sizeof(actual));
        if(test%2)gp::player_state::write(actual,0xcc,static_cast<std::int32_t>(test%17)-3);expected=actual;
        const auto expected_count=cpu<int>(0x477ff0,&expected),actual_count=b::bomb_count(actual);check("bomb count mutating clamp wholePlayer",&expected,&actual,sizeof(actual));check("bomb count return",&expected_count,&actual_count,4);
        alignas(gp::EnemyController) std::array<std::uint8_t,sizeof(gp::EnemyController)> enemy_bytes,enemy_expected;
        for(auto& byte:enemy_bytes)byte=static_cast<std::uint8_t>(random());enemy_expected=enemy_bytes;
        cpu<void>(0x477f40,enemy_expected.data(),amount);b::add_enemy_bomb_counter(*reinterpret_cast<gp::EnemyController*>(enemy_bytes.data()),amount);check("enemy counter moduloADD full prefix",enemy_expected.data(),enemy_bytes.data(),enemy_bytes.size());
        std::array<std::uint8_t,0x428> entity,entity_expected;for(auto& byte:entity)byte=static_cast<std::uint8_t>(random());entity_expected=entity;
        const auto value=random();cpu<void>(0x478260,entity_expected.data(),value);b::set_enemy_bomb_flag(entity.data(),value);check("Enemy+358 bit5 all other bytes unchanged",entity_expected.data(),entity.data(),entity.size());
    }
    for(unsigned count=0;count<65;++count){
        alignas(gp::EnemyController) std::array<std::uint8_t,sizeof(gp::EnemyController)> storage{};auto& enemy=*reinterpret_cast<gp::EnemyController*>(storage.data());q::initialize_list(enemy.enemies);
        std::vector<std::array<std::uint8_t,0x428>> entities(count),expected;std::vector<q::Link> links(count);
        for(unsigned i=0;i<count;++i){for(auto& byte:entities[i])byte=static_cast<std::uint8_t>(random());q::initialize_link(links[i],reinterpret_cast<q::Node*>(entities[i].data()));q::append(enemy.enemies,links[i]);}
        const auto before=entities;const auto before_links=links;const auto before_storage=storage;
        cpu<void>(0x478190,&enemy);expected=entities;const auto expected_links=links;const auto expected_storage=storage;
        // Preserve node addresses: assignment of an equal-sized vector does not reallocate.
        entities=before;links=before_links;storage=before_storage;b::mark_enemies_for_bomb(enemy);
        check("list loop each Enemy complete428",expected.data(),entities.data(),count*sizeof(entities[0]));
        check("list iterator registration and cleanup",expected_links.data(),links.data(),count*sizeof(q::Link));
        check("list sentinel and controller untouched",expected_storage.data(),storage.data(),storage.size());
    }
    for(unsigned test=0;test<8192;++test){
        alignas(b::Controller) std::array<std::uint8_t,sizeof(b::Controller)> controller_bytes{};auto& controller=*reinterpret_cast<b::Controller*>(controller_bytes.data());controller.active_state=test%3;
        th20::source::game_session::Context context{};context.objects_04[5]=&controller;
        std::array<std::uint8_t,0xc4> actual,expected;for(auto& byte:actual)byte=static_cast<std::uint8_t>(random());
        const auto* context_pointer=&context;std::memcpy(actual.data()+0xc0,&context_pointer,4);const auto elapsed=test%2?static_cast<std::int32_t>(test%120)-1:th20::recovered::signed_bits(random());std::memcpy(actual.data()+0x28,&elapsed,4);
        expected=actual;cpu<void>(0x487bb0,expected.data());b::notify_bomb_start(actual.data());check("487bb0 secondary-owner state transition completec4",expected.data(),actual.data(),actual.size());
    }
    std::ofstream report(argv[2]);report<<"{\n\"status\":\""<<(failed?"failed":"passed")<<"\",\n\"checks\":"<<checks<<",\n\"failed\":"<<failed<<",\n\"state_cpp_sha256\":\""<<TH20_BOMB_STATE_SHA<<"\",\n\"bomb_hpp_sha256\":\""<<TH20_BOMB_HPP_SHA<<"\",\n\"scope\":\"12000 randomized original477f40,477f60,477ff0,478260 comparisons including full storage and integer overflow; original478190 lists of0..64 entities including iterator cleanup;8192 secondary-owner487bb0 transition cases\",\n\"limitations\":[\"Bomb lifecycle, character subclasses, rendering and trigger integration are not exercised here\"],\n\"failure_examples\":[";
    for(std::size_t i=0;i<failures.size();++i){if(i)report<<',';report<<th20::json_string(failures[i]);}report<<"]\n}\n";if(!report)throw std::runtime_error("Report write failed");std::cout<<"Bomb state: "<<checks<<" checks, "<<failed<<" failures\n";return failed?1:0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}}
