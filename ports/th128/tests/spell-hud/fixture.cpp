#include "game/SpellVisuals.hpp"
#include "game/NativeNormalRecord.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <algorithm>
using th128::spell::Visuals;
using MedalFlags=std::array<bool,3>;
int checks=0,failed=0;
void check(bool value,const char* label){++checks;if(!value)++failed;std::cout<<(value?"PASS ":"FAIL ")<<label<<'\n';}
template<class V> bool begin(V& v,std::string& error,MedalFlags medals){
 if constexpr(requires{v.start(std::array<float,3>{},error,medals);})return v.start({0,128,0},error,medals);
 else return v.start({0,128,0},error);
}
template<class V> std::vector<th128::spell::Visual> bonus(const V& v,unsigned flags,unsigned value){
 if constexpr(requires{v.bonusGlyphs(flags,value);})return v.bonusGlyphs(flags,value);
 else return {};
}
template<class P> MedalFlags history(std::span<const std::uint8_t> bytes,int id){
 if constexpr(requires{P::spellMedals(bytes,id);})return P::spellMedals(bytes,id);
 else return {};
}
template<class N> MedalFlags ownedHistory(const N& n,int id){
 if constexpr(requires{n.spellMedals(id);})return n.spellMedals(id);
 else return {};
}
void write32(std::vector<std::uint8_t>& bytes,std::size_t offset,unsigned value){for(int i=0;i<4;++i)bytes[offset+i]=std::uint8_t(value>>(i*8));}
int main(int count,char** argv){try{
 if(count!=7)throw std::runtime_error("Six pinned original ANMs required");
 const int banks[]={0,3,6,11,12,7};std::array<std::vector<std::uint8_t>,6> assets;
 for(int i=0;i<6;++i){std::ifstream f(argv[i+1],std::ios::binary);assets[i]={std::istreambuf_iterator<char>(f),{}};if(assets[i].empty())throw std::runtime_error("Original ANM unavailable");}
 for(int mask=0;mask<8;++mask){
  Visuals v;std::string error;for(int i=0;i<6;++i)if(!v.load(banks[i],assets[i],error))throw std::runtime_error(error);
  const MedalFlags medals{bool(mask&1),bool(mask&2),bool(mask&4)};
  if(!begin(v,error,medals))throw std::runtime_error(error);
  check(bonus(v,7,248000).empty(),"bonus follows original label zero alpha at birth");
  for(int i=0;i<120;++i)if(!v.tick({0,128,0},true,error))throw std::runtime_error(error);
  const auto nodes=v.snapshots();const auto label=std::find_if(nodes.begin(),nodes.end(),[](const auto& n){return n.state.bank==11&&n.state.script==3;});
  check(label!=nodes.end()&&label->state.sprite==306&&label->x==480&&label->y==61,"original combined Bonus/Medal label retained");
  bool exact=true;for(int i=0;i<3;++i){const auto slot=std::find_if(nodes.begin(),nodes.end(),[i](const auto& n){return n.state.bank==11&&n.state.script==4+i;});exact=exact&&slot!=nodes.end()&&slot->state.sprite==(medals[i]?309+i:308)&&slot->x==464+i*12&&slot->y==57;}
  check(exact,"three medal sprites use corresponding stored gold/silver/bronze flags");
  if(mask==7){
   const auto digits=bonus(v,7,248000);bool match=digits.size()==6;
   const std::string text="248000";for(std::size_t i=0;match&&i<digits.size();++i)match=digits[i].state.sprite==179+text[i]&&digits[i].x==379+7*i&&digits[i].y==61&&digits[i].sprite&&digits[i].sprite->width==8&&digits[i].sprite->height==10;
   check(match,"original style2 %8d bonus uses actual sprite atlas and leading spaces");
   check(!digits.empty()&&std::all_of(digits.begin(),digits.end(),[](const auto& n){return bool(n.state.flags2&4u);}),"original style2 stays point sampled at non-unit canvas scale");
   const auto lost=bonus(v,5,248000);check(lost.size()==1&&lost[0].state.sprite==307&&lost[0].x==387&&lost[0].y==61,"failed spell uses original Failed sprite");
   check(bonus(v,0,248000).empty(),"inactive spell emits no bonus");
   const auto red=bonus(v,0x107,248000);check(!red.empty()&&(red[0].state.primary&0xffffffu)==0xff0000u,"original temporary red bonus flag");
   if(!v.finish(error))throw std::runtime_error(error);for(int i=0;i<40;++i)if(!v.tick({0,128,0},false,error))throw std::runtime_error(error);
   check(v.snapshots().empty()&&bonus(v,0,248000).empty(),"spell HUD exit retires all owners");
  }
 }
 using P=th128::record::NativeScoreProfile;
 th128::Random random{};std::array<std::uint8_t,250> ranks{};std::array<std::uint8_t,9> name{};auto profile=th128::record::NativeRecordBootstrap::create(random,ranks,name,name);
 const auto row=P::ClearOffset+12+70*152;write32(profile,row+132,3);write32(profile,row+128,5);write32(profile,row+136,9);
 const auto before=profile;check(history<P>(profile,70)==MedalFlags{true,true,true}&&profile==before,"medal history reads signed existing counters without writing profile");
 write32(profile,row+132,0xffffffffu);write32(profile,row+128,0);check(history<P>(profile,70)==MedalFlags{false,false,true},"nonpositive gold and silver remain unearned");
 th128::record::NativeNormalRecord record(profile);record.beginStage(1,0,0);check(ownedHistory(record,70)==MedalFlags{false,false,true},"ordinary live record supplies actual current card history");
 check(history<P>(profile,71)==MedalFlags{},"another card history never leaks");
 bool rejected=false;try{(void)history<P>(profile,250);}catch(const std::exception&){rejected=true;}check(rejected,"out-of-range spell id rejected");
 std::cout<<"CHECKS "<<checks<<" FAILED "<<failed<<" WORLDS 0\n";return failed?1:0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}}
