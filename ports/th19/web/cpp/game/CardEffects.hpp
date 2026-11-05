#pragma once
#include <algorithm>
#include <vector>
namespace th19 {
inline bool ownsCard(const std::vector<int>&ids,int id){return std::find(ids.begin(),ids.end(),id)!=ids.end();}
struct CardOptionDefinition {int id,script,group;float y;};
inline const CardOptionDefinition* cardOption(int id){
 static constexpr CardOptionDefinition defs[]={{1,2,20,48},{3,6,21,16},{5,12,24,-20},{6,12,25,28},{7,17,26,28},{8,17,27,-40},{13,7,22,-640},{14,7,23,-640},{27,30,32,-22},{35,46,36,-64},{15,21,28,40},{17,23,29,-22},{18,23,30,22},{19,26,31,48},{29,34,33,22},{31,37,34,16},{33,40,35,-64},{37,50,37,16}};
 for(const auto&d:defs)if(d.id==id)return &d;return nullptr;
}
//51bd50 sets emitter485=1 only on cross-field arrivals;4f3910 multiplies by .85.
inline float cardArrivalSpeed(float speed,bool enabled){return enabled?speed*.85f:speed;}
// Native509980 reads stat9c/ae/af/b0 before enemy contact, not bullet contact.
inline bool cardIgnoresSpirit(const std::vector<int>&ids,unsigned flags){
 const unsigned kind=flags&0x700000u;
 return (kind&&(flags&0x80000u)&&ownsCard(ids,16)) ||
  (kind==0x200000u&&ownsCard(ids,32)) || (kind==0x300000u&&ownsCard(ids,30)) ||
  (kind==0x400000u&&ownsCard(ids,34));
}
// 408xxx/409xxx flags a8..ad ->51b480. Kind2 wolf,3 otter,4 eagle.
inline bool cardEnhancesSpirit(const std::vector<int>&ids,int kind){return ownsCard(ids,kind==2?23:kind==3?21:kind==4?25:-1);}
inline bool cardSuppressesSpirit(const std::vector<int>&ids,int kind){return ownsCard(ids,kind==2?24:kind==3?22:kind==4?26:-1);}
// 520450: player distance squared<=2304 and statb4 nonzero: +floor(damage/2).
inline int cardNearDamage(int damage,float dx,float dy,bool enabled){return enabled&&dx*dx+dy*dy<=2304.f?damage+damage/2:damage;}
}
