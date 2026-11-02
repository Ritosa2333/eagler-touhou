#include "../multiplayer/native-world.hpp"
#include "multiplayer/AnimationState.hpp"
#include "multiplayer/LaserFrameState.hpp"
using namespace th15::multiplayer;
static std::vector<u32> signature(LaserScene& scene){
    std::vector<u32> out;
    const auto bytes=[&](const auto& value){const auto* p=reinterpret_cast<const u8*>(&value);for(size_t i=0;i<sizeof value;++i)out.push_back(p[i]);};
    scene.manager.each([&](LaserObject& object){bytes(object.kind);bytes(object.id);bytes(object.flags);bytes(object.state);bytes(object.age);bytes(object.outside_delay);bytes(object.outside_count);
        if(auto* m=scene.moving_motion(object.id))bytes(*m);
        if(auto* m=scene.stationary_motion(object.id))bytes(*m);
        if(auto* m=scene.segmented_motion(object.id))bytes(*m);
        if(auto* m=scene.curve_motion(object.id)){bytes(m->position);bytes(m->velocity);bytes(m->path_age);bytes(m->angle);for(const auto& p:m->points)bytes(p);for(const auto& p:m->path.segments)bytes(p);}
    });return out;
}
int main(int argc,char** argv){
    require(argc==2,"Pass TH15_TEST_ARCHIVE");RetailAssets files(argv[1]);World world(files,3,0,{0,1,3});
    auto& scene=*world.battle->laser_scene;auto& animations=world.animations;
    MovingLaserRequest moving;moving.position={-110,100,0};moving.maximum_length=150;moving.initial_length=30;moving.end_distance=600;moving.width=8;moving.speed=1.25f;
    const auto moving_id=scene.create_moving(moving);require(moving_id!=0,scene.error);
    StationaryLaserRequest stationary;stationary.id=1234;stationary.position={10,100,0};stationary.maximum_length=120;stationary.initial_length=100;stationary.width=10;stationary.warmup=30;stationary.active=240;stationary.fade=30;
    require(scene.create_stationary(stationary)!=0,scene.error);
    CurveLaserRequest curved;curved.position={-30,100,0};curved.width=10;curved.speed=1.125f;curved.count=32;
    const auto curved_id=scene.create_curved(curved);require(curved_id!=0,scene.error);
    SegmentedLaserRequest segmented;segmented.id=4321;segmented.position={-60,100,0};segmented.length=40;segmented.width=8;
    require(scene.create_segmented(segmented)!=0,scene.error);
    auto* moving_visual=scene.moving_visual(moving_id);auto* curved_visual=scene.curve_visual(curved_id);
    auto* moving_child=animations.spawn_child(moving_visual->body,0,2);require(moving_child!=nullptr,animations.error);
    const auto moving_child_id=animations.registry.handle(*moving_child);
    auto* curve_child=animations.spawn_child(curved_visual->body,0,2);require(curve_child!=nullptr,animations.error);
    const auto curve_child_id=animations.registry.handle(*curve_child);
    require(scene.update(.375f),scene.error);const auto initial=signature(scene);const auto counter=AnmVm::presentation_counter;
    AnimationState graph;LaserFrameState lasers;require(graph.capture(animations.registry),"capture laser graph");require(lasers.capture(scene),lasers.error);
    require(lasers.size()==4&&AnmVm::presentation_counter==counter,"capture identity and all four laser kinds");
    for(unsigned i=0;i<12;++i)require(scene.update(.375f),scene.error);
    const auto expected=signature(scene);const auto expected_game=world.environment.game_rng,expected_visual=world.visual;
    require(lasers.restore(scene)&&graph.restore(animations.registry),"restore before comparison");
    require(signature(scene)==initial,"exact initial laser state");
    for(unsigned i=0;i<12;++i)require(scene.update(.375f),scene.error);
    require(signature(scene)==expected,"twelve resimulated steps match on-time laser evolution");
    require(world.environment.game_rng.seed==expected_game.seed&&world.environment.game_rng.calls==expected_game.calls&&world.visual.seed==expected_visual.seed&&world.visual.calls==expected_visual.calls,"resimulation RNG streams");
    require(scene.manager.clear(),scene.manager.error);
    require(animations.registry.find(moving_child_id)==nullptr,"logical removal destroys moving children immediately");
    require(animations.registry.find(curve_child_id)!=nullptr,"curved retirement preserves embedded children immediately");
    require(scene.create_moving(moving)!=0,"future laser allocation");
    require(lasers.restore(scene)&&graph.restore(animations.registry),"restore after retirement and future allocation");
    require(scene.moving_visual(moving_id)==moving_visual&&scene.curve_visual(curved_id)==curved_visual,"retained visual owner identity");
    require(moving_visual->body.sprite_source==moving_visual&&curved_visual->body.sprite_source==curved_visual,"embedded sprite owner references");
    require(animations.registry.find(moving_child_id)&&animations.registry.find(curve_child_id),"original embedded children restored");
    require(signature(scene)==initial&&AnmVm::presentation_counter==counter,"retirement undo restores immutable captured state");
    std::puts("PASS: native retail four-kind lasers, retirement, embedded children and 12-frame resimulation");
}
