#include "enemy_path.hpp"
#include "enemy_path_runtime.hpp"
#include "stage_scene.hpp"
#include <iostream>
#include <set>
unsigned checks=0;void check(bool b){++checks;if(!b)throw std::runtime_error("Enemy path contract failed");}
soew::EnemySlots::Slot fresh(){soew::EnemySlots::Slot s;s.raw[0xe]=1;return s;}
int wmain(int argc,wchar_t**argv){try{
    if(argc!=2)throw std::runtime_error("Usage: enemy-path-tests <original-assets>");
    soew::RandomRing rng;for(unsigned i=0;i<256;++i)rng.bytes[i]=static_cast<std::uint8_t>(i);soew::EnemyPathContext context;
    check(soew::enemy_atan2(0,0)==0);check(soew::enemy_atan2(0,1)==0);check(soew::enemy_atan2(1,0)==64);check(soew::enemy_atan2(0,-1)==128);check(soew::enemy_atan2(-1,0)==192);
    for(unsigned a=0;a<256;++a){check(soew::enemy_sin(a)==-soew::enemy_sin(a+128));check(soew::enemy_vector(a,0)==std::array<int,2>{0,0});}
    {auto s=fresh();const std::array<std::uint8_t,5> path={1,2,255,2,0x10};auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Timed&&s.word(8)==0&&s.word(0)==65535&&s.word(2)==2);r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Timed&&s.word(8)==4&&s.word(0)==65534&&s.word(2)==4);r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Ended&&s.raw[14]==2);}
    {auto s=fresh();const std::array<std::uint8_t,5> path={0xa2,7,0,0,0x10};auto r=soew::run_enemy_path(s,path,rng,context);check(r.instructions==2&&r.status==r.Timed&&s.word(8)==4&&s.word(0x16)==7);}
    {auto s=fresh();const std::array<std::uint8_t,3> path={0xa0,0,0x10};auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Blocked&&r.instructions==4096&&s.word(8)==0);}
    {auto s=fresh();const std::array<std::uint8_t,4> path={0xa1,0,2,0x10};auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Ended&&r.instructions==4&&s.word(0x16)==3);}
    {auto s=fresh();const std::array<std::uint8_t,4> path={0xa7,0,2,0x10};auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Ended&&r.instructions==4&&s.word(0x1a)==0);}
    {auto s=fresh();const std::array<std::uint8_t,1> path={0xfe};auto before=s.raw;auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Blocked&&s.raw==before&&r.opcode==254);}
    for(auto op:{0x11u,0x20u,0x21u,0x22u,0x24u,0x26u,0x27u,0xa8u,0xafu}){auto s=fresh();std::vector<std::uint8_t> path(soew::enemy_opcode_width(op));path[0]=static_cast<std::uint8_t>(op);const auto before=s.raw;auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Blocked&&r.reason=="effect consumer not connected"&&s.raw==before);}
    {unsigned legal=0;for(unsigned op=0;op<256;++op)legal+=soew::enemy_opcode_width(op)!=0;check(legal==37);}
    {auto s=fresh();s.word(8,256);const std::array<std::uint8_t,4> path={1,1,255,1};auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Timed&&s.word(8)==260&&s.word(0)==65535&&s.word(2)==1);}
    {auto s=fresh();const std::array<std::uint8_t,5> path={5,0,0,0,0x10};auto before=s.raw;auto cursor=rng.cursor;auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Blocked&&s.raw==before&&rng.cursor==cursor);}
    {auto s=fresh();const std::array<std::uint8_t,6> path={7,1,64,0,2,0x10};auto before=s.raw;auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Blocked&&s.raw==before);}
    {auto s=fresh();s.word(0x18,1);const std::array<std::uint8_t,6> path={7,1,64,6,2,0x10};auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Timed&&s.word(0x16)==65535&&s.word(0)==0&&s.word(2)==2&&s.word(8)==5);}
    {auto s=fresh();const std::array<std::uint8_t,7> path={11,1,0,64,2,2,0x10};auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Timed&&s.word(0x16)==65534&&s.word(0)==0&&s.word(2)==2&&s.word(8)==6);}
    {auto s=fresh();s.word(0,232);s.word(2,200);const std::array<std::uint8_t,4> path={9,1,3,0x10};auto r=soew::run_enemy_path(s,path,rng,{0,0,208,184});check(r.status==r.Timed&&s.word(0)==235&&s.word(2)==200);}
    {auto s=fresh();s.word(0,100);s.word(2,10);s.word(0x18,0);const std::array<std::uint8_t,5> path={8,1,3,254,0x10};auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Timed&&s.word(0)==97&&s.word(2)==8);}
    {auto s=fresh();s.raw[0x1c]=255;s.raw[0x1d]=3;const std::array<std::uint8_t,3> path={10,0,0x10};auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Timed&&s.word(0)==65535&&s.word(2)==3&&s.word(8)==2);}
    {auto s=fresh();const std::array<std::uint8_t,3> path={2,1,0x10};auto r=soew::run_enemy_path(s,path,rng,{0,2,208,336});check(r.status==r.Timed&&s.word(2)==2);}
    // All signed-byte velocities use the original signed extension and 16-bit
    // coordinate wrap, on BOTH VRAM pages; no float/subpixel interpolation.
    for(unsigned page=0;page<2;++page)for(unsigned dx=0;dx<256;++dx){auto s=fresh();s.word(page*4,65500);s.word(page*4+2,32760);std::array<std::uint8_t,5> path={1,1,static_cast<std::uint8_t>(dx),static_cast<std::uint8_t>(255-dx),0x10};auto r=soew::run_enemy_path(s,path,rng,{page,0,0,0});const int expected_dx=dx<128?int(dx):int(dx)-256,dy=int(255-dx)<128?int(255-dx):int(255-dx)-256;check(r.status==r.Timed&&s.word(page*4)==static_cast<std::uint16_t>(65500+expected_dx)&&s.word(page*4+2)==static_cast<std::uint16_t>(32760+dy));}
    {auto s=fresh();const std::array<std::uint8_t,10> overlap={0xa0,5,0xa2,99,0xa9,0,2,0xa9,0x10,0};auto r=soew::run_enemy_path(s,overlap,rng,context);check(r.status==r.Timed&&s.word(8)==5);r=soew::run_enemy_path(s,overlap,rng,context);check(r.status==r.Timed&&s.word(8)==7);r=soew::run_enemy_path(s,overlap,rng,context);check(r.status==r.Ended&&s.raw[0x1e]==1);}
    {auto s=fresh();const std::array<std::uint8_t,4> path={1,1,0,0};auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Timed&&s.word(8)==4);auto before=s.raw;r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Blocked&&r.reason=="path byte unavailable (no synthetic padding)"&&s.raw==before);}
    {auto s=fresh();s.word(0xa,65535);const std::array<std::uint8_t,3> path={0,1,0x10};auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Timed&&s.word(8)==0&&s.word(0xa)==0);}
    {auto s=fresh();s.word(0x16,65535);const std::array<std::uint8_t,4> path={0xa1,3,0,0x10};auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Ended&&s.word(0x16)==0);}
    {auto s=fresh();const std::array<std::uint8_t,5> path={0xaa,4,10,1,0x10};soew::RandomRing fixed;fixed.bytes[0]=64;auto r=soew::run_enemy_path(s,path,fixed,context);check(r.status==r.Timed&&s.word(0)==0&&s.word(2)==4&&fixed.cursor==1);}
    for(auto op:{0xa3u,0xa4u,0xa5u,0xa6u,0xa9u,0xabu,0xacu,0xadu,0xaeu,0x25u,4u,5u,6u}){auto s=fresh();s.raw[0x22]=1;s.raw[0x23]=1;std::vector<std::uint8_t> path(soew::enemy_opcode_width(op)+2);path[0]=static_cast<std::uint8_t>(op);if(path.size()>3)path[1]=3;if(path.size()>4)path[2]=2;if(path.size()>5)path[3]=5;path[path.size()-2]=0;path[path.size()-1]=1;soew::RandomRing controlled;controlled.bytes[1]=1;controlled.bytes[2]=2;auto r=soew::run_enemy_path(s,path,controlled,context);check(r.status==r.Timed&&r.instructions==2);
        if(op==0xa3||op==0xa5)check(s.word(0x14)==0);if(op==0xa4)check(s.word(0x14)==1);if(op==0xa6)check(s.word(0x14)==2&&s.word(0x12)==3);if(op==0xa5)check(s.word(0x12)==3);if(op==0xa9)check(s.raw[0x1e]==1);if(op==0xab||op==0xac)check(s.raw[0x22]==(op==0xab));if(op==0xad||op==0xae)check(s.raw[0x23]==(op==0xad));if(op==0x25)check(s.raw[0x24]==3&&s.raw[0x25]==2);if(op==4)check(s.word(0)==24&&s.word(2)==16);if(op==5)check(s.word(0)==24&&s.word(2)==40&&controlled.cursor==1);if(op==6)check(s.word(0)==40&&s.word(2)==24&&controlled.cursor==1);
    }
    unsigned all_paths=0,all_instructions=0,operand_entry_paths=0,fallthrough_paths=0;
    for(unsigned stage=0;stage<6;++stage){auto data=soew::StageScene::read(std::filesystem::path(argv[1])/("STAGE"+std::to_string(stage)+".DT1"));auto script=soew::StageScript::decode(data,true);unsigned instructions=0;
        for(const auto &path:script.paths){++all_paths;std::set<unsigned> boundaries;unsigned pc=0,last=0;while(pc<path.bytes.size()){boundaries.insert(pc);last=path.bytes[pc];auto width=soew::enemy_opcode_width(last);check(width&&width<=path.bytes.size()-pc);pc+=width;++instructions;}check(pc==path.bytes.size());bool alternate=false;for(auto start:boundaries){const auto op=path.bytes[start];if(op==0xa0||op==0xa1||op==0xa7)alternate|=!boundaries.contains(path.bytes[start+1]);}operand_entry_paths+=alternate;fallthrough_paths+=last!=0x10&&last!=0x11&&last!=0xa0;}
        all_instructions+=instructions;std::cout<<"STAGE"<<stage<<" inline_paths="<<script.paths.size()<<" linear_instructions="<<instructions<<'\n';
        if(stage==0){const auto &path=script.paths[0].bytes;auto s=fresh();s.word(0,40);s.word(0x18,1);for(int i=0;i<21;++i){auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Timed);}check(s.word(8)==4&&s.word(2)==42);auto r=soew::run_enemy_path(s,path,rng,context);check(r.status==r.Timed&&s.word(0)==40&&s.word(2)==44&&s.word(0x16)==65535);for(int i=0;i<500;++i){auto result=soew::run_enemy_path(s,path,rng,context);check(result.status==result.Timed);} }
    }
    // Headless harness connects the original pool and page copy to path logic,
    // but must stop honestly at missing sound/bullet/tile consumers.
    for(unsigned stage=0;stage<6;++stage){auto data=soew::StageScene::read(std::filesystem::path(argv[1])/("STAGE"+std::to_string(stage)+".DT1"));auto script=soew::StageScript::decode(data,true);soew::EnemyPathRuntime first(stage,1234),second(stage,1234);for(unsigned i=0;i<100000&&!first.blocked&&!first.scroll.done;++i){first.tick(script);second.tick(script);check(first.ticks==second.ticks&&first.spawns==second.spawns&&first.random.cursor==second.random.cursor);for(unsigned n=0;n<25;++n)check(first.enemies.slots[n].raw==second.enemies.slots[n].raw);}
        check(first.blocked.has_value());check(first.spawns>0);const auto saved=first.enemies.slots;const auto time=first.ticks;first.tick(script);check(first.ticks==time);for(unsigned n=0;n<25;++n)check(first.enemies.slots[n].raw==saved[n].raw);
        std::cout<<"runtime_stage="<<stage<<" ticks="<<first.ticks<<" spawns="<<first.spawns<<" stop_type="<<first.blocked->type<<" stop_pc="<<first.blocked->result.pc<<" stop_opcode="<<first.blocked->result.opcode<<" reason="<<first.blocked->result.reason<<'\n';
    }
    {soew::StageScript script;script.map={0,0,0};script.enemies.resize(1);script.paths.resize(1);script.paths[0].bytes={0,255};script.enemies[0].raw[10]=2;script.enemies[0].raw[12]=1;
        soew::EnemyPathRuntime anim(0,0);auto &s=anim.enemies.slots[0];s.raw[0xe]=1;s.word(4,40);s.word(6,40);s.word(0x14,1);anim.tick(script);check(!anim.blocked&&s.word(0x12)==1);anim.tick(script);check(!anim.blocked&&s.word(0x12)==0);
        script.enemies[0].raw[12]=0;anim.tick(script);check(anim.blocked&&anim.blocked->result.reason=="original animation divide by zero");
    }
    check(all_paths==76&&all_instructions==706);check(operand_entry_paths==5);check(fallthrough_paths==19);
    std::cout<<"paths="<<all_paths<<" linear_instructions="<<all_instructions<<" operand_entry_paths="<<operand_entry_paths<<" possible_rawend_fallthrough="<<fallthrough_paths<<" enemy_path_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
