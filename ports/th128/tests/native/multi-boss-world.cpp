#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <cstdio>
#include <optional>
#include <vector>

using namespace ecl_fixture;

static std::vector<std::uint32_t> phaseWords(int slot,int threshold,int limit,const std::string& target){
    auto words=std::vector<std::uint32_t>{std::uint32_t(slot),std::uint32_t(threshold),std::uint32_t(limit)};
    const auto name=stringWords(target);words.insert(words.end(),name.begin(),name.end());return words;
}
static std::vector<std::uint32_t> spawnWords(const std::string& subroutine,float x,float y,int life){
    auto words=stringWords(subroutine);
    words.insert(words.end(),{floatBits(x),floatBits(y),std::uint32_t(life),0,0});return words;
}
static std::vector<std::uint32_t> asyncWords(const std::string& subroutine,int id){
    auto words=stringWords(subroutine);words.push_back(std::uint32_t(id));return words;
}
static th128::EnemyDamageSample damageSample(std::int32_t damage){
    th128::EnemyDamageSample sample;
    sample.query.continuation=th128::shotenemy::QueryContinuation::Complete;
    sample.query.completedDamage=damage;sample.spellModeFlags=0;return sample;
}
static std::optional<th128::EnemySnapshot> entity(const th128::StageWorld& world,std::uint32_t id){
    for(const auto& candidate:world.entities())if(candidate.id==id)return candidate;
    return std::nullopt;
}

