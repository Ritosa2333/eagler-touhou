// Resource proof only; this is not a UI/live/replay/device certification.
#include "../th15_web/cpp/game/Archive.hpp"
#include "../th15_web/cpp/game/EclResource.hpp"
#include "../th15_web/cpp/game/PracticeConfig.hpp"
#include "../th15_web/cpp/game/PracticeSections.hpp"
#include "../th15_web/cpp/game/PracticeSiteChecks.hpp"
#include "../th15_web/cpp/game/MusicLayout.hpp"
#include <fstream>
#include <iostream>
#include <functional>
#include <map>
#include <set>
#include <cassert>
using namespace th15;
u32 site_crc(const u8* bytes,u32 length){u32 crc=~0u;for(u32 i=0;i<length;++i){crc^=bytes[i];for(int bit=0;bit<8;++bit)crc=(crc>>1)^(0xedb88320u&u32(-i32(crc&1)));}return ~crc;}
int main(int argc,char** argv){
 assert(argc==2||argc==3);const bool emit=argc==3&&std::string(argv[2])=="--emit-sites";
 std::ifstream input(argv[1],std::ios::binary);std::vector<u8> bytes((std::istreambuf_iterator<char>(input)),{});
 Archive archive;assert(archive.open(bytes.data(),u32(bytes.size())));
 auto read=[&](const std::string& name){std::vector<u8> data;for(u32 i=0;i<archive.entries.size();++i)if(archive.entries[i].name==name){assert(archive.read(i,data));return data;}std::cerr<<"Missing "<<name<<'\n';std::abort();};
 const auto format=read("thbgm.fmt");MusicLayout music;assert(music.open(format.data(),u32(format.size())));
 const auto* stars_track=music.track(u32(music.original_index("th15_12.wav")));assert(stars_track&&stars_track->filename=="th15_12.wav");
 assert(practice_stars_bgm_byte_offset%stars_track->alignment==0&&practice_stars_bgm_byte_offset<stars_track->loop_end);
 std::array<PracticeBuffers,7> originals;
 for(int stage=0;stage<7;++stage){
  std::function<void(const std::string&)> load=[&](const std::string& name){auto data=read(name);EclResource resource;assert(resource.open(data.data(),u32(data.size())));originals[stage].ecl.push_back(std::move(data));for(const auto& include:resource.includes)load(include);};
  char name[40];std::snprintf(name,sizeof name,"st%02d.ecl",stage+1);load(name);
 }
 std::map<std::pair<int,int>,std::set<u32>> sites;unsigned passed=0;
 auto check=[&](const PracticeConfig& config){
  auto buffers=originals[config.stage];std::string error;std::vector<PracticeWrite> writes;
  if(!patch_practice_buffers(buffers,config,error,&writes)){
   std::cerr<<"section="<<config.section<<" phase="<<config.phase<<" dlg="<<config.dlg<<": "<<error<<'\n';
   if(config.stage==6){EclResource r;const auto& data=originals[6].ecl[3];assert(r.open(data.data(),u32(data.size())));for(const auto& sub:r.subroutines)if(sub.offset<0x800&&sub.offset+sub.size>0x600)std::cerr<<sub.name<<" offset="<<sub.offset<<" size="<<sub.size<<'\n';}
   std::exit(1);
  }
  for(const auto& write:writes)for(u32 i=0;i<write.length;++i)sites[{config.stage,int(write.file)}].insert(write.offset+i);
  EclProgram program;for(const auto& data:originals[config.stage].ecl)assert(program.attach(data.data(),u32(data.size()))>=0);
  PracticePatchEffects effects;
  if(!patch_practice_program(program,config,effects,error)){std::cerr<<"program section="<<config.section<<": "<<error<<'\n';std::exit(1);}
  for(size_t i=0;i<program.files.size();++i)assert(program.files[i]->bytes==buffers.ecl[i]);
  assert(effects.chapter_set==buffers.effects.chapter_set&&effects.chapter_disable==buffers.effects.chapter_disable);
  assert(effects.stars_bgm_sync==buffers.effects.stars_bgm_sync&&effects.extra_boss_chapter_bonus==buffers.effects.extra_boss_chapter_bonus);
  if(config.section==TH15_ST6_STARS)assert(effects.stars_bgm_sync==(config.phase!=1));
  if(config.section==TH15_ST7_END_S4||config.section==TH15_ST7_END_S8||config.section==TH15_ST7_END_S9||config.section==TH15_ST7_END_S10){
   const i32 boss=program.find_index("Boss"),retired=program.find_index("Boss1");assert(boss>=0&&retired>=0);assert(program.subroutine(boss).size>820&&program.subroutine(retired).size==16);
  }
  PracticeConfig decoded;const auto json=practice_replay_json(config);assert(practice_replay_parse(json.data(),json.size(),decoded));assert(practice_replay_json(decoded)==json);
  const auto block=practice_replay_block(config);assert(!block.empty()&&block.size()%4==0);
  std::vector<u8> replay(0x25,0);const u32 signature=0x72353174,packed=1;std::memcpy(replay.data(),&signature,4);replay[4]=3;std::memcpy(replay.data()+0x1c,&packed,4);replay.insert(replay.end(),block.begin(),block.end());assert(practice_replay_read(replay.data(),replay.size(),decoded));assert(practice_replay_json(decoded)==json);
  assert(practice_replay_status(replay.data(),replay.size(),decoded)==1);
  replay.insert(replay.end(),block.begin(),block.end());assert(!practice_replay_read(replay.data(),replay.size(),decoded)&&decoded.mode==0);assert(practice_replay_status(replay.data(),replay.size(),decoded)==-1);
  ++passed;
 };
 for(int section=1;section<int(sizeof(practice_sections)/sizeof(*practice_sections));++section){
  PracticeConfig p;p.section=section;p.stage=practice_sections[section].appearance-1;
  for(int phase=0;phase<practice_phase_count(section,3);++phase)for(bool dlg:{false,true})for(float parameter:{0.0f,1.0f}){p.phase=phase;p.dlg=dlg;p.enhanced_para=parameter;p.doremy_normal_1_phase=parameter;check(p);}
 }
 constexpr int chapters[]{6,6,6,6,10,4,9};
 for(int stage=0;stage<7;++stage)for(int chapter=1;chapter<=chapters[stage];++chapter){PracticeConfig p;p.stage=stage;p.section=10000+(stage+1)*100+chapter;for(int phase=0;phase<practice_phase_count(p.section,3);++phase){p.phase=phase;check(p);}}
 // Menu lifetime and replay gating are different owners: preserve preferences,
 // but never let a pending live selection or a replay cheat cross a fresh run.
 PracticeState owner;owner.enabled=true;owner.active=owner.menu=owner.accepted=owner.assisted=true;owner.cheats=PracticeAutoBomb|PracticeInvincible;owner.misses=2;owner.bombs=3;owner.configured.stage=2;
 assert(owner.cheat(PracticeAutoBomb));owner.replay=true;assert(!owner.cheat(PracticeAutoBomb));owner.clear_run();assert(!owner.active&&!owner.menu&&!owner.accepted&&!owner.assisted&&!owner.replay&&owner.misses==0&&owner.bombs==0&&owner.run.mode==0&&owner.configured.stage==2);
 std::vector<u8> ordinary(0x25,0);const u32 sig=0x72353174,packed_size=1;std::memcpy(ordinary.data(),&sig,4);ordinary[4]=3;std::memcpy(ordinary.data()+0x1c,&packed_size,4);PracticeConfig ordinary_config;assert(practice_replay_status(ordinary.data(),ordinary.size(),ordinary_config)==0);
 // Original mode does not acquire a patch owner or change resources.
 PracticeConfig p;p.mode=0;p.section=TH15_ST1_MID1;auto b=originals[0];std::string error;assert(patch_practice_buffers(b,p,error));assert(b.ecl==originals[0].ecl&&b.effects.chapter_set==-1);
 // A failure must leave *all* buffers and the effect owner untouched.
 p.mode=1;b.ecl[0].resize(64);const auto truncated=b;assert(!patch_practice_buffers(b,p,error));assert(b.ecl==truncated.ecl&&b.effects.chapter_set==truncated.effects.chapter_set);
 p.stage=2;p.section=TH15_ST1_MID1;b=originals[2];const auto wrong_stage=b;assert(!patch_practice_buffers(b,p,error));assert(b.ecl==wrong_stage.ecl);
 p.stage=2;p.section=TH15_ST8_AB_TEST;p.phase=5;assert(!p.valid());p.phase=4;assert(p.valid());
 p.stage=5;p.section=TH15_ST6_BOSS6;p.phase=1;assert(!p.valid(1)&&p.valid(2));
 // Metadata mismatch must not partially publish either bytes or effects.
 p={};p.stage=6;p.section=TH15_ST7_END_S4;EclProgram invalid_layout;
 for(const auto& data:originals[6].ecl)assert(invalid_layout.attach(data.data(),u32(data.size()))>=0);
 for(auto& sub:invalid_layout.files[3]->subroutines)if(sub.name=="Boss1")sub.name="invalid";
 PracticePatchEffects unchanged;unchanged.chapter_set=99;
 assert(!patch_practice_program(invalid_layout,p,unchanged,error)&&unchanged.chapter_set==99);
 for(size_t i=0;i<invalid_layout.files.size();++i)assert(invalid_layout.files[i]->bytes==originals[6].ecl[i]);
 // Complete stock metadata (including purple's writer-only bomb fragments).
 p={};p.section=TH15_ST1_MID1;p.score=9999999990LL;p.life_fragment=3;p.bomb_fragment=4;p.reisen_shield=3;
 const auto json=practice_replay_json(p);PracticeConfig parsed;assert(practice_replay_parse(json.data(),json.size(),parsed));assert(parsed.bomb_fragment==4&&parsed.score==p.score&&parsed.reisen_shield==3);
 for(const std::string bad:{"{\"game\":\"th15\",\"mode\":1,\"mode\":0,\"stage\":0}","{\"game\":\"th11\",\"mode\":1,\"stage\":0}","{\"game\":\"th15\",\"mode\":1,\"stage\":0,\"life\":1.5}","{\"game\":\"th15\",\"mode\":1,\"stage\":0,\"power\":401}","{\"game\":\"th15\",\"mode\":1,\"stage\":0,\"score\":1e999}","{\"game\":\"th15\",\"mode\":1,\"stage\":0,\"dlg\":1}","{\"game\":\"th15\",\"mode\":1,\"stage\":0}tail"})assert(!practice_replay_parse(bad.data(),bad.size(),parsed)&&parsed.mode==0);
#ifndef TH15_PRACTICE_SITE_GENERATION
 for(const auto& site:practice_site_checks){p={};p.stage=site.stage;p.section=10000+(p.stage+1)*100+1;b=originals[p.stage];b.ecl[site.file][site.offset]^=1;const auto mutated=b;assert(!patch_practice_buffers(b,p,error));assert(b.ecl==mutated.ecl);}
#endif
 if(emit){
  std::cout<<"// Generated from all purple TH15 source patch writes on original retail resources.\n// CRCs attest instruction sites; no original payload is embedded.\n#pragma once\n#include <array>\nnamespace th15 {\nstruct PracticeSiteCheck {int stage,file;u32 offset,length,crc;};\ninline constexpr PracticeSiteCheck practice_site_checks[]{\n";
  for(const auto& [key,offsets]:sites){const auto [stage,file]=key;const auto& data=originals[stage].ecl[file];auto it=offsets.begin();while(it!=offsets.end()){const u32 begin=*it;u32 end=*it++;while(it!=offsets.end()&&*it==end+1)end=*it++;std::cout<<"{"<<stage<<","<<file<<","<<begin<<","<<end-begin+1<<","<<site_crc(data.data()+begin,end-begin+1)<<"u},\n";}}
  std::cout<<"};\n}\n";
 }else std::cout<<"PASS purple resource cases="<<passed<<" stages=7 sections=74; source boundaries, parameter ranges, Original isolation and transactional rollback\n";
}
