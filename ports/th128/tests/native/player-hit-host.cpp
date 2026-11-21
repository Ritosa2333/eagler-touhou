#include "game/PlayerHitHost.hpp"
#include <cassert>
#include <fstream>
#include <iterator>
#include <limits>
#include <vector>

int main(int argc,char** argv){
    assert(argc==3);
    std::ifstream stream(argv[1],std::ios::binary);
    const std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(stream)),{});
    using namespace th128::player;
    const hit::Timer zero{-1,0,0,0x4b6f58,1};
    const HitHostSnapshot snapshot{{1,0,0},zero,zero,zero,{true,0},{0,0,0},{true,0}};
    PlayerHitHost host;std::string error;
    unsigned effects=0,sounds=0,particles=0;
    assert(host.configure(bytes.data(),bytes.size(),snapshot,
        [&](int script,hit::Position screen,int layer,std::string&){
            assert(screen.horizontal==320&&screen.vertical==416&&layer==0);
            assert(script==83||script==84);++effects;if(script==84)++particles;return true;
        },[&](int identifier,std::string&){assert(identifier==2);++sounds;return true;},error));
    const auto result=host.confirmedOverlap({0,400,0},{false,0},error);
    assert(result&&*result==1&&host.snapshot().gate.stateF78==4);
    assert(effects==33&&particles==32&&sounds==1);
    assert(host.snapshot().protection.current==6&&host.snapshot().stateTimer.current==0);
    assert(host.animationState()->script==0&&host.animationState()->ticks==1);
    const auto suppressed=host.confirmedOverlap({0,400,0},{false,0},error);
    assert(suppressed&&*suppressed==0&&effects==33);
    for(unsigned tick=0;tick<6;++tick)assert(host.stepProtection(1,error));
    assert(host.snapshot().protection.current==0);
    for(unsigned window=0;window<8;++window){
        assert(host.advanceWindowUnit({true,0,0,0},error));
        assert(host.snapshot().stateTimer.current==int(window+1));
    }
    assert(!host.advanceWindowUnit({true,0,0,0},error)&&error.find("death commit")!=std::string::npos);
    assert(host.snapshot().stateTimer.current==8);
    assert(host.animationState()!=nullptr&&host.spriteInfo()!=nullptr);
    for(const int power:{0,2999,3000,9999,10000}){
        const auto outline=host.powerOutline(power);assert(outline&&outline->state.sprite==host.animationState()->sprite+79);
        assert(outline->state.sx==outline->state.sy&&outline->state.sx==(power<3000?1.1f:1.3f));
        assert(outline->state.primary==(power<3000?0x00ffffffu:power<10000?0x50ffff80u:0xc0ffff00u)&&outline->state.blend==1);
    }
    assert(!host.powerOutline(-1)&&!host.powerOutline(10001));
    PlayerHitHost failing;
    unsigned attempts=0;
    assert(failing.configure(bytes.data(),bytes.size(),snapshot,
        [&](int,hit::Position,int,std::string& reason){++attempts;reason="required hit effect rejected";return false;},
        [](int,std::string&){return true;},error));
    assert(!failing.confirmedOverlap({0,400,0},{false,0},error));
    assert(failing.snapshot().gate.stateF78==4&&failing.snapshot().protection.current==0&&attempts==1);
    assert(!failing.confirmedOverlap({0,400,0},{false,0},error)&&attempts==1);
    std::ifstream badStream(argv[2],std::ios::binary);
    const std::vector<unsigned char> bad((std::istreambuf_iterator<char>(badStream)),{});
    PlayerHitHost camera;
    assert(!camera.configure(bad.data(),bad.size(),snapshot,[](int,hit::Position,int,std::string&){return true;},
        [](int,std::string&){return true;},error));
    assert(error.find("camera")!=std::string::npos||error.find("ANM")!=std::string::npos);
    PlayerHitHost deathReady;
    assert(deathReady.configure(bytes.data(),bytes.size(),snapshot,[](int,hit::Position,int,std::string&){return true;},
        [](int,std::string&){return true;},error));
    assert(deathReady.confirmedOverlap({0,400,0},{false,0},error)==1);
    for(unsigned tick=0;tick<8;++tick)assert(deathReady.advanceWindowUnit({true,0,0,0},error));
    const hit::Timer protection{179,180,180,0x4b6f58,1};
    assert(deathReady.commitDeathState(2,zero,protection,error));
    assert(deathReady.snapshot().gate.stateF78==2&&deathReady.snapshot().protection.current==180);
    assert(deathReady.advancePostDeathUnit(error)&&deathReady.snapshot().stateTimer.current==1
        &&deathReady.snapshot().protection.current==179);
    for(unsigned tick=1;tick<30;++tick)assert(deathReady.advancePostDeathUnit(error));
    assert(deathReady.snapshot().stateTimer.current==30&&deathReady.snapshot().protection.current==150
        &&deathReady.animationState()->ticks==31);
    unsigned respawnCalls=0;
    const auto respawn=deathReady.beginRespawn({12,400,.1f},90000,1,[&](const death::CallRequest& call,std::string&){
        assert(deathReady.snapshot().gate.stateF78==0);++respawnCalls;
        if(respawnCalls==1){assert(call.kind==death::CallKind::RespawnField43df90&&call.operands[2]==30&&call.operands[3]==150);}
        else assert(call.kind==death::CallKind::PostRespawn427190);
        return true;
    },error);
    assert(respawn&&respawnCalls==2&&respawn->savedPosition.horizontal==12&&respawn->position.vertical==480);
    assert(deathReady.snapshot().gate.stateF78==0&&deathReady.snapshot().stateTimer.current==0&&deathReady.snapshot().protection.current==280);
    assert(deathReady.advanceEntryTimersUnit(error));
    unsigned entryCalls=0;
    for(int elapsed=1;elapsed<=60;++elapsed){
        const auto entry=deathReady.entryUnit([&](int time,float radius,std::string&){
            assert(time==elapsed);++entryCalls;if(time==1)assert(radius>81&&radius<82);if(time>=30)assert(radius==0);return true;
        },error);
        assert(entry&&entry->vertical==61440-elapsed*10240/60);
        if(elapsed<60){assert(deathReady.snapshot().gate.stateF78==0);assert(deathReady.advanceEntryTimersUnit(error));}
        else assert(deathReady.snapshot().gate.stateF78==1&&deathReady.snapshot().stateTimer.current==0&&entry->vertical==51200);
    }
    assert(entryCalls==60);
    PlayerHitHost pendingRespawn;
    auto ready=snapshot;ready.gate.stateF78=2;ready.stateTimer={29,30,30,0x4b6f58,1};
    assert(pendingRespawn.configure(bytes.data(),bytes.size(),ready,[](int,hit::Position,int,std::string&){return true;},
        [](int,std::string&){return true;},error));
    unsigned rejectedRespawns=0;
    const auto reject=[&](const death::CallRequest&,std::string& reason){++rejectedRespawns;reason="required respawn call rejected";return false;};
    assert(!pendingRespawn.beginRespawn({0,400,0},90000,1,reject,error)&&error=="required respawn call rejected");
    assert(pendingRespawn.snapshot().gate.stateF78==0);
    assert(!pendingRespawn.beginRespawn({0,400,0},90000,1,reject,error)&&rejectedRespawns==1);
    for(const int wrongPrevious:{std::numeric_limits<int>::max(),std::numeric_limits<int>::min()}){
        PlayerHitHost malformedCommit;
        assert(malformedCommit.configure(bytes.data(),bytes.size(),snapshot,[](int,hit::Position,int,std::string&){return true;},
            [](int,std::string&){return true;},error));
        assert(malformedCommit.confirmedOverlap({0,400,0},{false,0},error)==1);
        for(unsigned tick=0;tick<8;++tick)assert(malformedCommit.advanceWindowUnit({true,0,0,0},error));
        auto invalidTimer=zero;invalidTimer.previous=wrongPrevious;
        assert(!malformedCommit.commitDeathState(2,invalidTimer,protection,error));
        assert(malformedCommit.snapshot().gate.stateF78==4);
        PlayerHitHost malformedProtection;
        assert(malformedProtection.configure(bytes.data(),bytes.size(),snapshot,[](int,hit::Position,int,std::string&){return true;},
            [](int,std::string&){return true;},error));
        assert(malformedProtection.confirmedOverlap({0,400,0},{false,0},error)==1);
        for(unsigned tick=0;tick<8;++tick)assert(malformedProtection.advanceWindowUnit({true,0,0,0},error));
        auto invalidProtection=protection;invalidProtection.previous=wrongPrevious;
        assert(!malformedProtection.commitDeathState(2,zero,invalidProtection,error));
        assert(malformedProtection.snapshot().gate.stateF78==4);
    }
    PlayerHitHost movement;
    assert(movement.configure(bytes.data(),bytes.size(),snapshot,[](int,hit::Position,int,std::string&){return true;},
        [](int,std::string&){return true;},error));
    const Input right{RightKey,RightKey,0};
    const auto movedRight=movement.moveUnit({0,51200},right,{true,true,4,0},error);
    assert(movedRight&&movedRight->position.horizontal==576&&movedRight->position.vertical==51200);
    assert(movement.animationState()->script==3&&movement.animationState()->ticks==2);
    const auto releasedRight=movement.moveUnit(movedRight->position,{}, {true,true,5,0},error);
    assert(releasedRight&&releasedRight->position.horizontal==576&&movement.animationState()->script==4);
    const Input left{LeftKey,LeftKey,0};
    const auto movedLeft=movement.moveUnit(releasedRight->position,left,{true,true,6,0},error);
    assert(movedLeft&&movedLeft->position.horizontal==0&&movement.animationState()->script==1);
    assert(movement.advanceNormalTimersUnit(error));
    assert(movement.snapshot().stateTimer.previous==0&&movement.snapshot().stateTimer.current==1);
    assert(movement.snapshot().heldTimer.current==0&&movement.focusCounter()==1);
    PlayerHitHost charging;auto charged=snapshot;charged.heldTimer={44,45,45,0x4b6f58,1};
    assert(charging.configure(bytes.data(),bytes.size(),charged,[](int,hit::Position,int,std::string&){return true;},
        [](int,std::string&){return true;},error));
    unsigned stopped=0;assert(charging.setSoundOperationHost([&](int operation,int identifier,std::string&){
        assert(operation==1&&identifier==31);++stopped;return true;},error));
    assert(charging.confirmedOverlap({0,400,0},{false,0},error)==1);
    assert(charging.advanceWindowUnit({true,0,0,0},error));assert(stopped==1);
    assert(charging.snapshot().heldTimer.current==0&&charging.snapshot().stateTimer.current==1);
    assert(charging.advanceWindowUnit({true,0,0,0},error)&&stopped==1);
    PlayerHitHost rejectedStop;assert(rejectedStop.configure(bytes.data(),bytes.size(),charged,[](int,hit::Position,int,std::string&){return true;},
        [](int,std::string&){return true;},error));
    assert(rejectedStop.setSoundOperationHost([](int,int,std::string& reason){reason="charge stop failed";return false;},error));
    assert(rejectedStop.confirmedOverlap({0,400,0},{false,0},error)==1);
    assert(!rejectedStop.advanceWindowUnit({true,0,0,0},error)&&error=="charge stop failed");
    assert(rejectedStop.snapshot().stateTimer.current==0);assert(!rejectedStop.advanceWindowUnit({true,0,0,0},error));
    return 0;
}
