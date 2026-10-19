#include "FighterCombat.hpp"
namespace th075 {
static i16 wrap_short(i32 value){const u16 bits=u16(value);i16 result;std::memcpy(&result,&bits,2);return result;}
bool FighterCombat::advance(FighterCombat& opponent,bool storyMode,const CombatView& view){
    bool spiritRecovered=false;
    if(opponent.opponentStop<1){
        if(freeze>0)--freeze;else if(actionDelay>0)--actionDelay;
        opponentPush=opponent.opponentPush=0;
        for(auto& timer:recoveryTimers)if(timer>0)--timer;
        if(frameFlags&0x1000)recoveryTimers[1]=6;
        const bool storySpell=storyMode&&storyOpponent;
        if(spellTimer>0){
            if(storySpell){
                if(spellPhase<spellDrain.size())spellValue=subtract_wrapped(spellValue,spellDrain[spellPhase]);
                if(spellValue<0)spellValue=0;
                spellFrames=increment_wrapped(spellFrames,-1);if(spellFrames<0)spellFrames=0;
            }else{
                spellTimer=wrap_short(spellTimer-2);
                if(spellTimer<1){spellTimer=0;spellEnded=1;spellExtent=0;}
            }
        }
        if(spellCharge>0&&spellEnded==0){
            spellCharge-=spellTimer==0?4.0f:0.4f;
            spellExtent+=spellTimer==0?0.2f:0.02f;
        }
        if(spellExtent<0)spellExtent=0;if(spellExtent>900)spellExtent=900;
        if(spellCharge<0)spellCharge=0;if(spellCharge>1000)spellCharge=1000;
        if(storySpell)spellExtent=900;
        else{
            if(spellTimer>0&&health<maximumHealth)health=wrap_short(health+100);
            if(health>maximumHealth)health=maximumHealth;
        }
        if(spiritBreak<1){
            if(spirit<previousSpirit)spiritDelay=60;
            else if(spiritDelay<1){
                const bool defensiveAction=action>=150&&action<=199;
                if((!defensiveAction||(action!=209&&action!=210))&&spiritDelay==0)spirit=wrap_short(spirit+3);
                if(spirit>1000)spirit=1000;
            }else --spiritDelay;
        }else{
            spiritBreak=wrap_short(spiritBreak-2);spirit=0;
            if(spiritBreak<1){spiritRecovered=true;spirit=1000;}
        }
        previousSpirit=spirit;
    }
    if(health<0)health=0;if(maximumHealth<0)maximumHealth=0;
    if(noticeTimer>0)--noticeTimer;if(flashTimer>0)--flashTimer;
    if(fade!=0)fade=u8(fade-3);
    const double span=320.0-double(view.spanOffset),x=view.horizontalOffset,y=double(view.verticalOffset)-300.0;
    screenBounds={float((-x-320.0)+640.0-span*0.5),float(span*0.5-x+320.0+640.0),float(span*210.0/320.0+y+420.0),float(y-60.0-span*30.0/320.0)};
    elapsed=wrap_short(elapsed+1);return spiritRecovered;
}
}
