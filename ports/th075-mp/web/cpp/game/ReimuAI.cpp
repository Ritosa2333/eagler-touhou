#include "ReimuAI.hpp"
#include <initializer_list>
#include <cmath>
namespace th075 {
namespace {
i16 decrement(i16 n){return i16(u16(n)-1u);}
bool hurt(i16 action){return action>=50&&action<=149;}
bool recovering(i16 action){return action>=93&&action<=99;}
bool invulnerable_recovery(i16 action){return action==93||action==94;}
bool one_of(i16 action,std::initializer_list<i16> values){for(auto n:values)if(action==n)return true;return false;}
struct Decision {
    FighterActor& self;const FighterActor& other;ReplayRandom& random;
    const i16 action;const float dx,dy;ControlCounts& input;
    Decision(FighterActor& self,const FighterActor& other,ReplayRandom& random):self(self),other(other),random(random),action(self.animation.cursor.action),dx(std::abs(float(double(other.motion.x)-self.motion.x))),dy(std::abs(float(double(other.motion.y)-self.motion.y))),input(self.controls.current){}
    bool chance(i32 threshold){return random.next()<threshold;}
    u8 forward()const{return u8(2-(self.motion.facing!=-1));}
    u8 backward()const{return u8(2-(1-(self.motion.facing!=-1)));}
    void command(u8 code){self.command=code;}
    void charge(i16 strategy,i32 frames,u32 button){self.commandTimers[0]=i16(random.range(0,frames));self.commandTimers[1]=strategy;input.buttons[button]=1;}
    bool continue_strategy(){
        const i16 strategy=self.commandTimers[1];
        if(strategy==1||strategy==6){if(self.commandTimers[0]>0)input.buttons[strategy==1?2:0]=5;else self.commandTimers[1]=0;return true;}
        if(strategy>=2&&strategy<=5){
            const i16 target=std::array<i16,4>{391,390,392,393}[strategy-2];
            if(action==target){self.commandTimers[1]=0;return true;}
            if(action==200){input.horizontal=self.motion.facing;input.buttons[strategy==2||strategy==5?1:0]=1;if(strategy>=4)input.vertical=1;return true;}
            command(u8(2-(1-(self.motion.facing!=1))));return true;
        }
        return false;
    }
    bool escape_guard(){
        if(!((action>=400&&action<500)||(action>=600&&action<700)))return false;
        if(dx>0){
            if(self.motion.y==0){for(u8 code:std::array<u8,3>{3,4,5})if(chance(4000)){command(code);return true;}}
            if(self.motion.y>0){if(chance(4000)){command(forward());return true;}if(chance(4000)){command(backward());return true;}}
        }
        if(action>=400&&action<500){
            if(dx<300&&self.combat.spirit>500&&chance(100)){command(20);return true;}
            if(dx>600&&self.combat.spirit>500&&chance(100)){command(21);return true;}
            if(dx<300&&self.combat.spirit>500&&chance(100)){command(10);return true;}
            if(dx>600&&self.combat.spirit>500&&chance(100)){command(11);return true;}
            if(dx<450&&chance(200)){command(20);return true;}
            if(dx>300&&dx<700&&chance(100)){command(21);return true;}
        }
        return false;
    }
    bool spell_cancel(){
        const auto ready=[&](u32 slot,i16 choice,u8 phase){return self.combat.spellTimer>0&&self.combat.spellExtent>=100&&self.spellChoices[slot]==choice&&self.combat.spellPhase==phase;};
        if(one_of(action,{501,309,393})&&chance(10000)&&ready(0,1,2)){command(13);return true;}
        if(one_of(action,{500,501,309,308,307,390,392,393})&&chance(10000)&&ready(1,1,2)){command(13);return true;}
        if(one_of(action,{500,501,309,310,390,393})&&chance(10000)&&ready(1,2,1)){command(13);return true;}
        if(one_of(action,{393,392,506,301,308,307})&&chance(10000)&&ready(0,2,2)){command(13);return true;}
        if(one_of(action,{393,392,506,301,303})&&chance(10000)&&ready(1,0,1)){command(13);return true;}
        if(one_of(action,{393,392,506,301,303})&&chance(10000)&&ready(0,0,2)){command(13);return true;}
        return false;
    }
    bool follow_hit(bool duel){
        if(!hurt(other.animation.cursor.action)||recovering(other.animation.cursor.action))return false;
        if(duel&&spell_cancel())return true;
        if(action==307&&chance(10000)&&self.combat.spirit>250){charge(6,45,2);return true;}
        if(action==307&&chance(1000)&&self.combat.spirit>500){command(20);return true;}
        if(action==307&&chance(500)){command(100);return true;}
        if(dx>150&&action==307&&chance(500)){command(101);return true;}
        if(action==307&&chance(500)){command(40);return true;}
        if(action==307&&chance(200)){command(41);return true;}
        if(dx<100&&action==307&&chance(5000)&&self.combat.spirit>300){charge(1,30,2);return true;}
        if(dx<200&&one_of(action,{301,308})&&chance(20000)){command(20);return true;}
        if(dx<100&&one_of(action,{301,308})&&chance(5000)&&self.response==1){command(31);return true;}
        if(action==300&&chance(20000)&&dx<80){input.buttons[0]=1;return true;}
        if(action==300&&chance(100)){input.buttons[1]=1;input.horizontal=self.motion.facing;return true;}
        if(action==300&&chance(10000)){input.buttons[0]=1;input.horizontal=self.motion.facing;return true;}
        if(action==301&&chance(15000)){input.buttons[0]=1;input.horizontal=self.motion.facing;return true;}
        if(action==303&&chance(550)){command(100);return true;}
        if(dx>150&&action==303&&chance(300)){command(101);return true;}
        if(dx<100&&action==301&&chance(1000)){command(30);return true;}
        if(action==303&&chance(200)){command(100);return true;}
        if(dx>150&&action==303&&chance(200)){command(101);return true;}
        if(action==301&&chance(1000)){self.commandTimers[1]=3;return true;}
        if(action==301&&chance(1000)){self.commandTimers[1]=5;return true;}
        if(action==302&&chance(15000)){input.buttons[0]=1;input.horizontal=self.motion.facing;return true;}
        if(action==302&&chance(1000)){self.commandTimers[1]=3;return true;}
        if(action==302&&chance(1000)){self.commandTimers[1]=5;return true;}
        if(action==302&&chance(1000)){self.commandTimers[1]=4;return true;}
        if(dx<100&&action==303&&chance(15000)){input.buttons[1]=1;input.horizontal=self.motion.facing;return true;}
        if(dx<100&&action==303&&chance(5000)&&self.combat.spirit>300){charge(1,30,2);return true;}
        if(action==308&&chance(10000)&&self.combat.spirit>300){charge(1,30,2);return true;}
        if(action==308&&chance(10000)&&self.combat.spirit>300){command(10);return true;}
        if(action==308&&chance(200)){command(20);return true;}
        if(dx<60&&chance(10000)){input.buttons[0]=1;return true;}
        return false;
    }
    void neutral(bool duel,i32& walkFrames){
        // Each probability consumes a native RNG value before later range/resource
        // predicates. Keeping that ordering is required for original replays.
        if(double(random.next())<double(self.combat.spirit)*double(0.8f)&&dx>320&&self.combat.spirit>=351){
            charge(1,45,2);if(chance(1000))input.horizontal=self.motion.facing;if(chance(1000))input.vertical=1;return;
        }
        if(double(random.next())<double(self.combat.spirit)&&dx>560&&self.combat.spirit>=251&&dy<100){
            self.commandTimers[1]=6;self.commandTimers[0]=i16(random.range(0,45));input.buttons[0]=1;
            if(chance(2000))input.horizontal=self.motion.facing;if(chance(2000))input.vertical=1;return;
        }
        if(double(random.next())<double(self.combat.spirit)*double(0.5f)&&dx>560&&self.combat.spirit>=251){input.buttons[1]=1;return;}
        if(chance(50))input.vertical=-1;
        if(self.motion.y==0){if(chance(1000)){command(3);return;}if(chance(1000)){command(4);return;}if(dx>400&&chance(1000)){command(5);return;}}
        if(self.motion.y>0){if(chance(1100)){command(forward());return;}if(chance(1100)){command(backward());return;}}
        if(dx>120&&chance(100)){input.horizontal=self.motion.facing;if(chance(18000))input.horizontal=0;input.vertical=-1;return;}
        if(dx>200&&self.motion.y>0&&chance(80)){command(forward());return;}
        if(dx>350&&self.motion.y==0&&chance(50)){command(forward());return;}
        if(dx<400&&chance(100)){command(backward());return;}
        const i16 target=other.animation.cursor.action;
        if(self.motion.y==0&&!recovering(target)){
            if(dx<80&&chance(5000)){input.buttons[0]=1;input.vertical=random.next()>20000?1:0;if(other.motion.y>0)input.vertical=0;return;}
            if(dx<120&&chance(2000)){input.buttons[1]=1;input.vertical=0;return;}
            if(dx<140&&chance(dx>0&&dx<200?1000:100)){input.buttons[1]=1;input.vertical=1;return;}
            if(dx<120&&dx>60&&other.motion.y==0&&chance(100)){input.buttons[1]=1;input.horizontal=self.motion.facing;return;}
            if(dx>60&&dx<300&&chance(400)){input.buttons[0]=1;input.horizontal=self.motion.facing;return;}
            if(dx>200&&dx<400&&chance(600)){self.commandTimers[1]=3;return;}
            if(other.motion.y==0&&dx>100&&dx<500&&chance(100)){self.commandTimers[1]=2;return;}
            if(other.motion.y==0&&dx>200&&dx<500&&chance(600)){self.commandTimers[1]=4;return;}
            if(other.motion.y==0&&dx>300&&dx<500&&chance(250)){self.commandTimers[1]=5;return;}
        }
        if(self.motion.y>0&&!recovering(target)){
            if(dx<300&&chance(6000)&&dy<200&&dy>0){input.buttons[0]=1;return;}
            if(dx<200&&chance(4000)&&dy<250&&dy>80){input.buttons[1]=1;input.vertical=0;return;}
        }
        if(duel){
            const auto ready=[&](u32 slot,i16 choice,u8 phase){return self.combat.spellTimer>0&&self.combat.spellExtent>=100&&self.spellChoices[slot]==choice&&self.combat.spellPhase==phase;};
            if(chance(1500)&&ready(1,2,1)){command(13);return;}
            if(chance(other.motion.y>0?2500:1000)&&dx<350&&ready(1,1,1)){command(13);return;}
            if(chance(1500)&&ready(1,0,1)){command(13);return;}
            if(chance(500)&&dx<300&&ready(1,2,2)){command(13);return;}
            if(chance(1500)&&ready(0,0,2)){command(13);return;}
            if(chance(other.motion.y>0?2000:500)&&dx<200&&ready(0,1,2)){command(13);return;}
        }
        if(dy<150&&dx<100&&chance(600)&&!invulnerable_recovery(target)){command(30);return;}
        if(dx>100&&dx<300&&chance(200)){command(100);return;}
        if(dx>400&&dx<500&&other.motion.y==0&&chance(200)){command(101);return;}
        if(dx>350&&chance(200)&&self.combat.spirit>=351){command(10);return;}
        if(dx>350&&chance(200)&&self.combat.spirit>=351){command(11);return;}
        if(chance(250)&&!invulnerable_recovery(target)){command(40);return;}
        if(chance(100)&&!invulnerable_recovery(target)){command(41);return;}
        if(dy<250&&dy>100&&dx<200&&chance(100)&&!invulnerable_recovery(target)){command(31);return;}
        if(dy<150&&dx>200&&double(random.next())<double(self.combat.spirit)*double(0.1f)){command(20);return;}
        if(dy<150&&dx>400&&double(random.next())<double(self.combat.spirit)*double(0.1f)){command(21);return;}
        if(walkFrames>0){input.horizontal=1;--walkFrames;}
        else if(walkFrames<0){input.horizontal=-1;++walkFrames;}
        else{const i32 duration=random.next()%90+10;walkFrames=duration*(1-2*(random.next()%2));}
    }
    void common(bool duel,i32& walkFrames){if(escape_guard()||follow_hit(duel))return;neutral(duel,walkFrames);}
};
}
void choose_reimu_duel_input(FighterActor& a,const FighterActor& other,ReplayRandom& random,CharacterAIContext& context){
    Decision d(a,other,random);a.commandTimers[0]=decrement(a.commandTimers[0]);
    if(hurt(d.action)){a.commandTimers[1]=0;a.commandTimers[0]=0;}
    if(d.continue_strategy())return;
    const bool canDeclare=a.combat.spellEnded==0&&a.combat.spellTimer<1;
    if(a.combat.spellExtent>300){if(d.dx>560&&d.chance(3)&&canDeclare){d.command(103);return;}if(other.animation.cursor.action==98&&d.chance(20)&&canDeclare){d.command(103);return;}}
    if(a.combat.spellExtent>100){
        if(a.combat.health<4000&&d.dx>560&&d.chance(50)&&canDeclare){d.command(103);return;}
        if(a.combat.health<4000&&d.chance(5)&&canDeclare){d.command(103);return;}
        if(hurt(other.animation.cursor.action)&&a.combat.health<3000&&canDeclare){d.command(103);return;}
    }
    d.common(true,context.duelWalkFrames);
}
void choose_reimu_story_input(FighterActor& a,const FighterActor& other,ReplayRandom& random,CharacterAIContext& context){
    Decision d(a,other,random);a.commandTimers[0]=decrement(a.commandTimers[0]);
    if(hurt(d.action)){
        if(a.commandTimers[2]<0)a.commandTimers[2]=i16(random.range(30,90));a.commandTimers[1]=a.commandTimers[0]=0;
        d.input.horizontal=random.range(0,100)<50?1:-1;if(random.range(0,100)<5)d.input.buttons[0]=1;return;
    }
    const u8 variant=a.combat.spellPhase<a.phaseVariants.size()?a.phaseVariants[a.combat.spellPhase]:255;
    const bool active=a.combat.spellTimer>0;
    const auto hop=[&]{if(a.motion.x<320)d.command(u8(206-(a.motion.facing!=-1)));if(a.motion.x>960)d.command(u8(206-(1-(a.motion.facing!=-1))));};
    if(variant==4&&active){a.commandTimers[2]=decrement(a.commandTimers[2]);if(random.range(0,100)<10){d.command(u8(random.range(0,2)+205));hop();}return;}
    if(variant==3&&active){a.commandTimers[2]=decrement(a.commandTimers[2]);if(a.commandTimers[2]<1)d.command(230);}
    if(variant==2&&active){
        a.commandTimers[2]=decrement(a.commandTimers[2]);if(a.commandTimers[2]<1&&a.motion.y==0)d.command(210);
        else if(random.range(0,100)<10)d.command(u8((random.range(0,100)>50)+207));
        else{d.command(u8((random.range(0,100)>50)+205));hop();}return;
    }
    if(variant==1&&active){a.commandTimers[2]=decrement(a.commandTimers[2]);if(a.commandTimers[2]<1)d.command(220);}
    if(variant==0&&active){a.commandTimers[2]=decrement(a.commandTimers[2]);if(a.commandTimers[2]<1)d.command(200);if(random.range(0,100)<2){d.command(u8(random.range(0,2)+205));hop();return;}}
    if(!d.continue_strategy())d.common(false,context.storyWalkFrames);
}
}
