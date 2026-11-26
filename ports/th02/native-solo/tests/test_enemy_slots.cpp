#include "enemy_slots.hpp"
#include "stage_progress.hpp"
#include <iostream>
unsigned checks=0;void check(bool b){++checks;if(!b)throw std::runtime_error("Original enemy-slot contract failed");}
int main(){try{
    soew::StageScript script;script.enemies.resize(1);script.enemies[0].raw[2]=0xf0;script.enemies[0].raw[3]=0xff;
    for(unsigned page=0;page<2;++page){
        soew::EnemySlots pool;soew::RandomRing random;for(unsigned i=0;i<256;++i)random.bytes[i]=static_cast<std::uint8_t>(i^0x95);
        for(auto &s:pool.slots)s.raw.fill(0x5a);for(auto &s:pool.slots)s.raw[0x0e]=0;
        for(unsigned i=0;i<25;++i){auto index=pool.spawn(script,0,i+1,page,random);check(index&&*index==i);auto &s=pool.slots[i];check(s.raw[0x0e]==1);check(s.word(page*4)==16+(i+1)*8);check(s.word(page*4+2)==0xfff0);check(s.raw[0x0f]==(i^0x95));check(s.word(0x08)==0&&s.word(0x12)==0&&s.word(0x0a)==0);check(s.word(0x14)==1);check(s.word(0x18)==(16+(i+1)*8<208?1:0));check(s.word((page^1)*4)==0x5a5a);check(s.raw[0x11]==0x5a&&s.word(0x1c)==0x5a5a&&s.raw[0x1f]==0x5a);}
        check(random.cursor==25&&pool.high_water==24);check(!pool.spawn(script,0,48,page,random));check(random.cursor==25);
        pool.slots[0].raw[0x0e]=2;check(!pool.spawn(script,0,48,page,random));check(random.cursor==25);pool.slots[3].raw[0x0e]=0;auto reused=pool.spawn(script,0,48,page,random);check(reused&&*reused==3);check(pool.slots[3].word(page*4)==400);check(random.cursor==26);
        bool rejected=false;try{pool.spawn(script,0,0,page,random);}catch(const std::runtime_error&){rejected=true;}check(rejected&&random.cursor==26);
    }
    {soew::EnemySlots exact;soew::RandomRing rng;auto slot=exact.spawn(script,0,3,0,rng);check(slot&&exact.high_water==0);exact.begin_frame(1);check(exact.high_water==1&&exact.slots[0].word(4)==40&&exact.slots[0].word(6)==0xfff0);exact.slots[0].raw[0xe]=2;exact.begin_frame(0);check(exact.high_water==0&&exact.slots[0].raw[0xe]==0);}
    // Suppressed and pool-full rows advance their timeline without consuming RNG.
    script.formations.resize(2);script.declared_formations=2;for(auto &r:script.formations){r.trigger=1;r.selectors.fill(0);}
    soew::EnemySlots pool;soew::RandomRing rng;soew::FormationDispatch dispatch;
    dispatch.dispatch(script,1,true,[&](auto i){return bool(pool.spawn(script,i.template_id,i.lane,0,rng));});check(rng.cursor==0&&dispatch.suppressed==48&&dispatch.cursor==1);
    dispatch.dispatch(script,1,false,[&](auto i){return bool(pool.spawn(script,i.template_id,i.lane,0,rng));});check(rng.cursor==25&&dispatch.accepted==25&&dispatch.capacity_rejected==23&&dispatch.cursor==2);
    std::cout<<"enemy_slot_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
