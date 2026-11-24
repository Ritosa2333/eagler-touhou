#pragma once
#include "game/PlayerHitRules.hpp"
#include "game/EnemyPhaseRules.hpp"
#include <limits>
#include <array>
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
namespace th128::laser::selector3 {
struct Position {float x=0,y=0,z=0;};
struct Descriptor {
    Position source,velocity;
    float angle=0,angularVelocity=0,length=0,width=0,offset=0;
    int lifetime=0,identifier=0;
    std::uint32_t color=0;
    bool followBoss=false;
};
struct State {
    Position source,velocity;
    float angle=0,angularVelocity=0,length=0,width=1,targetWidth=0,previousWidth=0,offset=0;
    int lifetime=0,identifier=0,age=0,kind=3;
    std::uint32_t color=0;
    unsigned retirement=0;
    bool followBoss=false;
    std::array<float,512> lengths{},rounded{};
};
struct Circle {Position position;float radius=0;};
struct FrameInput {Position player;std::optional<Position> boss0;std::vector<Circle> ice,enemyBlockers;};
struct StepResult {unsigned playerColumns=0;};
struct Vertex {float x=0,y=0,z=0,rhw=1;std::uint32_t color=0;};
struct Quad {unsigned column=0;std::array<Vertex,4> vertices;};
inline bool finite(float value){return std::isfinite(value)&&std::abs(value)<=65536;}
inline bool finite(Position value){return finite(value.x)&&finite(value.y)&&finite(value.z);}
inline float normalize(float value){
    for(unsigned step=0;step<34&&value>3.1415927410125732;++step)value=float(double(value)-6.2831854820251465);
    for(unsigned step=0;step<34&&value< -3.1415927410125732;++step)value=float(double(value)+6.2831854820251465);
    return value;
}
inline Position vector(float angle,float length){return{float(std::cos(double(angle))*length),float(std::sin(double(angle))*length),0};}
inline Position add(Position first,Position second){return{float(double(first.x)+second.x),float(double(first.y)+second.y),float(double(first.z)+second.z)};}
inline bool initialize(const Descriptor& input,State& state,std::string& error){
    error.clear();
    if(!finite(input.source)||!finite(input.velocity)||!finite(input.angle)||!finite(input.angularVelocity)
        ||!finite(input.length)||input.length<0||!finite(input.width)||input.width<=0||input.width>512
        ||std::trunc(input.width)!=input.width||!finite(input.offset)||input.lifetime<0||input.lifetime>1000000||input.identifier<=0){
        error="original selector3 descriptor numeric or column-owner domain rejected";return false;
    }
    state={};state.source=input.source;state.velocity=input.velocity;state.angle=normalize(input.angle);
    state.angularVelocity=input.angularVelocity;state.length=input.length;state.targetWidth=input.width;state.offset=input.offset;
    state.lifetime=input.lifetime;state.identifier=input.identifier;state.color=input.color;state.followBoss=input.followBoss;
    state.lengths.fill(input.length);state.rounded.fill(input.length);return true;
}
inline void advanceWidth(State& state){
    if(state.age==state.lifetime)state.targetWidth=0;
    if(state.width<state.targetWidth)state.width=std::min(float(double(state.width)+4.),state.targetWidth);
    else if(state.width>state.targetWidth){state.width=std::max(float(double(state.width)-4.),state.targetWidth);if(state.width<=0)state.retirement=1;}
}
inline std::optional<std::array<float,2>> rayCircle(Position origin,float angle,Circle circle){
    if(!finite(origin)||!finite(angle)||!finite(circle.position)||!finite(circle.radius)||circle.radius<0)return std::nullopt;
    const float horizontal=float(double(circle.position.x)-origin.x),vertical=float(double(circle.position.y)-origin.y);
    const float sine=float(std::sin(double(-angle))),cosine=float(std::cos(double(-angle)));
    const float along=float(double(horizontal)*cosine-double(vertical)*sine);
    const float lateral=float(double(horizontal)*sine+double(vertical)*cosine);
    if(std::abs(lateral)>circle.radius||along< -circle.radius)return std::nullopt;
    if(along<0&&double(along)*along+double(lateral)*lateral>double(circle.radius)*circle.radius)return std::nullopt;
    if(circle.radius==0){if(lateral!=0)return std::nullopt;return std::array<float,2>{along,along};}
    const float ratio=float(double(lateral)/circle.radius);
    const float squared=float(1.-double(ratio)*ratio);
    const float root=float(std::sqrt(double(std::max(0.f,squared))));
    const float distance=float(double(root)*circle.radius);
    return std::array<float,2>{float(double(along)-distance),float(double(along)+distance)};
}
inline bool advance(State& state,const FrameInput& input,StepResult& result,std::string& error){
    error.clear();result={};
    if(state.retirement||state.kind!=3||state.age<0||state.age>=1000000||!finite(input.player)
        ||state.width<0||state.width>512||!finite(state.width)){error="selector3 update requires current live finite owner";return false;}
    state.angle=normalize(float(double(state.angle)+state.angularVelocity));
    if(state.followBoss&&input.boss0){if(!finite(*input.boss0)){error="selector3 actual boss0 position is invalid";return false;}state.source=*input.boss0;}
    state.source=add(state.source,state.velocity);
    advanceWidth(state);if(state.retirement)return true;
    const auto columns=unsigned(state.width);
    if(state.width!=float(columns)||columns>512){error="selector3 original integer column extent rejected";return false;}
    if(state.previousWidth!=state.width){
        state.previousWidth=state.width;
        const float half=float(double(state.width)*.5);
        for(unsigned column=0;column<columns;++column){
            const float ratio=float((double(half)-column)/half);
            const float arc=float(std::acos(double(std::clamp(ratio,-1.f,1.f))));
            const float sine=float(std::sin(double(arc)));
            state.rounded[column]=float((1.-double(sine))*state.width*.5);
        }
    }
    auto origin=add(state.source,vector(normalize(float(double(state.angle)+1.5707963705062866)),float(double(state.width)*.5-.5)));
    origin=add(origin,vector(state.angle,state.offset));
    const auto lateralStep=vector(normalize(float(double(state.angle)-1.5707963705062866)),1);
    const auto direction=vector(state.angle,1);
    for(unsigned column=0;column<columns;++column){
        const auto start=add(origin,{float(double(direction.x)*state.rounded[column]),float(double(direction.y)*state.rounded[column]),0});
        float nearest=9999;
        for(const auto& circle:input.ice){const auto contact=rayCircle(start,state.angle,circle);if(contact)nearest=std::min(nearest,std::max(0.f,(*contact)[0]));}
        for(const auto& circle:input.enemyBlockers){const auto contact=rayCircle(start,state.angle,circle);if(contact)nearest=std::min(nearest,std::max(0.f,(*contact)[0]));}
        if(nearest<9999)state.lengths[column]=nearest;else if(state.lengths[column]<state.length)state.lengths[column]=float(double(state.lengths[column])+320.);
        if(state.width>0){const auto contact=rayCircle(start,state.angle,{input.player,6});if(contact&&(*contact)[0]<state.lengths[column]&&(*contact)[1]>0){++result.playerColumns;state.lengths[column]=std::max(0.f,(*contact)[0]);}}
        origin=add(origin,lateralStep);
    }
    ++state.age;return true;
}
inline std::array<Vertex,4> nativeColumnQuad(Position start,float length,float thickness,float angle,std::uint32_t color){
    const float cosine=float(std::cos(double(angle))),sine=float(std::sin(double(angle)));
    return{{{start.x,start.y,start.z,1,color},
        {float(double(start.x)+double(cosine)*length),float(double(start.y)+double(sine)*length),start.z,1,color},
        {float(double(start.x)-double(sine)*thickness),float(double(start.y)+double(cosine)*thickness),start.z,1,color},
        {float(double(start.x)+double(cosine)*length-double(sine)*thickness),float(double(start.y)+double(sine)*length+double(cosine)*thickness),start.z,1,color}}};
}
inline std::optional<std::vector<Quad>> columnQuads(const State& state,std::string& error){
    error.clear();if(state.width<0||state.width>512||std::trunc(state.width)!=state.width){error="selector3 draw column extent invalid";return std::nullopt;}
    std::vector<Quad> result;
    auto origin=add(state.source,{320,16,0});
    origin=add(origin,vector(normalize(float(double(state.angle)+1.5707963705062866)),float(double(state.width)*.5)));
    origin=add(origin,vector(state.angle,state.offset));
    const auto lateralStep=vector(normalize(float(double(state.angle)-1.5707963705062866)),1);
    const auto direction=vector(state.angle,1);
    for(unsigned column=0;column<unsigned(state.width);++column){
        if(state.lengths[column]!=0){
            auto start=add(origin,{float(double(direction.x)*state.rounded[column]),float(double(direction.y)*state.rounded[column]),0});
            result.push_back({column,nativeColumnQuad(start,state.lengths[column],1.2999999523162842f,state.angle,state.color)});
        }
        origin=add(origin,lateralStep);
    }
    return result;
}
inline bool contactSoundRequired(unsigned columns,const player::hit::PlayerGate& gate) noexcept{
    return columns>0&&columns<=512&&gate.counter438dc==0&&!(gate.flags438ec&2u);
}
inline bool applyContactTail(unsigned columns,player::hit::PlayerGate& gate,std::int32_t& counter,
                             std::int32_t& motivation,std::int32_t& flag,enemy::PhaseSpell& spell) noexcept{
    if(columns>512)return false;
    if(!contactSoundRequired(columns,gate))return true;
    const auto drain=std::int32_t(columns*5u);
    if(!flag&&motivation<std::numeric_limits<std::int32_t>::min()+drain)return false;
    if(!flag){
        motivation-=drain;
        if(motivation>100000)motivation=100000;
        else if(motivation<0)flag=1;
    }
    if(motivation<0){motivation=0;flag=0;}
    spell.baseBonus=std::max(std::bit_cast<std::int32_t>(std::uint32_t(spell.baseBonus)-columns*20u),10);
    spell.flags|=0x100u;gate.flags438ec|=8u;
    counter=std::bit_cast<std::int32_t>(std::uint32_t(counter)+1u);return true;
}
inline std::int32_t beginContactPhase(player::hit::PlayerGate& gate,std::int32_t& counter) noexcept{
    const auto previous=counter;
    if(counter!=0){
        counter=std::bit_cast<std::int32_t>(std::uint32_t(counter)-1u);
        if(counter<=0){counter=0;gate.flags438ec&=~8u;}
    }
    return previous;
}
inline bool contactEntryEffectRequired(std::int32_t previous,std::int32_t current) noexcept{
    return previous==0&&current!=0;
}
}
