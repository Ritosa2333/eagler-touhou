#include "StageSelection.hpp"
#include <algorithm>
namespace th09 {
namespace {
struct RouteTable {const StageRoute* rows;u32 size;};
#include "StageData.inc"
}
i32 StageSelection::story_policy(i32 difficulty,i32 stage,i32 retry_index)noexcept{
    if(difficulty<0||difficulty>=5||stage<0||stage>=9||retry_index<0)return 0;
    const i32 index=(difficulty*9+stage)*5+retry_index;
    if(index<225)return cpuPolicies[index/45][index/5%9][index%5];
    return index<230?versusPolicies[index-225]:0;
}
std::vector<StageRoute> StageSelection::candidates(const StageSelectionState& s){
    std::vector<StageRoute> out;if(s.mode==GameMode::versus||s.characters[0]<0||s.characters[0]>=14||s.stage<0||s.stage>8)return out;
    const auto& table=routes[s.characters[0]];for(u32 n=0;n<table.size;++n){const auto& r=table.rows[n];if(r.stage==s.stage&&r.opponent>=0&&r.opponent<16&&!s.visited[r.opponent])out.push_back(r);}return out;
}
bool StageSelection::select(StageSelectionState& s,Rng& random,StageSelectionActions& actions){
    if(s.difficulty<0||s.difficulty>4)return false;
    if(s.mode!=GameMode::versus){
        if(s.characters[0]<0||s.characters[0]>=14||s.stage<0||s.stage>8)return false;
        const auto& route=routes[s.characters[0]];std::array<const StageRoute*,16> candidates{};u32 count=0,weight=0;
        for(u32 i=0;i<route.size;++i)if(route.rows[i].stage==s.stage){if(count>=candidates.size())return false;candidates[count++]=&route.rows[i];weight+=u32(i32(route.rows[i].weight));}
        if(!count)return false;u32 choice=random.bounded32(weight);const StageRoute* selected=nullptr;
        for(u32 i=0;i<count;++i){const auto& c=*candidates[i];if(c.opponent<0||c.opponent>=16)return false;if(i16(choice)<c.weight&&!s.visited[c.opponent]){selected=&c;break;}choice-=u16(c.weight);}
        if(!selected)for(u32 i=0;i<count;++i)if(!s.visited[candidates[i]->opponent]){selected=candidates[i];break;}
        if(!selected)return false;s.route=*selected;s.background=selected->background;s.characters[1]=selected->opponent;actions.encounter(s.characters[1]);s.visited[s.characters[1]]=1;
        s.cpu_levels[1]=story_policy(s.difficulty,s.stage,!s.continued&&s.lives>0?0:std::min(s.round,3)+1);return true;
    }
    s.stage=9;i32 character=s.selector;
    if(character==-1)character=s.characters[random.bounded32(2)];
    else if(character==-2){
        u32 count=0;for(u32 i=0;i<14;++i)if(s.unlocked[i]||s.all_unlocked)++count;
        u32 choice=random.bounded32(count);character=16;
        for(i32 i=0;i<16;++i)if(s.unlocked[i]||s.all_unlocked){if(!choice){character=i;break;}--choice;}
    }
    s.route=versusRoutes[0];for(const auto& route:versusRoutes)if(route.opponent==character){s.route=route;break;}
    s.background=s.route.background;s.cpu_levels[0]=s.cpu_levels[1]=versusPolicies[s.difficulty];return true;
}
}
