#include "game/SpellRules.hpp"
#include <iostream>
int main(){char command=0;while(std::cin>>command){th128::enemy::PhaseSpell spell;
 if(command=='S'){int mode=0;std::cin>>mode;spell.flags=0xf0;spell.playerMode=mode;if(!th128::spell::begin(spell,1,2280,1))return 1;std::cout<<spell.flags<<' '<<spell.age<<' '<<spell.id<<'\n';}
 else if(command=='B'){int difficulty=0;std::cin>>difficulty;if(!th128::spell::begin(spell,1,2280,difficulty))return 1;std::cout<<spell.baseBonus<<'\n';}
 else{std::cin>>spell.age>>spell.flags;spell.limit=2280;spell.baseBonus=200000;if(!th128::spell::tick(spell))return 1;std::cout<<spell.age<<' '<<spell.value80<<'\n';}
}}
