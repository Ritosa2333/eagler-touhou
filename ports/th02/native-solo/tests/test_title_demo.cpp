#include "title_demo.hpp"
#include <iostream>
#include <string_view>

namespace {
using namespace soew;
using P=TitleDemoPhase;
using O=TitleDemoOutcome;

void expect(bool passed,std::string_view message){
    if(!passed)throw std::runtime_error(std::string(message));
}

struct FakeScene {
    unsigned *steps;
    unsigned terminal_index=title_demo_play_ticks;
};
struct FakeStep {
    void operator()(FakeScene &s,std::uint16_t input,unsigned index)const{
        expect(input==(index?Shot:UpRight),"exact REC input, never live input");
        expect(*s.steps==index,"one simulation update per sampled input word");
        if(index==s.terminal_index)throw std::runtime_error("non-sampler phase");
        ++*s.steps;
    }
};
using Controller=BasicTitleDemoController<FakeScene,FakeStep>;

void recording_contract(){
    std::array<std::byte,title_demo_file_bytes> bytes{};
    bytes[0]=std::byte{0x10};bytes[1]=std::byte{0x40};
    bytes[2*(title_demo_play_ticks-1)]=std::byte{0x22};
    bytes[2*title_demo_play_ticks]=bytes[2*title_demo_play_ticks+1]=std::byte{0xff};
    const auto recording=decode_title_demo(bytes);
    expect(recording.size()==6949&&recording.front()==(Shot|DownLeft)&&recording.back()==(Bomb|Down),
           "little-endian input; last word used is 6948 and trailing garbage is ignored");
    const auto reject=[](std::span<const std::byte> input,std::string_view message){
        try{decode_title_demo(input);}catch(const std::runtime_error&){return;}
        throw std::runtime_error(std::string(message));
    };
    reject(std::span(bytes).first(bytes.size()-1),"short REC must be rejected");
    const std::array<std::byte,title_demo_file_bytes+1> longer{};
    reject(longer,"long REC must be rejected");
    bytes[2]=std::byte{Cancel};
    reject(bytes,"an unverified control-phase input cannot be sampled from the REC");
}

template<class C,class Loader> void wait_for_demo(C &d,Loader &load){
    for(unsigned i=0;i<title_demo_idle_ticks-1;++i)
        expect(!d.update(true,false,load)&&d.phase()==P::Idle,"idle threshold is not early");
    expect(d.update(true,false,load)&&d.phase()==P::Load,"641st idle tick enters load");
}

void controller_contract(){
    unsigned loads=0,steps=0,fail_index=title_demo_play_ticks;
    unsigned last_loaded=0;
    auto load=[&](const TitleDemoSpec &spec){
        last_loaded=spec.number;
        ++loads;steps=0;
        TitleDemoRecording recording;
        recording.fill(Shot);recording.front()=UpRight;
        return TitleDemoRun<FakeScene>{std::make_unique<FakeScene>(FakeScene{&steps,fail_index}),recording};
    };
    Controller disabled;
    for(unsigned i=0;i<title_demo_idle_ticks+1;++i)
        expect(!disabled.update(true,false,load),"attract mode is disabled by default");
    expect(loads==0,"disabled controller never loads resources");
    Controller demo(true);
    for(unsigned i=0;i<title_demo_idle_ticks-1;++i)demo.update(true,false,load);
    expect(!demo.update(true,true,load),"real title input reaches the title menu and resets idle");
    for(unsigned i=0;i<title_demo_idle_ticks-1;++i)demo.update(true,false,load);
    expect(!demo.update(false,false,load),"submenu or loss of focus resets idle");

    wait_for_demo(demo,load);
    expect(demo.current()->number==1&&demo.next().number==2&&loads==0,"first demo selected before loading");
    expect(demo.update(true,false,load)&&last_loaded==1&&demo.phase()==P::Play&&demo.scene()&&steps==0,
           "successful load owns a separate step for presentation setup");
    expect(demo.update(false,true,load)&&steps==0&&demo.phase()==P::Play,"suspend playback when ineligible");
    for(unsigned i=0;i<title_demo_play_ticks-1;++i){
        expect(demo.update(true,false,load)&&demo.phase()==P::Play&&demo.played_ticks()==i+1,
               "REC sampler advances exactly once per eligible update");
    }
    expect(demo.update(true,false,load)&&demo.phase()==P::Score&&demo.outcome()==O::Natural&&
           demo.played_ticks()==6949&&steps==6949&&!demo.scene(),"natural completion retires the scene and enters scoreboard");
    for(unsigned i=0;i<title_demo_score_ticks-1;++i)
        expect(demo.update(true,true,load)&&demo.phase()==P::Score,"initial held input cannot dismiss Score");
    expect(demo.update(true,true,load)&&demo.phase()==P::Rearm&&demo.outcome()==O::Natural,"Score times out without a gameplay step");
    expect(demo.update(true,true,load)&&demo.phase()==P::Rearm,"held input cannot reach the title after Score");
    expect(demo.update(true,false,load)&&demo.phase()==P::Idle,"release rearms title");

    wait_for_demo(demo,load);
    expect(demo.update(true,false,load)&&last_loaded==2&&demo.phase()==P::Play,"next idle selects DEMO2");
    for(unsigned i=0;i<title_demo_play_ticks-1;++i)demo.update(true,false,load);
    expect(demo.update(true,true,load)&&demo.phase()==P::Rearm&&demo.outcome()==O::InputAbort&&
           !demo.scene()&&steps==6948,"live input on final tick aborts before simulation and skips Score");
    demo.update(true,false,load);

    wait_for_demo(demo,load);
    expect(demo.current()->number==3&&demo.next().number==1,"rotation wraps from DEMO3 to DEMO1");
    expect(demo.update(true,true,load)&&demo.outcome()==O::InputAbort&&demo.played_ticks()==0&&loads==2,
           "input during Load aborts without resource work");
    demo.update(true,false,load);

    wait_for_demo(demo,load);
    expect(demo.update(true,false,load)&&last_loaded==1,"rotation persists across input aborts");
    for(unsigned i=0;i<title_demo_play_ticks;++i)demo.update(true,false,load);
    expect(demo.update(true,false,load)&&demo.phase()==P::Score,"release enables Score dismissal");
    expect(demo.update(true,true,load)&&demo.phase()==P::Rearm&&demo.outcome()==O::Natural,"fresh input dismisses Score without altering completion");
    demo.update(true,false,load);

    wait_for_demo(demo,load);
    fail_index=1;
    demo.update(true,false,load);demo.update(true,false,load);
    expect(demo.update(true,false,load)&&demo.phase()==P::Rearm&&demo.outcome()==O::Failed&&!demo.scene()&&
           demo.played_ticks()==1&&steps==1&&demo.error()=="non-sampler phase","unexpected simulation phase is not natural completion");
    demo.update(true,false,load);
    const auto before=loads;
    for(unsigned i=0;i<title_demo_idle_ticks+1;++i)
        expect(!demo.update(true,false,load),"failure disables retry but leaves title usable");
    expect(loads==before,"failure does not retry asset loading");

    Controller missing(true);
    auto bad_load=[](const TitleDemoSpec &)->TitleDemoRun<FakeScene>{throw std::runtime_error("missing REC");};
    wait_for_demo(missing,bad_load);
    expect(missing.update(true,false,bad_load)&&missing.phase()==P::Rearm&&missing.outcome()==O::Failed&&
           !missing.scene()&&missing.played_ticks()==0&&missing.error()=="missing REC","missing recording fails closed");
}

void original_asset_contract(const std::filesystem::path &root){
    struct Checkpoint {int x,y,playperf; unsigned scroll,lives,bombs,power,misses,spawns,score,delta,rng;};
    constexpr std::array<Checkpoint,3> expected{{
        {278,53,1,1737,3,2,73,1,239,265791,0,118},
        {190,202,16,217,3,2,80,0,263,175616,3651,153},
        {241,284,16,217,2,3,80,0,170,69589,0,6},
    }}; // Last sampled tick of each recording in the native demo probe.
    for(const auto &spec:title_demos){
        auto run=load_title_demo(root,spec);
        auto &s=*run.scene;
        expect(s.stage==spec.stage&&s.player.shot==spec.shot&&s.rank==1&&s.life.lives==2&&s.life.bombs==3&&
               s.life.power==80&&s.pickups.power==80&&s.resident_frame==18&&s.life.playperf==12&&s.bullet_core.playperf==12&&
               !s.script.stage4_compatibility,"verified original demo initialization");
        const auto &e=expected[spec.number-1];
        for(unsigned i=0;i<title_demo_play_ticks;++i)TickTitleDemoScene{}(s,run.recording[i],i);
        expect(s.ticks==6949&&s.resident_frame==6967&&s.player.x==e.x&&s.player.y==e.y&&s.life.playperf==e.playperf&&
               s.runtime.scroll.progress==e.scroll&&s.life.lives==e.lives&&s.life.bombs==e.bombs&&s.life.power==e.power&&
               s.life.total_misses==e.misses&&s.runtime.spawns==e.spawns&&s.score_core.score==e.score&&
               s.score_core.delta==e.delta&&s.runtime.random.cursor==e.rng,"verified final gameplay checkpoint");
    }
}
}

int main(int argc,char **argv){
    try{
        recording_contract();controller_contract();
        if(argc==2)original_asset_contract(std::filesystem::path(argv[1]));
        else if(argc!=1)throw std::runtime_error("Usage: test_title_demo [original-assets-directory]");
        std::cout<<"Title demo contract passed\n";
        return 0;
    }catch(const std::exception &e){
        std::cerr<<e.what()<<'\n';
        return 1;
    }
}
