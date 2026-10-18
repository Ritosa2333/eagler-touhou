#include "../th11_web/cpp/game/Archive.hpp"
#include "../th11_web/cpp/game/EclResource.hpp"
#include "../th11_web/cpp/game/PracticePatcher.hpp"
#include "../th11_web/cpp/game/PracticeSections.hpp"
#include "../th11_web/cpp/game/PracticeBgm.hpp"
#include "../th11_web/cpp/game/PracticeSiteChecks.hpp"
#include "../th11_web/cpp/game/StageCompletion.hpp"
#include "../th11_web/cpp/game/GameResources.hpp"
#include <fstream>
#include <iostream>
#include <cassert>
#include <functional>
#include <map>
#include <set>
#include <sstream>
using namespace th11;
static u32 site_crc(const u8* data,size_t size){u32 crc=~0u;for(size_t i=0;i<size;++i){crc^=data[i];for(int bit=0;bit<8;++bit)crc=(crc>>1)^(0xedb88320u&u32(-i32(crc&1)));}return ~crc;}
struct ClearEffects:StageCompletionEffects {bool stage_result_animation()override{return true;}};
int main(int argc,char** argv){
    for(int stage:{6,7})for(bool bonus:{false,true})for(bool replay:{false,true}){GameEconomy economy;economy.lives=3;economy.power=80;economy.point_value=5000000;economy.difficulty=1;ClearRecords records;ClearEffects effects;float rate=1;StageCompletion clear(economy,records,effects,&rate);clear.mode.stage=stage;clear.mode.practice=!replay;clear.mode.replay_practice=replay;clear.mode.control_mode=clear.mode.replay_mode=replay;clear.mode.all_clear_bonus=bonus;assert(clear.complete());assert(clear.state.exit==(replay?StageExit::ReplayEnd:StageExit::Results));assert(records.clears[0][1]==0);assert(economy.score_units==(bonus?(stage==6?14600000:stage==7?21200000:0):(3+stage)*100000));}
    {PracticeBgm bgm;assert(!bgm.filter(PracticeBgmEvent::Play,5,true,true));assert(bgm.filter(PracticeBgmEvent::Play,5,true,true));assert(!bgm.filter(PracticeBgmEvent::Pause,0,false,true));assert(!bgm.filter(PracticeBgmEvent::Resume,0,true,true));assert(bgm.filter(PracticeBgmEvent::Other,0,true,true));assert(!bgm.filter(PracticeBgmEvent::Play,6,true,true));assert(!bgm.filter(PracticeBgmEvent::Stop,0,false,false));}
    assert(argc==2||argc==3);const bool emit_sites=argc==3&&std::string(argv[2])=="--emit-sites";std::ifstream input(argv[1],std::ios::binary);
    std::vector<u8> bytes((std::istreambuf_iterator<char>(input)),{});
    Archive archive;assert(archive.open(bytes.data(),u32(bytes.size())));
    auto source=std::make_unique<GameResources>();assert(source->open_archive(bytes.data(),u32(bytes.size())));
    auto read=[&](const std::string& name){std::vector<u8> out;bool found=false;for(u32 i=0;i<archive.entries.size();++i)if(archive.entries[i].name==name){assert(archive.read(i,out));found=true;break;}assert(found);return out;};
    std::vector<PracticeBuffers> originals;
    for(int stage=1;stage<=7;++stage){
        PracticeBuffers b;std::function<void(const std::string&)> load=[&](const std::string& name){auto data=read(name);EclResource resource;assert(resource.open(data.data(),u32(data.size())));b.ecl.push_back(std::move(data));for(const auto& include:resource.includes)load(include);};
        char name[40];std::snprintf(name,sizeof(name),"stage%02d.ecl",stage);load(name);
        std::snprintf(name,sizeof(name),"stage%02d.std",stage);b.scene=read(name);
        std::snprintf(name,sizeof(name),"stage%02d.anm",stage);b.background=read(name);originals.push_back(std::move(b));
    }
    unsigned passed=0;
    std::map<std::pair<int,int>,std::set<u32>> sites;
    auto collect=[&](int stage,const PracticeBuffers& after){const auto& before=originals[stage];auto compare=[&](int file,const std::vector<u8>& a,const std::vector<u8>& b){assert(a.size()==b.size());for(u32 i=0;i<a.size();++i)if(a[i]!=b[i])sites[{stage,file}].insert(i);};for(u32 file=0;file<before.ecl.size();++file)compare(file,before.ecl[file],after.ecl[file]);compare(-1,before.scene,after.scene);compare(-2,before.background,after.background);};
    for(u32 section=1;section<sizeof(practice_sections)/sizeof(*practice_sections);++section){
        PracticeConfig config;config.section=section;const int appearance=practice_sections[section].appearance;config.stage=appearance>7?3:appearance-1;
        const int phases=section==TH11_ST6_BOSS9?5:section==TH11_ST7_END_S9?3:section==TH11_ST7_END_S10?4:1;
        for(int phase=0;phase<phases;++phase)for(bool dialogue:{false,true}){
            config.phase=phase;config.dlg=dialogue;auto b=originals[config.stage];std::string error;
            if(!patch_practice_buffers(b,config,error)){std::cerr<<"section "<<section<<" phase "<<phase<<": "<<error<<'\n';return 1;}
            // The patcher verifies source tables, protected bytes and injected
            // jump destinations transactionally. Do not reinterpret dead tail
            // operands of shorter THPrac replacements as new instructions.
            auto loaded=std::make_unique<StageResources>();
            if(!source->load_practice_stage(config.stage+1,*loaded,config)){u32 offset;std::memcpy(&offset,b.scene.data()+8,4);std::cerr<<"stage load section "<<section<<": "<<source->error()<<" STD script "<<offset<<'\n';return 3;}
            assert(loaded->timeline.files.size()==b.ecl.size());
            for(size_t i=0;i<b.ecl.size();++i)assert(loaded->timeline.files[i]->bytes==b.ecl[i]);
            assert(loaded->practice_stage_section==b.stage_section);
            collect(config.stage,b);
            ++passed;
        }
    }
    constexpr int chapters[]{4,4,3,7,6,6,7};
    for(int stage=0;stage<7;++stage)for(int chapter=1;chapter<=chapters[stage];++chapter){PracticeConfig p;p.stage=stage;p.section=10000+(stage+1)*100+chapter;auto b=originals[stage];std::string error;assert(patch_practice_buffers(b,p,error));auto loaded=std::make_unique<StageResources>();assert(source->load_practice_stage(stage+1,*loaded,p));collect(stage,b);++passed;}
    PracticeConfig p;p.mode=0;p.section=TH11_ST1_MID1;auto b=originals[0];std::string error;assert(patch_practice_buffers(b,p,error));assert(b.ecl==originals[0].ecl&&b.scene==originals[0].scene&&b.background==originals[0].background);
    p.mode=1;b.ecl[0].resize(64);const auto before=b.ecl;assert(!patch_practice_buffers(b,p,error));assert(b.ecl==before);
    for(const auto& site:practice_site_checks)if(site.stage==0&&site.file>=0){b=originals[0];b.ecl[site.file][site.offset]^=1;const auto changed=b;assert(!patch_practice_buffers(b,p,error));assert(b.ecl==changed.ecl&&b.scene==changed.scene&&b.background==changed.background);break;}
    p.stage=5;p.section=TH11_ST6_BOSS1;b=originals[5];b.background.resize(64);const auto truncated=b;assert(!patch_practice_buffers(b,p,error));assert(b.ecl==truncated.ecl&&b.scene==truncated.scene&&b.background==truncated.background);
    p.stage=0;p.section=TH11_ST6_BOSS1;assert(!source->load_practice_stage(1,*std::make_unique<StageResources>(),p));
    p.reset();assert(!p.mode&&!p.life&&!p.power&&!p.value&&!p.section);
    for(int phase=0;phase<4;++phase){
        PracticeConfig original;original.stage=6;original.section=TH11_ST7_END_S10;original.phase=phase;original.score=9999999990LL;original.marisa_b_formation=4;original.dlg=true;
        double words[PracticeConfig::word_count];original.encode(words);PracticeConfig decoded;assert(decoded.decode(words,PracticeConfig::word_count));
        const auto json=practice_replay_json(decoded);PracticeConfig restored;assert(practice_replay_parse(json.data(),u32(json.size()),restored));double roundtrip[PracticeConfig::word_count];restored.encode(roundtrip);for(u32 i=0;i<PracticeConfig::word_count;++i)assert(words[i]==roundtrip[i]);
        // A preceding touch USER block must not hide the subsequent PRAC.
        std::vector<u8> replay(0x24+16,0);std::memcpy(replay.data(),"t11r",4);replay[4]=4;std::memcpy(replay.data()+0x24,"USER",4);u32 length=16;std::memcpy(replay.data()+0x28,&length,4);std::memcpy(replay.data()+0x2c,"TOUC",4);
        const auto block=practice_replay_block(original);replay.insert(replay.end(),block.begin(),block.end());assert(practice_replay_read(replay.data(),u32(replay.size()),restored));assert(restored.phase==phase&&restored.score==original.score);
        replay.pop_back();assert(!practice_replay_read(replay.data(),u32(replay.size()),restored));words[3]=4;assert(!decoded.decode(words,PracticeConfig::word_count));
    }
    auto vanilla=std::make_unique<StageResources>();assert(source->load_stage(1,*vanilla));assert(vanilla->timeline.files[0]->bytes==originals[0].ecl[0]);
    if(emit_sites){
        std::cout<<"// Generated retail TH11 instruction-site CRCs, not game resources.\n// Source: all 136 upstream sections, phases, dialogs and chapters.\n#pragma once\nnamespace th11 {\nstruct PracticeSiteCheck {int stage,file;u32 offset,length,crc;};\ninline constexpr PracticeSiteCheck practice_site_checks[]{\n";
        for(const auto& entry:sites){const auto [stage,file]=entry.first;const auto& b=originals[stage];const auto& bytes=file==-1?b.scene:file==-2?b.background:b.ecl[file];auto it=entry.second.begin();while(it!=entry.second.end()){const u32 start=*it;u32 end=*it++;while(it!=entry.second.end()&&*it<=end+8){end=*it++;}std::cout<<"{"<<stage<<","<<file<<","<<start<<","<<(end-start+1)<<","<<site_crc(bytes.data()+start,end-start+1)<<"u},\n";}}
        std::cout<<"};\n}\n";
    }else std::cout<<"TH11 source patches: "<<passed<<" section/phase/dialogue/chapter cases, decoded-stage loading, Original-mode isolation and failed-transaction rollback passed\n";
}
