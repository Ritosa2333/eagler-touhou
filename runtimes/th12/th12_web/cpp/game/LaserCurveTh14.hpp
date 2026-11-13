#pragma once
#include "Types.hpp"
#if PORT_TITLE == 14
#include <vector>
#include <array>
#endif
namespace th11 {
struct Timer;
#if PORT_TITLE == 14
// The payload of the retail 0x3c-byte analytic node. Links stay in the owner.
struct Th14CurveNode {
    float start=0,end=999999;
    i32 mode=0;
    Vec3 direction{},position{};
    float angle=0,speed=0,acceleration=0,turn=0;
};
struct Th14CurveSample {
    Vec3 position{},auxiliary{};
    float angle=0,speed=0;
};
// 440f60 / 441430: head evaluation and recurrence along the sampled tail.
// These are independent of ANM, allocation, collision and the laser registry.
bool th14_curve_head(const Th14CurveNode&,float time,Th14CurveSample&)noexcept;
bool th14_curve_tail(const Th14CurveNode&,float time,const Th14CurveSample& previous,Th14CurveSample&)noexcept;
struct Th14CurvePath {
    u8 request[0x354]{};
    std::vector<Th14CurveNode> nodes;
    std::vector<Th14CurveSample> samples;
    i32 transform_index=0;
    // Only independently verified analytic ETEX 4 / 8 are accepted here.
    bool initialize(const u8*,const std::vector<Th14CurveNode>* inherited=nullptr);
    bool transforms();
    void sample(float time,bool reset_negative=true)noexcept;
    i32 cut_circle(Vec3 center,float radius,bool protection,bool skip,std::vector<Vec3>& deliveries,bool& mark);
    i32 cut_rectangle(Vec3 center,Vec2 size,float angle,bool protection,bool skip,Timer&,std::vector<Vec3>& deliveries,bool& mark,std::vector<std::array<u8,0x354>>& fragments);
};
struct Th14CurveVertex {float x,y,z,rhw;u32 color;float u,v;};
void th14_curve_mesh(const Th14CurveSample*,u32 count,float width,float top_v,float bottom_v,Vec2 origin,Th14CurveVertex*)noexcept;
#endif
}
