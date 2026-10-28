// H-LASER-ANM: 433260/433470/4337c0, real bullet.anm -> production renderer.
#ifndef TH13_TEST_PORT_DIR
#define TH13_TEST_PORT_DIR "."
#endif
#include "../ecl/ecl_test_fixtures.hpp"
#include "game/LaserPresentation.hpp"
#include <vector>
using namespace th13;
namespace {
struct Graphics final : ZunGraphics {
    touhou::graphics::PipelineState state;
    std::vector<AnmGeometryVertex> vertices;
    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&, u32 id) override { return id+1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32 n, const void* p, u32 stride) override {
        CHECK_EQ(stride, sizeof(AnmGeometryVertex));
        auto* v=static_cast<const AnmGeometryVertex*>(p);
        vertices.insert(vertices.end(),v,v+n*3);
    }
    void primitives(touhou::graphics::Topology,u32,const void*,u32) override {}
    bool select_target(const AnmResource*,u32) override { return true; }
    bool clear_target(u32,const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}
};
struct Rig {
    GameRandom random{0};
    AnmEnvironment env;
    AnmManager animations{random,env};
    Graphics graphics;
    AnmRenderer renderer{graphics};
    StageSim sim{&random};
    LaserPresentation presentation{animations,renderer,sim,5};
    StageEt et;
    StageEnemy enemy;
    bool init() {
        std::vector<u8> bytes;
        if(!th13test::TestFileReader{}.read("artifacts/reference/raw/bullet.anm",bytes))return false;
        env.screen_offsets={32,16,320,16};
        if(!animations.load(5,bytes.data(),u32(bytes.size())))return false;
        et.reset();et.sprite=8;et.subtype=6;et.spawn[0]=et.spawn[1]=480;
        et.laser_width=16;et.speed1=8;et.sound=-1;
        et.laser_timing[0]=20;et.laser_timing[1]=8;
        et.laser_timing[2]=140;et.laser_timing[3]=10;
        enemy.position[0]=96;enemy.position[1]=144;
        return true;
    }
    u32 spawn() { sim.emit_infinite_laser(et,enemy,257);return sim.lasers.back().generation; }
    void draw() { graphics.vertices.clear();CHECK(presentation.draw());renderer.flush(); }
};
TEST(infinite_laser_real_anm_birth_scaling_and_submission) {
    Rig r;CHECK(r.init());auto gen=r.spawn();CHECK(r.sim.error().empty());
    auto* body=r.presentation.body(gen);auto* orb=r.presentation.orb(gen);
    CHECK(body && orb);if(!body||!orb)return;
    CHECK_EQ(body->source_script,43);CHECK_EQ(body->visual.sprite,134);
    CHECK_EQ(orb->source_script,62);
    CHECK_EQ(body->timer.current,48);CHECK_EQ(orb->timer.current,1);
    r.draw();CHECK_EQ(r.graphics.vertices.size(),12u);
    CHECK_EQ(float_to_bits(r.graphics.vertices[0].position.x),float_to_bits(430.0f));
    CHECK_EQ(float_to_bits(r.graphics.vertices[0].position.y),float_to_bits(152.0f));
    r.sim.advance_lasers();r.draw();CHECK(r.sim.error().empty());
    CHECK_EQ(body->visual.scale.x,0.125f);
    CHECK_EQ(float_to_bits(body->visual.scale.y),0x42092492u); // 480/14, retail store
    // 4337c0: beam starts at origin+320/16 and extends along +X by480.
    CHECK_EQ(r.graphics.vertices[0].position.x,896.0f);
    CHECK_EQ(r.graphics.vertices[2].position.x,416.0f);
    CHECK_EQ(body->timer.current,48);CHECK_EQ(orb->timer.current,2);
    for(int n=1;n<178;++n)r.sim.advance_lasers();
    CHECK(r.presentation.body(gen));
    r.sim.advance_lasers();CHECK(r.sim.lasers.empty());
    CHECK(!r.presentation.body(gen));CHECK(!r.presentation.orb(gen));
    r.draw();CHECK(r.graphics.vertices.empty());
}
TEST(infinite_laser_duplicate_labels_keep_distinct_animation_clocks) {
    Rig r;CHECK(r.init());auto first=r.spawn();r.sim.advance_lasers();
    auto second=r.spawn();CHECK(first!=second);
    CHECK_EQ(r.presentation.orb(first)->timer.current,2);
    CHECK_EQ(r.presentation.orb(second)->timer.current,1);
    r.sim.lasers.front().phase=1;r.sim.advance_lasers();
    CHECK(!r.presentation.body(first));CHECK(r.presentation.body(second));
    CHECK_EQ(r.presentation.orb(second)->timer.current,2);
    CHECK_EQ(r.sim.lasers.front().generation,second);
    CHECK_EQ(r.sim.lasers.front().id,257);
}
TEST(infinite_laser_birth_is_immediate_and_animation_is_not_registry_ticked) {
    Rig r;CHECK(r.init());int sound_calls=0;
    r.et.sound=18;
    r.sim.laser_sound=[&](i32 sound){
        CHECK_EQ(sound,18);CHECK(r.presentation.body(r.sim.lasers.back().generation));
        ++sound_calls;return true;
    };
    auto gen=r.spawn();CHECK_EQ(sound_calls,1);
    auto before=r.presentation.orb(gen)->timer.current;
    CHECK(r.animations.update());
    CHECK_EQ(r.presentation.orb(gen)->timer.current,before);
    r.sim.lasers.front().start_offset=16;
    r.sim.advance_lasers();CHECK_EQ(r.presentation.orb(gen)->timer.current,before);
    r.draw();CHECK_EQ(r.graphics.vertices.size(),6u);
}
TEST(split_line_has_independent_tip_and_retires_all_three_animations) {
    Rig r;CHECK(r.init());r.et.spawn[0]=r.et.spawn[1]=128;
    r.enemy.position[0]=0;r.enemy.position[1]=144;auto parent=r.spawn();
    r.sim.lasers[0].width=16;
    r.sim.split_laser(0,{48,144,0},{32,32,0},0,false,false,true);
    CHECK_EQ(r.sim.lasers.size(),2u);auto child=r.sim.lasers[1].generation;
    CHECK(r.presentation.body(child));CHECK(r.presentation.orb(child));CHECK(r.presentation.tip(child));
    CHECK_EQ(r.presentation.tip(child)->source_script,88);
    CHECK_EQ(r.presentation.tip(child)->timer.current,1);
    r.draw(); // All animations use the normal production renderer.
    CHECK_EQ(r.graphics.vertices.size(),24u); // tip's first frame has no sprite
    r.sim.advance_lasers();r.draw();
    CHECK_EQ(r.presentation.tip(child)->timer.current,2);
    CHECK_EQ(r.presentation.orb(child)->timer.current,1); // moved origin hides orb
    CHECK_EQ(r.presentation.body(child)->variables.position.x,72.0f);
    CHECK_EQ(r.presentation.tip(child)->variables.position.x,128.0f);
    for(int i=0;i<7;++i)r.sim.advance_lasers();
    CHECK(!r.presentation.body(child));CHECK(!r.presentation.orb(child));CHECK(!r.presentation.tip(child));
    CHECK(r.presentation.body(parent));
}
TEST(real_stage1_mboss_script_drives_two_independent_laser_animations) {
    Rig r;CHECK(r.init());
    CHECK(r.sim.load("artifacts/reference/raw",
        TH13_TEST_PORT_DIR "/tests/ui/build/laser-script.jsonl"));
    if(!r.sim.error().empty())return;
    // Isolated real-script integration, not natural full-stage completion.
    r.sim.spawn_enemy("MBossCard1At",96,144,1,nullptr,false);
    CHECK(r.sim.error().empty());CHECK_EQ(r.sim.lasers.size(),2u);
    if(r.sim.lasers.size()!=2)return;
    auto first=r.sim.lasers[0].generation,second=r.sim.lasers[1].generation;
    CHECK_EQ(r.sim.lasers[0].id,1);CHECK_EQ(r.sim.lasers[1].id,2);
    CHECK(r.presentation.body(first));CHECK(r.presentation.body(second));
    for(r.sim.frame=1;r.sim.frame<=240;++r.sim.frame){
        CHECK(r.sim.step_frame());
        r.draw();
        if(r.sim.frame==30){
            CHECK_EQ(r.sim.lasers.size(),2u);
            CHECK_EQ(r.graphics.vertices.size(),24u);
            CHECK(r.sim.lasers[0].angle<0);
            CHECK(r.sim.lasers[1].angle<0); // pi-A wrapped at runtime update
            CHECK_EQ(r.presentation.body(first)->visual.sprite,134);
            CHECK_EQ(r.presentation.body(second)->visual.sprite,134);
        }
    }
    CHECK(r.sim.lasers.empty());CHECK(!r.presentation.body(first));
    CHECK(!r.presentation.body(second));CHECK(r.graphics.vertices.empty());
}
TEST(real_stage6_line_emitter_reaches_body_tip_and_production_draw) {
    Rig r;CHECK(r.init());
    CHECK(r.sim.load("artifacts/reference/raw",
        TH13_TEST_PORT_DIR "/tests/ui/build/stage6-line-script.jsonl","st06.ecl"));
    if(!r.sim.error().empty())return;
    r.sim.spawn_enemy("BossCard1_EtAt",0,320,1000,nullptr,false);
    CHECK(r.sim.error().empty());CHECK_EQ(r.sim.lasers.size(),1u);
    if(r.sim.lasers.empty())return;
    const u32 generation=r.sim.lasers.front().generation;
    CHECK(r.sim.lasers.front().kind==StageLaser::Kind::Line);
    CHECK_EQ(r.sim.lasers.front().sprite,20u);
    CHECK_EQ(r.sim.lasers.front().target_length,128.f);
    for(int i=0;i<20;++i){r.sim.advance_lasers();r.draw();}
    CHECK(r.sim.error().empty());CHECK(!r.graphics.vertices.empty());
    const auto* body=r.presentation.body(generation);
    const auto* tip=r.presentation.tip(generation);
    CHECK(body&&tip);if(!body||!tip)return;
    CHECK(body->sprite);CHECK(tip->sprite);
    CHECK_EQ(body->visual.scale.x,14.f/body->sprite->width);
    CHECK_EQ(body->visual.scale.y,128.f/body->sprite->height);
    CHECK_EQ(tip->variables.position.y,185.f); // 320 - one7px movement -128px length
}
}
int main(){return th13test::run_all("infinite-laser-presentation");}
