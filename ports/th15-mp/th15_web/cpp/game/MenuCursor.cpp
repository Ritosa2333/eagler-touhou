#include "MenuCursor.hpp"
namespace th15 {
bool MenuCursor::enabled(i32 entry)const noexcept{for(i32 i=0;i<disabled_count;i++)if(disabled[i]==entry)return false;return true;}
bool MenuCursor::any_enabled()const noexcept{for(i32 i=0;i<count;i++)if(enabled(i))return true;return false;}
i32 MenuCursor::select(i32 requested){
    if(count==0)return cursor=requested;
    cursor=requested>=count?count-1:requested<0?0:requested;
    if(!any_enabled()){error="Menu has no enabled entries";return cursor;}
    while(!enabled(cursor)){cursor=wrapping_add(cursor,1);if(cursor>=count)cursor=0;}
    return cursor;
}
i32 MenuCursor::move(i32 direction){
    if(count<1)return cursor;
    if(!any_enabled()){error="Menu has no enabled entries";return cursor;}
    for(i32 attempts=0;attempts<=count;attempts++){
        cursor=wrapping_add(cursor,direction);
        if(cursor>=count)cursor=wrapping?cursor%count:count-1;
        if(cursor<0)cursor=wrapping?((cursor%count)+count)%count:0;
        if(enabled(cursor))return cursor;
    }
    error="Menu movement cannot reach an enabled entry";return cursor;
}
bool MenuCursor::disable(i32 entry){
    if(disabled_count>=i32(disabled.size())){error="Menu disabled-entry capacity exceeded";return false;}
    disabled[disabled_count++]=entry;
    if(count>0&&!any_enabled()){error="Menu has no enabled entries";return false;}
    for(i32 attempts=0;attempts<=disabled_count;attempts++){
        if(enabled(cursor))return true;cursor=wrapping_add(cursor,1);if(cursor>=count)cursor=0;
    }
    error="Menu disabled entries prevent selection";return false;
}
void MenuCursor::push()noexcept{history_cursor[depth]=cursor;history_count[depth]=count;depth++;if(depth>15)depth=15;disabled_count=0;}
void MenuCursor::pop()noexcept{depth--;if(depth<0){depth=0;cursor=0;}else{cursor=history_cursor[depth];count=history_count[depth];}disabled_count=0;}
}
