#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

// TH12 1.00b movement: 413840 / 464d50 / 464db0 / 465320.
// Each named float temporary represents a retail fstp dword boundary.
namespace th12::enemy_motion {
inline constexpr float pi=3.1415927410125732421875f;
inline constexpr float tau=6.283185482025146484375f;
inline constexpr float keep=-999999.f;
struct Vec2 {float x=0,y=0;};
inline float quantize(float value){
    const float scaled=float(double(value)*100.0);
    const float integral=float(std::floor(double(scaled)));
    return float(double(integral)/100.0);
}
inline Vec2 quantize(Vec2 value){return {quantize(value.x),quantize(value.y)};}
inline float normalize(float value){
    int wraps=0;
    while(value>pi&&wraps++<=33)value=float(double(value)-tau);
    while(value<-pi&&wraps++<=33)value=float(double(value)+tau);
    return value;
}
inline float normalizedTwice(float value){return normalize(normalize(value));}
inline Vec2 polar(float angle,float radius){
    return {float(std::cos(double(angle))*radius),float(std::sin(double(angle))*radius)};
}
inline float linearPair(float start,float end,float fraction){
    const float difference=float(double(end)-start);
    const float product=float(double(difference)*fraction);
    return float(double(start)+product);
}
// Other easing modes are supplied by the core's verified interpolation table.
inline float sampledEasing(float fraction,int mode){
    const double t=fraction;
    if(mode==4)return float(1.0-(1.0-t)*(1.0-t));
    return fraction;
}
struct Motion {
    Vec2 position{},vector{}; // vector is linear velocity or the circle center.
    float speed=0,angle=0,radius=0,radiusDelta=0;
    std::uint32_t flags=0; // bit 0 circle, bit 1 ellipse (ellipse not restored here).
    bool circular()const{return flags&1;}
};
inline void geometry(Motion& motion){
    if(motion.circular()){
        const auto offset=polar(motion.angle,motion.radius);
        motion.position={float(double(motion.vector.x)+offset.x),float(double(motion.vector.y)+offset.y)};
    }
    motion.position=quantize(motion.position);
}
inline void advanceParameters(Motion& motion){
    if(motion.circular()){
        motion.radius=float(double(motion.radius)+motion.radiusDelta);
        motion.angle=normalizedTwice(float(double(motion.angle)+motion.speed));
    }else motion.vector=polar(motion.angle,motion.speed);
}
inline void advancePosition(Motion& motion){
    if(!motion.circular())motion.position={float(double(motion.position.x)+motion.vector.x),float(double(motion.position.y)+motion.vector.y)};
    geometry(motion);
}
inline void setCircle(Motion& motion,float angle,float speed,float radius,float delta){
    if(!motion.circular())motion.vector=motion.position;
    motion.flags|=1;
    if(angle>keep)motion.angle=normalizedTwice(angle);
    if(speed>keep)motion.speed=speed;
    if(radius>keep)motion.radius=radius;
    if(delta>keep)motion.radiusDelta=delta;
    geometry(motion);
}
struct PairInterpolation {
    Vec2 start{},end{};
    int current=0,duration=0,mode=0;
    void begin(Vec2 from,Vec2 to,int ticks,int interpolationMode){
        duration=std::max(0,ticks);
        if(!duration)return;
        start=from;end=to;current=0;mode=interpolationMode;
    }
    template<class Ease> Vec2 tick(Ease ease){
        if(++current>=duration){current=duration;duration=0;return end;}
        const float fraction=float(double(current)/duration);
        const float amount=ease(fraction,mode);
        return {linearPair(start.x,end.x,amount),linearPair(start.y,end.y,amount)};
    }
};
struct Movement {
    Motion absolute{},relative{},combined{};
    PairInterpolation absolutePolar{},relativePolar{},absoluteRadius{},relativeRadius{};
    bool clampEnabled=false;
    Vec2 clampCenter{},clampSize{};
};
inline void combine(Movement& movement){
    const Vec2 sum{float(double(movement.absolute.position.x)+movement.relative.position.x),float(double(movement.absolute.position.y)+movement.relative.position.y)};
    movement.combined.vector={float(double(sum.x)-movement.combined.position.x),float(double(sum.y)-movement.combined.position.y)};
    advancePosition(movement.combined);
    if(movement.clampEnabled){
        auto& p=movement.combined.position;
        p.x=std::clamp(p.x,float(double(movement.clampCenter.x)-double(movement.clampSize.x)*.5),float(double(movement.clampCenter.x)+double(movement.clampSize.x)*.5));
        p.y=std::clamp(p.y,float(double(movement.clampCenter.y)-double(movement.clampSize.y)*.5),float(double(movement.clampCenter.y)+double(movement.clampSize.y)*.5));
        movement.absolute.position={float(double(p.x)-movement.relative.position.x),float(double(p.y)-movement.relative.position.y)};
    }
}
inline void setCircle(Movement& movement,bool relative,float angle,float speed,float radius,float delta){
    setCircle(relative?movement.relative:movement.absolute,angle,speed,radius,delta);combine(movement);
}
inline void interpolateCircle(Movement& movement,bool relative,int duration,int mode,float speed,float radius,float delta){
    auto& m=relative?movement.relative:movement.absolute;
    auto& polarPair=relative?movement.relativePolar:movement.absolutePolar;
    auto& radiusPair=relative?movement.relativeRadius:movement.absoluteRadius;
    polarPair.begin({0,m.speed},{0,speed<=keep?m.speed:speed},duration,mode);
    radiusPair.begin({m.radius,m.radiusDelta},{radius<=keep?m.radius:radius,delta<=keep?m.radiusDelta:delta},duration,mode);
    if(duration>0){m.flags|=1;geometry(m);combine(movement);}
}
template<class Ease> inline void interpolateParameters(Motion& m,PairInterpolation& polarPair,PairInterpolation& radiusPair,Ease ease){
    if(polarPair.duration){const auto v=polarPair.tick(ease);if(!m.circular())m.angle=normalizedTwice(v.x);m.speed=v.y;}
    if(radiusPair.duration){const auto v=radiusPair.tick(ease);m.radius=v.x;m.radiusDelta=v.y;}
}
template<class Ease> inline void tick(Movement& movement,Ease ease){
    interpolateParameters(movement.absolute,movement.absolutePolar,movement.absoluteRadius,ease);
    interpolateParameters(movement.relative,movement.relativePolar,movement.relativeRadius,ease);
    advanceParameters(movement.absolute);advanceParameters(movement.relative);
    advancePosition(movement.absolute);advancePosition(movement.relative);combine(movement);
}
inline void tick(Movement& movement){tick(movement,sampledEasing);}
} // namespace th12::enemy_motion
