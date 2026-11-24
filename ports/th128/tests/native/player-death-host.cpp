#include "game/PlayerDeathHost.hpp"
#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

int main(){
    using namespace th128::player;
    const DeathContext context{{100000,0},0,{0,0},{0,0,0,0},{true,0},{}};
    const hit::Timer zero{-1,0,0,0x4b6f58,1};
    std::string error;
    DeathOperation operation(context,4,zero,zero);
    std::vector<death::CallKind> called;
    assert(operation.run([&](const death::CallRequest& call,std::string&){
        const auto& progress=operation.progress();
        assert(progress.context.motivation.value4b4d64==90000&&progress.context.perfectCharge==2500);
        assert(progress.pending&&progress.pending->callSite==call.callSite);
        assert(progress.powerRaw==10000&&!progress.field438f8);
        called.push_back(call.kind);return true;
    },error));
    assert(operation.progress().completed&&operation.progress().state==2&&operation.progress().protection.current==180);
    assert((called==std::vector{death::CallKind::ControlRefresh41fc80,death::CallKind::PlayerAnimation}));
    assert(!operation.run([](const death::CallRequest&,std::string&){return true;},error));
    auto crossing=context;crossing.perfectCharge=9000;
    DeathOperation failed(crossing,4,zero,zero);
    unsigned attempts=0;
    assert(!failed.run([&](const death::CallRequest& call,std::string& reason){
        ++attempts;
        assert(failed.progress().context.motivation.value4b4d64==90000&&failed.progress().context.perfectCharge==11500);
        assert(!failed.progress().powerRaw&&!failed.progress().field438f8);
        if(call.kind==death::CallKind::ChargeNotify424b80){reason="charge notification rejected";return false;}
        return true;
    },error));
    assert(attempts==2&&failed.progress().pending->kind==death::CallKind::ChargeNotify424b80);
    assert(failed.progress().context.motivation.value4b4d64==90000&&failed.progress().context.perfectCharge==11500);
    assert(!failed.run([&](const death::CallRequest&,std::string&){++attempts;return true;},error)&&attempts==2);
    const std::vector expected{death::CallKind::Sound,death::CallKind::ChargeNotify424b80,death::CallKind::ChargeUi425820,
        death::CallKind::ControlRefresh41fc80,death::CallKind::PlayerAnimation};
    for(std::size_t rejected=0;rejected<expected.size();++rejected){
        DeathOperation boundary(crossing,4,zero,zero);std::vector<death::CallKind> order;
        assert(!boundary.run([&](const death::CallRequest& call,std::string&){order.push_back(call.kind);return order.size()<=rejected;},error));
        assert(order==std::vector(expected.begin(),expected.begin()+rejected+1));
        assert(boundary.progress().pending->kind==expected[rejected]);
        assert(boundary.progress().state==(rejected==4?2:4));
    }
    DeathOperation completed(crossing,4,zero,zero);std::vector<death::CallKind> order;
    assert(completed.run([&](const death::CallRequest& call,std::string&){order.push_back(call.kind);return true;},error));
    assert(order==expected);
    auto live=context;live.records[3]={1,17,0};
    DeathOperation cleanup(live,4,zero,zero);
    assert(!cleanup.run([](const death::CallRequest&,std::string&){return true;},error));
    assert(cleanup.progress().state==2&&cleanup.progress().cleanupRecord==3&&cleanup.progress().context.records[3].active==0
        &&cleanup.progress().unresolvedHandle==17&&error.find("cleanup")!=std::string::npos);
    DeathOperation reentry(context,4,zero,zero);unsigned outer=0;
    assert(!reentry.run([&](const death::CallRequest&,std::string&){
        ++outer;std::string nested;
        assert(!reentry.run([](const death::CallRequest&,std::string&){assert(false);return true;},nested));return true;
    },error));
    assert(outer==1&&error.find("reentrant")!=std::string::npos);
    return 0;
}
