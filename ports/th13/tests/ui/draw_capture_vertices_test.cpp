// DrawCapture UP 序列化契约：op 的 count 字段 = D3D 图元参数，verts 数组 =
// vertex_count(topology, count)（与 oracle/render_oracle.py 的
// d3d_vertex_count 同式）。修复前 emit_up 只写 count 个顶点，strip/fan 的
// 顶点列表被截断，title01/rank00/shape 的几何比对因此失效。
#include "../harness/test_framework.hpp"
#include "game/DrawCapture.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

using namespace th13;
using touhou::graphics::Topology;

namespace {

struct CaptureVertex {
    float x, y, z, w;
    u32 color;
    float u, v;
};

std::string capture(Topology topology, u32 count, const void* data, u32 stride,
                    touhou::graphics::VertexLayout layout =
                        touhou::graphics::VertexLayout::ScreenColorUv) {
    const char* path = "/tmp/th13_draw_capture_vertices_test.jsonl";
    DrawCapture capture;
    if (!capture.open_output(path)) return {};
    capture.begin_frame(0);
    capture.set_layout(layout);
    capture.primitives(topology, count, data, stride);
    if (!capture.end_frame() || !capture.close_output()) return {};
    std::ifstream input(path);
    std::stringstream buffer;
    buffer << input.rdbuf();
    std::remove(path);
    return buffer.str();
}

i32 field(const std::string& line, const char* key) {
    const std::string needle = std::string("\"") + key + "\":";
    const size_t at = line.find(needle);
    if (at == std::string::npos) return -1;
    return std::atoi(line.c_str() + at + needle.size());
}

i32 vertex_count_of(const std::string& line) {
    const size_t at = line.find("\"verts\":[");
    if (at == std::string::npos) return -1;
    // verts 是 up op 的最后一个键（"…"]}），顶点数组闭合在行尾 "]}".
    const size_t end = line.rfind("]}");
    if (end == std::string::npos || end < at) return -1;
    i32 vertices = 0;
    for (size_t i = at + 9; i < end; i++)
        if (line[i] == '[') vertices++;
    return vertices;
}

constexpr u32 kStride = sizeof(CaptureVertex);
CaptureVertex quad[4] = {
    {0, 0, 0, 1, 0xffffffffu, 0, 0},
    {1, 0, 0, 1, 0xffffffffu, 1, 0},
    {0, 1, 0, 1, 0xffffffffu, 0, 1},
    {1, 1, 0, 1, 0xffffffffu, 1, 1},
};
CaptureVertex triangles[6] = {
    {0, 0, 0, 1, 0xff0000ffu, 0, 0}, {1, 0, 0, 1, 0xff0000ffu, 1, 0},
    {0, 1, 0, 1, 0xff0000ffu, 0, 1}, {1, 0, 0, 1, 0xff00ff00u, 1, 0},
    {1, 1, 0, 1, 0xff00ff00u, 1, 1}, {0, 1, 0, 1, 0xff00ff00u, 0, 1},
};
CaptureVertex fan[5] = {
    {0, 0, 0, 1, 0xff112233u, 0, 0}, {1, 0, 0, 1, 0xff112233u, 1, 0},
    {1, 1, 0, 1, 0xff112233u, 1, 1}, {0, 1, 0, 1, 0xff112233u, 0, 1},
    {0, 0, 0, 1, 0xff112233u, 0, 0},
};

}  // namespace

TEST(draw_capture_strip_serializes_all_four_vertices) {
    const std::string line = capture(Topology::Strip, 2, quad, kStride);
    CHECK(!line.empty());
    CHECK_EQ(field(line, "count"), 2);
    CHECK_EQ(vertex_count_of(line), 4);
    CHECK_EQ(field(line, "prim"), 5);
}

TEST(draw_capture_triangles_serialize_three_per_primitive) {
    const std::string line = capture(Topology::Triangles, 2, triangles, kStride);
    CHECK(!line.empty());
    CHECK_EQ(field(line, "count"), 2);
    CHECK_EQ(vertex_count_of(line), 6);
    CHECK_EQ(field(line, "prim"), 4);
}

TEST(draw_capture_fan_serializes_count_plus_two) {
    const std::string line = capture(Topology::Fan, 3, fan, kStride);
    CHECK(!line.empty());
    CHECK_EQ(field(line, "count"), 3);
    CHECK_EQ(vertex_count_of(line), 5);
    CHECK_EQ(field(line, "prim"), 6);
}

// [反证 #4] 矩阵路径（WorldUv 静态 VB + DrawPrimitive）此前固定写
// "verts":[]，oracle 却按 D3D 顶点口径读回 4 个顶点（窗B 15k+ ascii/loading
// 粒子 quad 因此首色 None vs 0）。矩阵路径必须序列化 4 顶点，且 WorldUv
// 走 oracle _read_vertices 的 per==5/fvf==0x102 分支 [x,y,z,0,0,u,v]。
struct WorldUvVertex {
    float x, y, z, u, v;
};
constexpr u32 kWorldUvStride = sizeof(WorldUvVertex);
WorldUvVertex world_quad[4] = {
    {-128, -128, 0, 0, 0},
    {128, -128, 0, 1, 0},
    {-128, 128, 0, 0, 1},
    {128, 128, 0, 1, 1},
};

TEST(draw_capture_matrix_path_serializes_all_four_vertices) {
    const std::string line = capture(Topology::Strip, 2, world_quad, kWorldUvStride,
                                     touhou::graphics::VertexLayout::WorldUv);
    CHECK(!line.empty());
    CHECK_EQ(field(line, "count"), 2);
    CHECK_EQ(vertex_count_of(line), 4);
    CHECK_EQ(field(line, "prim"), 5);
    // oracle 格式：float 串 + w/color 为 JSON int 0。
    CHECK(line.find("\"verts\":[[\"-128\",\"-128\",\"0\",0,0,\"0\",\"0\"],"
                    "[\"128\",\"-128\",\"0\",0,0,\"1\",\"0\"],"
                    "[\"-128\",\"128\",\"0\",0,0,\"0\",\"1\"],"
                    "[\"128\",\"128\",\"0\",0,0,\"1\",\"1\"]]") != std::string::npos);
}

// [反证 #7] strip/fan 的 0 图元特例：oracle d3d_vertex_count 返回 0，
// port 此前返回 count+1 / count+2。0 图元绘制不产生顶点。
TEST(draw_capture_zero_count_strip_serializes_no_vertices) {
    const std::string line = capture(Topology::Strip, 0, world_quad, kWorldUvStride,
                                     touhou::graphics::VertexLayout::WorldUv);
    CHECK(!line.empty());
    CHECK_EQ(field(line, "count"), 0);
    CHECK_EQ(vertex_count_of(line), 0);
}

TEST(draw_capture_zero_count_fan_serializes_no_vertices) {
    const std::string line = capture(Topology::Fan, 0, world_quad, kWorldUvStride,
                                     touhou::graphics::VertexLayout::WorldUv);
    CHECK(!line.empty());
    CHECK_EQ(field(line, "count"), 0);
    CHECK_EQ(vertex_count_of(line), 0);
}

int main() { return th13test::run_all("draw_capture_vertices"); }
