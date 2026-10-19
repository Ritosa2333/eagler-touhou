#include "StationaryLaserProgram.hpp"
namespace th15 {
bool StationaryLaserProgram::update(const std::array<BulletTransform,18>& program,LaserVisual& visual,i32& state,i32& protection,float rate){
    error.clear();if(index<0){error="Negative stationary laser instruction index";return false;}
    while(index<18){const auto& entry=program[u32(index)];if(!entry.type||(!entry.active&&flags))break;
        switch(entry.type){case 128:protection=entry.integers[0];break;case 1024:state=3;break;
            case 1048576:visual.body.visual.flags=entry.integers[0]?(visual.body.visual.flags&~0x1c0u)|0x20:(visual.body.visual.flags&~0x1e0u);break;
            // The native stationary handler explicitly advances past every
            // other transform type, without invoking the moving-laser actions.
            default:break;}index=wrapping_add(index,1);
    }
    if(flags){if(flags&0x80000000u){if(freeze.current<=0)flags^=0x80000000u;else freeze.decrement(&rate);}if(protection)protection=wrapping_sub(protection,1);}return true;
}
}
