#include "../th10_web/cpp/multiplayer/SessionRecords.hpp"
#include <cassert>
#include <cstring>
#include <memory>
#include <cstdio>
using namespace th10;
static i32 count(const u8* at){i32 n;std::memcpy(&n,at,4);return n;}
static void count(u8* at,i32 n){std::memcpy(at,&n,4);}
int main(){
    std::int8_t difficulties[110]{};
    auto a=std::make_unique<ScoreData>(),b=std::make_unique<ScoreData>();
    Rng rng{};for(auto* data:{a.get(),b.get()}){
        for(auto& record:data->characters)record.initialize(difficulties);
        data->settings.initialize(rng);
    }
    a->characters[0].high_scores[0][0].score=9000000;
    count(a->characters[0].statistics,100);count(b->characters[0].statistics,3);
    std::memcpy(a->settings.last_name,"ALICE   ",9);
    std::memcpy(b->settings.last_name,"BOB     ",9);
    const auto originalA=std::make_unique<ScoreData>(*a),originalB=std::make_unique<ScoreData>(*b);
    multiplayer::SessionRecords first,second;
    assert(first.Begin(*a,difficulties,false)&&second.Begin(*b,difficulties,true));
    assert(!std::memcmp(a->characters,b->characters,sizeof(a->characters)));
    assert(!std::memcmp(&a->settings,&b->settings,sizeof(a->settings)));
    assert(!first.Begin(*a,difficulties,false));
    count(a->characters[0].statistics,2);count(a->characters[0].statistics+4,1000);
    a->characters[0].spells[0].attempts=2;a->characters[0].spells[0].captures=1;
    auto& scores=a->characters[0].high_scores[0];for(int i=9;i>0;--i)scores[i]=scores[i-1];
    scores[0]={};scores[0].score=2000000;scores[0].stage=8;
    std::memcpy(scores[0].name,"TEAM    ",9);std::memcpy(a->settings.last_name,"TEAM    ",9);
    const auto beforeCommit=std::make_unique<ScoreData>(*a);
    assert(first.Checkpoint(*a,123456)&&first.Checkpoint(*a,999999));
    assert(!std::memcmp(a->characters,beforeCommit->characters,sizeof(a->characters)));
    assert(count(first.Persistent()->characters[0].statistics)==102);
    assert(count(first.Persistent()->characters[0].statistics+4)==1000);
    assert(first.Persistent()->characters[0].high_scores[0][0].score==9000000);
    assert(first.Persistent()->characters[0].high_scores[0][1].score==2000000);
    assert(first.Persistent()->characters[0].high_scores[0][1].timestamp==123456);
    assert(first.Persistent()->characters[0].spells[0].attempts==2);
    count(a->characters[0].statistics,500); // speculative/uncheckpointed future
    first.Finish(*a);assert(count(a->characters[0].statistics)==102);
    count(b->characters[0].statistics,1000);
    assert(second.Checkpoint(*b,123456));second.Finish(*b);
    assert(!std::memcmp(b->characters,originalB->characters,sizeof(b->characters)));
    assert(!std::memcmp(&b->settings,&originalB->settings,sizeof(b->settings)));
    assert(!std::memcmp(a->settings.random_words,originalA->settings.random_words,sizeof(a->settings.random_words)));
    std::puts("MP run-local records, confirmed merge and read-only restoration: PASS");
}
