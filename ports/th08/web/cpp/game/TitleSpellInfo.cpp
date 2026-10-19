// Spell information's delayed line updates recovered from TH08, including
// separate normal/Last Word statistics and the original hint table indexing.
#include "TitleMenus.hpp"
#include "TitleSpellText.hpp"
#include "Localization.hpp"
#include <cstdio>
#include <cstdarg>
namespace th08 {
namespace {
// thcrap stringdefs IDs for spell_info_difficulties, in table order.
const char* const difficulty_ids[]={
    "th08 Spell Practice Easy Mode","th08 Spell Practice Normal Mode","th08 Spell Practice Hard Mode",
    "th08 Spell Practice Lunatic Mode","th08 Spell Practice Extra Mode","th08 Spell Practice Last Word",
};
// thcrap stringdefs IDs for spell_info_characters, in table order.
const char* const practice_character_ids[]={
    "th08 Stats Reimu & Yukari","th08 Stats Marisa & Alice","th08 Stats Sakuya & Remilia",
    "th08 Stats Youmu & Yuyuko","th08 Stats Reimu","th08 Stats Yukari","th08 Stats Marisa",
    "th08 Stats Alice","th08 Stats Sakuya","th08 Stats Remilia","th08 Stats Youmu",
    "th08 Stats Yuyuko",
};
// Last Word unlock-condition lines, indexed like spell_unlock_hints
// ([number-204][line]); the contract ID suffix is the displayed card number
// (index+205). Shared second lines use their combined contract IDs. Rows
// whose original second line is blank have no contract record (nullptr).
const char* const condition_ids[18][2]={
    {"th08 Spell Condition Line 1 205","th08 Spell Condition Line 2 205"},
    {"th08 Spell Condition Line 1 206","th08 Spell Condition Line 2 206 207 209"},
    {"th08 Spell Condition Line 1 207","th08 Spell Condition Line 2 206 207 209"},
    {"th08 Spell Condition Line 1 208","th08 Spell Condition Line 2 208 211 217 220"},
    {"th08 Spell Condition Line 1 209","th08 Spell Condition Line 2 206 207 209"},
    {"th08 Spell Condition Line 1 210",nullptr},
    {"th08 Spell Condition Line 1 211","th08 Spell Condition Line 2 208 211 217 220"},
    {"th08 Spell Condition Line 1 212","th08 Spell Condition Line 2 212"},
    {"th08 Spell Condition Line 1 213","th08 Spell Condition Line 2 213 214 222"},
    {"th08 Spell Condition Line 1 214","th08 Spell Condition Line 2 213 214 222"},
    {"th08 Spell Condition Line 1 215","th08 Spell Condition Line 2 215"},
    {"th08 Spell Condition Line 1 216","th08 Spell Condition Line 2 216"},
    {"th08 Spell Condition Line 1 217","th08 Spell Condition Line 2 208 211 217 220"},
    {"th08 Spell Condition Line 1 218","th08 Spell Condition Line 2 218"},
    {"th08 Spell Condition Line 1 219",nullptr},
    {"th08 Spell Condition Line 1 220","th08 Spell Condition Line 2 208 211 217 220"},
    {"th08 Spell Condition Line 1 221","th08 Spell Condition Line 2 221"},
    {"th08 Spell Condition Line 1 222","th08 Spell Condition Line 2 213 214 222"},
};
}
void TitleMenus::DrawTextFormatted(AnmVm* vm,const char* format,...){char buffer[512];va_list args;va_start(args,format);std::vsnprintf(buffer,sizeof(buffer),format,args);va_end(args);text.draw(*vm,TextAlignment::Left,0xffffff,0,buffer);}
void TitleMenus::FormatSpellCardInfo(){
    if(state.currentScreenState==1&&!state.unk0xc29c)return;
    if(context.currentStage<0||context.currentStage>=10||state.cursor<0||state.cursor>=spell_stage_counts[context.currentStage])return;
    const i32 number=spells_by_stage[context.currentStage][state.cursor],difficulty=spell_difficulty(number),shot=context.character;
    if(shot<0||shot>=12)return;
    auto& record=context.spells[number];const u32 attempts=record.practice.attempts[12]+record.game.attempts[12];
    auto* info=state.spellCardInfoVms;const auto line=[&](i32 delay){return state.currentScreenState==0||state.unk0xc29c==delay;};
    char name[49]{},owner[49]{};std::memcpy(name,record.name,48);std::memcpy(owner,record.owner,48);
    if(line(11)){
        char digits[7]{};i32 value=number+1;for(i32 i=2;i>=0;--i){std::memcpy(digits+i*2,spell_info_digits[value%10],2);value/=10;}
        DrawTextFormatted(info,Localization::Utf8(Localization::FormatStringById("th08_spell_description_string_format",spell_info_title)),Localization::Utf8(digits),
            attempts?Localization::Utf8(Localization::SpellName(u32(number),name)):Localization::Utf8(Localization::StringById("th08_????????",spell_info_unknown)));
    }
    if(line(9))DrawTextFormatted(info+1,Localization::Utf8(Localization::FormatStringById("th08 Spell Practice Owner/Difficulty Line",spell_info_owner)),
        attempts?Localization::Utf8(Localization::SpellCommentOwner(u32(number),owner)):Localization::Utf8(Localization::StringById("th08_????????",spell_info_unknown)),
        Localization::Utf8(Localization::StringById(difficulty_ids[difficulty],spell_info_difficulties[difficulty])),
        is_last_spell(number)?Localization::Utf8(Localization::StringById("th08 Spell Practice Last Spell",spell_info_last_spell)):spell_info_empty);
    if(state.currentScreenState==0)DrawTextFormatted(info+2,Localization::Utf8(Localization::FormatStringById("th08 Spell Practice Highscore Title",spell_info_character)),Localization::Utf8(Localization::StringById(practice_character_ids[shot],spell_info_characters[shot])));
    const bool encountered=context.HasSpellCardBeenEncountered(number,12);
    if(line(7)){
        if(!encountered)DrawTextLeft(info+3,0xffffff,0,Localization::Utf8(spell_info_unknown_stats));
        else if(difficulty<=4)DrawTextFormatted(info+3,Localization::Utf8(spell_info_normal_stats),
            record.practice.captures[shot],record.practice.attempts[shot],record.game.captures[shot],record.game.attempts[shot],record.practice.max_bonus[shot],
            record.practice.captures[12],record.practice.attempts[12],record.game.captures[12],record.game.attempts[12],record.practice.max_bonus[12]);
        else DrawTextFormatted(info+3,Localization::Utf8(spell_info_last_word_stats),record.practice.captures[shot],record.practice.attempts[shot],record.practice.max_bonus[shot],record.practice.captures[12],record.practice.attempts[12],record.practice.max_bonus[12]);
    }
    if(state.currentScreenState==0)DrawTextLeft(info+4,0xffffff,0,Localization::Utf8(Localization::StringById("th08 Spell Practice Comment",spell_info_comments)));
    for(i32 i=0;i<2;++i)if(line(i?3:5)){
        if(encountered||number<204||number>221||context.IsLastWordSpellCardAttempted(number)){
            char comment[128]{};std::memcpy(comment,i?record.comment2:record.comment1,64);
            const char* localized=Localization::SpellComment(u32(number),static_cast<std::uint16_t>(i),comment);
            DrawTextLeft(info+5+i,0xffffff,0,record.practice.captures[12]?Localization::Utf8(localized):Localization::Utf8(Localization::StringById("th08_?????????",spell_info_locked_comment)));
        }else{
            const auto& hint=spell_unlock_hints[number-204][i];const char* id=condition_ids[number-204][i];
            DrawTextFormatted(info+5+i,id?Localization::Utf8(Localization::FormatStringById(id,hint.format)):Localization::Utf8(hint.format),hint.arguments[0],hint.arguments[1],hint.arguments[2],hint.arguments[3],hint.arguments[4]);
        }
    }
    info[5].pos.x=info[6].pos.x=96;
    if(line(3))for(i32 i=0;i<7;++i)info[i].color1.a=255;
    if(state.unk0xc29c)state.unk0xc29c=wrapping_sub(state.unk0xc29c,1);
}
}
