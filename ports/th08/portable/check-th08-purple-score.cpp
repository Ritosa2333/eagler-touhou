#include "../th08_web/cpp/game/SpellHistory.hpp"
#include <cassert>
#include <cstring>
using namespace th08;
int main(){
 SpellRecord base{};std::strcpy(base.name,"old");base.game.attempts[0]=12;base.game.captures[0]=8;base.unknown=spell_history_checksum(base);
 SpellRecord original=base,kept=base;std::strcpy(original.name,"changed");std::strcpy(kept.name,"changed");
 encounter_spell(original,0,false,1);encounter_spell(kept,0,false,1,true);
 assert(original.game.attempts[0]==1&&original.game.captures[0]==0);
 assert(kept.game.attempts[0]==13&&kept.game.captures[0]==8);
 original=base;kept=base;std::strcpy(original.name,"changed");std::strcpy(kept.name,"changed");
 capture_spell(original,0,false,1,100);capture_spell(kept,0,false,1,100,true);
 assert(original.game.attempts[0]==0&&original.game.captures[0]==1);
 assert(kept.game.attempts[0]==12&&kept.game.captures[0]==9);
 assert(kept.unknown==spell_history_checksum(kept));
}
