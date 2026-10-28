// Stage 1 asset-to-submission integration check. This exercises the real
// st01.std -> st01wl.anm -> StageScenery -> AnmRenderer path with repository
// reference assets. It verifies source coverage, not GPU output equivalence.
#include "anm_test_fixtures.hpp"

#include "game/AnmRenderer.hpp"
#include "game/StageResource.hpp"
#include "game/StageScenery.hpp"
#include "game/ZunGraphics.hpp"

#include <set>
#include <vector>

using namespace th13;
using namespace touhou::graphics;

namespace {

struct SceneSubmission {
    i32 layer = 0;
    i32 script = -1;
    i32 sprite = -1;
    u32 instance = 0;
    u32 first_vertex = 0;
    u32 vertex_total = 0;
};

struct RecordingGraphics final : ZunGraphics {
    PipelineState state;
    u32 bound_texture_id = 0;
    u32 triangle_batches = 0;
    u32 primitive_batches = 0;
    u32 submitted_triangles = 0;
    u32 submitted_primitives = 0;
    std::set<u32> textures;
    std::set<u32> instances;
    std::vector<SceneSubmission> submissions;
    std::vector<AnmGeometryVertex> screen_vertices;

    PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource& resource, u32 index) override {
        return index < resource.textures.size() ? index + 1 : 0;
    }
    void bind_texture(u32 id) override {
        bound_texture_id = id;
        if (id) textures.insert(id);
    }
    void set_layout(VertexLayout) override {}
    void set_matrix(MatrixKind, const Matrix4&) override {}
    void triangles(u32 count, const void* vertices, u32 stride) override {
        ++triangle_batches;
        submitted_triangles += count;
        if (stride == sizeof(AnmGeometryVertex)) {
            const auto* data = static_cast<const AnmGeometryVertex*>(vertices);
            screen_vertices.insert(screen_vertices.end(),data,data+count*3);
        }
    }
    void primitives(Topology, u32 count, const void*, u32) override {
        ++primitive_batches;
        submitted_primitives += count;
    }
    void presentation_range(const AnmVm& vm, u32 first_vertex, u32 vertex_total,
                            u32 instance) override {
        if (instance) instances.insert(instance);
        submissions.push_back({vm.visual.layer, vm.source_script, vm.visual.sprite, instance,
                               first_vertex, vertex_total});
    }
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}
};

bool load_reference(const char* name, std::vector<u8>& bytes) {
    return th13test::TestFileReader{}.read(std::string("artifacts/reference/raw/") + name,
                                           bytes);
}

}  // namespace

TEST(real_stage1_std_anm_reaches_renderer_submissions) {
    std::vector<u8> std_bytes, animation_bytes;
    CHECK(load_reference("st01.std", std_bytes));
    CHECK(load_reference("st01wl.anm", animation_bytes));
    if (std_bytes.empty() || animation_bytes.empty()) return;

    StageScenery scenery;
    CHECK(scenery.file.open(std_bytes.data(), u32(std_bytes.size())));
    CHECK_EQ(scenery.file.animation_name, std::string("st01wl.anm"));
    CHECK_EQ(scenery.file.objects.size(), 3u);
    CHECK_EQ(scenery.file.animation_count, 24u);
    CHECK_EQ(scenery.file.instances.size(), 40u);

    GameRandom random{0};
    AnmEnvironment environment;
    environment.screen_offsets = {32, 16, 320, 16};
    environment.playfield_origin = {32, 16};
    AnmManager animations{random, environment};
    CHECK(animations.load(7, animation_bytes.data(), u32(animation_bytes.size())));
    if (!animations.resource(7)) return;

    CHECK(scenery.initialize(animations, 7));
    if (!scenery.ready()) return;
    // Stage scenery advances from the STD camera stream and its 24 bound ANM
    // primitive VMs together, as RunScene does before its draw passes.
    for (u32 frame = 0; frame < 4; ++frame)
        CHECK(scenery.update(animations, environment));

    RecordingGraphics graphics;
    AnmRenderer renderer{graphics};
    const GraphicsViewport viewport{116, 2, 408, 476, 0, 1};
    CHECK(scenery.draw(animations, renderer, graphics, 0, 7, viewport, {408, 476}));

    CHECK(graphics.triangle_batches + graphics.primitive_batches > 0);
    CHECK(graphics.submitted_triangles + graphics.submitted_primitives > 0);
    CHECK(!graphics.submissions.empty());
    CHECK(!graphics.instances.empty());
    CHECK(!graphics.textures.empty());
    for (u32 instance : graphics.instances) CHECK(instance >= 1 && instance <= 40);
    for (const auto& submission : graphics.submissions) {
        CHECK(submission.instance >= 1 && submission.instance <= 40);
        CHECK(submission.vertex_total > 0);
    }

    // Stage 1's parsed object layers are 0, 1 and 2, so the production second
    // scenery pass (layers 8-11) has no Stage 1 object to submit.
    const size_t prior_submissions = graphics.submissions.size();
    const u32 prior_batches = graphics.triangle_batches + graphics.primitive_batches;
    CHECK(scenery.draw(animations, renderer, graphics, 8, 11, viewport, {408, 476}));
    CHECK_EQ(graphics.submissions.size(), prior_submissions);
    CHECK_EQ(graphics.triangle_batches + graphics.primitive_batches, prior_batches);
}

