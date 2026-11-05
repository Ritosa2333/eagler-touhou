#include "ReimuObjects.hpp"
#include "ObjectTools.hpp"
namespace th075 {
void advance_reimu_object(ReimuObject& p,FighterActor& owner,FighterActor& opponent,ReimuState& state,ReplayStreams& random,FighterActionEvents& events){
    if(opponent.combat.opponentStop!=0||(owner.combat.opponentStop!=0&&p.contactDelay==0))return;
    if(p.freeze!=0){p.freeze=object::add(p.freeze,-1);return;}
    if(!advance_reimu_object_basic(p,owner,opponent,state,random,events)&&!advance_reimu_object_story(p,owner,opponent,state,random,events))p.advance();
}
}
