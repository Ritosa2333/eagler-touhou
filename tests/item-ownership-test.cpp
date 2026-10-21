#include "../web/cpp/multiplayer/ItemOwnership.hpp"
#include <cassert>
#include <cstdio>
#include <initializer_list>
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
    // Full collectors forward both contact and POC claims to the lowest Power,
    // including a teammate below the line and outside the pickup rectangle.
    for(bool poc:{false,true}){
        claim={};item.kind=1;item.state=1;item.position={0,300,0};
        for(unsigned i=0;i<3;++i){p[i].eligible=true;p[i].auto_collect=false;p[i].power=i==0?100:i==1?80:20;p[i].position={float(i)*100,300,0};p[i].pickup=p[i].fast=p[i].slow={{1000,1000,0},{1001,1001,0}};}
        if(poc)p[0].position.y=100;else p[0].pickup={{-2,298,0},{2,302,0}};
        assert(select_item_collector(claim,item,p,3)==2&&claim.homing==2&&item.state==3);
        p[1].position={0,300,0};assert(select_item_collector(claim,item,p,3)==2);
        p[2].power=100;assert(select_item_collector(claim,item,p,3)==1);
        p[1].eligible=false;assert(select_item_collector(claim,item,p,3)==0);
    }
    claim={};item.state=1;item.kind=1;item.position={0,297,0};item.velocity={0,3,0};
    for(unsigned i=0;i<3;++i){p[i].eligible=true;p[i].auto_collect=false;p[i].position={float(i)*100,300,0};p[i].power=i==0?100:i==1?50:20;p[i].pickup=p[i].slow=p[i].fast={{1000,1000,0},{1001,1001,0}};}
    p[0].pickup={{-1,299,0},{1,301,0}};
    assert(select_item_collector(claim,item,p,3)==2&&item.state==3);
    std::puts("item collector, homing and targeted-transfer ownership: PASS");
}
