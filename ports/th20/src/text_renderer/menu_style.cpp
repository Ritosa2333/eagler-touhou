#include "menu_style.hpp"
namespace th20::source::text {
void style_menu_line(Renderer& r,sprite::Vec3& position,int index,const menu::Cursor& cursor,int flash,int jitter){
    if(cursor.is_excluded(index)){r.color=0xff808080;r.shadow_color=0x40ffffff;}
    else if(cursor.current!=index){r.color=0xff608080;r.shadow_color=0xff404040;}
    else {
        if(flash==0){r.color=0xff80ffff;r.shadow_color=0xff000000;}
        else if(flash%4<2){r.color=0xff000000;r.shadow_color=0xffffff80;}
        else {r.color=0xff80ffff;r.shadow_color=0xffffffff;}
        if(jitter>0){
            // Read-only 56f680, eight-byte pairs. Both original coordinates use the first component.
            static constexpr float displacement[9][2]={{2,0},{-1,2},{0,2},{3,1},{-3,-1},{2,3},{-2,2},{0,-4},{2,0}};
            position.x=displacement[jitter][0]/2.f+position.x;position.y=displacement[jitter][0]/2.f+position.y;
        }
    }
}
}
