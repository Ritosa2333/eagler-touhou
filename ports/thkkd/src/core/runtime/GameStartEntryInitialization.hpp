// 657310完整初始化本体；池清零和资源调用遵循原控制流，所有别名即时同步。
#pragma once
#include "core/runtime/GameStartEntryBindings.hpp"
#include "core/runtime/GameStartEntryConstants.hpp"
#include "core/runtime/GameStartEntryServices.hpp"
#include "core/shared/FloorToInt32.hpp"
namespace kaikeidou {
namespace game_start_entry_detail {
struct ResourceAdapter final:StageResourceServices {
 GameStartEntryMemory& memory;GameStartEntryServices& api;
 ResourceAdapter(GameStartEntryMemory& m,GameStartEntryServices& a):memory(m),api(a){}
 template<class F>auto call(F f){memory.reconcile();auto result=f();memory.reconcile();return result;}
 std::int32_t delete_graph(std::uint32_t h,std::uint32_t f)override{return call([&]{return api.delete_graph(h,f);});}
 std::uint32_t load_graph(std::string_view p,std::uint32_t f)override{return call([&]{return api.load_graph(p,f);});}
 std::uint32_t platform_random(std::uint32_t n)override{return call([&]{return api.platform_random(n);});}
 std::int32_t movie_seek(std::uint32_t h,std::uint32_t p)override{return call([&]{return api.movie_seek(h,p);});}
 std::int32_t movie_play(std::uint32_t h,std::uint32_t m,std::uint32_t f)override{return call([&]{return api.movie_play(h,m,f);});}
};
struct MathAdapter final:GameMath {
 GameStartEntryMemory& memory;GameMath& math;
 MathAdapter(GameStartEntryMemory& m,GameMath& f):memory(m),math(f){}
 template<class F>binary_float::Number call(F f){memory.reconcile();auto result=f();memory.reconcile();return result;}
 binary_float::Number atan2(std::uint64_t y,std::uint64_t x)override{return call([&]{return math.atan2(y,x);});}
 binary_float::Number cosine(std::uint64_t x)override{return call([&]{return math.cosine(x);});}
 binary_float::Number sine(std::uint64_t x)override{return call([&]{return math.sine(x);});}
};
inline std::uint32_t difficulty_value(std::uint32_t difficulty,const std::array<std::uint32_t,6>& values){return values[difficulty<6?difficulty:3];}
inline void initialize(GameStartEntryRefs& r,GameStartEntryMemory& memory,std::uint32_t parameter,bool replay_variant,unsigned precision,GameMath& math,GameStartEntryServices& services){
 using namespace binary_float;
 memory.word(0xa74c08,0);memory.word(0xa78e2c,0);memory.word(0xa7a234,0);
 memory.word(0xa78fe8,0);memory.word(0xa78fec,0);memory.word(0xa78ff0,1000);memory.word(0xa78ff4,2000);
 if(memory.word(0xa7e618)==0&&memory.word(0xb3e2ec)!=6&&memory.word(0xb3e2ec)!=7&&parameter==0)memory.word(0xb3e2ec,0);
 memory.word(0xa77390,memory.word(0xb3e2ec)==7?memory.word(0xa87f30):0);
 memory.word(0xa733c0,0);memory.word(0xb3e2e8,0);if(replay_variant)memory.word(0xb43110,0);memory.word(0xa82f8c,1);
 constexpr std::array<std::array<std::uint32_t,2>,17> first{{{{0xa8cd68,240}},{{0xa87fe8,19840}},{{0xa8e660,720000}},{{0xa87be0,480}},{{0xa79090,4480}},{{0xa74f70,2240}},{{0xa735c0,5600}},{{0xa8ce60,6144}},{{0xa72248,3360}},{{0xa73058,192}},{{0xa7e658,420}},{{0xa7e5ec,1}},{{0xa7e5f0,32}},{{0xa7ea40,440}},{{0xa78fd8,16}},{{0xa758b0,6400}},{{0xa79000,80}}}};
 for(const auto& block:first)memory.clear(block[0],block[1]);memory.word(0xa7e7fc,0);
 for(const auto& block:std::array<std::array<std::uint32_t,2>,4>{{{{0xa78558,160}},{{0xa87dc0,320}},{{0xa75830,40}},{{0xa73400,160}}}})memory.clear(block[0],block[1]);
 memory.quad(0xa8cdf8,constant(0x6d1a60));memory.quad(0xa8ce00,constant(0x6d1cb0));
 reset_player_bomb_speed(r.player,memory.word(0xa74c0c),0x3ff0000000000000ull,precision);memory.reconcile();
 memory.quad(0xa8ce38,to_binary64(integer(memory.word(0xa74c0c)==1?96u:48u)));
 memory.quad(0xa8ce40,constant(memory.word(0xa74c0c)==0||memory.word(0xa74c0c)>2?0x6d1960:0x6d1900));
 memory.word(0xa8ce48,difficulty_value(memory.word(0xa7e610),{{6,8,10,12,10,12}}));
 memory.word(0xa8ce4c,difficulty_value(memory.word(0xa7e610),{{32,0,0xffffffe0u,0xffffffc0u,0xffffffe0u,0xffffffc0u}})+128u);memory.word(0xa8cd88,memory.word(0xa8ce4c));
 if(memory.word(0xa7e618)!=0){memory.word(0xa8cd7c,8);memory.word(0xa8cd80,8);memory.word(0xa8cd70,200);}
 else if(memory.word(0xa7e610)==5){
  memory.word(0xa8cd7c,0);memory.word(0xa8cd80,0);memory.word(0xa8cd70,400);
  const auto selected=memory.word(0xa87f30);
  memory.word(0xa7eba4,options_detail::signed_word(selected)<=4?selected+153u:(selected-5u)*3u+158u+memory.word(0xa74c0c));
 }else{memory.word(0xa8cd7c,2);memory.word(0xa8cd80,2);memory.word(0xa8cd70,200);}
 const auto power_scale=to_binary64(multiply(integer(memory.word(0xa8cd70)),binary64(constant(0x6a7708)),precision));
 reset_player_options(r.options,r.player,r.options_context,0,floor_to_int32(power_scale));memory.reconcile();
 const auto selector=memory.word(0xa74c0c)*10u+memory.word(0xa7e614);const auto label=name(selector);
 if(!label.empty()){std::vector<std::uint8_t> text(label.begin(),label.end());text.push_back(0);memory.write(0xa8cda8,text.data(),text.size());}
 if(selector==20){memory.quad(0xa75100,0);memory.quad(0xa75108,constant(0x6d1b60));memory.quad(0xa75110,0);}
 else if(selector==22){
  for(const auto& p:std::array<std::array<std::uint32_t,2>,8>{{{{0xa74f88,0x6d1e48}},{{0xa751b8,0x6d1d18}},{{0xa753e8,0x6d1878}},{{0xa75618,0x6d1a90}},{{0xa74f98,0x6d1a18}},{{0xa751c8,0x6d1918}},{{0xa753f8,0x6d1968}},{{0xa75628,0x6d1a08}}}})memory.quad(p[0],constant(p[1]));
 }
 ResourceAdapter api(memory,services);MathAdapter math_api(memory,math);change_stage_resources(memory.word(0xb3e2ec),r.resources,r.meshes,r.player,r.random,static_cast<RandomPrecision>(precision),math_api,api);memory.reconcile();
 control_stage_movies(1,r.resources,api);memory.reconcile();memory.byte(0xa7e5ec,255);
}
}
inline void initialize_game_start(GameStartEntryRefs& refs,GameStartEntryGaps& gaps,GameStartEntryReplayContext& replay,std::uint32_t parameter,bool replay_variant,unsigned precision,GameMath& math,GameStartEntryServices& services,const GameStartEntryMemoryExtensions& extensions={}){
 GameStartEntryMemory memory(refs.random);bind_game_start_entry(memory,refs,gaps,replay,replay_variant);for(const auto& extra:extensions)memory.bind_bytes(extra.address,extra.data,extra.size);memory.seal();game_start_entry_detail::initialize(refs,memory,parameter,replay_variant,precision,math,services);
}
}
