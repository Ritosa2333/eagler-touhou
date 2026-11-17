#pragma once
#include "Types.hpp"
#include <algorithm>
namespace th17 {
struct MenuCursor {
    i32 selected=0,reserved=0,count=0;
    i32 saved_selection[16]{},saved_count[16]{},depth=0,disabled[16]{},wrap=0,disabled_count=0;
    bool is_disabled(i32 value)const noexcept{for(i32 i=0;i<disabled_count&&i<16;i++)if(disabled[i]==value)return true;return false;}
    bool disable(i32 value)noexcept{if(disabled_count<0||disabled_count>=16)return false;disabled[disabled_count++]=value;for(i32 tries=0;tries<count&&is_disabled(selected);tries++)if(++selected>=count)selected=0;return !is_disabled(selected);}
    void push()noexcept{saved_selection[depth]=selected;saved_count[depth]=count;depth=std::min(depth+1,15);disabled_count=0;}
    void pop()noexcept{if(--depth<0){depth=selected=disabled_count=0;return;}selected=saved_selection[depth];count=saved_count[depth];disabled_count=0;}
    bool step(i32 direction)noexcept{
        if(count<1)return false;
        for(i32 tries=0;tries<count;tries++){
            selected=wrapping_add(selected,direction);
            while(selected>=count)selected=wrap?selected-count:count-1;
            while(selected<0)selected=wrap?selected+count:0;
            if(!is_disabled(selected))return true;
        }
        return false;
    }
    i32 select(i32 value)noexcept{
        if(!count){selected=value;return value;}
        selected=std::max(0,std::min(value,count-1));
        for(i32 tries=0;tries<count;tries++){if(!is_disabled(selected))break;if(++selected>=count)selected=0;}
        return selected;
    }
};
static_assert(sizeof(MenuCursor)==0xd8);
}
