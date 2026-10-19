#include "ReimuCommands.hpp"
namespace th075 {
void select_reimu_commands(FighterActor& a,const FighterActor& opponent,const ReimuCommandState& state){
    if(!a.animation.pattern||!a.animation.frame)return;
    const i16 action=a.animation.cursor.action;if(action<0||u32(action)>=a.animation.pattern->sequences.size())return;
    const auto& pattern=*a.animation.pattern;const i16 priority=i16(pattern.sequences[action].next);
    const bool allowed=(a.animation.frame->flags[0]&0x20)!=0,ready=action<300||(a.response!=0&&a.response!=3);
    const bool ground=a.motion.y==0,air=a.motion.y>0,spirit=a.combat.spirit>0;
    const auto level=[&](i16 target,bool dash=false,bool repeat=false){return target>=0&&u32(target)<pattern.sequences.size()&&pattern.sequences[target].present&&(priority<=i16(pattern.sequences[target].mode)||(dash&&(priority==49||priority==99))||(repeat&&action==target));};
    const auto can=[&](i16 target,bool dash=false,bool repeat=false){return allowed&&ready&&level(target,dash,repeat);};
    const auto special=[&](i16 target){a.reset_command_history();a.select_action(target);};
    const auto ordinary=[&](i16 target){a.select_action(target);a.controls.buffered.clear();};
    const u8 command=a.command;
    struct StoryCommand {u8 command;i16 target;bool groundOnly;};
    for(const StoryCommand& choice:std::array<StoryCommand,7>{{{230,761,true},{220,751,true},{210,741,true},{200,731,false},{205,735,true},{206,736,true},{207,720,false}}})if(command==choice.command&&(!choice.groundOnly||ground)&&can(choice.target)){special(choice.target==720&&air?722:choice.target);return;}
    if(command==208&&air&&can(723)){special(723);return;}
    if(command==103&&ground&&a.combat.spellExtent>=100&&a.combat.spellEnded==0&&a.combat.spellTimer<=0&&can(795)){special(795);return;}
    if(command==13&&a.combat.spellTimer>=1&&a.combat.spellExtent>=100&&spirit){
        if(a.combat.spellPhase==1){
            if(a.spellChoices[1]==0&&can(701)){special(air?707:701);return;}
            if(a.spellChoices[1]==1&&ground&&can(703)){special(703);return;}
            if(a.spellChoices[1]==2&&ground&&state.activeSeals==0&&can(705)){special(705);return;}
        }
        if(a.combat.spellPhase==2){
            if(a.spellChoices[0]==0&&can(700)){special(air?706:700);return;}
            if(a.spellChoices[0]==1&&ground&&can(702)){special(702);return;}
            if(a.spellChoices[0]==2&&ground&&can(704)){special(704);return;}
        }
    }
    if(command==102&&i8(a.selectedSpell)>0&&ground){if(can(594)){special(action>299?595:594);return;}if(action<=299&&allowed&&level(596)){special(596);return;}}
    if(spirit){
        struct SkillCommand{u8 command;i16 priority,groundAction,airAction;bool groundOnly;};
        for(const SkillCommand& choice:std::array<SkillCommand,8>{{{41,501,501,504,false},{40,500,500,503,false},{31,507,507,507,true},{30,506,506,506,true},{11,600,605,607,false},{10,600,604,606,false},{21,601,601,603,false},{20,600,600,602,false}}})if(command==choice.command&&(!choice.groundOnly||ground)&&can(choice.priority)){special(air?choice.airAction:choice.groundAction);return;}
    }
    if(command==101&&ground&&can(309)){special(309);return;}if(command==100&&ground&&can(310)){special(310);return;}
    const auto& input=a.controls.current;const auto& buffered=a.controls.buffered;
    const i32 relative=a.motion.facing==1?input.horizontal:subtract_wrapped(0,input.horizontal),storedRelative=a.motion.facing==1?buffered.horizontal:subtract_wrapped(0,buffered.horizontal);
    const bool forward=(command==1&&a.motion.facing==1)||(command==2&&a.motion.facing==-1)||(input.buttons[3]!=0&&relative==1)||(input.buttons[3]==1&&relative>0);
    const bool backward=(command==2&&a.motion.facing==1)||(command==1&&a.motion.facing==-1)||(input.buttons[3]!=0&&relative==-1)||(input.buttons[3]==1&&relative<0);
    const bool dashHeight=a.motion.y>=60||a.motion.velocityY<=0,airDash=i8(a.movement.usedAirDashes)<2;
    const auto dash=[&](i16 target){a.reset_command_history();a.movement.usedAirDashes=u8(a.movement.usedAirDashes+1);a.select_action(target);};
    if(forward&&ground&&action!=207&&can(200)){special(200);return;}
    if(forward&&air&&dashHeight&&airDash&&can(202,true)){dash(202);return;}
    if(backward&&ground&&can(201)){special(201);return;}
    if(backward&&air&&airDash&&can(203,true)){dash(203);return;}
    const bool up=(input.buttons[3]!=0&&input.vertical==-1)||(input.buttons[3]==1&&input.vertical<-1);
    if((command==3||(up&&input.horizontal==0))&&ground&&can(204,true)){special(204);return;}
    if((command==4||(up&&relative<0))&&ground&&can(206,true)){special(206);return;}
    if((command==5||(up&&relative>0))&&ground&&can(205,true)){special(205);return;}
    const bool pressA=input.buttons[0]==1||buffered.buttons[0]!=0,pressB=input.buttons[1]==1||buffered.buttons[1]!=0,pressC=input.buttons[2]==1||buffered.buttons[2]!=0;
    // The original B dash checks buffered A in two branches; retain this
    // behavior because changing it alters recorded inputs and cancel timing.
    const bool dashB=input.buttons[1]==1||buffered.buttons[0]!=0;
    const bool heldForward=relative>0||storedRelative>0,down=input.vertical>0||buffered.vertical>0;
    const double distance=double(a.motion.x)-double(opponent.motion.x);
    const auto far=[&](double pixels){return distance>pixels||distance<-pixels;};
    if(ground&&down){
        if(pressA&&heldForward&&action==200&&allowed){ordinary(392);return;}
        if(pressA&&spirit&&far(480)&&can(408)){ordinary(408);return;}
        if(pressA&&can(303)){ordinary(303);return;}
        if(dashB&&heldForward&&action==200&&allowed){ordinary(393);return;}
        if(pressB&&spirit&&far(360)&&can(414)){ordinary(414);return;}
        if(pressB&&can(304)){ordinary(304);return;}
        if(pressC&&spirit&&can(402)){ordinary(402);return;}
    }
    if(ground&&input.vertical==0){
        if(pressA&&heldForward&&action==200&&allowed){ordinary(390);return;}
        if(pressA&&heldForward){if(spirit&&far(320)&&can(407)){ordinary(407);return;}if(can(308)){ordinary(308);return;}}
        if(pressA){if(spirit&&far(320)&&can(406)){ordinary(406);return;}if(can(300,false,true)){ordinary(300);return;}}
        if(dashB&&heldForward&&action==200&&allowed){ordinary(391);return;}
        if(pressB&&heldForward){if(spirit&&far(320)&&can(412)){ordinary(412);return;}if(can(307)){ordinary(307);return;}}
        if(pressB){if(spirit&&far(320)&&can(412)){ordinary(412);return;}if(can(301)){ordinary(301);return;}}
        if(pressC&&heldForward&&spirit&&can(401)){ordinary(401);return;}
        if(pressC&&spirit&&can(400)){ordinary(400);return;}
    }
    if(air){
        if(pressA){if(down&&spirit&&far(360)&&can(411)){ordinary(411);return;}if(spirit&&far(480)&&can(410)){ordinary(410);return;}if(can(305)){ordinary(305);return;}}
        if(pressB){if(spirit&&far(360)&&can(416)){ordinary(416);return;}if(can(306)){ordinary(306);return;}}
        if(pressC&&down&&spirit&&can(405)){ordinary(405);return;}
        if(pressC&&spirit&&can(404)){ordinary(404);return;}
        if(input.buttons[3]!=0||buffered.buttons[3]!=0){if(airDash&&relative==1&&dashHeight&&can(202,true)){dash(202);return;}if(airDash&&relative==-1&&can(203,true)){dash(203);return;}}
        if(input.buttons[3]==1||buffered.buttons[3]==1){if(airDash&&relative>0&&dashHeight&&can(202,true)){dash(202);return;}if(airDash&&relative<0&&can(203,true))dash(203);}
    }
}
}
