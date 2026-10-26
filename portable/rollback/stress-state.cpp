// Share the retail-state digest with the correctness regression.
#include "battle-digest.hpp"
#include <algorithm>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#endif
struct Times {
    std::vector<double> values;
    template<class F> void run(F f){const auto start=std::chrono::steady_clock::now();f();values.push_back(std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count());}
    void print(const char* name){std::sort(values.begin(),values.end());double sum=0;for(auto n:values)sum+=n;std::printf("\"%s\":{\"count\":%zu,\"mean\":%.4f,\"p95\":%.4f,\"max\":%.4f}",name,values.size(),sum/values.size(),values[size_t(values.size()*.95)],values.back());}
};
static size_t private_bytes(){
#if defined(_WIN32)
    PROCESS_MEMORY_COUNTERS_EX counters{};counters.cb=sizeof counters;
    if(GetProcessMemoryInfo(GetCurrentProcess(),reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counters),sizeof counters))return counters.PrivateUsage;
#endif
    return 0;
}
static void fill(GameBattle& b){
    BulletShooter shooter;shooter.count=1932;shooter.rows=1;shooter.speed=.3f;shooter.angle_step=6.283185307179586f/1932;shooter.position={0,100,0};shooter.pattern=2;shooter.sprite=0;shooter.color=0;
    require(b.bullet_scene->manager.emit(shooter,0),b.bullet_scene->error);
    for(unsigned i=0;i<24;++i)b.items->spawn(i%2?2:9,{float(int(i)*12-140),80,0},.3f,1);
    if(b.laser_scene->manager.count()<4){
        MovingLaserRequest m;m.position={-100,100,0};m.maximum_length=150;m.initial_length=30;m.end_distance=600;m.width=8;m.speed=1;
        StationaryLaserRequest s;s.position={100,100,0};s.maximum_length=120;s.initial_length=80;s.width=8;s.warmup=30;s.active=120;s.fade=30;
        CurveLaserRequest c;c.position={0,80,0};c.width=8;c.speed=1;c.count=32;
        SegmentedLaserRequest g;g.position={-40,80,0};g.length=40;g.width=8;
        require(b.laser_scene->create_moving(m)&&b.laser_scene->create_stationary(s)&&b.laser_scene->create_curved(c)&&b.laser_scene->create_segmented(g),b.laser_scene->error);
    }
}
int main(int argc,char** argv){
    require(argc>=2,"Pass TH15_TEST_ARCHIVE");ignore_presentation_identity=argc>3&&std::string(argv[3])=="--ignore-presentation-identity";RetailAssets assets(argv[1]);const unsigned limit=argc>2?unsigned(std::atoi(argv[2])):1500;
    unsigned failures=0;for(unsigned stage:{1u,7u})for(unsigned count:{2u,3u}){
        World world(assets,count,0,stage==1?std::array<int,3>{0,1,3}:std::array<int,3>{2,3,0},false,stage);auto& b=*world.battle;
        EnemySpawnRequest main;main.routine="main";require(b.spawn(main),b.error);
        for(unsigned n=0;n<180;++n)world.step(input(n));
        std::array<BattleFrameState,13> ring;BattleFrameState saved;Times capture,restore,step;unsigned max_bullets=0,max_nodes=0,replayed=0;size_t max_memory=0;bool passed=true;unsigned first_difference=0;std::string difference_section;
        for(unsigned frame=0;frame<limit&&passed;){
            if(frame%60==0)fill(b);
            auto& state=ring[frame%ring.size()];capture.run([&]{require(state.capture(b),state.error);});
            step.run([&]{world.step(input(frame));});++frame;
            max_bullets=std::max(max_bullets,b.bullet_scene->manager.visible_count);max_nodes=std::max(max_nodes,world.animations.registry.count());
            if(frame%120==0){
                max_memory=std::max(max_memory,private_bytes());require(saved.capture(b),saved.error);const auto before=digest(world);const auto identity=AnmVm::presentation_counter;
                std::array<DigestTrace,12> traces;std::array<u64,12> expected{};for(unsigned n=0;n<12;++n){world.step(input(frame+n));digest_trace=&traces[n];expected[n]=digest(world);digest_trace=nullptr;}
                restore.run([&]{require(saved.restore(b),saved.error);});
                for(unsigned n=0;n<12;++n){auto wrong=input(frame+n+99);if(n==0)for(auto& lane:wrong)lane.held|=2;world.step(wrong);}
                restore.run([&]{require(saved.restore(b),saved.error);});
                require(digest(world)==before&&AnmVm::presentation_counter==identity,"Dense state or RNG differs immediately after restore");
                for(unsigned n=0;n<12;++n){world.step(input(frame+n));DigestTrace actual;digest_trace=&actual;const auto hash=digest(world);digest_trace=nullptr;if(hash!=expected[n]){passed=false;first_difference=frame+n;for(size_t i=0;i<std::min(actual.size(),traces[n].size());++i)if(actual[i]!=traces[n][i]){difference_section=actual[i].first;std::fprintf(stderr,"FIRST DIFFERENCE stage=%u players=%u frame=%u section=%s expected=%llu actual=%llu\n",stage,count,frame+n,actual[i].first.c_str(),traces[n][i].second,actual[i].second);break;}}if(!passed)break;++replayed;}
                frame+=12;
            }
        }
        max_memory=std::max(max_memory,private_bytes());if(!passed)++failures;
        std::printf("{\"passed\":%s,\"firstDifferenceFrame\":%u,\"firstDifferenceSection\":\"%s\",",passed?"true":"false",first_difference,difference_section.c_str());
        std::printf("\"stage\":%u,\"players\":%u,\"configuredFrames\":%u,\"ringSnapshots\":13,\"validationSnapshots\":1,\"maxBullets\":%u,\"maxAnimationNodes\":%u,\"exactReplaySteps\":%u,\"peakPrivateBytes\":%zu,",stage,count,limit,max_bullets,max_nodes,replayed,max_memory);
        capture.print("captureMs");std::printf(",");restore.print("restoreMs");std::printf(",");step.print("stepMs");std::printf("}\n");std::fflush(stdout);
    }
    return failures?1:0;
}
