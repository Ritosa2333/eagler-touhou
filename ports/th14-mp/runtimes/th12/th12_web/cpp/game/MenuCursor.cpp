#include "MenuCursor.hpp"
#include <algorithm>
namespace th11 {
i32 MenuCursor::select(i32 value)noexcept{selected=count==0?value:value>=count?count-1:std::max(value,0);return selected;}
i32 MenuCursor::select_th16(i32 value)noexcept{
    // Own418720: selection itself skips disabled entries, always forwards.
    select(value);if(count<=0)return selected;
    const i32 old=selected;for(i32 tries=0;tries<count;++tries){bool skip=false;for(i32 n=0;n<std::min(disabled_count,16);++n)if(disabled[n]==selected){skip=true;break;}
        if(!skip)return selected;selected=wrapping_add(selected,1);if(selected>=count)selected=0;
    }return selected=old; // Retail does not terminate when all entries are disabled.
}
bool MenuCursor::disable_th16(i32 value)noexcept{
    // Own440c00; reject the native unchecked array overflow.
    if(disabled_count<0||disabled_count>=16)return false;disabled[disabled_count++]=value;select_th16(selected);return true;
}
i32 MenuCursor::move(i32 amount)noexcept{
    if(count<=0)return selected;
    // Native loops indefinitely if every entry is disabled. A bounded cycle
    // retains the old selection for an invalid/empty interactive menu.
    const i32 old=selected;
    for(i32 tries=0;tries<=count;++tries){selected=wrapping_add(selected,amount);
        if(selected>=count)selected=wrap?selected%count:count-1;
        if(selected<0)selected=wrap?(selected%count+count)%count:0;
        bool skip=false;for(i32 i=0;i<std::min(disabled_count,16);++i)if(disabled[i]==selected){skip=true;break;}
        if(!skip)return selected;
    }
    return selected=old;
}
void MenuCursor::push()noexcept{depth=std::clamp(depth,0,15);selections[depth]=selected;counts[depth]=count;depth=std::min(depth+1,15);disabled_count=0;}
void MenuCursor::pop()noexcept{depth=std::clamp(depth-1,0,15);selected=selections[depth];count=counts[depth];disabled_count=0;}
}
