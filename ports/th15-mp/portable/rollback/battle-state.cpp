#include "battle-digest.hpp"
#include "multiplayer/ItemFrameState.hpp"
#include "multiplayer/BulletFrameState.hpp"
static void cold_bullet_reuse(RetailAssets& assets){
    World world(assets,2,0,{0,1,0});auto& scene=*world.battle->bullet_scene;auto& manager=scene.manager;
    th15::multiplayer::BulletFrameState origin,branch;
    require(origin.capture(scene),origin.error);
    for(unsigned n=0;n<8;++n){
        BulletShooter shot;shot.count=1;shot.rows=1;shot.speed=float(n+1);shot.position={float(n*7),100,0};
        shot.transforms[17].payload={u8(n+1),0};
        require(manager.emit(shot,0),manager.error);const auto id=manager.first_active();require(id!=BulletManager::none,"spawn versioned bullet");
        manager.recycle(id);require(branch.capture(scene),branch.error);
        const auto& read=static_cast<const BulletManager&>(manager);Digest expected;expected.add(*read.state(id));
        require(origin.restore(scene),origin.error);require(branch.restore(scene),branch.error);
        Digest actual;actual.add(*read.state(id));require(actual.value==expected.value,"cold bullet lost an abandoned future's version");
        require(origin.restore(scene),origin.error);
    }
    // A mutable pool API can hand out a reference that outlives one capture.
    auto* retained=manager.state(0);retained->transform_sound=19;retained->transforms[3].payload={19};
    require(branch.capture(scene),branch.error);retained->transform_sound=37;retained->transforms[3].payload={37,38};
    require(branch.capture(scene),branch.error);retained->transform_sound=99;retained->transforms[3].payload={99};
    require(branch.restore(scene),branch.error);require(retained->transform_sound==37&&retained->transforms[3].payload==std::vector<u8>({37,38}),"retained mutable cold bullet reference was skipped");
}
static void same_tick_movement(RetailAssets& assets){
    for(unsigned count:{2u,3u})for(unsigned seat=0;seat<count;++seat){
        World world(assets,count,seat,{0,1,3});auto& player=*world.battle->pilots[seat]->player;
        std::array<MultiplayerInput,3> lanes{};const float start=player.motion.position.x;
        lanes[seat].held=0x80;world.step(lanes);const float right=player.motion.position.x;
        require(right>start,"local movement waited for a second logical frame");
        lanes[seat].held=0x40;world.step(lanes);require(player.motion.position.x<right,"local reversal waited for a second logical frame");
    }
}
// A free cancellation slot keeps its old body until the delayed falling
// transition binds a new script. Standalone snapshots must preserve that VM.
static void cancel_item_reuse(RetailAssets& assets){
    World world(assets,2,0,{0,1,0});auto& manager=*world.battle->items;
    const auto id=manager.spawn(9,{30,120,0},.3f,1);require(id&&manager.falling(id),manager.error);
    auto* slot=manager.find(id);Digest before;before.vm(*slot->body);require(manager.release(id),manager.error);
    th15::multiplayer::ItemFrameState saved;require(saved.capture(manager),saved.error);
    require(manager.spawn(10,{80,60,0},.6f,2)==id,"reuse the same free cancellation slot");require(manager.falling(id),manager.error);
    slot->body->variables.position={99,88,77};const float rate=.375f;slot->body->timer.tick(&rate);
    require(saved.restore(manager),saved.error);Digest restored;restored.vm(*manager.find(id)->body);
    require(restored.value==before.value,"cold cancellation VM was not fully restored");
    require(manager.spawn(9,{30,120,0},.3f,1)==id,"restored cancellation free-list order");
    require(manager.find(id)->state.state==5,"cancellation pickup starts in delayed state");
    Digest delayed;delayed.vm(*manager.find(id)->body);require(delayed.value==before.value,"delayed pickup lost its retained VM");
}
int main(int argc,char** argv){
    require(argc==2,"Pass TH15_TEST_ARCHIVE");RetailAssets assets(argv[1]);cancel_item_reuse(assets);cold_bullet_reuse(assets);same_tick_movement(assets);unsigned replayed=0;double capture_ms=0,restore_ms=0;unsigned captures=0;
    struct Case {unsigned players,local,stage;std::array<int,3> selection;};
    for(const Case config:std::array<Case,3>{{{3,2,1,{0,1,2}},{3,2,1,{3,3,0}},{2,1,7,{2,2,0}}}}){
        World world(assets,config.players,config.local,config.selection,false,config.stage);auto& b=*world.battle;EnemySpawnRequest main;main.routine="main";require(b.spawn(main),b.error);
        BattleFrameState saved;
        for(unsigned frame=0;frame<720;){
            for(unsigned n=0;n<48&&frame<720;++n,++frame)world.step(input(frame));
            if(frame>=720)break;
            b.items->spawn(2,{-90,150,0},.3f,1);b.items->spawn(9,{70,100,0},.4f,1);
            auto start=std::chrono::steady_clock::now();require(saved.capture(b),saved.error);capture_ms+=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();++captures;
            const auto before=digest(world);const auto identity=AnmVm::presentation_counter;
            std::array<u64,12> expected{};for(unsigned n=0;n<12;++n){world.step(input(frame+n));expected[n]=digest(world);}
            start=std::chrono::steady_clock::now();require(saved.restore(b),saved.error);restore_ms+=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
            // An abandoned future uses different movement and edge-triggered
            // bombs. Its allocations, damage and RNG must all disappear.
            for(unsigned n=0;n<12;++n){auto wrong=input(frame+n+99);if(n==0)for(auto& lane:wrong)lane.held|=2;world.step(wrong);}
            require(saved.restore(b),saved.error);
            require(digest(world)==before,"Full battle differs immediately after restore at "+std::to_string(frame));require(AnmVm::presentation_counter==identity,"capture/restore changed presentation identity");
            for(unsigned n=0;n<12;++n){world.step(input(frame+n));require(digest(world)==expected[n],"Full battle resimulation mismatch at "+std::to_string(frame+n));++replayed;}
            frame+=12;
        }
        saved.discard();
    }
    std::printf("PASS: %u exact retail battle resimulations; mean capture %.3f ms, restore %.3f ms\n",replayed,capture_ms/captures,restore_ms/captures);
    return 0;
}
