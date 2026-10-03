#include "../th10_web/cpp/multiplayer/ItemOwnership.hpp"
#include <cassert>
#include <cstdio>
using namespace th10;
using namespace th10::multiplayer;
int main(){
    arithmetic_mode(Precision::Single,Rounding::NearestEven);
    Item item{};item.position={0,300,0};item.state=1;
    ItemCandidate p[3]{};
    for(u32 i=0;i<3;++i){p[i].eligible=true;p[i].position={float(i)*20-20,300,0};p[i].pickup={{1000,1000,0},{1001,1001,0}};p[i].slow=p[i].fast=p[i].pickup;}
    ItemOwnership claim;
    assert(select_item_collector(claim,item,p,3)==1);
    p[1].eligible=false;
    assert(select_item_collector(claim,item,p,3)==0); // Equidistant seats, stable order.
    p[2].auto_collect=true;
    assert(select_item_collector(claim,item,p,3)==2);
    p[0].pickup={{-2,298,0},{2,302,0}};
    assert(select_item_collector(claim,item,p,3)==0); // Immediate pickup beats global attraction.
    claim.homing=2;
    assert(select_item_collector(claim,item,p,3)==2); // Do not steal an already homing item.
    p[2].eligible=false;
    assert(select_item_collector(claim,item,p,3)==0&&claim.homing==-1);
    claim.recipient=2;
    assert(select_item_collector(claim,item,p,3)==-1); // A gift cannot be stolen while its recipient is down.
    assert(claim.recipient==2);
    p[2].eligible=true;
    assert(select_item_collector(claim,item,p,3)==2);
    const auto checkpoint=claim;claim={};claim=checkpoint;
    assert(select_item_collector(claim,item,p,3)==2);
    claim={};for(auto& pilot:p)pilot.eligible=false;
    assert(select_item_collector(claim,item,p,3)==-1);
    assert(select_item_collector(claim,item,p,1)==-1);
    std::puts("item collector, homing and targeted-transfer ownership: PASS");
}
