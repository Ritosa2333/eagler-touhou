#include "BattleHud.hpp"
namespace th075 {
namespace {
const PatternFrame& spell_frame(const FighterActor& actor,i32 sequence,i32 index){
    static const PatternFrame empty{};const auto* pattern=actor.animation.pattern;
    if(!pattern||sequence<0||u32(sequence)>=pattern->sequences.size())return empty;
    const auto& frames=pattern->sequences[sequence].frames;
    return index>=0&&u32(index)<frames.size()?frames[index]:empty;
}
u32 white_alpha(u8 alpha){return u32(alpha)<<24|0xffffff;}
}
void BattleHud::render_spells(ImageRenderer& images,ImageRenderer& firstImages,ImageRenderer& secondImages,const BattleHudView& view,const RoundRules& rules,u8 difficulty){
    const std::array<ImageRenderer*,2> characters{&firstImages,&secondImages};
    for(u32 side=0;side<2;side++){
        const auto& actor=*view.fighters[side];if(actor.combat.spellTimer<=0)continue;
        const bool story=view.mode==GameMode::Story&&side==1;
        const i32 sequence=story?i32(actor.phaseVariants[actor.combat.spellPhase])+42:
            actor.combat.spellPhase==2?actor.spellChoices[0]+42:actor.spellChoices[1]+45;
        const i32 origin=spell_frame(actor,sequence,story?difficulty+5:i32(side)).originX;
        const i16 sprite=i16(spell_frame(actor,sequence,story?difficulty+5:0).sprite);
        const auto& p=rules.spellPresentation[side];const double y=p.angle,spread=p.spread;
        auto quad=rectangle_quad(side?440:0,float(y),side?661:221,float(y+42));
        images.quad(0,quad,{641,140,862,182},white_alpha(p.portraitAlpha));
        const u32 text=white_alpha(p.textAlpha);
        if(story){
            images.quad(0,rectangle_quad(425,float(y+spread+32),618,float(y+spread+32+16)),{720,124,913,140},text);
            number(images,520,float(y+spread+32),text,actor.combat.spellValue,4,7);
            const auto& record=rules.storyCards[actor.cardIds[actor.combat.spellPhase]];
            number(images,610,float(y+spread+31),text,u16(record.captures),4,3);
            number(images,639,float(y+spread+33),text,u16(record.attempts),4,3);
        }else{
            const float left=side?430:90,right=side?507:167;
            images.quad(0,rectangle_quad(left,float(y+spread+32),right,float(y+spread+32+16)),{641,124,718,140},text);
            number(images,side?500:160,float(y+spread+31),text,actor.combat.spellTimer,4);
            number(images,side?537:197,float(y+spread+33),text,p.initialTime,4);
        }
        quad=rectangle_quad(float(-origin),float(y+5),float(256-origin),float(y+5+32));
        scale_quad(quad,side?630:10,float(y+16),0,p.scale,p.scale,1);
        characters[side]->quad(sprite,quad,{0,0,256,32},white_alpha(p.ringAlpha));
    }
}
}
