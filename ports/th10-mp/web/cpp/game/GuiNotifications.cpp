#include "Gui.hpp"
namespace th10 {
// 0x413810. Constructors before the final clear leave no retained VM state.
void Gui::initialize(Gui** current) noexcept {std::memset(this,0,sizeof(*this));flags=2;*current=this;}
// 0x413790. A negative life count hides all nine icons.
void Gui::update_lives(i32 count) noexcept {i32 i=0;for(;i<count;++i)life_icons[i].flags|=2;for(;i<9;++i)life_icons[i].flags&=~2u;}
// 0x4172e0. The zero-valued bonus hides every digit; the first nonzero digit
// makes all remaining digits visible. Power/life messages use a separate slot.
void Gui::notify(i32 kind,i32 value,GuiAnimationEnvironment& env){
    if(kind==0){
        env.registry->delete_and_clear(notification);notification=env.create(*animations,71);
        bool visible=false;i32 divisor=10000000;
        for(u32 i=0;i<8;++i){auto& id=bonus_digits[i];env.registry->delete_and_clear(id);id=env.create(*animations,39+i);const auto digit=value/divisor;value%=divisor;if(digit)visible=true;if(auto* vm=env.registry->find(id))env.bind_sprite(*vm,wrapping_add(digit,8));env.registry->set_visibility(id,visible);divisor/=10;}
    }else if(kind==1||kind==6){env.registry->delete_and_clear(notification);notification=env.create(*animations,kind==1?72:76);}
    else if(kind>=2&&kind<=4){env.registry->delete_and_clear(power_notification);power_notification=env.create(*animations,kind+71);}
}
}
