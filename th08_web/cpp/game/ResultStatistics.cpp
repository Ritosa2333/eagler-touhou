#include "ResultScreen.hpp"
#include "ResultText.hpp"
#include "SpellCatalogData.hpp"
#include "Localization.hpp"
#include <cstdarg>
#include <cstdio>
namespace th08 {
namespace {
// thcrap stringdefs IDs for the character/team rows of the statistics screen,
// in result_text::characters order (index 12 is the all-characters total).
const char* const stats_character_ids[]={
    "th08 Stats Reimu & Yukari","th08 Stats Marisa & Alice","th08 Stats Sakuya & Remilia",
    "th08 Stats Youmu & Yuyuko","th08 Stats Reimu","th08 Stats Yukari","th08 Stats Marisa",
    "th08 Stats Alice","th08 Stats Sakuya","th08 Stats Remilia","th08 Stats Youmu",
    "th08 Stats Yuyuko","th08 Stats Total (All Characters)",
};
}
bool ResultScreen::text_left(AnmVm& vm,const char* format,...){char buffer[512];va_list args;va_start(args,format);const i32 length=std::vsnprintf(buffer,sizeof(buffer),format,args);va_end(args);return length>=0&&length<i32(sizeof(buffer))&&text.draw(vm,TextAlignment::Left,0xffffff,0,buffer);}
i32 ResultScreen::HandleSpellCardScreen(){
    auto& s=state;if(s.exitingSpellcardResults&&s.frameTimer>=10)set_state(RESULT_SCREEN_STATE_SPELLCARDS_CHOOSING_CHARACTER);
    if(u32(s.selectedSpellcardDifficulty)>=6)return 0;const i32 count=spell_difficulty_counts[s.selectedSpellcardDifficulty];
    if((s.spellcardPage!=s.cursor||s.previousShotType!=s.shotTypeCursor)&&s.frameTimer==10){
        s.spellcardPage=s.cursor;s.previousShotType=s.shotTypeCursor;
        for(i32 i=s.spellcardPage*10;i<s.spellcardPage*10+10&&i<count;i++){
            if(i<0)return 0;const i32 number=spells_by_difficulty[s.selectedSpellcardDifficulty][i];const auto& record=session.records[number];
            text.draw(s.textVms[i%10],TextAlignment::Left,0xffffff,0,record.game.attempts[12]?Localization::SpellName(u32(number),record.name):result_text::unknown_spell);s.textVms[i%10].color1.a=255;
        }
        text_left(s.textVms[10],Localization::FormatStringById("th08 Spell Result Character Select",result_text::spell_heading),s.capturedSpellCards[s.selectedSpellcardDifficulty][s.shotTypeCursor],count);s.textVms[10].color1.a=255;
    }
    if(s.frameTimer<6)return 0;
    if(move_horizontal((count+9)/10)){s.frameTimer=0;s.spriteVms[40].pendingInterrupt=10;}
    else if(move_shot(13)){s.frameTimer=0;s.updateSpellcardResults=1;s.spriteVms[s.previousShotType+27].pendingInterrupt=24;s.spriteVms[s.shotTypeCursor+27].pendingInterrupt=25;}
    if(context.input.pressed(Menu|Bomb)){s.exitingSpellcardResults=1;s.frameTimer=0;actions.sound(11,0);s.spriteVms[40].pendingInterrupt=1;return 1;}
    s.frameTimer2=wrapping_add(s.frameTimer2,1);return 0;
}
i32 ResultScreen::HandleOtherStatsScreen(){
    auto& s=state;auto& p=session.statistics;auto time=[&](){scores.update_time(actions.now_ms());};auto total=[&](){text_left(s.textVms[0],Localization::FormatStringById("th08 Stats Time Since Startup",result_text::total_time),p.total_time[0],p.total_time[1],p.total_time[2]);};
    switch(s.currentState){
    case RESULT_SCREEN_STATE_OTHER_STATS_SCREEN_INIT:
        if(s.frameTimer==1){
            s.textVms[0].pos={56,64,0};time();total();time();s.totalSeconds=p.total_time[2];
            s.textVms[1].pos={56,81,0};text_left(s.textVms[1],Localization::FormatStringById("th08 Stats Total Playtime",result_text::game_time),p.game_time[0],p.game_time[1],p.game_time[2]);s.textVms[2].pos={56,98,0};text_left(s.textVms[2],Localization::StringById("th08 Stats Play Count",result_text::play_count));
            for(i32 i=0;i<12;i++){s.textVms[i+3].pos={56,float(115+i*17),0};text_left(s.textVms[i+3],Localization::FormatStringById("th08 Stats Character Format","%s %6d %6d %6d %6d %6d %6d"),Localization::StringById(stats_character_ids[i],result_text::characters[i]),p.counts[0].characters[i],p.counts[1].characters[i],p.counts[2].characters[i],p.counts[3].characters[i],p.counts[4].characters[i],p.counts[6].characters[i]);}
            s.textVms[15].pos={56,319,0};text_left(s.textVms[15],Localization::FormatStringById("th08 Stats Character Format","%s %6d %6d %6d %6d %6d %6d"),Localization::StringById(stats_character_ids[12],result_text::characters[12]),p.counts[0].total,p.counts[1].total,p.counts[2].total,p.counts[3].total,p.counts[4].total,p.counts[6].total);
            s.textVms[16].pos={56,353,0};text_left(s.textVms[16],Localization::FormatStringById("th08 Stats Clear Count",result_text::clears),p.counts[0].clears,p.counts[1].clears,p.counts[2].clears,p.counts[3].clears,p.counts[4].clears,p.counts[6].clears);
            s.textVms[17].pos={56,370,0};text_left(s.textVms[17],Localization::FormatStringById("th08 Stats Continue",result_text::continues),p.counts[0].continues,p.counts[1].continues,p.counts[2].continues,p.counts[3].continues,p.counts[4].continues,p.counts[5].continues);
            s.textVms[18].pos={56,387,0};text_left(s.textVms[18],Localization::FormatStringById("th08 Stats Practice",result_text::practices),p.counts[0].practices,p.counts[1].practices,p.counts[2].practices,p.counts[3].practices,p.counts[4].practices,p.counts[6].practices);s.textVms[19].pos={56,404,0};
        }
        if(s.frameTimer<40)for(i32 i=0;i<20;i++)s.textVms[i].color1.a=signed_bits(u32(s.frameTimer)*255u)/40;else s.currentState=RESULT_SCREEN_STATE_OTHER_STATS_SCREEN;break;
    case RESULT_SCREEN_STATE_OTHER_STATS_SCREEN:
        if(s.frameTimer%60==0){time();if(p.total_time[2]!=s.totalSeconds){total();s.totalSeconds=p.total_time[2];}}
        if(context.input.pressed(Shoot|Enter|Menu|Bomb)){s.currentState=RESULT_SCREEN_STATE_OTHER_STATS_TO_INIT_TRANSITION;s.frameTimer=0;}break;
    case RESULT_SCREEN_STATE_OTHER_STATS_TO_INIT_TRANSITION:
        if(s.frameTimer<20)for(i32 i=0;i<20;i++)s.textVms[i].color1.a=255-signed_bits(u32(s.frameTimer)*255u)/20;else{s.currentState=RESULT_SCREEN_STATE_INIT;s.frameTimer=0;return 1;}break;
    default:break;
    }return 0;
}
}
