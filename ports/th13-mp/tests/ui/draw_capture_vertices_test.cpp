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
                    touhou::graphics::VertexLayout layout = touhou::graphics::VertexLayout::ScreenColorUv) {
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
    {0, 0, 0, 1, 0xff0000ffu, 0, 0}, {1, 0, 0, 1, 0xff0000ffu, 1, 0}, {0, 1, 0, 1, 0xff0000ffu, 0, 1},
    {1, 0, 0, 1, 0xff00ff00u, 1, 0}, {1, 1, 0, 1, 0xff00ff00u, 1, 1}, {0, 1, 0, 1, 0xff00ff00u, 0, 1},
};
CaptureVertex fan[5] = {
    {0, 0, 0, 1, 0xff112233u, 0, 0}, {1, 0, 0, 1, 0xff112233u, 1, 0}, {1, 1, 0, 1, 0xff112233u, 1, 1},
    {0, 1, 0, 1, 0xff112233u, 0, 1}, {0, 0, 0, 1, 0xff112233u, 0, 0},
};

}

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
    const std::string line =
        capture(Topology::Strip, 2, world_quad, kWorldUvStride, touhou::graphics::VertexLayout::WorldUv);
    CHECK(!line.empty());
    CHECK_EQ(field(line, "count"), 2);
    CHECK_EQ(vertex_count_of(line), 4);
    CHECK_EQ(field(line, "prim"), 5);
    CHECK(line.find("\"verts\":[[\"-128\",\"-128\",\"0\",0,0,\"0\",\"0\"],"
                    "[\"128\",\"-128\",\"0\",0,0,\"1\",\"0\"],"
                    "[\"-128\",\"128\",\"0\",0,0,\"0\",\"1\"],"
                    "[\"128\",\"128\",\"0\",0,0,\"1\",\"1\"]]") != std::string::npos);
}

TEST(draw_capture_zero_count_strip_serializes_no_vertices) {
    const std::string line =
        capture(Topology::Strip, 0, world_quad, kWorldUvStride, touhou::graphics::VertexLayout::WorldUv);
    CHECK(!line.empty());
    CHECK_EQ(field(line, "count"), 0);
    CHECK_EQ(vertex_count_of(line), 0);
}

TEST(draw_capture_zero_count_fan_serializes_no_vertices) {
    const std::string line =
        capture(Topology::Fan, 0, world_quad, kWorldUvStride, touhou::graphics::VertexLayout::WorldUv);
    CHECK(!line.empty());
    CHECK_EQ(field(line, "count"), 0);
    CHECK_EQ(vertex_count_of(line), 0);
}

int main() { return th13test::run_all("draw_capture_vertices"); }
