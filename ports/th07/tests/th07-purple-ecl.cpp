// Execute the production adapter against locally supplied retail ECL. The game
// globals below are boundary doubles, not claims of full-game lifecycle proof.
#include <cassert>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <emscripten/emscripten.h>
#include "BulletManager.hpp"
#include "Stage.hpp"
#include "Gui.hpp"
#include "RuntimeExtension.hpp"
#include "pbg4/Lzss.hpp"
#include "thprac/portable/session.hpp"
#include "section_catalog.hpp"
Stage::Stage() {} // Test boundary only; no stage presentation is executed here.
Stage g_Stage;
BulletManager g_BulletManager;
static int lastName=0;
void Gui::CopyEnemyNameTexture(i32 index) { lastName=index; }
namespace RuntimeExtension {
static EclLoadedHook eclHook;
static GameStartedHook startHook;
static int warpCount=0,warpTime=0;
void SetEclLoadedHook(EclLoadedHook hook){eclHook=hook;}
void SetGameStartedHook(GameStartedHook hook){startHook=hook;}
void SetTimelineTime(std::size_t count,i32 time){warpCount=count;warpTime=time;}
bool OnEclLoaded(const char* path,u8* data,std::size_t size){return eclHook(path,data,size);}
void OnGameStarted(void* game){startHook(game);}
}
#include "../portable/adapters/th07/adapter.cpp"

static std::uint32_t read32(const std::vector<std::uint8_t>& data,std::size_t at){
 assert(at+4<=data.size());std::uint32_t result;std::memcpy(&result,data.data()+at,4);return result;
}
static std::vector<std::uint8_t> decompress(std::vector<std::uint8_t> data,std::size_t begin,std::size_t end,std::size_t size){
 assert(begin<=end&&end<=data.size());
 auto* bytes=Lzss::Decompress(data.data()+begin,int(end-begin),nullptr,unsigned(size));
 assert(bytes);std::vector<std::uint8_t> result(bytes,bytes+size);std::free(bytes);return result;
}
static void install(const thprac::portable::Session& session){
 auto json=thprac::portable::SerializeReplaySession(session);
 assert(ThpracPortableTh07SetSessionJson(json.c_str()));
 EM_ASM({Module.eaglerOptions={thpracSession:JSON.parse(UTF8ToString($0))};},json.c_str());
}
int main(){
 using namespace thprac::portable;
 using namespace thprac::portable::th07::adapter;
 std::ifstream stream("/fixture.dat",std::ios::binary);assert(stream);
 std::vector<std::uint8_t> dat{std::istreambuf_iterator<char>(stream),std::istreambuf_iterator<char>()};
 assert(std::memcmp(dat.data(),"PBG4",4)==0);
 const auto tableOffset=read32(dat,8);
 auto table=decompress(dat,tableOffset,dat.size(),read32(dat,12));
 struct Entry{std::string name;std::size_t offset,size;};
 std::vector<Entry> entries;
 std::size_t at=0;
 for(unsigned i=0;i<read32(dat,4);++i){
  const auto start=at;while(at<table.size()&&table[at])++at;assert(at<table.size());
  std::string name(reinterpret_cast<const char*>(table.data()+start),at-start);++at;
  entries.push_back({name,read32(table,at),read32(table,at+4)});at+=12;
 }
 std::vector<std::uint8_t> ecls[8];
 for(std::size_t i=0;i<entries.size();++i)for(int stage=0;stage<8;++stage){
  if(entries[i].name=="data/ecldata"+std::to_string(stage+1)+".ecl"||entries[i].name=="ecldata"+std::to_string(stage+1)+".ecl")
   ecls[stage]=decompress(dat,entries[i].offset,i+1<entries.size()?entries[i+1].offset:tableOffset,entries[i].size);
 }
 const unsigned crc[8]={0xb0aec128,0xb5b3ccc6,0x034533b8,0x794f283e,0x49e331a9,0x92758a31,0x22ebe51d,0xbfa7c466};
 for(int i=0;i<8;++i){assert(!ecls[i].empty());assert(Crc32(ecls[i].data(),ecls[i].size())==crc[i]);}
 bool seen[104]{};int cases=0;
 for(const auto& section:generated::th07SectionLabels){
  if(seen[section.id])continue;seen[section.id]=true;
  for(int phase=0;phase<3;++phase)for(int difficulty=0;difficulty<6;++difficulty)
  for(int shot=0;shot<6;++shot)for(int dialogue=0;dialogue<2;++dialogue){
   Session s;s.game=Game::Th07;s.active=true;s.mode=1;s.stage=section.stage;s.section=section.patchId;
   s.phase=phase;s.difficulty=difficulty;s.shotType=shot;s.dialogue=dialogue;
   install(s);auto bytes=ecls[s.stage];
   auto path="data/ecldata"+std::to_string(s.stage+1)+".ecl";
   assert(RuntimeExtension::OnEclLoaded(path.c_str(),bytes.data(),bytes.size()));
   // Compare production dispatch against its mechanically extracted native body.
   auto expected=ecls[s.stage];PatchContext context;ECLHelper helper(expected.data(),expected.size());
   ApplySection(helper,context,s,s.section);assert(helper.Valid());assert(expected==bytes);
   assert(g_Context.warpCount==context.warpCount&&g_Context.warpTime==context.warpTime);
   assert(g_Context.fakeShot==context.fakeShot&&g_Context.nameFix==context.nameFix);
   auto bad=ecls[s.stage];bad[0]^=1;const auto before=bad;
   assert(!RuntimeExtension::OnEclLoaded(path.c_str(),bad.data(),bad.size()));assert(bad==before);
   assert(ThpracPortableTh07FakeShot()==-1);++cases;
  }
 }
 for(int i=1;i<=103;++i)assert(seen[i]);
 // Original mode and cleared sessions must not mutate bytes or inherit context.
 Session original;original.game=Game::Th07;original.active=true;original.mode=0;original.section=32;install(original);
 auto bytes=ecls[0];assert(RuntimeExtension::OnEclLoaded("data/ecldata1.ecl",bytes.data(),bytes.size()));assert(bytes==ecls[0]);
 assert(ThpracPortableTh07FakeShot()==-1);
 assert(!ThpracPortableTh07SetSessionJson(nullptr));
 EM_ASM({Module.eaglerOptions={};});
 assert(RuntimeExtension::OnEclLoaded("data/ecldata1.ecl",bytes.data(),bytes.size()));assert(bytes==ecls[0]);
 std::printf("TH07 production ECL adapter: 103 sections, %d phase/difficulty/shot/dialogue cases PASS\n",cases);
}