int main(){
    try{
    {
        const auto data=program({{"main",join({instruction(257,spawnWords("bossB",0,100,40),0,0,255,6),
                instruction(257,spawnWords("bossC",0,100,40),0,0,255,6),
                instruction(257,spawnWords("bossA",0,100,40),0,0,255,6),instruction(0,{},100)})},
            {"bossB",join({instruction(412,{1}),instruction(411,{10000}),
                instruction(414,phaseWords(0,0,0,"escapeB"),0,0,255,4),
                instruction(414,phaseWords(1,0,0,"bCard"),0,0,255,4),
                instruction(16,asyncWords("bThread",7),0,0,255,2),instruction(83,{500}),instruction(0,{},100)})},
            {"bossC",join({instruction(412,{2}),instruction(411,{12000}),
                instruction(414,phaseWords(0,0,0,"escapeC"),0,0,255,4),
                instruction(414,phaseWords(1,0,0,"cCard"),0,0,255,4),instruction(83,{500}),instruction(0,{},100)})},
            {"bCard",join({instruction(411,{11001}),instruction(83,{500}),instruction(0,{},100)})},
            {"cCard",join({instruction(411,{13001}),instruction(83,{500}),instruction(0,{},100)})},
            {"escapeB",join({instruction(411,{9999}),instruction(1)})},
            {"escapeC",join({instruction(411,{9999}),instruction(1)})},
            {"bThread",join({instruction(83,{2}),instruction(411,{4444}),instruction(1)})},
            {"bossA",join({instruction(412,{0}),instruction(411,{18000}),
                instruction(414,phaseWords(0,2000,3600,"aCard"),0,0,255,4),
                instruction(414,phaseWords(1,0,0,"aTrap"),0,0,255,4),
                instruction(613,{1}),instruction(83,{500}),instruction(0,{},100)})},
            {"aCard",join({instruction(411,{3000}),instruction(83,{500}),instruction(0,{},100)})},
            {"aTrap",join({instruction(411,{9999}),instruction(83,{500}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        require(world.load("sync.ecl",data.data(),data.size(),reason)&&world.start("sync.ecl",1,0),reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        const auto states=world.entities();
        require(states.size()==4,"three fairies are independent world entities");
        require(states[1].entry=="bCard"&&states[1].life==10000,"613 restarts the second owner at its own slot target");
        require(states[2].entry=="cCard"&&states[2].life==12000,"613 restarts the third owner at its own slot target");
        require(states[3].entry=="bossA"&&states[3].life==18000,"613 skips the caller and never starts its own target");
        require(states[3].phases[1].healthTarget=="aTrap","613 preserves the caller's phase records");
        require(world.bossSlots().owners[0]==states[3].id&&world.bossSlots().owners[1]==states[1].id
            &&world.bossSlots().owners[2]==states[2].id,"original owner slots 0/1/2 follow the three fairies");
        require(world.tick()&&world.tick(),world.error());
        const auto after=world.entities();
        require(after[1].life==11001&&after[2].life==13001,"restarted targets execute on their own next update");
        require(after[3].life==18000,"the caller's restrained waiting ECL keeps its life");
        require(after[1].life!=4444,"613 clears the restarted owner's old asynchronous thread");
        require(world.tick()&&world.tick(),world.error());
        require(world.entities()[1].life==11001,"the discarded async thread never resumes");
    }
    {
        const auto data=program({{"main",join({instruction(257,spawnWords("bossB",0,100,40),0,0,255,6),
                instruction(257,spawnWords("bossA",0,100,40),0,0,255,6),instruction(0,{},100)})},
            {"bossB",join({instruction(412,{1}),instruction(411,{10000}),
                instruction(414,phaseWords(1,0,0,"emptyCard"),0,0,255,4),instruction(83,{500}),instruction(0,{},100)})},
            {"emptyCard",{}},
            {"bossA",join({instruction(412,{0}),instruction(411,{18000}),instruction(613,{1}),instruction(83,{500}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        require(world.load("missing.ecl",data.data(),data.size(),reason)&&world.start("missing.ecl",1,0),reason);
        require(!world.enableLocalEnemyPasses(reason),"613 must reject a target with no authored instructions");
        require(world.faulted()&&world.error().find("ECL613 cross-Boss target is missing")!=std::string::npos,world.error());
        require(world.entities()[1].entry=="bossB"&&world.entities()[1].life==10000,
            "rejected target does not restart the other boss");
    }
    {
        const auto data=program({{"main",join({instruction(257,spawnWords("bossB",0,100,40),0,0,255,6),
                instruction(257,spawnWords("bossA",0,100,40),0,0,255,6),instruction(0,{},100)})},
            {"bossB",join({instruction(412,{1}),instruction(411,{10000}),
                instruction(414,phaseWords(1,0,0,"bCard"),0,0,255,4),instruction(83,{500}),instruction(0,{},100)})},
            {"bossA",join({instruction(412,{0}),instruction(411,{18000}),instruction(613,{8}),instruction(83,{500}),instruction(0,{},100)})},
            {"bCard",join({instruction(411,{11001}),instruction(83,{500}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        require(world.load("slot.ecl",data.data(),data.size(),reason)&&world.start("slot.ecl",1,0),reason);
        require(!world.enableLocalEnemyPasses(reason),"613 rejects an out-of-range phase slot");
        require(world.faulted()&&world.error().find("opcode=613")!=std::string::npos,world.error());
    }
    {
        const auto data=program({{"main",join({instruction(257,spawnWords("bossB",0,100,40),0,0,255,6),
                instruction(257,spawnWords("bossA",0,100,40),0,0,255,6),instruction(0,{},100)})},
            {"bossB",join({instruction(412,{1}),instruction(411,{10000}),
                instruction(414,phaseWords(1,0,0,"bCard"),0,0,255,4),instruction(83,{500}),instruction(0,{},100)})},
            {"bossA",join({instruction(412,{0}),instruction(411,{18000}),instruction(412,{2}),instruction(613,{1}),
                instruction(83,{500}),instruction(0,{},100)})},
            {"bCard",join({instruction(411,{11001}),instruction(83,{500}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        require(world.load("empty.ecl",data.data(),data.size(),reason)&&world.start("empty.ecl",1,0),reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        const auto states=world.entities();
        require(world.bossSlots().owners[0]==states[2].id&&world.bossSlots().owners[1]==states[1].id
            &&world.bossSlots().owners[2]==states[2].id,"duplicate registration keeps both stale and current slots");
        require(states[1].entry=="bCard"&&states[2].entry=="bossA","613 skips the empty owner and its own stale id");
        require(world.tick()&&world.tick()&&!world.faulted(),"empty slot does not abort the sync");
        require(world.entities()[1].life==11001,"the valid owner still restarts with an empty third slot");
    }
    {
        const auto data=program({{"main",join({instruction(257,spawnWords("bossB",0,100,40),0,0,255,6),
                instruction(257,spawnWords("bossC",0,100,40),0,0,255,6),
                instruction(257,spawnWords("bossA",0,100,40),0,0,255,6),instruction(420),instruction(402,{0x1000}),
                instruction(0,{},100)})},
            {"bossA",join({instruction(412,{0}),instruction(411,{18000}),
                instruction(427,{0,floatBits(6000),0xffffa080u}),instruction(83,{2}),instruction(402,{0x4000000}),instruction(1)})},
            {"bossB",join({instruction(412,{1}),instruction(411,{10000}),instruction(83,{4}),instruction(402,{0x4000000}),instruction(1)})},
            {"bossC",join({instruction(412,{2}),instruction(411,{10000}),instruction(83,{6}),instruction(402,{0x4000000}),instruction(1)})}});
        th128::StageWorld world;std::string reason;
        require(world.load("retire.ecl",data.data(),data.size(),reason)&&world.start("retire.ecl",1,0),reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        const auto states=world.entities();
        require(world.bossGauges().values[0][0].ratio==float(6000.0/18000.0),"the first fairy owns its gauge group");
        bool firstRetired=false;
        for(int tick=0;tick<24&&!(world.entities()[0].flags&0x1000)&&!world.faulted();++tick){
            require(world.tick(),world.error());
            if(!firstRetired&&world.bossSlots().owners[0]==0){
                firstRetired=true;
                require(world.bossSlots().owners[1]==states[1].id&&world.bossSlots().owners[2]==states[2].id,
                    "retiring one fairy preserves the other two owners");
                require(world.bossGauges().values[0][0].ratio==0,"retirement clears only the released HUD group");
                require(!(world.entities()[0].flags&0x1000),"420 stays held while the other fairies remain registered");
            }
        }
        require(firstRetired&&!world.faulted(),world.error());
        require((world.entities()[0].flags&0x1000)!=0,"420 resumes after every registered owner retires");
        require(world.bossSlots().owners[0]==0&&world.bossSlots().owners[1]==0&&world.bossSlots().owners[2]==0,
            "each fairy retirement releases its own real owner slot");
    }
    {
        const auto data=program({{"main",join({instruction(257,spawnWords("bossC",0,100,40),0,0,255,6),
                instruction(257,spawnWords("bossA",0,100,40),0,0,255,6),instruction(0,{},100)})},
            {"bossC",join({instruction(412,{2}),instruction(411,{100}),instruction(83,{3}),instruction(402,{0x4000000}),instruction(1)})},
            {"bossA",join({instruction(412,{0}),instruction(411,{50}),instruction(83,{1}),instruction(412,{2}),instruction(83,{6}),instruction(1)})}});
        th128::StageWorld world;std::string reason;
        require(world.load("stale.ecl",data.data(),data.size(),reason)&&world.start("stale.ecl",1,0),reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        const auto states=world.entities();
        require(world.bossSlots().owners[2]==states[1].id,"the first fairy owns the shared slot initially");
        for(int tick=0;tick<6&&world.bossSlots().owners[2]!=states[2].id&&!world.faulted();++tick)require(world.tick(),world.error());
        require(world.bossSlots().owners[2]==states[2].id,"the later fairy re-registers the same owner slot");
        for(int tick=0;tick<12&&(world.entities()[1].flags&0x1000000u)&&!world.faulted();++tick)require(world.tick(),world.error());
        const auto after=world.entities();
        require(world.bossSlots().owners[2]==after[2].id&&after[2].flags&0x1000000u,
            "a stale retained slot cannot clear the later owner's real registration");
        require(!(after[1].flags&0x1000000u),"the retired fairy loses only its own registration flag");
        require(world.bossSlots().owners[0]==after[2].id,"the stale first slot is untouched by the unrelated owner");
    }
    {
        const auto data=program({{"main",join({instruction(257,spawnWords("zeroBoss",0,100,40),0,0,255,6),
                instruction(0,{},100)})},
            {"zeroBoss",join({instruction(412,{1}),instruction(411,{2000}),
                instruction(414,phaseWords(0,0,0,"afterZero"),0,0,255,4),instruction(83,{1000}),instruction(0,{},100)})},
            {"afterZero",join({instruction(83,{1}),instruction(411,{15000}),instruction(83,{1000}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;bool damaged=false;
        require(world.load("zero.ecl",data.data(),data.size(),reason)&&world.start("zero.ecl",1,0),reason);
        require(world.setEnemyDamageHost([&](const th128::EnemySnapshot& state,th128::enemy::Bounds,std::string&){
            if(!damaged&&state.protection==0&&state.life>0){damaged=true;return std::optional<th128::EnemyDamageSample>{damageSample(2500)};}
            return std::optional<th128::EnemyDamageSample>{damageSample(0)};},reason),reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        for(int tick=0;tick<10&&world.entities()[1].life>0&&!world.faulted();++tick)require(world.tick(),world.error());
        require(!world.faulted(),world.error());
        require(world.entities()[1].life==0&&(world.entities()[1].flags&0x1000000u),
            "registered boss HP0 after an authored phase switch is not an unimplemented death");
        require(world.phaseTransitions().size()==1&&world.phaseTransitions()[0].target=="afterZero",
            "HP0 executes the authored target only once");
        require(world.entities()[1].entry=="afterZero","the registered boss entry follows the authored target");
        require(world.tick(),world.error());
        require(world.entities()[1].life==15000,"the restarted fairies write their real life on the next tick");
        require(world.phaseTransitions().size()==1,"the immediate continuation is not queried twice in the same tick");
    }
    {
        const auto data=program({{"main",join({instruction(257,spawnWords("deadBoss",0,100,40),0,0,255,6),
                instruction(0,{},100)})},
            {"deadBoss",join({instruction(412,{1}),instruction(411,{2000}),instruction(83,{1000}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;bool damaged=false;unsigned deaths=0;
        require(world.load("nodeath.ecl",data.data(),data.size(),reason)&&world.start("nodeath.ecl",1,0),reason);
        require(world.setEnemyDamageHost([&](const th128::EnemySnapshot& state,th128::enemy::Bounds,std::string&){
            if(!damaged&&state.protection==0){damaged=true;return std::optional<th128::EnemyDamageSample>{damageSample(2500)};}
            return std::optional<th128::EnemyDamageSample>{damageSample(0)};},reason),reason);
        require(world.setEnemyDeathHost([&](const th128::EnemySnapshot&,std::string&){++deaths;return true;},reason),reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        for(int tick=0;tick<10&&!world.faulted();++tick)if(!world.tick())break;
        require(world.faulted()&&world.error().find("boss death/phase")!=std::string::npos,
            "registered HP0 without an authored continuation still fails explicitly");
        require(deaths==0,"the explicit boundary never emits ordinary death effects");
    }
    {
        const auto data=program({{"main",join({instruction(257,spawnWords("ordinary",0,100,40),0,0,255,6),
                instruction(0,{},100)})},
            {"ordinary",join({instruction(411,{2000}),instruction(414,phaseWords(0,500,0,"afterOrdinary"),0,0,255,4),
                instruction(83,{1000}),instruction(0,{},100)})},
            {"afterOrdinary",join({instruction(411,{7000}),instruction(83,{1000}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;bool damaged=false;
        require(world.load("ordinary.ecl",data.data(),data.size(),reason)&&world.start("ordinary.ecl",1,0),reason);
        require(world.setEnemyDamageHost([&](const th128::EnemySnapshot& state,th128::enemy::Bounds,std::string&){
            if(!damaged&&state.protection==0){damaged=true;return std::optional<th128::EnemyDamageSample>{damageSample(1600)};}
            return std::optional<th128::EnemyDamageSample>{damageSample(0)};},reason),reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        for(int tick=0;tick<10&&world.entities()[1].life!=7000&&!world.faulted();++tick)require(world.tick(),world.error());
        require(!world.faulted()&&world.entities()[1].life==7000,"ordinary authored HP phase still executes its target");
        require(world.phaseTransitions().size()==1,"ordinary phase switching stays a single transition");
    }
    {
        const auto data=program({{"main",join({instruction(257,spawnWords("dying",0,100,10),0,0,255,6),
                instruction(0,{},100)})},
            {"dying",join({instruction(411,{10}),instruction(83,{1000}),instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;bool damaged=false;unsigned deaths=0;
        require(world.load("dying.ecl",data.data(),data.size(),reason)&&world.start("dying.ecl",1,0),reason);
        require(world.setEnemyDamageHost([&](const th128::EnemySnapshot& state,th128::enemy::Bounds,std::string&){
            if(!damaged&&state.protection==0){damaged=true;return std::optional<th128::EnemyDamageSample>{damageSample(20)};}
            return std::optional<th128::EnemyDamageSample>{damageSample(0)};},reason),reason);
        require(world.setEnemyDeathHost([&](const th128::EnemySnapshot&,std::string&){++deaths;return true;},reason),reason);
        require(world.enableLocalEnemyPasses(reason),reason);
        for(int tick=0;tick<10&&deaths==0&&!world.faulted();++tick)require(world.tick(),world.error());
        require(!world.faulted()&&deaths==1,"ordinary enemy death keeps its existing host/retirement path");
        require(!world.entities()[1].active,"ordinary death unlinks the real node once");
    }
    {
        const auto data=program({{"main",join({instruction(0,{},100)})}});
        th128::StageWorld world;std::string reason;
        require(world.load("carry.ecl",data.data(),data.size(),reason)&&world.start("carry.ecl",1,0x1234),reason);
        const auto fresh=world.gameRandomState();
        require(fresh.seed==0x1234&&fresh.calls==0,"fresh world exposes its actual seeded RNG state");
        th128::Random carried{0x5a5a,7};
        require(world.restoreGameRandomState(carried,reason),reason);
        require(world.gameRandomState().seed==0x5a5a&&world.gameRandomState().calls==7,
            "restore installs the exact inherited seed and call count");
        require(world.restoreGameRandomState(carried,reason),"the configuration window accepts repeated restore");
        th128::Random reference{0x5a5a,7};
        require(world.effectGameRandom32()==reference.next32(),"restored RNG resumes the original sequence without extra draws");
        require(world.gameRandomState().calls==9,"restore does not consume or recompute draw counts");
        const auto live=world.gameRandomState();
        require(world.tick(),world.error());
        require(!world.restoreGameRandomState(th128::Random{1,2},reason)&&!reason.empty(),
            "restore after the first tick is rejected");
        require(world.gameRandomState().seed==live.seed&&world.gameRandomState().calls==live.calls,
            "rejected post-tick restore leaves the live random state untouched");
        th128::StageWorld faulted;std::string faultReason;
        require(faulted.load("carry.ecl",data.data(),data.size(),faultReason)&&faulted.start("carry.ecl",1,0xbeef),faultReason);
        const auto beforeFault=faulted.gameRandomState();
        faulted.reportRuntimeFault("synthetic fault");
        require(faulted.faulted(),"fault injection must be reported");
        require(!faulted.restoreGameRandomState(th128::Random{3,4},reason),"a faulted world cannot accept inherited RNG state");
        require(faulted.gameRandomState().seed==beforeFault.seed&&faulted.gameRandomState().calls==beforeFault.calls,
            "fault rejection preserves the old random state");
        th128::StageWorld unstarted;std::string unstartedReason;
        require(!unstarted.restoreGameRandomState(th128::Random{9,9},unstartedReason)&&!unstartedReason.empty(),
            "an unstarted world cannot install inherited RNG state");
        require(unstarted.gameRandomState().seed==0&&unstarted.gameRandomState().calls==0,
            "rejected unstarted restore keeps the default random state");
    }
    std::puts("PASS three-fairy cross-Boss sync, HP0 continuation and owner release");
    return 0;
    }catch(const std::exception& error){std::fprintf(stderr,"FAIL multi-boss world: %s\n",error.what());return 1;}
}
