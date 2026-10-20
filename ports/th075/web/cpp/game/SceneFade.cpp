#include "SceneFade.hpp"
namespace th075 {
u16 SceneFade::advance(u16 code,bool animated,SceneFadeSignals& signals){if(count>=0){signals.render_scene();signals.present_scene();--count;if(count>=0)return code;style=0;return code&0xf00?code&0xf0ff:code&0xff;}if(!animated&&(code&0xf00))code=u16((code&0xf0ff)|0x200);const u16 out=code&0xf00;if(out){if(out==0xf00){count=0;style=0;return code;}if(out==0x100||out==0x300)count=40;else if(out>=0x200&&out<=0x700)count=10;else return code;style=out;signals.render_scene();return code;}if(!animated&&(code&0xf000))code=u16((code&0xfff)|0x2000);const u16 in=code&0xf000;if(in==0xf000){count=0;style=0;return code;}if(in==0x1000||in==0x3000)count=40;else if(in==0x2000||in==0x6000)count=10;else return code;style=in;signals.render_scene();return code;}
}
