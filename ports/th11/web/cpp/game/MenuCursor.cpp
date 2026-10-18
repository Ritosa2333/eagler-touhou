#include "MenuCursor.hpp"
#include <algorithm>
namespace th11 {
i32 MenuCursor::select(i32 value)noexcept{selected=count==0?value:value>=count?count-1:std::max(value,0);return selected;}
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
