#include "battle-digest.hpp"
int main(int argc,char** argv){
    require(argc==2,"Pass TH15_TEST_ARCHIVE");RetailAssets assets(argv[1]);unsigned replayed=0;double capture_ms=0,restore_ms=0;unsigned captures=0;
    for(const auto selection:{std::array<int,3>{0,1,2},std::array<int,3>{3,3,0}}){
        World world(assets,3,2,selection);auto& b=*world.battle;EnemySpawnRequest main;main.routine="main";require(b.spawn(main),b.error);
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
