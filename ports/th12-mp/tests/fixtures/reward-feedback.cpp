#include "../../src/game/GameState.hpp"
#include "../../src/game/ItemPresentation.hpp"
#include <cassert>
using namespace th12;
extern "C" {
int load_feedback_bank(int bank,const uint8_t* bytes,int size){std::string error;return animationScene.registry.loadBank(bank,bytes,size,error);}
void feedback_preview(int kind){
  animationScene.reset();spellState.reset();spellAnimationKeys={};bossHud={};
  if(kind==0){spell_system::Result r;r.kind=spell_system::ResultKind::Captured;r.bonusDisplayed=1008030;endSpellAnimations(r);}
  else if(kind==1)showFullPowerNotice();
  else{
    ufoManager.reset();itemManager.reset();resetItemPresentation();auto r=item_system::ResourceState::initial(1);
    for(int color:{0,0,1}){const auto plan=ufoManager.collectToken(color,r);itemPresentationPlan(plan);consumeUfoPresentation();}
    animationScene.reset();resetItemPresentation();
  }
  for(int tick=0;tick<90;tick++){frame=tick;syncItemPresentation();animationScene.tickPrimary();animationScene.collect();}
}
int check_spell_feedback(){
  for(int bonus:{0,90,1230,1008030,12345670}){
    animationScene.reset();spellState.reset();spellAnimationKeys={};
    spell_system::Result result;result.kind=spell_system::ResultKind::Captured;result.bonusDisplayed=bonus;
    endSpellAnimations(result);
    for(int tick=0;tick<54;tick++){
      int digits=0,commas=0;
      for(const auto& [id,node]:animationScene.nodes()){
        const auto& s=node->vm.state();if(s.bank!=11)continue;
        if(s.script>=5&&s.script<=12){const auto d=spell_system::resultDigits(bonus)[s.script-5];assert(s.sprite==d.sprite);assert(s.visible==d.visible);++digits;}
        if(s.script==13||s.script==14){assert(s.sprite==257);const auto& sprite=animationScene.registry.bank(11)->sprites[257];assert(s.spriteWidth==sprite.width&&s.spriteHeight==sprite.height);++commas;}
      }
      assert(digits==8);assert(commas==int(bonus>=1000)+int(bonus>=1000000));
      animationScene.tickPrimary();animationScene.collect();
    }
  }
  return 1;
}
int check_power_feedback(){
  character=2;stageNumber=6;dialogue=0;gameSpeed=1;playerState=0;px=0;py=200*128;
  for(int initial:{100,399,400}){
    animationScene.reset();itemManager.reset();ufoManager.reset();bossHud={};resetItemPresentation();
    resources=item_system::ResourceState::initial(1);resources.power=initial;
    itemManager.spawn(8,{0,200},0,0);tickItems(false);
    assert(resources.power==resources.maxPower);
    auto* notice=animationScene.find(bossHud.itemNotice);
    if(initial<400){assert(notice&&notice->vm.state().bank==7&&notice->vm.state().script==45&&notice->vm.state().sprite==44);}
    else assert(!notice);
  }
  return 1;
}
int check_stock_feedback(){
  for(int rotations=1;rotations<=3;rotations++){
    animationScene.reset();itemManager.reset();ufoManager.reset();resetItemPresentation();
    auto r=item_system::ResourceState::initial(1);
    auto collect=[&](int color){auto plan=ufoManager.collectToken(color,r);itemPresentationPlan(plan);consumeUfoPresentation();};
    collect(0);collect(0);for(int n=0;n<rotations;n++)collect(n%2?0:1);
    assert(ufoManager.inventory.rotation==rotations%3);
    // Rebuild the stage's ANM scene while carrying the live inventory.
    animationScene.reset();resetItemPresentation();
    for(int tick=0;tick<12;tick++){frame=tick;syncItemPresentation();animationScene.tickPrimary();}
    for(int logical=0;logical<3;logical++){
      const int physical=ufoManager.inventory.physicalSlot(logical);
      bool found=false;for(const auto& [id,node]:animationScene.nodes())if(node->owner==0xfffe0000u+physical){
        const auto& s=node->vm.state();assert(s.x==52+21*logical&&s.y==456);assert(s.sprite==59+ufoManager.inventory.colors[logical]);found=true;
      }assert(found);
    }
  }
  return 1;
}
}
