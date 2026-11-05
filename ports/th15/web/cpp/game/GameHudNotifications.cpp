#include "GameHud.hpp"
namespace th15 {
bool GameHud::notification(i32 value,i32 kind){
 if(kind<0||kind>6){error="HUD notification kind outside original table";return false;}if(kind==5)return true;const u32 slot=kind<2||kind==6?0:1;if(!check(animations.retire(result_banners[slot])))return false;result_banners[slot]=create(front,kind==6?65:60+kind);if(!result_banners[slot])return false;
 if(kind==0){i32 remaining=value,place=10000000;bool leading=false;for(u32 i=0;i<8;i++){if(!check(animations.retire(bonus_digits[i])))return false;bonus_digits[i]=create(ascii,i32(i)+4);if(!bonus_digits[i])return false;const i32 digit=remaining/place;remaining%=place;place/=10;if(digit)leading=true;auto* vm=animations.registry.find(bonus_digits[i]);if(!vm||!vm->select_sprite(digit+239)){error="Spell bonus digit sprite unavailable";return false;}if(!check(animations.pause(bonus_digits[i],!leading)))return false;}
  for(u32 i=0;i<2;i++){auto& handle=bonus_digits[i+8];if(!check(animations.retire(handle)))return false;if(value>=(i?1000:1000000)){handle=create(ascii,i32(i)+12);if(!handle)return false;auto* vm=animations.registry.find(handle);if(!vm||!vm->select_sprite(253)){error="Spell bonus comma sprite unavailable";return false;}}}
 }
 if(kind<2){tutorial_state=1;background_notice=create(front,95);if(!background_notice)return false;}
 return true;
}
}
