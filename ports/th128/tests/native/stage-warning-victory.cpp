#include "game/PerfectHud.hpp"
#include "game/StageIntroLogos.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
using Bytes=std::vector<std::uint8_t>;
int checks=0,failed=0;
void check(bool yes,const char* message){++checks;if(!yes)++failed;std::cout<<(yes?"PASS ":"FAIL ")<<message<<'\n';}
Bytes read(const char* p){std::ifstream f(p,std::ios::binary);Bytes b{std::istreambuf_iterator<char>(f),{}};if(b.empty())throw std::runtime_error("Pinned ANM missing");return b;}
template<class H> bool font(H& h,const Bytes& b,std::string& error){if constexpr(requires{h.loadScoreFont(b,error);})return h.loadScoreFont(b,error);else return false;}
template<class H> bool finish(H& h,int n,int bonus,std::string& error){if constexpr(requires{h.spellResult(n,bonus,error);})return h.spellResult(n,bonus,error);else return false;}
template<class H> bool times(H& h,unsigned frames,std::optional<double> seconds,std::string& error){if constexpr(requires{h.spellTimes(frames,seconds,error);})return h.spellTimes(frames,seconds,error);else return false;}
int main(int count,char** argv){try{
 if(count!=5)throw std::runtime_error("front/ascii/second/third ANM required");
 const auto front=read(argv[1]),ascii=read(argv[2]);
 for(int stage=2;stage<=3;++stage){th128::stageintro::Host host;std::string error;const auto bytes=read(argv[stage+1]);const auto name=std::string(stage==2?"logo_a1_2.anm":"logo_a1_3.anm");if(!host.load(std::to_string(stage),name,bytes.data(),bytes.size(),error))throw std::runtime_error(error);
  bool warning=false,after=false;for(int t=0;t<545;++t){host.tick();if(!host.error().empty())throw std::runtime_error(host.error());const auto& rows=host.draw();for(const auto& r:rows)if(r.script==2&&(r.sprite==1||r.sprite==2)&&r.width==256&&r.height==32)warning=true;if(t>=540)for(const auto& r:rows)if(r.script==2)after=true;}
  check(warning,stage==2?"stage2 authored Chinese fire warning renders":"stage3 authored Chinese laser warning renders");check(!after,"authored warning lifetime retires");
 }
 for(int notice=7;notice<=9;++notice){th128::hud::PerfectHud hud;std::string error;
  if(!hud.load(front,error)||!hud.boot(10000,error))throw std::runtime_error(error);const bool loaded=font(hud,ascii,error);check(loaded,"actual ASCII bank is registered for result numbers");
  const bool complete=finish(hud,notice,341060,error);check(complete,"native result notice accepts original7/8/9");
  check(times(hud,671,11.5,error),"victory frame clock and independent real clock latch");
  for(int t=0;t<36;++t)if(!hud.tick(error))throw std::runtime_error(error);
  const auto rows=hud.snapshots();
  const auto logic=std::count_if(rows.begin(),rows.end(),[](const auto& r){return r.id>=500000&&r.id<500100&&r.y==240;});
  const auto real=std::count_if(rows.begin(),rows.end(),[](const auto& r){return r.id>=500000&&r.id<500100&&r.y==256;});
  check(logic==3&&real==3,"original integer time lines use %3d dot at318/240 and318/256");
  check(std::all_of(rows.begin(),rows.end(),[](const auto& r){return r.id<500000||r.id>=500100||r.state.visible;}),"generated time glyphs must survive actual native visibility filter");
  check(std::any_of(rows.begin(),rows.end(),[](const auto& r){return r.id>=500000&&r.y==246&&r.x==368&&r.state.sx>.59f&&r.state.sx<.61f;}),"fractional time follows original small style4 line");
  const int script=170+notice-7;check(std::any_of(rows.begin(),rows.end(),[&](const auto& r){return r.state.bank==5&&r.state.script==script&&r.state.sprite==116+notice-7;}),"authored Chinese victory banner and medal source");
  if(notice!=9){int digits=0;for(const auto& r:rows)if(r.state.bank==11&&r.state.script>=9&&r.state.script<=16&&(r.state.flags&2u))digits++;check(digits==6,"six significant native animated reward digits");}
  for(int t=0;t<180;++t)if(!hud.tick(error))throw std::runtime_error(error);
  const auto ended=hud.snapshots();check(std::none_of(ended.begin(),ended.end(),[](const auto& r){return r.state.script>=170&&r.state.bank==5;}),"result banner original VM lifespan retires");
 }
 std::cout<<"CHECKS "<<checks<<" FAILED "<<failed<<" WORLDS 0\n";return failed?1:0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}}
