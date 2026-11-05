#pragma once
#include "Types.hpp"
#include "Rng.hpp"
#include "Timer.hpp"
#include <array>
#include <vector>
#include <memory>
namespace th15 {
struct AnmOverlay;
struct AnmGeometryVertex {Vec3 position{};float reciprocal_w=0;u32 color=0;Vec2 uv{};};
struct AnmWorldVertex {Vec3 position{};u32 color=0;Vec2 uv{};};
struct AnmDistortion {
    std::array<AnmGeometryVertex,33> vertices{};std::array<float,31> radius{},radial_speed{};Vec2 uv_speed{};
    void initialize(const Vec3& position,const Vec3& translation,Rng&);
    void update(const Vec3& position,const Vec3& translation,u32 color);
};
struct AnmTrail {
    std::array<Vec2,64> points{};std::array<u32,64> colors{};float angle=0;Timer age;
    void initialize(Rng&);i32 update(Rng&,float rate);
    void initialize_orange(const Vec3&,Rng&);
};
static_assert(sizeof(AnmTrail)==0x318);
struct AnmGatherEffect {
    std::array<u32,200> handles{};std::array<Vec3,200> points{},tangents{};std::array<i32,200> phase{};
    std::array<Vec3,3> centers{};u32 reserved=0;Timer age;
};
static_assert(sizeof(AnmGatherEffect)==0x193c);
struct AnmGeometry {
    std::vector<AnmGeometryVertex> screen_vertices;std::vector<AnmWorldVertex> world_vertices;
    std::unique_ptr<AnmDistortion> distortion;u32 allocation_bytes=0;
    std::unique_ptr<AnmTrail> trail;
    std::unique_ptr<AnmGatherEffect> gather;
    std::unique_ptr<AnmOverlay> overlay;
    AnmGeometry();~AnmGeometry();AnmGeometry(const AnmGeometry&);AnmGeometry& operator=(const AnmGeometry&);
    AnmGeometry(AnmGeometry&&)noexcept;AnmGeometry& operator=(AnmGeometry&&)noexcept;
    bool allocate(i32 segments,bool world);void clear();
};
}
