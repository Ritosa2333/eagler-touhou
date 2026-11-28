#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <vector>

namespace th128::geometry526 {
struct Vertex {
    float x=0,y=0,z=0,rhw=1;
    std::uint32_t color=0xffffffffu;
    float u=0,v=0;
};
struct Grid {
    unsigned columns=0,rows=0;
    std::vector<Vertex> vertices;
    std::vector<std::array<float,3>> positions;
};
struct EffectState {
    float radius=0,scale=16;
    std::uint32_t color=0;
    std::array<float,2> angles{};
};
struct EffectStep {
    EffectState next;
    Grid grid;
};
std::optional<Grid> initializeGrid(unsigned columns,unsigned rows,float x,float y,float width,float height);
std::optional<std::vector<Vertex>> rowStrips(const Grid& grid);
std::optional<EffectStep> stepEffect(const EffectState& state,const std::array<float,3>& position,
                                   float delta=1,unsigned columns=17,unsigned rows=17);
}
