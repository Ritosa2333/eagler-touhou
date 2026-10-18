#include "SpellHistory.hpp"
namespace th08 {
u8 spell_history_checksum(const SpellRecord& record)noexcept{
    u32 checksum=0;for(u32 i=0;i<48&&record.name[i];i++)checksum+=u32(i32(i8(record.name[i])));
    for(const auto* h:{&record.game,&record.practice})for(u32 i=0;i<13;i++)checksum+=u32(h->max_bonus[i])+h->attempts[i]+h->captures[i];return u8(checksum);
}
void validate_spell_history(SpellRecord& record)noexcept{if(record.unknown!=spell_history_checksum(record)){record.game={};record.practice={};}}
void encounter_spell(SpellRecord& record,u32 character,bool practice,u8 difficulty)noexcept{
    validate_spell_history(record);auto& h=practice?record.practice:record.game;
    for(u32 index:{character,12u})if(index<13&&h.attempts[index]<9999)++h.attempts[index];record.difficulty=difficulty;record.unknown=spell_history_checksum(record);
}
void capture_spell(SpellRecord& record,u32 character,bool practice,u8 difficulty,u32 bonus)noexcept{
    validate_spell_history(record);auto& h=practice?record.practice:record.game;
    for(u32 index:{character,12u})if(index<13){if(u32(h.max_bonus[index])<bonus)h.max_bonus[index]=signed_bits(bonus);if(h.captures[index]<9999)++h.captures[index];}
    record.difficulty=difficulty;record.unknown=spell_history_checksum(record);
}
}
