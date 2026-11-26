#pragma once
#include "core/stages/OmniverseTimelineState.hpp"
namespace kaikeidou::omniverse_timeline_detail {
struct View{
 OmniverseTimelineState& s;std::uint32_t address;
 const std::uint8_t* byte(std::uint32_t a)const{
  if(a>=0xa87ef0u&&a<0xa87fe8u)return s.boss.enemy_prefix.data.data()+a-0xa87ef0u;
  if(a>=0xa87fe8u&&a<0xa8cd68u)return reinterpret_cast<const std::uint8_t*>(s.enemies.enemies.data())+a-0xa87fe8u;
  if(a>=0xa8cd68u&&a<0xa8ce58u)return s.player.data.data()+a-0xa8cd68u;
  if(a>=0xa8ce60u&&a<0xa8e660u)return reinterpret_cast<const std::uint8_t*>(s.projectiles.secondary.data())+a-0xa8ce60u;
  if(a>=0xa8e660u&&a<0xb3e2e0u)return reinterpret_cast<const std::uint8_t*>(s.projectiles.bullets.data())+a-0xa8e660u;
  if(a>=0xa73550u&&a<0xa735c0u)return s.runtime.effect_prefix.data.data()+a-0xa73550u;
  if(a>=0xa735c0u&&a<0xa74ba0u)return reinterpret_cast<const std::uint8_t*>(s.effects.effects.data())+a-0xa735c0u;
  if(a>=0xa758b0u&&a<0xa771b0u)return reinterpret_cast<const std::uint8_t*>(s.items.items.data())+a-0xa758b0u;
  if(a>=0xa74f70u&&a<0xa75830u)return reinterpret_cast<const std::uint8_t*>(s.options.options.data())+a-0xa74f70u;
  if(a>=0xa79090u&&a<0xa7a210u)return reinterpret_cast<const std::uint8_t*>(s.player_shots.weapon.data())+a-0xa79090u;
  throw std::out_of_range("Omniverse object address outside supplied views");
 }
 std::uint32_t word(unsigned o)const{std::uint32_t v=0;for(unsigned i=0;i<4;++i)v|=std::uint32_t(*byte(address+o+i))<<(8*i);return v;}
 std::uint64_t quad(unsigned o)const{std::uint64_t v=0;for(unsigned i=0;i<8;++i)v|=std::uint64_t(*byte(address+o+i))<<(8*i);return v;}
 void write(unsigned o,std::uint64_t v,unsigned n)const{for(unsigned i=0;i<n;++i){const auto a=address+o+i;if((a>=0xa74f70u&&a<0xa75830u)||(a>=0xa79090u&&a<0xa7a210u))throw std::out_of_range("Omniverse read-only player geometry");*const_cast<std::uint8_t*>(byte(a))=static_cast<std::uint8_t>(v>>(8*i));}}
 void word(unsigned o,std::uint32_t v)const{write(o,v,4);}void quad(unsigned o,std::uint64_t v)const{write(o,v,8);}
};
}
