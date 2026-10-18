#include "AsciiText.hpp"
#include <cstring>
namespace th11 {
bool AsciiText::add(const char* text,Vec3 position,const AsciiStyle& style){
    if(!text||style.font<0||style.font>3)return false;
    if(requests.size()>=320)return true;
    requests.push_back({std::string(text,std::min(std::strlen(text),size_t(255))),position,style});return true;
}
bool AsciiText::draw(AnmRenderer& renderer,i32 pass,SceneCamera& full,SceneCamera& play){
    AnmVm vm;vm.initialize();vm.resource=&resource;
    for(const auto& request:requests){
        const auto& style=request.style;if(style.pass!=pass)continue;
        renderer.set_camera(style.playfield?play:full,true);
        vm.flags=0x140003u|((style.font==0&&style.scale.x!=1)?0x80000000u:0);
        vm.scale=style.scale;vm.script_position=request.position;
        float advance=float(double(style.scale.x)*(style.font==0?spacing:style.font==3?12:7));
        const i32 height=style.font==0?14:style.font==1?9:style.font==2?10:16;
        for(u8 ch:request.text){
            if(ch=='\n'){vm.script_position.y=float(double(style.scale.y)*height+vm.script_position.y);vm.script_position.x=request.position.x;continue;}
            if(ch==' '){vm.script_position.x=float(double(advance)+vm.script_position.x);continue;}
            i32 sprite=0;
            if(style.font==0)sprite=i32(ch)-32;
            else if(style.font==1)sprite=i32(ch)+0x42;
            else if(style.font==2){
                advance=float(double(style.scale.x)*7);
                switch(ch){case '/':sprite=237;break;case '+':sprite=238;break;case '-':sprite=239;break;case '*':sprite=240;break;case '%':sprite=241;break;case '$':sprite=260;break;case '.':sprite=242;advance=float(double(style.scale.x)*4);break;default:sprite=i32(ch)+0xb3;break;}
            }else{switch(ch){case '/':sprite=253;break;case '.':sprite=254;break;case 's':sprite=255;break;default:sprite=i32(ch)+0xc3;break;}}
            if(!vm.bind_sprite(sprite))return false;
            if(style.shadow){
                vm.color=(style.color>>25)<<24;
                vm.script_position.x=float(double(vm.script_position.x)+2);vm.script_position.y=float(double(vm.script_position.y)+2);
                if(renderer.draw_ascii_sprite(vm)==-2)return false;
                vm.script_position.x=float(double(vm.script_position.x)-2);vm.script_position.y=float(double(vm.script_position.y)-2);
            }
            vm.color=style.color;if(renderer.draw_ascii_sprite(vm)==-2)return false;
            vm.script_position.x=float(double(advance)+vm.script_position.x);
        }
    }
    renderer.set_camera(full,true);return true;
}
}
