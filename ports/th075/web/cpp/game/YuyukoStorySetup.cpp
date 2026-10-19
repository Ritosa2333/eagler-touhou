#include "YuyukoStory.hpp"
namespace th075 {
void initialize_yuyuko_story(FighterActor& a,FighterActor& other,StoryCutscene& s,StoryEnvironment& env){{

  switch(env.stage) {
  case 1:
    s.opponent = 5;
    break;
  case 2:
    s.opponent = 1;
    break;
  case 3:
    s.opponent = 0;
    break;
  case 4:
    s.opponent = 2;
    break;
  case 5:
    s.opponent = 6;
    break;
  case 6:
    s.opponent = 8;
    break;
  case 7:
  case 8:
  case 9:
  case 10:
    s.opponent = 9;
  }
  
}{

  int iVar1;
  
  switch(env.stage) {
  case 1:
    s.step = 0;
    env.background = 5;
    other.stageClearBonus = 50000;
    env.fadeColor = 0;
    env.fadeAlpha = 0xff;
    env.camera.override_view(1,0.0f,120.0f,320.0f);
    env.camera.snap();
    a.motion.x = 0.0f;
    a.motion.y = 0.0f;
    s.alpha[0] = 0;
    s.alpha[1] = 0;
    s.alpha[2] = 0;
    s.alpha[3] = 0;
    set_story_phase(other,3,4000,0,0,0,0,0,0);
    iVar1 = (env.difficulty * 25000 + 50000) / 0x960;
    set_story_phase(other,2,5000,1,env.difficulty,0,env.difficulty * 25000 + 50000,0x960,iVar1);
    set_story_phase(other,1,5000,2,env.difficulty + 4,1,env.difficulty * 25000 + 50000,0x960,(env.difficulty * 25000 + 50000) / 0x960);
    other.motion.x = 800.0f;
    other.select_action(0);
    break;
  case 2:
    s.step = 10;
    env.background = 0;
    other.stageClearBonus = 100000;
    env.fadeColor = 0;
    env.fadeAlpha = 0xff;
    env.camera.override_view(1,0.0f,120.0f,320.0f);
    env.camera.snap();
    a.motion.x = 0.0f;
    a.motion.y = 0.0f;
    s.alpha[0] = 0;
    s.alpha[1] = 0;
    s.alpha[2] = 0;
    s.alpha[3] = 0;
    set_story_phase(other,3,4000,0,0,0,0,0,0);
    set_story_phase(other,2,5000,1,env.difficulty + 8,0,env.difficulty * 50000 + 100000,0x960,(env.difficulty * 50000 + 100000) / 0x960);
    set_story_phase(other,1,5000,2,env.difficulty + 0xc,1,env.difficulty * 50000 + 100000,0x960,(env.difficulty * 50000 + 100000) / 0x960);
    other.motion.x = 800.0f;
    other.select_action(0);
    break;
  case 3:
    s.step = 0x14;
    env.background = 0;
    other.stageClearBonus = 200000;
    env.fadeColor = 0;
    env.fadeAlpha = 0xff;
    env.camera.override_view(1,0.0f,120.0f,320.0f);
    env.camera.snap();
    a.motion.x = 480.0f;
    a.motion.y = 0.0f;
    s.alpha[0] = 0;
    s.alpha[1] = 0;
    s.alpha[2] = 0;
    s.alpha[3] = 0;
    set_story_phase(other,5,4000,0,0,0,0,0,0);
    set_story_phase(other,4,5000,1,env.difficulty + 0x10,0,env.difficulty * 75000 + 150000,0x960,(env.difficulty * 75000 + 150000) / 0x960);
    set_story_phase(other,3,6000,0,0,0,0,0,0);
    set_story_phase(other,2,5000,1,env.difficulty + 0x14,1,env.difficulty * 75000 + 150000,0x960,(env.difficulty * 75000 + 150000) / 0x960);
    set_story_phase(other,1,5000,2,env.difficulty + 0x18,2,env.difficulty * 75000 + 150000,0x960,(env.difficulty * 75000 + 150000) / 0x960);
    other.motion.x = 1280.0f;
    other.select_action(0);
    break;
  case 4:
    s.step = 0x1e;
    env.background = 0xc;
    other.stageClearBonus = 400000;
    env.fadeColor = 0;
    env.fadeAlpha = 0xff;
    env.camera.override_view(1,0.0f,120.0f,320.0f);
    env.camera.snap();
    a.motion.x = 0.0f;
    a.motion.y = 0.0f;
    s.alpha[0] = 0;
    s.alpha[1] = 0;
    s.alpha[2] = 0;
    s.alpha[3] = 0;
    set_story_phase(other,5,4000,0,0,0,0,0,0);
    set_story_phase(other,4,5000,1,env.difficulty + 0x1c,0,env.difficulty * 100000 + 200000,0x960,(env.difficulty * 100000 + 200000) / 0x960);
    set_story_phase(other,3,6000,0,0,0,0,0,0);
    set_story_phase(other,2,5000,1,env.difficulty + 0x20,1,env.difficulty * 100000 + 200000,0x960,(env.difficulty * 100000 + 200000) / 0x960);
    set_story_phase(other,1,5000,2,env.difficulty + 0x24,2,env.difficulty * 100000 + 200000,0x960,(env.difficulty * 100000 + 200000) / 0x960);
    other.motion.x = 800.0f;
    other.select_action(0);
    break;
  case 5:
    s.step = 0x28;
    env.background = 6;
    other.stageClearBonus = 600000;
    env.fadeColor = 0;
    env.fadeAlpha = 0xff;
    env.camera.override_view(1,0.0f,120.0f,320.0f);
    env.camera.snap();
    a.motion.x = 0.0f;
    a.motion.y = 0.0f;
    s.alpha[0] = 0;
    s.alpha[1] = 0;
    s.alpha[2] = 0;
    s.alpha[3] = 0;
    set_story_phase(other,6,4000,0,0,0,0,0,0);
    set_story_phase(other,5,5000,1,env.difficulty + 0x28,0,env.difficulty * 150000 + 300000,0x960,(env.difficulty * 150000 + 300000) / 0x960);
    set_story_phase(other,4,5000,2,env.difficulty + 0x2c,1,env.difficulty * 150000 + 300000,0x960,(env.difficulty * 150000 + 300000) / 0x960);
    set_story_phase(other,3,6000,0,0,0,0,0,0);
    set_story_phase(other,2,5000,1,env.difficulty + 0x30,2,env.difficulty * 150000 + 300000,0x960,(env.difficulty * 150000 + 300000) / 0x960);
    set_story_phase(other,1,5000,2,env.difficulty + 0x34,3,env.difficulty * 150000 + 300000,0x960,(env.difficulty * 150000 + 300000) / 0x960);
    other.motion.x = 1280.0f;
    other.select_action(0);
    break;
  case 6:
    s.step = 0x32;
    env.background = 0x14;
    other.stageClearBonus = 800000;
    env.fadeColor = 0;
    env.fadeAlpha = 0xff;
    env.camera.override_view(1,0.0f,120.0f,320.0f);
    env.camera.snap();
    a.motion.x = 0.0f;
    a.motion.y = 0.0f;
    s.alpha[0] = 0;
    s.alpha[1] = 0;
    s.alpha[2] = 0;
    s.alpha[3] = 0;
    set_story_phase(other,7,4000,0,0,0,0,0,0);
    set_story_phase(other,6,5000,1,env.difficulty + 0x38,0,env.difficulty * 200000 + 400000,0x960,(env.difficulty * 200000 + 400000) / 0x960);
    set_story_phase(other,5,5000,2,env.difficulty + 0x3c,1,env.difficulty * 200000 + 400000,0x960,(env.difficulty * 200000 + 400000) / 0x960);
    set_story_phase(other,4,6000,0,0,0,0,0,0);
    set_story_phase(other,3,5000,1,env.difficulty + 0x40,2,env.difficulty * 200000 + 400000,0x960,(env.difficulty * 200000 + 400000) / 0x960);
    set_story_phase(other,2,5000,2,env.difficulty + 0x44,3,env.difficulty * 200000 + 400000,0x960,(env.difficulty * 200000 + 400000) / 0x960);
    set_story_phase(other,1,5000,2,env.difficulty + 0x48,4,env.difficulty * 200000 + 400000,0x960,(env.difficulty * 200000 + 400000) / 0x960);
    other.motion.x = 800.0f;
    other.select_action(0x89);
    break;
  case 7:
    s.step = 0x3c;
    env.background = 9;
    other.stageClearBonus = 1000000;
    env.camera.override_view(1,0.0f,320.0f,320.0f);
    env.camera.snap();
    a.motion.x = 480.0f;
    a.motion.y = 0.0f;
    s.alpha[0] = 0;
    s.alpha[1] = 0;
    s.alpha[2] = 0;
    s.alpha[3] = 0;
    set_story_phase(other,8,4000,0,0,0,0,0,0);
    set_story_phase(other,7,5000,1,env.difficulty + 0x4c,0,env.difficulty * 250000 + 500000,0x960,(env.difficulty * 250000 + 500000) / 0x960);
    set_story_phase(other,6,0x9c4,2,env.difficulty + 0x50,1,env.difficulty * 250000 + 500000,0x960,(env.difficulty * 250000 + 500000) / 0x960);
    set_story_phase(other,5,6000,0,0,0,0,0,0);
    set_story_phase(other,4,5000,1,env.difficulty + 0x54,2,env.difficulty * 250000 + 500000,0x960,(env.difficulty * 250000 + 500000) / 0x960);
    set_story_phase(other,3,5000,2,env.difficulty + 0x58,3,env.difficulty * 250000 + 500000,0x960,(env.difficulty * 250000 + 500000) / 0x960);
    set_story_phase(other,2,5000,2,env.difficulty + 0x5c,4,env.difficulty * 250000 + 500000,0x960,(env.difficulty * 250000 + 500000) / 0x960);
    set_story_phase(other,1,10000,2,env.difficulty + 0x60,5,env.difficulty * 250000 + 500000,0x12c0,(env.difficulty * 250000 + 500000) / 0x12c0);
    other.motion.x = 800.0f;
    other.select_action(0);
    break;
  case 8:
    s.step = 0x46;
    s.alpha[0] = 0xff;
    s.alpha[1] = 0;
    s.alpha[2] = 0;
    s.alpha[3] = 0;
    break;
  case 9:
    s.step = 0x50;
    s.alpha[0] = 0xff;
    s.alpha[1] = 0;
    s.alpha[2] = 0;
    s.alpha[3] = 0;
    break;
  case 10:
    s.step = 0x5a;
    s.alpha[0] = 0xff;
    s.alpha[1] = 0;
    s.alpha[2] = 0;
    s.alpha[3] = 0;
  }
  return;

}}
}
