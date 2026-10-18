#include "ResultScreen.hpp"
namespace th08 {
i32 ResultScreen::move(i32& cursor,i32 length,u16 negative,u16 positive){
    if(scrolling(negative)){cursor=wrapping_sub(cursor,1);if(cursor<0)cursor=wrapping_add(cursor,length);actions.sound(12,0);return -1;}
    if(scrolling(positive)){cursor=wrapping_add(cursor,1);if(cursor>=length)cursor=wrapping_sub(cursor,length);actions.sound(12,0);return 1;}return 0;
}
}
