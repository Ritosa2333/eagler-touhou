#include "MarisaStory.hpp"
namespace th075 {
void initialize_marisa_story(FighterActor& a,FighterActor& other,StoryCutscene& scene,StoryEnvironment& env){
    const u8 stage=env.stage;
    if(stage>=1&&stage<=10){scene.opponent=std::array<u8,10>{3,2,4,6,0,8,9,9,9,9}[stage-1];scene.step=u8((stage-1)*10);scene.alpha={stage>=8?u8(255):u8(0),0,0,0};}
    if(stage<1||stage>7)return;
    constexpr i32 bonuses[]={50000,100000,200000,400000,600000,800000,1000000};
    constexpr u8 backgrounds[]={3,10,4,6,0,20,9};
    other.stageClearBonus=bonuses[stage-1];env.background=backgrounds[stage-1];
    env.fadeColor=0;env.fadeAlpha=stage<=6?255:0;
    env.camera.override_view(true,0,stage==7?320.f:120.f,320);env.camera.snap();
    a.motion.x=stage==7?480.f:0.f;a.motion.y=0;
    const i32 values=i32(env.difficulty)*std::array<i32,7>{25000,50000,75000,100000,150000,200000,250000}[stage-1]+std::array<i32,7>{50000,100000,150000,200000,300000,400000,500000}[stage-1];
    const auto normal=[&](u8 phase,i16 health){set_story_phase(other,phase,health,0,0,0,0,0,0);};
    u8 card=std::array<u8,7>{0,8,16,28,40,56,76}[stage-1];
    const auto spell=[&](u8 phase,u8 kind,u8 variant,i16 health=5000,i32 time=2400){set_story_phase(other,phase,health,kind,u8(card+env.difficulty),variant,values,time,values/time);card=u8(card+4);};
    switch(stage){
    case 1:case 2:normal(3,4000);spell(2,1,0);spell(1,2,1);break;
    case 3:case 4:normal(5,4000);spell(4,1,0);normal(3,6000);spell(2,1,1);spell(1,2,2,stage==4?7500:5000);break;
    case 5:normal(6,4000);spell(5,1,0);spell(4,2,1);normal(3,6000);spell(2,1,2);spell(1,2,3);break;
    case 6:normal(7,4000);spell(6,1,0);spell(5,2,1);normal(4,6000);spell(3,1,2);spell(2,2,3);spell(1,2,4);break;
    case 7:normal(8,4000);spell(7,1,0);spell(6,2,1,2500);normal(5,6000);spell(4,1,2);spell(3,2,3);spell(2,2,4);spell(1,2,5,10000,4800);break;
    }
    other.motion.x=stage==7?800.f:1280.f;other.select_action(0);
}
}
