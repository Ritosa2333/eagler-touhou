// Hand-authored TH12 1.00b item semantics. Addresses and native samples:
// docs/reverse-engineering/UFO-TOKENS.md; scripts/reverse/inspect-ufo-*.py.
#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <functional>
#include <vector>

namespace th12::item_system {
constexpr float pi = 3.1415927410125732f;
struct Vec2 { float x=0, y=0; };
inline float add(float a,float b){return float(double(a)+b);}
inline Vec2 polar(float angle,float speed){return {float(std::cos(double(angle))*speed),float(std::sin(double(angle))*speed)};}
enum class State : int { Inactive=0, Falling=1, Automatic=2, CollectedAbove=3, CollectedNear=4, DelayedPoint=5, Token=6, Absorbing=7, Aggregate=8 };
enum class Kind : int { None=0, Power=1, Point=2, LargePower=3, LifePiece=4, BombPiece=5, Life=6, Bomb=7, FullPower=8, SmallPoint=9, Red=10, Blue=11, Green=12, ChangingRed=13, ChangingBlue=14, ChangingGreen=15, PowerAggregate=16, PointAggregate=17, MixedAggregate=18 };
struct Entity {
    uint32_t id=0;
    int type=0;
    State state=State::Inactive;
    Vec2 position{},velocity{};
    int age=0,colorTimer=0,delay=0;
    float elapsed=0,colorElapsed=0,attractionSpeed=0,aggregateSpeed=0;
    bool near=false;
    int aggregateKind=0,bucketA=0,bucketB=0;
    float multiplier=1;
    int visualScript=0,visualAge=0,visualInterrupt=0,visualInterruptAge=0;
    uint32_t visualInterruptVersion=0;
    float alpha=1;
    bool hintActive=false;
    int hintScript=211,hintInterrupt=0;
    bool active()const{return state!=State::Inactive;}
    bool visible()const{return active()&&state!=State::DelayedPoint;}
};
struct PlayerView {
    Vec2 position{};
    int character=0,state=0; // Retail state 2 disables pickup; state 4 cancels ordinary attraction.
    bool focus=false;
    float pickupHalfWidth=30,pickupHalfHeight=30,nearHalfWidth=30,nearHalfHeight=30,attractionSpeed=5;
    static PlayerView native(int character,Vec2 position,bool focus=false,int state=0){
        constexpr float normal[3]={30,35,30},focused[3]={50,59,50},speed[3]={5,7,6};
        PlayerView p;p.character=std::clamp(character,0,2);p.position=position;p.focus=focus;p.state=state;
        p.pickupHalfWidth=p.pickupHalfHeight=normal[p.character];
        p.nearHalfWidth=p.nearHalfHeight=(focus?focused:normal)[p.character];p.attractionSpeed=speed[p.character];return p;
    }
};
struct UfoView {bool active=false;int kind=0,age=0;Vec2 position{};};
struct FrameView {
    PlayerView player{};
    UfoView ufo{};
    bool dialogue=false;
    float rate=1;
    // Stable pointers allow a pickup callback to update later tokens in the same native pool traversal.
    const std::array<int,3>* inventoryColors=nullptr;
    const int* inventoryCount=nullptr;
};
enum class EventKind { CollectOrdinary,CollectToken,Absorbed,CollectAggregate,RankDelta,AnimationInterrupt };
struct Event {EventKind kind;Entity entity{};int value=0;};
using EventSink=std::function<void(const Event&)>;
struct SpawnRequest {int type=0;Vec2 position{};float angle=-pi/2,speed=2.2f;int aggregateKind=0,bucketA=0,bucketB=0;float multiplier=1;};

class Manager {
public:
    static constexpr size_t ordinaryCapacity=600,tokenCapacity=16,smallPointCapacity=2048,totalCapacity=2664;
    std::array<Entity,totalCapacity> entities{};
    uint32_t nextId=1;
    int tokenSideCounter=0,smallPointCursor=0,smallPointSpawns=0;
    uint32_t poolMisses=0,unsupportedTypes=0;
    std::vector<Event> events;
    void reset(int side=0){for(auto& e:entities)e=Entity{};nextId=1;tokenSideCounter=side;smallPointCursor=smallPointSpawns=0;poolMisses=unsupportedTypes=0;events.clear();}
    Entity* find(uint32_t id){for(auto& e:entities)if(e.id==id&&e.active())return &e;return nullptr;}
    const Entity* find(uint32_t id)const{for(const auto& e:entities)if(e.id==id&&e.active())return &e;return nullptr;}
    size_t activeCount()const{size_t n=0;for(const auto& e:entities)n+=e.active();return n;}
    Entity* spawn(const SpawnRequest& r){
        if(r.type<1||r.type>18){if(r.type)++unsupportedTypes;return nullptr;}
        // Native increments before searching the 16-slot pool, including failed token births.
        if(r.type>=10&&r.type<=15)++tokenSideCounter;
        Entity* e=nullptr;
        if(r.type==9){
            e=&entities[ordinaryCapacity+tokenCapacity+smallPointCursor];
            ++smallPointSpawns;const int ordinal=smallPointCursor;smallPointCursor=(smallPointCursor+1)&2047;
            if(e->active()){++poolMisses;return nullptr;}
            *e={};e->state=State::DelayedPoint;
            e->delay=smallPointSpawns>=1024?(ordinal&31)+16:smallPointSpawns>=512?(ordinal&15)+8:smallPointSpawns>=256?(ordinal&7)+4:ordinal&3;
        }else{
            const size_t first=r.type>=10?ordinaryCapacity:0,last=r.type>=10?ordinaryCapacity+tokenCapacity:ordinaryCapacity;
            for(size_t i=first;i<last;i++)if(!entities[i].active()){e=&entities[i];break;}
            if(!e){++poolMisses;return nullptr;}
            *e={};e->state=r.type>=16?State::Aggregate:r.type>=10?State::Token:State::Falling;
        }
        e->id=nextId++;e->type=r.type;e->position=r.position;
        if(r.type!=9)e->position.x=std::clamp(e->position.x,-192.f,192.f);
        if(r.type>=16)e->position.y=std::max(0.f,e->position.y);
        if(r.type>=10&&r.type<=15)e->velocity=polar((tokenSideCounter&1)?float(pi*.75):float(pi*.25),1.5f);
        else if(r.type<16)e->velocity=polar(r.angle,r.speed);
        e->visualScript=r.type+165;e->aggregateKind=r.aggregateKind;e->bucketA=r.bucketA;e->bucketB=r.bucketB;e->multiplier=r.multiplier;
        return e;
    }
    Entity* spawn(int type,Vec2 position,float angle=-pi/2,float speed=2.2f){return spawn(SpawnRequest{type,position,angle,speed});}
    void tick(const FrameView& frame,const EventSink& sink={}){
        events.clear();
        const auto emit=[&](EventKind kind,const Entity& entity,int value=0){Event ev{kind,entity,value};events.push_back(ev);if(sink)sink(ev);};
        const float rate=frame.rate;const auto& p=frame.player;
        const auto move=[&](Entity& e){e.position.x=add(e.position.x,float(double(e.velocity.x)*rate));e.position.y=add(e.position.y,float(double(e.velocity.y)*rate));};
        const auto home=[&](Entity& e,float speed,Vec2 target){
            const float dx=float(double(target.x)-e.position.x),dy=float(double(target.y)-e.position.y);
            const float angle=dx==0&&dy==0?pi/2:float(std::atan2(double(dy),double(dx)));
            e.velocity=polar(angle,speed);move(e);
        };
        const auto interrupt=[&](Entity& e,int value){e.visualInterrupt=value;e.visualInterruptAge=0;++e.visualInterruptVersion;emit(EventKind::AnimationInterrupt,e,value);};
        const auto setAge=[](Entity& e,float value){e.elapsed=value;e.age=int(value);};
        const auto setColor=[](Entity& e,float value){e.colorElapsed=value;e.colorTimer=int(value);};
        for(auto& e:entities){
            if(!e.active())continue;
            if(e.state==State::DelayedPoint){if(--e.delay<0){e.state=State::Automatic;e.visualAge=0;}continue;}
            if(e.state==State::Falling&&((p.state!=2&&p.state!=4&&p.position.y<128)||frame.dialogue)){e.state=State::CollectedAbove;e.attractionSpeed=p.attractionSpeed;}
            if((e.state==State::Falling||e.state==State::Automatic||e.state==State::CollectedAbove||e.state==State::CollectedNear||e.state==State::Token)&&frame.ufo.active&&frame.ufo.age>=60&&frame.ufo.kind!=0&&(e.type==1||e.type==2)){
                e.state=State::Absorbing;e.velocity={};e.attractionSpeed=0;
            }
            if(e.state==State::Falling||e.state==State::Automatic){
                move(e);e.velocity.y=add(e.velocity.y,float(double(.03f)*rate));
                if(e.state==State::Falling&&e.velocity.y>=0)e.velocity.x=0;
                e.velocity.y=std::min(e.velocity.y,2.f);
                if(e.state==State::Automatic&&e.velocity.y>=0){
                    e.state=State::CollectedAbove;e.attractionSpeed=p.attractionSpeed;home(e,e.attractionSpeed,p.position);
                    if(e.attractionSpeed<12)e.attractionSpeed=add(e.attractionSpeed,.2f);
                    if(p.state==4){e.velocity={};e.state=State::Falling;}
                }
                if(e.position.y>=472){const bool rank=e.state==State::Automatic;e.state=State::Inactive;if(rank)emit(EventKind::RankDelta,e,-4);continue;}
            }else if(e.state==State::CollectedAbove||e.state==State::CollectedNear){
                home(e,e.attractionSpeed,p.position);if(e.attractionSpeed<12)e.attractionSpeed=add(e.attractionSpeed,.2f);
                if(p.state==4){e.velocity={};e.state=State::Falling;}
            }else if(e.state==State::Aggregate){
                home(e,e.aggregateSpeed,p.position);if(e.age>=60&&e.aggregateSpeed<12)e.aggregateSpeed=add(e.aggregateSpeed,.2f);
                if(p.state==4){e.velocity={};e.state=State::Falling;}
            }else if(e.state==State::Token){
                if(frame.dialogue)setAge(e,10000);
                if(e.type>=13&&e.type<=15){
                    const double dx=double(e.position.x)-p.position.x,dy=double(e.position.y)-p.position.y;
                    const float distance=float(dx*dx+dy*dy);
                    if(distance<=3600){
                        setColor(e,add(e.colorElapsed,-rate));setAge(e,add(e.elapsed,-rate));
                        if(e.colorTimer==90)setColor(e,add(e.colorElapsed,-rate));
                        if(!e.near){e.near=true;interrupt(e,7);}
                    }else if(e.near){interrupt(e,e.colorTimer>=90?2:3);e.near=false;}
                    if(e.colorTimer==90)interrupt(e,2);
                    else if(e.colorTimer>=150){e.type=13+(e.type-12)%3;e.visualScript=e.type+165;e.visualAge=0;e.visualInterrupt=0;e.hintActive=false;setColor(e,0);}
                    setColor(e,add(e.colorElapsed,rate));
                }
                move(e);
                if(e.age<1200){
                    if((e.position.x<=-176&&e.velocity.x<0)||(e.position.x>=176&&e.velocity.x>0))e.velocity.x=-e.velocity.x;
                    if((e.position.y<=160&&e.velocity.y<0)||(e.position.y>=370&&e.velocity.y>0))e.velocity.y=-e.velocity.y;
                }else if(e.position.x<=-208||e.position.x>=208||e.position.y<=-16||e.position.y>=464){e.state=State::Inactive;emit(EventKind::RankDelta,e,-4);continue;}
                e.hintActive=false;
                if(frame.inventoryColors&&(!frame.inventoryCount||*frame.inventoryCount<3)){
                    const auto& colors=*frame.inventoryColors;const int color=(e.type-10)%3+1;
                    e.hintActive=colors[0]&&colors[1]&&((colors[0]==color&&colors[1]==color)||(colors[0]!=colors[1]&&colors[0]!=color&&colors[1]!=color));
                    if(e.hintActive)e.hintInterrupt=color+6;
                }
            }else if(e.state==State::Absorbing){
                if(!frame.ufo.active){e.velocity={};e.state=State::Falling;}
                else{
                    home(e,e.attractionSpeed,frame.ufo.position);
                    if(e.attractionSpeed<6)e.attractionSpeed=add(e.attractionSpeed,float(double(frame.ufo.age)/600*.05f+.02f));
                    const float dx=float(double(frame.ufo.position.x)-e.position.x),dy=float(double(frame.ufo.position.y)-e.position.y);
                    if(float(double(dx)*dx+double(dy)*dy)<324){const Entity old=e;e.state=State::Inactive;emit(EventKind::Absorbed,old,e.type);continue;}
                }
            }
            const float dx=std::abs(e.position.x-p.position.x),dy=std::abs(e.position.y-p.position.y);
            if(p.state!=2&&dx<=p.pickupHalfWidth&&dy<=p.pickupHalfHeight){
                const Entity old=e;e.state=State::Inactive;
                emit(old.type>=16?EventKind::CollectAggregate:old.type>=10?EventKind::CollectToken:EventKind::CollectOrdinary,old,old.type>=10&&old.type<=15?(old.type-10)%3:old.type);
                continue;
            }
            if(e.state!=State::CollectedAbove&&e.state!=State::CollectedNear&&e.state!=State::Token&&e.state!=State::Absorbing&&e.state!=State::Aggregate&&dx<p.nearHalfWidth&&dy<p.nearHalfHeight){e.state=State::CollectedNear;e.attractionSpeed=float(double(p.attractionSpeed)/3);}
            setAge(e,add(e.elapsed,rate));++e.visualAge;++e.visualInterruptAge;
        }
        smallPointSpawns=0;
    }
};

// Retail power: 100 units per level. scoreStored is displayed score / 10.
// pointValueStored is displayed point value * 100; fragments retain native counters.
struct ResourceState {
    int power=100,maxPower=400,powerStep=100,scoreStored=0,pointValueStored=1000000,maxPointValueStored=20000000;
    int lives=2,bombs=2,lifeFragments=0,bombFragments=0,rank=0,difficulty=1;
    static ResourceState initial(int difficulty){
        constexpr int base[5]={5000,10000,15000,20000,20000},cap[5]={100000,200000,300000,400000,500000};
        ResourceState r;r.difficulty=std::clamp(difficulty,0,4);r.pointValueStored=base[r.difficulty]*100;r.maxPointValueStored=cap[r.difficulty]*100;return r;
    }
    int pointValue()const{return (pointValueStored/100)/10*10;}
    void score(int displayed){scoreStored=int(std::min<int64_t>(999999999,int64_t(scoreStored)+displayed/10));}
    void addRank(int n){rank=std::clamp(rank+n,-1024,1024);}
    void addPointValue(int stored){pointValueStored=std::min(maxPointValueStored,pointValueStored+stored);}
};
struct RewardDelta {int scoreStored=0,power=0,pointValueStored=0,lives=0,bombs=0,lifeFragments=0,bombFragments=0,rank=0;};
inline RewardDelta difference(const ResourceState& before,const ResourceState& after){return {after.scoreStored-before.scoreStored,after.power-before.power,after.pointValueStored-before.pointValueStored,after.lives-before.lives,after.bombs-before.bombs,after.lifeFragments-before.lifeFragments,after.bombFragments-before.bombFragments,after.rank-before.rank};}
inline void smallPower(ResourceState& r){if(r.power>=r.maxPower){r.score(10);return;}const int previous=r.power;r.power=std::min(r.maxPower,r.power+1);if(previous/r.powerStep!=r.power/r.powerStep)r.addRank(12);}
inline RewardDelta collectOrdinary(ResourceState& r,const Entity& item,const PlayerView& player){
    const ResourceState before=r;
    const auto increasePower=[&](int amount,int rank){const int previous=r.power;r.power=std::min(r.maxPower,r.power+amount);if(previous/r.powerStep!=r.power/r.powerStep)r.addRank(rank);};
    switch(item.type){
    case 1:smallPower(r);break;
    case 2:{int value=r.pointValue();if(player.position.y>128&&item.state!=State::CollectedAbove){value=value*3/4;value-=((int(player.position.y)-128)*value)/450;}r.score(std::max(10,value/10*10));break;}
    case 3:if(r.power>=r.maxPower)r.addPointValue(20000);else increasePower(r.powerStep,24);break;
    case 4:if(r.lives>=9)r.lifeFragments=0;else{r.lifeFragments+=r.lifeFragments?1:2;if(r.lifeFragments>=5){r.lifeFragments=0;r.lives=std::min(8,r.lives+1);}}r.addRank(256);break;
    case 5:if(r.bombs>=9)r.bombFragments=0;else{r.bombFragments+=2;if(r.bombFragments>=5){r.bombFragments=0;r.bombs=std::min(9,r.bombs+1);}}r.addRank(256);break;
    case 6:r.lives=std::min(8,r.lives+1);r.addRank(256);break;
    case 7:r.bombs=std::min(9,r.bombs+1);r.addRank(256);break;
    case 8:if(r.power>=r.maxPower)r.addPointValue(10000);else increasePower(r.maxPower,12);break;
    case 9:r.addPointValue(10);r.score(10);break;
    default:break;
    }
    return difference(before,r);
}
inline RewardDelta collectAggregate(ResourceState& r,const Entity& item){
    const ResourceState before=r;for(int i=0;i<item.bucketA;i++)smallPower(r);
    const int value=r.pointValue();int displayed;
    if(item.aggregateKind==1)displayed=int(double(value*item.bucketA)*item.multiplier)+value*item.bucketB;
    else displayed=int(double(value*item.bucketB)*item.multiplier);
    r.score(std::max(10,displayed/10*10));return difference(before,r);
}
struct DeathDropPlan {RewardDelta immediate{};std::vector<SpawnRequest> spawns;};
inline DeathDropPlan deathPowerDrops(ResourceState& resources,Vec2 position){
    // Original player state 2, timer==3: 436de6 / 439440 / 436e3d.
    const ResourceState before=resources;resources.power=std::max(resources.powerStep,resources.power-resources.powerStep);
    DeathDropPlan plan;plan.immediate=difference(before,resources);
    const float base=float(std::atan2(-224.0,-double(position.x)));
    for(int i=0;i<7;i++)plan.spawns.push_back(SpawnRequest{1,position,float(double(base)+double(i)*pi/28-double(pi/8)),3.f});
    return plan;
}
} // namespace th12::item_system
