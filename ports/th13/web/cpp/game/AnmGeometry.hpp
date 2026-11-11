#pragma once
#include "Types.hpp"
#include "GameRandom.hpp"
#include "Timer.hpp"
#include <array>
#include <vector>
#include <memory>
namespace th13 {
struct AnmOverlay;
struct AnmGeometryVertex {Vec3 position{};float reciprocal_w=0;u32 color=0;Vec2 uv{};};
struct AnmWorldVertex {Vec3 position{};u32 color=0;Vec2 uv{};};
struct AnmDistortion {
    std::array<AnmGeometryVertex,33> vertices{};std::array<float,31> radius{},radial_speed{};Vec2 uv_speed{};
    void initialize(const Vec3& position,const Vec3& translation,GameRandom&);
    void update(const Vec3& position,const Vec3& translation,u32 color);
};
struct AnmTrail {
    std::array<Vec2,64> points{};std::array<u32,64> colors{};float angle=0;Timer age;
    // 有效段数：0x318 家族（graze 星等）=64；4bba80[1]（401000/401190）状态仅
    // 0x78=8 段，401190 idx>=8 返回 1 退休。显式字段，不做位/字段复用。
    i32 segment_count=64;
    void initialize(GameRandom&);i32 update(GameRandom&,float rate);
    void initialize_orange(const Vec3&,GameRandom&);
    void initialize_small(const Vec3&,GameRandom&);
};
// 415d60/415e80 kind2: 32 triangles with separate center/rim colors.
struct AnmCircle {
    std::array<AnmGeometryVertex,34> vertices{};
    Vec3 center{};
    float radius=128.f, angle=0.f;
    u32 color=0x40ff0000u, rim_color=0x00ff00ffu;
    Timer age;
    void initialize(const Vec3& position,float r=128.f);
    void rebuild();
};
// 原 64 点族状态 0x318；kind1 原生状态 0x78（8×12 字节条目+angle+Timer 同构截断）。
// port 合并模型携带显式 segment_count（i32），sizeof = 0x318+4；两原尺寸见上注。
static_assert(sizeof(AnmTrail)==0x31c);
struct AnmGatherEffect {
    std::array<u32,200> handles{};std::array<Vec3,200> points{},tangents{};std::array<i32,200> phase{};
    std::array<Vec3,3> centers{};u32 reserved=0;Timer age;
};
// Effect table 4bba80 kind5: Stage 3's animated world-space ribbons.
struct AnmRibbon {
    std::array<Vec3,16> segments{};
    std::array<AnmWorldVertex,32> vertices{};
    float angle=0;
    Timer age;
    void initialize(GameRandom&);
    void update(GameRandom&,float rate);
};
struct AnmEnvironment;
// 415650 effect6: 401730 initialization,401c90 history,402340 world draw.
struct AnmWindTrail {
    std::array<AnmWorldVertex,64> vertices{};
    std::array<Vec3,32> points{},tangents{};
    Vec3 start{},end{},start_tangent{},end_tangent{},drift{};
    Timer interpolation,age;
    i32 duration=160;
    u32 color=0x80ffffff;
    void initialize(GameRandom&,const AnmEnvironment&);
    bool update(const AnmEnvironment&,u32 alpha,float rate);
};
static_assert(sizeof(AnmGatherEffect)==0x193c);
struct AnmGeometry {
    std::vector<AnmGeometryVertex> screen_vertices;std::vector<AnmWorldVertex> world_vertices;
    std::unique_ptr<AnmDistortion> distortion;u32 allocation_bytes=0;
    std::unique_ptr<AnmTrail> trail;
    std::unique_ptr<AnmCircle> circle;
    std::unique_ptr<AnmGatherEffect> gather;
    std::unique_ptr<AnmRibbon> ribbon;
    std::unique_ptr<AnmWindTrail> wind;
    std::unique_ptr<AnmOverlay> overlay;
    AnmGeometry();~AnmGeometry();AnmGeometry(const AnmGeometry&);AnmGeometry& operator=(const AnmGeometry&);
    AnmGeometry(AnmGeometry&&)noexcept;AnmGeometry& operator=(AnmGeometry&&)noexcept;
    bool allocate(i32 segments,bool world);void clear();
};
}