TEST(stage3_ribbon_matches_original_update_sample) {
    // Original 00402b20 executed for 120 updates, initialized by 00402a20's
    // sixteen segment values and timer=1, seed0 plus its initialization draw.
    GameRandom random{0};AnmRibbon ribbon;ribbon.initialize(random);
    for(int frame=0;frame<120;++frame)ribbon.update(random,1);
    auto bits=[](float value){u32 result;std::memcpy(&result,&value,4);return result;};
    CHECK_EQ(random.seed(),37006);
    CHECK_EQ(random.index(),62u);
    CHECK_EQ(bits(ribbon.vertices[2].position.x),1083973356u);
    CHECK_EQ(bits(ribbon.vertices[14].position.x),1105432446u);
    CHECK_EQ(bits(ribbon.vertices[31].position.x),1108392431u);
    CHECK_EQ(ribbon.vertices[2].color,0x80ffffffu);
    CHECK_EQ(ribbon.vertices[26].color,0xb0ffffffu);
    CHECK_EQ(ribbon.vertices[28].color,0x50ffffffu);
    CHECK_EQ(ribbon.vertices[31].color,0x00ffffffu);
}

TEST(real_stage3_std_and_ribbon_effect_reach_renderer) {
    std::vector<u8> std_bytes,anm_bytes;
    CHECK(load_reference("st03.std",std_bytes));
    CHECK(load_reference("st03wl.anm",anm_bytes));
    GameRandom random{0};AnmEnvironment environment;
    AnmManager animations{random,environment};
    CHECK(animations.load(7,anm_bytes.data(),u32(anm_bytes.size())));
    StageScenery scenery;
    CHECK(scenery.file.open(std_bytes.data(),u32(std_bytes.size())));
    CHECK(scenery.initialize(animations,7));
    for(int frame=0;frame<120;++frame)CHECK(scenery.update(animations,environment));
    RecordingGraphics graphics;AnmRenderer renderer{graphics};
    CHECK(scenery.draw(animations,renderer,graphics,0,7,{116,2,408,476,0,1},{408,476}));
    CHECK(!graphics.submissions.empty());
    CHECK(graphics.submitted_primitives>0);
}

TEST(stage_shake_moves_submitted_projected_vertices_without_changing_camera_or_culling) {
    // A centered synthetic projected primitive isolates manager+d0/d4.
    // Real Stage1 first-frame primitives use world quads; a full projected
    // scene can also enter/leave the final viewport when shaken.
    std::vector<u8> script;
    th13test::push_insn(script,300,0,0,{0});
    th13test::push_insn(script,302,0,0,{4});
    th13test::push_insn(script,2,0);
    th13test::push_terminator(script);
    const auto anm_bytes=th13test::build_simple_anm({script});
    GameRandom random{0}; AnmEnvironment environment;
    AnmManager animations{random,environment};
    CHECK(animations.load(7,anm_bytes.data(),u32(anm_bytes.size())));
    StageScenery scenery;
    scenery.file.animation_count=1;
    StageObject object;object.size={32,32,32};
    StagePrimitive primitive;primitive.size={32,32};
    object.primitives.push_back(primitive);
    scenery.file.objects.push_back(object);
    scenery.file.instances.push_back({0,0,{0,0,100}});
    scenery.file.instructions.push_back({0,0,8,0,{}});
    scenery.script.state.camera.position={0,0,-100};
    scenery.script.state.camera.direction={0,0,1};
    scenery.script.state.camera.up={0,1,0};
    scenery.script.state.camera.fov=1.f;
    CHECK(scenery.initialize(animations,7));
    for(int i=0;i<4;++i) CHECK(scenery.update(animations,environment));
    RecordingGraphics before,after;
    AnmRenderer plain{before},shaken{after};
    const GraphicsViewport viewport{116,2,408,476,0,1};
    CHECK(scenery.draw(animations,plain,before,0,7,viewport,{408,476}));
    CHECK(scenery.draw(animations,shaken,after,0,7,viewport,{408,476},{4,-3}));
    plain.flush();shaken.flush();
    CHECK(!before.screen_vertices.empty());
    CHECK_EQ(before.screen_vertices.size(),after.screen_vertices.size());
    CHECK_EQ(before.submissions.size(),after.submissions.size());
    CHECK(before.instances==after.instances);
    for(size_t i=0;i<before.screen_vertices.size()&&i<after.screen_vertices.size();++i) {
        CHECK_EQ(after.screen_vertices[i].position.x,float(before.screen_vertices[i].position.x+4));
        CHECK_EQ(after.screen_vertices[i].position.y,float(before.screen_vertices[i].position.y-3));
        CHECK_EQ(after.screen_vertices[i].position.z,before.screen_vertices[i].position.z);
        CHECK_EQ(after.screen_vertices[i].color,before.screen_vertices[i].color);
    }
    for(int i=0;i<16;++i) {
        CHECK_EQ(plain.camera.view.m[i],shaken.camera.view.m[i]);
        CHECK_EQ(plain.camera.projection.m[i],shaken.camera.projection.m[i]);
    }
}

int main() { return th13test::run_all("stage-asset-render-chain"); }
