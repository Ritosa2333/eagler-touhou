#include "weapon_cancellation.hpp"
namespace th20::source::bullet {
int cancel_filtered_circle(Controller& owner,const sprite::Vec3& center,float radius,std::function<int(const sprite::Vec3&)> accept){
 std::uint32_t count=0;scheduler::Iterator iterator(owner.active.sentinel.next);
 for(;iterator.current;iterator.advance()){
  auto& b=*reinterpret_cast<Bullet*>(iterator.current->value);
  if((b.state!=1&&b.state!=2)||b.field_18||!in_circle(b.position,center,b.size.x/2.f+radius)||!accept(b.position))continue;
  b.flags&=~0x1800u;cancel(b,0);++owner.cancel_counter;++count;
 }
 return recovered::signed_bits(count);
}
}
