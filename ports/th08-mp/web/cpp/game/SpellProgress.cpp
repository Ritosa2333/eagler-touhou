// Last Word conditions recovered from the original TH08 routine. The public
// source snapshot leaves this function unimplemented. Keep saved unlock bytes
// when a condition is no longer satisfied, as the original does.
#include "SpellProgress.hpp"
namespace th08 {
i32 spell_difficulty(i32 number) noexcept{
    for(i32 difficulty=0;difficulty<5;++difficulty)for(i32 i=0;i<spell_difficulty_counts[difficulty];++i)if(spells_by_difficulty[difficulty][i]==number)return difficulty;return 5;
}
bool is_last_spell(i32 number) noexcept{for(i32 value:last_spell_numbers)if(value==number)return true;return false;}
bool SpellProgress::encountered(i32 number,i32 shot)const noexcept{return number>=0&&number<222&&shot>=0&&shot<=12&&(spells[number].game.attempts[shot]||spells[number].practice.attempts[shot]);}
bool SpellProgress::captured(i32 number,i32 shot)const noexcept{return number>=0&&number<222&&shot>=0&&shot<=12&&(spells[number].game.captures[shot]||spells[number].practice.captures[shot]);}
bool SpellProgress::last_word_available(i32 number)const noexcept{
    if(number<205)return encountered(number);if(number>=222)return false;
    return last_words.unlocked[number-205]==number;
}
void SpellProgress::unlock_last_words(){
    i32 practice_captures=0,final_b_clears=0,extra_clears=0,last_spell_captures=0;
    for(i32 i=0;i<222;++i)if(spells[i].practice.captures[12])++practice_captures;
    for(i32 shot=0;shot<12;++shot){
        bool final_b=false;for(i32 d=0;d<4;++d)if(clears[shot].without_retries[d]&0x4000)final_b=true;
        if(final_b)++final_b_clears;if(clears[shot].without_retries[4]&0x100)++extra_clears;
    }
    for(i32 number:last_spell_numbers)if(spells[number].practice.captures[12])++last_spell_captures;
    const auto unlock=[&](i32 number,bool condition){if(condition)last_words.unlocked[number-205]=u8(number);};
    unlock(205,final_b_clears>=2);unlock(206,final_b_clears>=3);unlock(207,practice_captures>=50);
    unlock(210,last_spell_captures>=15);unlock(209,captured(137));unlock(208,final_b_clears>=4);
    unlock(211,captured(195)&&encountered(204)&&captured(145));
    unlock(212,encountered(208)&&encountered(209)&&encountered(210));
    unlock(213,encountered(205)&&encountered(206)&&encountered(207)&&encountered(211));
    i32 normal_captures=0;for(i32 i=0;i<spell_difficulty_counts[1];++i)if(captured(spells_by_difficulty[1][i],6))++normal_captures;
    unlock(214,normal_captures==spell_difficulty_counts[1]);
    unlock(215,(clears[4].without_retries[2]&0x4000)||(clears[4].without_retries[3]&0x4000));
    unlock(216,practice_captures>=120);unlock(217,final_b_clears>=6);unlock(218,extra_clears>=3);
    unlock(219,last_spell_captures>=30);unlock(220,clears[12].with_retries[3]&0xc000);
    bool all=true;for(i32 number=205;number<=220;++number)if(!encountered(number))all=false;
    unlock(221,all);
}
}
