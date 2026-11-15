#include "game/StageWorld.hpp"
#include "../ecl-fixture.hpp"
#include <cstdio>
#include <fstream>
#include <iterator>

using namespace ecl_fixture;

int main(int argc,char** argv) {
    try {
    {
        th128::StageWorld arrival;
        const auto data=program({{"main",join({instruction(301,{2,4,floatBits(0),floatBits(144)}),instruction(0,{},100)})}});
        std::string reason;
        require(arrival.load("arrival.ecl",data.data(),data.size(),reason)&&arrival.start("arrival.ecl",1,0),reason);
        require(arrival.enableLocalEnemyPasses(reason),reason);
        require(arrival.entities()[0].y==0&&arrival.tick()&&arrival.entities()[0].y==0,"301 initialization does not move during the duplicate constructor guard pass");
        require(arrival.tick()&&arrival.entities()[0].y==108,"native301 mode4 first interpolation tick precedes ECL");
        require(arrival.tick()&&arrival.entities()[0].y==144,"native301 reaches exact authored target, not an accumulated approximation");
        require(arrival.tick()&&arrival.entities()[0].y==144,"completed interpolation returns to retained zero polar motion");
    }
    {
        th128::StageWorld damagedPhase;
        auto spawn=stringWords("target");spawn.insert(spawn.end(),{floatBits(0),floatBits(100),100,0,0});
        auto registration=std::vector<std::uint32_t>{0,50,100};const auto name=stringWords("phase");registration.insert(registration.end(),name.begin(),name.end());
        const auto data=program({{"main",join({instruction(257,spawn,0,0,255,6),instruction(0,{},100)})},
            {"target",join({instruction(412,{0}),instruction(411,{100}),instruction(414,registration,0,0,255,4),
                instruction(400,{floatBits(24),floatBits(24)}),instruction(0,{},100)})},{"phase",join({instruction(411,{500}),instruction(0,{},100)})}});
        std::string reason;unsigned deaths=0;
        require(damagedPhase.load("damaged-phase.ecl",data.data(),data.size(),reason)&&damagedPhase.start("damaged-phase.ecl",1,0),reason);
        require(damagedPhase.setEnemyDamageHost([](const th128::EnemySnapshot&,th128::enemy::Bounds,std::string&)
            ->std::optional<th128::EnemyDamageSample>{th128::EnemyDamageSample sample;
                sample.query.continuation=th128::shotenemy::QueryContinuation::Complete;sample.query.completedDamage=60;sample.spellModeFlags=0;return sample;},reason),reason);
        require(damagedPhase.setEnemyDeathHost([&](const th128::EnemySnapshot&,std::string&){++deaths;return true;},reason),reason);
        require(damagedPhase.enableLocalEnemyPasses(reason),reason);
        for(unsigned tick=0;tick<5&&damagedPhase.entities()[1].life!=500;++tick)require(damagedPhase.tick(),damagedPhase.error());
        require(!damagedPhase.faulted()&&deaths==0,"real post-damage phase switches before ordinary death bookkeeping");
        const auto child=damagedPhase.entities()[1];
        require(child.life==500&&child.phases[0].threshold==-1&&child.lifeWords[2]==500,
            "damage phase continuation executes the new ECL synchronously before the ordinary life-copy tail");
    }
    {
        th128::StageWorld completed;
        auto spawn=stringWords("target");spawn.insert(spawn.end(),{floatBits(0),floatBits(100),100,0,0});
        auto registration=std::vector<std::uint32_t>{0,0,100};const auto name=stringWords("finished");registration.insert(registration.end(),name.begin(),name.end());
        const auto data=program({{"main",join({instruction(257,spawn,0,0,255,6),instruction(0,{},100)})},
            {"target",join({instruction(412,{0}),instruction(411,{100}),instruction(414,registration,0,0,255,4),instruction(0,{},100)})},
            {"finished",join({instruction(412,{std::uint32_t(-1)}),instruction(1)})}});
        std::string reason;unsigned deaths=0;
        require(completed.load("finished.ecl",data.data(),data.size(),reason)&&completed.start("finished.ecl",1,0),reason);
        require(completed.setEnemyDamageHost([](const th128::EnemySnapshot&,th128::enemy::Bounds,std::string&)->std::optional<th128::EnemyDamageSample>{
            th128::EnemyDamageSample sample;sample.query.continuation=th128::shotenemy::QueryContinuation::Complete;sample.query.completedDamage=200;sample.spellModeFlags=0;return sample;
        },reason),reason);
        require(completed.setEnemyDeathHost([&](const th128::EnemySnapshot&,std::string&){++deaths;return true;},reason),reason);
        require(completed.enableLocalEnemyPasses(reason),reason);
        for(int tick=0;tick<4;++tick)require(completed.tick(),completed.error());
        require(!completed.entities()[1].active&&deaths==0,"completed authored Boss death script retires once without ordinary death effects");
    }
    {
        th128::StageWorld timeout;
        auto registration=std::vector<std::uint32_t>{0,50,1};const auto name=stringWords("phase");registration.insert(registration.end(),name.begin(),name.end());
        const auto data=program({{"main",join({instruction(411,{100}),instruction(414,registration,0,0,255,4),instruction(0,{},100)})},
            {"phase",join({instruction(1)})}});
        std::string reason;
        require(timeout.load("timeout.ecl",data.data(),data.size(),reason)&&timeout.start("timeout.ecl",1,0),reason);
        require(timeout.enableLocalEnemyPasses(reason),reason);
        require(timeout.tick(),timeout.error());
        require(!timeout.tick()&&timeout.error().find("explicit spell/player")!=std::string::npos,
            "time-trigger cannot invent required spell/player global state");
        th128::StageWorld knownTimeout;
        require(knownTimeout.load("timeout.ecl",data.data(),data.size(),reason)&&knownTimeout.start("timeout.ecl",1,0),reason);
        require(knownTimeout.setPhaseSpellContext({0,0,0,0},reason)&&knownTimeout.enableLocalEnemyPasses(reason),reason);
        require(knownTimeout.tick()&&knownTimeout.tick(),knownTimeout.error());
        require(knownTimeout.entities()[0].life==50&&(knownTimeout.entities()[0].flags&0x2000000u),
            "explicit inactive spell context permits real timeout clamp and next-entry scheduling");
    }
    {
        th128::StageWorld phases;
        auto words=std::vector<std::uint32_t>{0,6300,1800};const auto name=stringWords("next");words.insert(words.end(),name.begin(),name.end());
        const auto data=program({{"main",join({instruction(411,{10900}),instruction(414,words,0,0,255,4),instruction(0,{},100)})},
            {"next",join({instruction(1)})}});
        std::string reason;
        require(phases.load("phases.ecl",data.data(),data.size(),reason)&&phases.start("phases.ecl",1,0),reason);
        require(phases.enableLocalEnemyPasses(reason),reason);
        const auto initial=phases.entities()[0];const auto& phase=initial.phases[0];
        require(phase.threshold==6300&&phase.limit==1800&&phase.healthTarget=="next"&&phase.timeTarget=="next",
            "actual414 copies authored phase ownership, not score/death placeholders");
        require(phases.entities()[0].phaseRemaining==4600&&phases.entities()[0].phaseThreshold==6300,
            "zero-query after-ECL phase path still commits native remaining/threshold fields");
        require(phases.tick()&&phases.tick(),phases.error());
        require(phases.entities()[0].phaseTimer.current==2,"constructor guard suppresses duplicate first pass before native phase timer advances");
    }
    {
        th128::StageWorld triggered;
        auto words=std::vector<std::uint32_t>{0,6300,1800};const auto name=stringWords("next");words.insert(words.end(),name.begin(),name.end());
        const auto data=program({{"main",join({instruction(15,stringWords("stale"),0,0,255,1),instruction(411,{6299}),instruction(414,words,0,0,255,4),instruction(0,{},100)})},
            {"next",join({instruction(411,{7000}),instruction(0,{},100)})},{"stale",join({instruction(411,{1},1),instruction(0,{},100)})}});
        std::string reason;
        require(triggered.load("triggered.ecl",data.data(),data.size(),reason)&&triggered.start("triggered.ecl",1,0),reason);
        require(triggered.enableLocalEnemyPasses(reason),reason);
        require(triggered.entities()[0].life==6300&&triggered.entities()[0].phases[0].threshold==-1,
            "normal phase query clamps and consumes the record without executing the new script during that tail");
        require(triggered.tick()&&triggered.tick()&&triggered.tick(),triggered.error());
        require(triggered.entities()[0].life==7000,"phase reset cancels stale asynchronous threads and schedules the authored next entry");
    }
    {
        th128::StageWorld gauge;
        const auto data=program({{"main",join({instruction(412,{0}),instruction(411,{10900}),
            instruction(427,{0,floatBits(6300),0xffff8080}),instruction(427,{1,floatBits(2000),0xff800000}),instruction(0,{},100)})}});
        std::string reason;
        require(gauge.load("gauge.ecl",data.data(),data.size(),reason)&&gauge.start("gauge.ecl",1,0),reason);
        require(gauge.enableLocalEnemyPasses(reason),reason);
        const auto& segments=gauge.bossGauges().values[0];
        require(segments[0].configured&&segments[0].ratio==float(6300.0/10900.0)&&segments[0].color==0xffff8080,
            "actual427 stores first authored gauge ratio/color on the registered boss");
        require(segments[1].configured&&segments[1].ratio==float(2000.0/10900.0)&&segments[1].color==0xff800000,
            "actual427 second slot is separate, not a guessed full-HP bar");
        gauge.reset();require(!gauge.bossGauges().values[0][0].configured,"gauge state belongs to manager lifetime");
    }
    {
        th128::StageWorld secondary;
        const auto data=program({{"main",join({instruction(258,{0}),instruction(259,{1,99}),instruction(259,{2,189}),instruction(0,{},100)})}});
        std::string reason;unsigned births=0;
        require(secondary.load("secondary.ecl",data.data(),data.size(),reason)&&secondary.start("secondary.ecl",1,0),reason);
        require(secondary.setAnimationHost([&](const th128::AnimationBirth& birth,std::string&){
            ++births;const auto& request=birth.request;
            require(request.bank==0&&request.slot==int(births)&&request.script==(births==1?99:189)&&request.layer==7&&!request.primary,
                "actual259 secondary requests retain the native bullet bank/slot/script/layer");return true;
        },[](std::uint32_t){},reason),reason);
        require(secondary.enableLocalEnemyPasses(reason),reason);
        require(births==2&&secondary.entities()[0].animation[1]==99&&secondary.entities()[0].animation[2]==189,
            "259 secondary handles are retained in the real owner's slots");
        require(secondary.entities()[0].animation[0]==-1,"secondary259 does not replace primary animation metadata");
    }
    {
        th128::StageWorld guardedBoss;
        auto spawn=stringWords("target");spawn.insert(spawn.end(),{floatBits(0),floatBits(100),20,0,1});
        const auto data=program({{"main",join({instruction(257,spawn,0,0,255,6),instruction(0,{},100)})},
            {"target",join({instruction(412,{0}),instruction(411,{20}),instruction(400,{floatBits(24),floatBits(24)}),instruction(0,{},100)})}});
        std::string reason;unsigned ordinaryDeaths=0;
        require(guardedBoss.load("guarded-boss.ecl",data.data(),data.size(),reason)&&guardedBoss.start("guarded-boss.ecl",1,0),reason);
        require(guardedBoss.setEnemyDamageHost([](const th128::EnemySnapshot&,th128::enemy::Bounds,std::string&)
            ->std::optional<th128::EnemyDamageSample>{th128::EnemyDamageSample sample;
                sample.query.continuation=th128::shotenemy::QueryContinuation::Complete;sample.query.completedDamage=25;sample.spellModeFlags=0;return sample;},reason),reason);
        require(guardedBoss.setEnemyDeathHost([&](const th128::EnemySnapshot&,std::string&){++ordinaryDeaths;return true;},reason),reason);
        require(guardedBoss.enableLocalEnemyPasses(reason),reason);
        while(guardedBoss.frame()<4&&guardedBoss.tick()){}
        require(guardedBoss.faulted()&&guardedBoss.error().find("boss death/phase")!=std::string::npos,
            "unimplemented boss death must not take the ordinary enemy retirement path");
        require(ordinaryDeaths==0,"boss phase/death cannot emit ordinary death effects/drops before the explicit boundary");
    }
    {
        th128::StageWorld boss;
        const auto data=program({{"main",join({instruction(412,{0}),instruction(411,{10900}),
            instruction(412,{std::uint32_t(-1)},1),instruction(411,{13400},1),instruction(0,{},100)})}});
        std::string reason;
        require(boss.load("boss.ecl",data.data(),data.size(),reason)&&boss.start("boss.ecl",1,0),reason);
        require(boss.enableLocalEnemyPasses(reason),reason);
        require((boss.entities()[0].flags&0x81000000u)==0x81000000u&&boss.entities()[0].life==10900,
            "real412 registration precedes411 boss life/high-bit stores");
        require(boss.entities()[0].lifeWords==std::array<std::uint32_t,6>{10900,10900,10900,76300,0,0},
            "411 retains all original life-word assignments, including native sevenfold field");
        require(boss.tick()&&boss.tick(),boss.error());
        require(!(boss.entities()[0].flags&0x1000000u)&&(boss.entities()[0].flags&0x80000000u)&&boss.entities()[0].life==13400,
            "negative412 removes boss binding without clearing retained native high-bit");
    }
    {
        th128::StageWorld shared;
        auto spawn=stringWords("writer");spawn.insert(spawn.end(),{floatBits(0),floatBits(100),20,0,0});
        const auto data=program({{"main",join({instruction(506,{0,std::uint32_t(-9949),std::uint32_t(-9948)},0,6),
            instruction(42,{12}),instruction(43,{std::uint32_t(-9949)},0,1),
            instruction(42,{std::uint32_t(-34)}),instruction(43,{std::uint32_t(-9948)},0,1),
            instruction(42,{2}),instruction(43,{std::uint32_t(-9947)},0,1),
            instruction(257,spawn,0,0,255,6),instruction(506,{0,std::uint32_t(-9949),std::uint32_t(-9948)},0,6),
            instruction(42,{std::uint32_t(-7)},2),instruction(43,{std::uint32_t(-9949)},2,1),instruction(0,{},100)})},
            {"writer",join({instruction(42,{std::uint32_t(-9949)},0,1),instruction(42,{1}),instruction(50),
                instruction(43,{std::uint32_t(-9949)},0,1),
                instruction(304,{floatBits(-9947),floatBits(-9948)},0,3),
                instruction(506,{0,std::uint32_t(-9949),std::uint32_t(-9948)},0,6),
                instruction(506,{0,std::uint32_t(-9949),std::uint32_t(-9947)},3,6),instruction(0,{},100)})}});
        std::string reason;
        require(shared.load("shared.ecl",data.data(),data.size(),reason)&&shared.start("shared.ecl",1,0),reason);
        require(shared.enableLocalEnemyPasses(reason),reason);
        require(shared.entities()[0].emitter0.count==13&&shared.entities()[0].emitter0.layers==-34,
            "child writes actual manager words shared with parent, not inherited copies");
        require(shared.entities()[1].angle==2&&shared.entities()[1].speed==-34,
            "float reader converts signed manager integers, never bitcasts them");
        for(int tick=0;tick<4;++tick)require(shared.tick(),shared.error());
        require(shared.entities()[1].emitter0.count==-7&&shared.entities()[1].emitter0.layers==2,
            "live manager writes remain visible to existing child contexts");
        const auto initial=program({{"main",join({instruction(506,{0,std::uint32_t(-9949),std::uint32_t(-9948)},0,6),
            instruction(304,{floatBits(-9947),floatBits(0)},0,1),instruction(0,{},100)})}});
        shared.reset();
        require(shared.load("initial.ecl",initial.data(),initial.size(),reason)&&shared.start("initial.ecl",1,0),reason);
        require(shared.enableLocalEnemyPasses(reason),reason);
        require(shared.entities()[0].emitter0.count==0&&shared.entities()[0].emitter0.layers==0&&shared.entities()[0].angle==0,
            "new manager lifetime clears all three original shared words");
        require(shared.randomCalls()==0,"shared integer reads and writes consume no RNG");
    }
    {
        th128::StageWorld typed;
        const auto data=program({{"main",join({instruction(44,{floatBits(1)}),instruction(45,{floatBits(-9949)},0,1),instruction(0,{},100)})}});
        std::string reason;
        require(typed.load("typed.ecl",data.data(),data.size(),reason)&&typed.start("typed.ecl",1,0),reason);
        require(!typed.enableLocalEnemyPasses(reason)&&typed.faulted(),"integer manager words are not float lvalues");
        const auto retained=typed.error();
        require(!typed.tick()&&typed.error()==retained,"unsupported manager write is a sticky failure");
    }
    {
        th128::StageWorld absolute;
        auto request=stringWords("target");request.insert(request.end(),{floatBits(10),floatBits(100),20,1000,2});
        const auto data=program({{"main",join({instruction(261,request,0,0,255,6),instruction(0,{},100)})},
            {"target",join({instruction(304,{floatBits(.25f),floatBits(1)}),instruction(0,{},100)})}});
        std::string reason;
        require(absolute.load("absolute.ecl",data.data(),data.size(),reason)&&absolute.start("absolute.ecl",1,0),reason);
        require(absolute.enableLocalEnemyPasses(reason),reason);
        const auto target=absolute.entities()[1];
        require(target.x==10&&target.y==100&&(target.flags&0x100000u)&&target.angle==2.8915927410125732f,
            "261 keeps absolute position while its genuine mode1 applies native mirrored angle");
    }
    for(int difficulty=0;difficulty<=4;++difficulty){
        th128::StageWorld ranks;
        const auto data=program({{"main",join({instruction(42,{std::uint32_t(-9953)},0,1),instruction(42,{std::uint32_t(-9952)},0,1),
            instruction(68),instruction(44,{floatBits(-9951)},0,1),instruction(44,{floatBits(-9950)},0,1),instruction(1,{},100)})}});
        std::string reason;
        require(ranks.load("ranks.ecl",data.data(),data.size(),reason)&&ranks.start("ranks.ecl",difficulty,0),reason);
        require(ranks.enableLocalEnemyPasses(reason),reason);
    }
    {
        th128::StageWorld ints;
        auto spawn=stringWords("child");spawn.insert(spawn.end(),{floatBits(0),floatBits(100),20,0,0});
        const auto data=program({{"main",join({instruction(435,{std::uint32_t(-9985),1,2,3,4},0,1),
            instruction(435,{std::uint32_t(-9984),7,8,9,10},0,1),instruction(257,spawn,0,0,255,6),instruction(0,{},100)})},
            {"child",join({instruction(304,{floatBits(-9985),floatBits(-9984)},0,3),instruction(0,{},100)})}});
        std::string reason;
        require(ints.load("ints.ecl",data.data(),data.size(),reason)&&ints.start("ints.ecl",1,0),reason);
        require(ints.enableLocalEnemyPasses(reason),reason);
        require(ints.entities()[1].angle==2&&ints.entities()[1].speed==8,"435 integer global destinations inherit through native four-word fields");
    }
    {
        th128::StageWorld variables;
        auto spawn=stringWords("child");spawn.insert(spawn.end(),{floatBits(0),floatBits(100),20,0,0});
        const auto data=program({{"main",join({instruction(44,{floatBits(.25f)}),instruction(45,{floatBits(-9981)},0,1),
            instruction(44,{floatBits(-.75f)}),instruction(45,{floatBits(-9980)},0,1),instruction(257,spawn,0,0,255,6),instruction(0,{},100)})},
            {"child",join({instruction(304,{floatBits(-9981),floatBits(-9980)},0,3),instruction(0,{},100)})}});
        std::string reason;
        require(variables.load("variables.ecl",data.data(),data.size(),reason)&&variables.start("variables.ecl",1,0),reason);
        require(variables.enableLocalEnemyPasses(reason),reason);
        require(variables.entities()[1].angle==.25f&&variables.entities()[1].speed==-.75f,
            "original float lvalue writes and native spawn inheritance reach the child's float readers");
        require(variables.randomCalls()==0,"inherited word access consumes no RNG");
    }
    {
        th128::StageWorld extended;
        const auto data=program({{"main",join({instruction(500,{0}),instruction(537,{0,1,0,16,60,1,2,0,floatBits(0),floatBits(-999),floatBits(-999999),floatBits(-999999)}),
            instruction(509,{0,1,1,8,10,0xffffffffu,floatBits(.125f),floatBits(-2)},1),instruction(0,{},100)})}});
        std::string reason;
        require(extended.load("extended.ecl",data.data(),data.size(),reason)&&extended.start("extended.ecl",1,0),reason);
        require(extended.enableLocalEnemyPasses(reason),reason);
        const auto record=extended.entities()[0].emitter0.laterExtensions[0];
        require(record.type==16&&record.parameterA==60&&record.parameterC==2&&record.parameterS==-999,"537 retains actual record1 fields");
        require(extended.tick()&&extended.tick(),extended.error());
        const auto changed=extended.entities()[0].emitter0.laterExtensions[0];
        require(changed.type==8&&changed.control==1&&changed.parameterR==.125f&&changed.parameterS==-2&&changed.parameterC==2
            &&changed.parameterT==-999999&&changed.parameterU==-999999,"509 retains the537-only fields in selected later slot");
    }
    {
        th128::StageWorld direct;
        const auto data=program({{"main",join({instruction(500,{0}),instruction(505,{0,floatBits(2.8f),floatBits(1)}),
            instruction(506,{0,11,1}),instruction(506,{0,65535,32768},1),instruction(0,{},100)})}});
        std::string reason;
        require(direct.load("direct.ecl",data.data(),data.size(),reason)&&direct.start("direct.ecl",1,0),reason);
        require(direct.enableLocalEnemyPasses(reason),reason);
        require(direct.entities()[0].emitter0.count==11&&direct.entities()[0].emitter0.layers==1
            &&direct.entities()[0].emitter0.speed==2.8f&&direct.entities()[0].emitter0.endSpeed==1,"native direct count/speed ECL stores");
        require(direct.tick()&&direct.tick(),direct.error());
        require(direct.entities()[0].emitter0.count==-1&&direct.entities()[0].emitter0.layers==-32768,"ECL506 retains native signed-word truncation");
    }
    {
        th128::StageWorld additional;
        auto spawn=stringWords("target");spawn.insert(spawn.end(),{floatBits(0),floatBits(100),20,0,1});
        const auto data=program({{"main",join({instruction(260,spawn,0,0,255,6),instruction(0,{},100)})},
            {"target",join({instruction(407,{2,2}),instruction(0,{},100)})}});
        std::string reason;unsigned deaths=0;
        require(additional.load("additional.ecl",data.data(),data.size(),reason)&&additional.start("additional.ecl",1,0),reason);
        require(additional.setEnemyDamageHost([](const th128::EnemySnapshot&,th128::enemy::Bounds,std::string&)->std::optional<th128::EnemyDamageSample>{
            th128::EnemyDamageSample sample;sample.query.continuation=th128::shotenemy::QueryContinuation::Complete;
            sample.query.completedDamage=100;sample.spellModeFlags=0;return sample;
        },reason),reason);
        require(additional.setEnemyDeathHost([&](const th128::EnemySnapshot&,std::string&){++deaths;return true;},reason),reason);
        require(additional.enableLocalEnemyPasses(reason),reason);
        for(unsigned tick=0;tick<5;++tick)if(!additional.tick())break;
        require(additional.faulted()&&deaths==0&&additional.error().find("additional drop")!=std::string::npos,
            "configured nonempty drop lists cannot be silently treated as the legacy empty list");
        require(additional.start("additional.ecl",1,0),additional.error());
        require(additional.setEnemyDamageHost([](const th128::EnemySnapshot&,th128::enemy::Bounds,std::string&)->std::optional<th128::EnemyDamageSample>{
            th128::EnemyDamageSample sample;sample.query.continuation=th128::shotenemy::QueryContinuation::Complete;
            sample.query.completedDamage=100;sample.spellModeFlags=0;return sample;
        },reason),reason);
        require(additional.setEnemyDeathHost([&](const th128::EnemySnapshot& state,std::string&){
            require(state.drops.words[2]==2,"explicit additional-drop host receives actual authored count");++deaths;return true;
        },reason,true),reason);
        require(additional.enableLocalEnemyPasses(reason),reason);
        for(unsigned tick=0;tick<5;++tick)require(additional.tick(),additional.error());
        require(deaths==1&&!additional.entities()[1].active&&additional.entities()[1].drops.words[2]==0,
            "completed capable death clears additional table and unlinks once");
    }
    {
        th128::StageWorld drops;
        const auto data=program({{"main",join({instruction(407,{0,1}),instruction(407,{2,2}),instruction(408,{floatBits(24),floatBits(24)}),instruction(406,{},1),instruction(0,{},100)})}});
        std::string reason;
        require(drops.load("drops.ecl",data.data(),data.size(),reason)&&drops.start("drops.ecl",1,0),reason);
        require(drops.enableLocalEnemyPasses(reason),reason);
        require(drops.entities()[0].drops.words[2]==2&&drops.entities()[0].drops.words[0]==1,"ECL407 updates real native drop slots");
        require(drops.entities()[0].drops.width==24&&drops.entities()[0].drops.height==24,"ECL408 preserves authored spread");
        require(drops.tick()&&drops.tick(),drops.error());
        require(drops.entities()[0].drops.words[2]==0&&drops.entities()[0].drops.words[0]==1&&drops.entities()[0].drops.width==24,
            "ECL406 clears only additional drops, not primary/spread");
    }
        th128::StageWorld world;
        {
            th128::StageWorld retired;std::string reason;
            auto target=stringWords("target");target.insert(target.end(),{floatBits(0),floatBits(100),20,0,2});
            const auto data=program({{"main",join({instruction(260,target,0,0,255,6),instruction(0,{},200)})},
                {"target",join({instruction(400,{floatBits(24),floatBits(24)}),instruction(0,{},200)})}});
            require(retired.load("retired.ecl",data.data(),data.size(),reason)&&retired.start("retired.ecl",1,0),reason);
            require(retired.setEnemyDamageHost([](const th128::EnemySnapshot&,th128::enemy::Bounds,std::string&)->std::optional<th128::EnemyDamageSample>{
                th128::EnemyDamageSample result;result.query.continuation=th128::shotenemy::QueryContinuation::Complete;
                result.query.completedDamage=26;result.spellModeFlags=0;return result;
            },reason),reason);
            unsigned deaths=0;
            require(retired.setEnemyDeathHost([&](const th128::EnemySnapshot& enemy,std::string&){
                ++deaths;require(enemy.life==-6&&enemy.drop==2,"death sees committed native state");return true;
            },reason),reason);
            require(retired.enableLocalEnemyPasses(reason),reason);
            for(int pass=0;pass<4;++pass)require(retired.tick(),retired.error());
            require(deaths==1&&!retired.entities()[1].active,"completed death unlinks actual node once");
        }
        {
            th128::StageWorld damageWorld;
            auto target=stringWords("target");target.insert(target.end(),{floatBits(0),floatBits(100),20,0,0});
            const auto data=program({{"main",join({instruction(260,target,0,0,255,6),instruction(0,{},100)})},
                {"target",join({instruction(400,{floatBits(24),floatBits(24)}),instruction(0,{},100)})}});
            std::string reason;unsigned queries=0;
            require(damageWorld.load("damage.ecl",data.data(),data.size(),reason)&&damageWorld.start("damage.ecl",1,0),reason);
            require(damageWorld.setEnemyDamageHost([&](const th128::EnemySnapshot&,th128::enemy::Bounds,std::string&)
                ->std::optional<th128::EnemyDamageSample>{
                ++queries;th128::EnemyDamageSample sample;sample.query.continuation=th128::shotenemy::QueryContinuation::Complete;
                sample.query.completedDamage=13;sample.playerState=1;sample.spellModeFlags=0;return sample;
            },reason),reason);
            require(damageWorld.enableLocalEnemyPasses(reason),reason);
            require(damageWorld.entities()[1].life==20&&damageWorld.damageStats()[1]==13,"native birth protection suppresses HP but not stats");
            require(damageWorld.entities()[1].lifeWords[1]==20&&damageWorld.entities()[1].lifeWords[2]==20,"post-damage query copies native life reference");
            require(damageWorld.tick(),damageWorld.error());
            require(damageWorld.tick(),damageWorld.error());
            require(damageWorld.entities()[1].life==20,"birth protection remains positive for the next eligible body");
            require(damageWorld.tick(),damageWorld.error());
            require(damageWorld.entities()[1].life==7,"real native HP prefix runs after protection expires");
            require(!damageWorld.tick(),"second real query reaches death continuation");
            require(damageWorld.entities()[1].life==-6&&queries==4,"death prefix preserves committed damage exactly once");
            require(damageWorld.error().find("0x417370")!=std::string::npos,"death remains mandatory, not automatic removal");
            require(!damageWorld.tick()&&queries==4,"fault cannot replay damage");
        }
        for(const auto hiddenFlag:{1u,32u}){
            th128::StageWorld gated;
            auto targetWords=stringWords("target");targetWords.insert(targetWords.end(),{floatBits(0),floatBits(100),20,0,0});
            const auto data=program({{"main",join({instruction(260,targetWords,0,0,255,6),instruction(0,{},100)})},
                {"target",join({instruction(402,{hiddenFlag}),instruction(403,{hiddenFlag},2),instruction(0,{},100)})}});
            std::string reason;unsigned contacts=0;
            require(gated.load("gated.ecl",data.data(),data.size(),reason)&&gated.start("gated.ecl",1,0),reason);
            require(gated.setEnemyContactHost([&](const th128::EnemySnapshot&,th128::enemy::Bounds,std::string&){++contacts;return true;},reason),reason);
            require(gated.enableLocalEnemyPasses(reason),reason);
            require(contacts==0,"flags1/32 skip actual after-ECL shot query");
            require(gated.tick()&&gated.tick(),gated.error());
            require(contacts==0,"constructor guard prevents a duplicate first-pass update before ECL time2");
            require(gated.tick(),gated.error());
            require(contacts==1,"clearing the gate enables the query on that actual ECL tick");
        }
        {
            th128::StageWorld visible;
            const auto data=program({{"main",join({instruction(402,{32}),instruction(402,{0}),instruction(403,{32}),instruction(403,{0}),instruction(0,{},100)})}});
            std::string reason;std::vector<th128::enemy::VisibilityEffect> changes;
            require(visible.load("visible.ecl",data.data(),data.size(),reason)&&visible.start("visible.ecl",1,0),reason);
            require(visible.setAnimationVisibilityHost([&](std::uint32_t,th128::enemy::VisibilityEffect change,std::string&){changes.push_back(change);return true;},reason),reason);
            require(visible.enableLocalEnemyPasses(reason),reason);
            require(changes==std::vector{th128::enemy::VisibilityEffect::Hide,th128::enemy::VisibilityEffect::Hide,
                th128::enemy::VisibilityEffect::Show,th128::enemy::VisibilityEffect::Show},"flag visibility is level-triggered on existing animations");
        }
        {
            th128::StageWorld contactWorld;
            auto childWords=stringWords("target");
            childWords.insert(childWords.end(),{floatBits(7),floatBits(100),20,0,0});
            const auto data=program({{"main",join({instruction(260,childWords,0,0,255,6),instruction(1,{},200)})},
                {"target",join({instruction(400,{floatBits(24),floatBits(28)}),instruction(0,{},200)})}});
            std::string reason;
            require(contactWorld.load("contact.ecl",data.data(),data.size(),reason),reason);
            require(contactWorld.start("contact.ecl",1,0),contactWorld.error());
            unsigned visits=0;
            require(contactWorld.setEnemyContactHost([&](const th128::EnemySnapshot& enemy,th128::enemy::Bounds bounds,std::string& error){
                require(!enemy.root&&enemy.x==7&&enemy.y==100,"contact uses actual combined state after ECL");
                require(bounds.width==24&&bounds.height==28,"opcode400 extents reach real query");
                ++visits;error="contact continuation required";return false;
            },reason),reason);
            require(!contactWorld.enableLocalEnemyPasses(reason),"constructor-time contact must fail before linking");
            require(visits==1&&contactWorld.error()=="contact continuation required","contact host is mandatory and sticky");
            require(!contactWorld.tick()&&visits==1,"failed contact cannot be replayed");
        }
        auto initial=program({{"main",join({instruction(40,{0}),instruction(900),instruction(1)})}});
        std::string error;
        require(world.load("fixture.ecl",initial.data(),initial.size(),error),error);
        require(world.start("fixture.ecl",1,0x1234),world.error());
        require(!world.tick(),"unknown world callback must halt");
        require(world.error().find("900")!=std::string::npos,"world diagnostics preserve actual opcode");
        const auto retained=world.error();
        require(!world.tick()&&world.error()==retained,"world fault is sticky");
        require(!world.start("missing.ecl",1,0),"missing stage rejects");
        world.reset();
        auto spawnWords=stringWords("child");
        spawnWords.insert(spawnWords.end(),{floatBits(0),floatBits(0),1000,1000,2});
        const auto guarded=program({{"main",join({instruction(260,spawnWords,0,0,255,6),instruction(1)})},
            {"child",join({instruction(304,{floatBits(0),floatBits(1)}),instruction(1)})}});
        require(world.load("mode.ecl",guarded.data(),guarded.size(),error),error);
        require(world.start("mode.ecl",1,0),world.error());
        require(world.tick(),world.error());
        require(world.spawnCount()==1,"mirrored mode creates its actual enemy");
        const auto mirrored=world.entities().at(1);
        require((mirrored.flags&0x100000U)!=0,"spawn mode1 must set the original mirror flag");
        require((mirrored.flags&0x80000000U)!=0,"native constructor marks life>=1000 before the initial update");
        require(std::abs(mirrored.angle)>3,"mirrored polar setup must not act like normal birth");
        world.reset();
        const auto ranked=program({{"main",join({instruction(40,{4}),instruction(435,{0,std::uint32_t(-10000),std::uint32_t(-10000),std::uint32_t(-10000),std::uint32_t(-10000)},0,31),instruction(1)})}});
        require(world.load("ranked.ecl",ranked.data(),ranked.size(),error),error);
        require(world.start("ranked.ecl",1,0),world.error());
        require(world.tick(),world.error());
        require(world.randomCalls()==8,"rank choice evaluates all four referenced alternatives before storing");
        world.reset();
        const auto zeroDuration=program({{"main",join({instruction(305,{0,std::uint32_t(-1),floatBits(0),floatBits(1)},0,2),instruction(1)})}});
        require(world.load("zero-duration.ecl",zeroDuration.data(),zeroDuration.size(),error),error);
        require(world.start("zero-duration.ecl",1,0)&&world.tick(),"zero duration cancels without resolving masked mode/empty expression stack");
        world.reset();
        const auto doubleDuration=program({{"main",join({instruction(305,{std::uint32_t(-10000),0,floatBits(0),floatBits(1)},0,1),instruction(1)})}});
        require(world.load("double-duration.ecl",doubleDuration.data(),doubleDuration.size(),error),error);
        require(world.start("double-duration.ecl",1,0)&&world.tick(),world.error());
        require(world.randomCalls()==4,"positive duration resolves its stateful reference twice in native order");
        world.reset();
        const auto retainedWorld=program({{"main",join({instruction(0,{},100),instruction(1,{},200)})}});
        require(world.load("retained.ecl",retainedWorld.data(),retainedWorld.size(),error),error);
        require(world.start("retained.ecl",1,0)&&world.tick(),world.error());
        const auto previousFrame=world.frame();
        const auto previousCount=world.entities().size();
        const auto previousSources=world.linkedSources();
        require(!world.start("retained.ecl",5,0),"invalid difficulty rejects");
        require(world.frame()==previousFrame&&world.entities().size()==previousCount&&world.linkedSources()==previousSources,"rejected start preserves active world");
        require(world.tick()&&world.frame()==previousFrame+1,"preserved world can continue after rejected start");
        world.reset();
        if(argc>1) {
            for(int index=1;index<argc;++index) {
                std::ifstream stream(argv[index],std::ios::binary);
                require(bool(stream),"actual resource missing");
                Bytes bytes((std::istreambuf_iterator<char>(stream)),{});
                std::string path=argv[index];
                require(world.load(path.substr(path.find_last_of("/\\")+1),bytes.data(),bytes.size(),error),error);
            }
            require(world.start("st_a1_1.ecl",1,0x1234),world.error());
            require(world.linkedSources()==3,"first-stage root/default/boss linked");
            for(int frame=0;frame<200&&world.error().empty();++frame)world.tick();
            std::printf("ACTUAL world tick=%u entities=%zu spawn=%u stop=%s\n",world.frame(),world.entities().size(),world.spawnCount(),world.error().c_str());
            require(world.spawnCount()>=1,"actual first spawn reached, not a fabricated stage");
            require(world.entities().size()>=2,"stage root and actual enemy retained");
            const auto first=world.entities()[1];
            require(first.emitter0.count==1&&first.emitter0.layers==1,"actual first volley selects local4, not rank loop count local8");
            require(first.emitter0.speed==1.f&&first.emitter0.type==8&&first.emitter0.color==10,"actual emitter parameters reached before unsupported ETEX/fire");
            require(first.emitter0.extension0.type==2&&first.emitter0.extension0.parameterA==1&&first.emitter0.extension0.parameterB==-999999,
                    "actual raw extension words retained without inventing lifecycle");
            require(world.error().find("opcode=501")!=std::string::npos,"raw ETEX preparation reaches explicit unsupported bullet manager call");
        }
        std::puts("PASS bounded StageWorld with actual script-driven birth");
        return 0;
    } catch(const std::exception& error) {std::fprintf(stderr,"FAIL StageWorld: %s\n",error.what());return 1;}
}
