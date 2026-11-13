#include "AsciiText.hpp"
#include "PortTitle.hpp"
#include <cstring>
namespace th11 {
bool AsciiText::add(const char* text,Vec3 position,const AsciiStyle& style){
    if(!text||style.font<0||style.font>(kPortTitle==13?7:kPortTitle==14?5:3))return false;
    if(requests.size()>=320)return true;
    requests.push_back({std::string(text,std::min(std::strlen(text),size_t(255))),position,style});return true;
}
bool AsciiText::draw(AnmRenderer& renderer,i32 pass,SceneCamera& full,SceneCamera& play){
#if PORT_TITLE == 14
    // Retail40b920 selects playfield for pass2;40be30/40c770 inherit
    // the current camera for the other passes. Glyphs do not reset cameras.
    if(pass==2)renderer.upload_camera(play);
#endif
#if PORT_TITLE == 14
    const float display_scale=float(renderer.render_width)/640.f;
#else
    constexpr float display_scale=1.f;
#endif
    AnmVm vm;vm.initialize();vm.resource=&resource;
    for(const auto& request:requests){
        const auto& style=request.style;if(style.pass!=pass)continue;
#if PORT_TITLE != 14
        renderer.set_camera(style.playfield?play:full,true);
#endif
        const bool filtered=(style.font==0&&style.scale.x!=1)||
            ((kPortTitle==13||kPortTitle==14)&&(style.font==3||style.font==5||(kPortTitle==13&&style.font==7)));
        vm.flags=0x140003u|(filtered?0x80000000u:0);
        vm.scale=style.scale;
#if PORT_TITLE == 14
        Vec3 position=request.position;
        // Retail40b9b0 stores display-scaled request coordinates. This queue
        // keeps logical coordinates until draw; device size is fixed per pass.
        position={float(position.x*display_scale),float(position.y*display_scale),float(position.z*display_scale)};
        if(style.pass==1){position.x=float(position.x+float(i32(renderer.render_width)/2));position.y=float(position.y+float(i32(float(display_scale*16.f))));}
        vm.script_position=position;
#else
        const Vec3& position=request.position; // Name needed in discarded TH14 branch.
        vm.script_position=request.position;
#endif
        const i32 width=style.font==0?spacing:kPortTitle==14?(style.font==1?6:style.font<4?7:12):kPortTitle==13?(style.font<4?7:style.font<6?12:30):style.font==3?12:7;
        float advance=float(double(style.scale.x)*width);
        const i32 height=style.font==0?14:style.font==1?9:kPortTitle==14?(style.font<4?10:16):kPortTitle==13?(style.font<4?10:style.font<6?16:80):style.font==2?10:16;
#if PORT_TITLE == 14
        if(style.horizontal==0||style.horizontal==2){
            if(style.font>=2){
                for(u8 ch:request.text){
                    const bool narrow=style.font<4?ch=='.':ch==',';
                    float step=narrow?float(style.scale.x*4.f):advance;
                    if(style.horizontal==0)step=float(step*.5f);
                    vm.script_position.x=float(vm.script_position.x-float(step*display_scale));
                }
            }else{
                float step=float(float(request.text.size())*advance);
                if(style.horizontal==0)step=float(step*.5f);
                vm.script_position.x=float(vm.script_position.x-float(step*display_scale));
            }
        }
        if(style.vertical==0||style.vertical==2){
            float step=float(height);if(style.vertical==0)step=float(step*.5f);
            vm.script_position.y=float(vm.script_position.y-float(step*display_scale));
        }
#endif
        for(u8 ch:request.text){
            if(ch=='\n'){
#if PORT_TITLE == 14
                // Retail40c2b6 rounds both products before adding the line step.
                vm.script_position.y=float(float(float(style.scale.y*float(height))*display_scale)+vm.script_position.y);
#else
                vm.script_position.y=float(double(style.scale.y)*height+vm.script_position.y);
#endif
#if PORT_TITLE == 14
                vm.script_position.x=position.x;
#else
                vm.script_position.x=request.position.x;
#endif
                continue;}
            if(ch==' '){
#if PORT_TITLE == 14
                vm.script_position.x=float(float(display_scale*advance)+vm.script_position.x);
#else
                vm.script_position.x=float(double(advance)+vm.script_position.x);
#endif
                continue;}
            i32 sprite=0;
            if(style.font==0)sprite=i32(ch)-32;
            else if(style.font==1)sprite=i32(ch)+0x42;
            else if constexpr(kPortTitle==13){
                // TH13 4042a0: fonts 2/3, 4/5 and 6/7 share glyphs;
                // each odd variant sets the native alternate-filter bit.
                advance=float(double(style.scale.x)*width);
                if(style.font<4){
                    switch(ch){case '/':sprite=237;break;case ':':sprite=238;break;case '-':sprite=239;break;case '*':sprite=240;break;case '%':sprite=241;break;case '$':sprite=300;break;case '+':sprite=243;break;case '.':sprite=242;advance=float(double(style.scale.x)*4);break;default:sprite=i32(ch)+(ch>='a'&&ch<='z'?147:179);break;}
                }else if(style.font<6){
                    vm.script_position.y=request.position.y;
                    switch(ch){case '/':sprite=280;break;case '.':sprite=281;break;case 's':sprite=282;break;case '*':sprite=283;break;case ',':sprite=284;advance=float(double(style.scale.x)*4);vm.script_position.y=float(double(request.position.y)+3);break;default:sprite=i32(ch)+222;break;}
                }else{
                    vm.script_position.y=request.position.y;
                    switch(ch){case '.':sprite=295;advance=float(double(style.scale.x)*10);break;case 's':sprite=296;advance=float(double(style.scale.x)*22);break;default:sprite=i32(ch)+237;break;}
                }
            }else if constexpr(kPortTitle==14){
                // Retail 40bf20 uses six fonts. TH14 removed 31 atlas entries
                // before these numeric fonts; lowercase has its own range.
                advance=style.scale.x*float(width);
                if(style.font<4){
                    switch(ch){case '/':sprite=206;break;case ':':sprite=207;break;case '-':sprite=208;break;case '*':sprite=209;break;case '%':sprite=210;break;case '$':sprite=257;break;case '+':sprite=212;break;case '.':sprite=211;advance=style.scale.x*4.f;break;default:sprite=i32(ch)+(ch>='a'&&ch<='z'?116:148);break;}
                }else{
                    vm.script_position.y=position.y;
                    switch(ch){case '/':sprite=249;break;case '.':sprite=250;break;case 's':sprite=251;break;case '*':sprite=252;break;case ',':sprite=253;advance=style.scale.x*4.f;vm.script_position.y=float(float(display_scale*3.f)+position.y);break;default:sprite=i32(ch)+191;break;}
                }
            }else if(style.font==2){
                advance=float(double(style.scale.x)*7);
                switch(ch){case '/':sprite=237;break;case '+':sprite=238;break;case '-':sprite=239;break;case '*':sprite=240;break;case '%':sprite=241;break;case '$':sprite=260;break;case '.':sprite=242;advance=float(double(style.scale.x)*4);break;default:sprite=i32(ch)+0xb3;break;}
            }else{switch(ch){case '/':sprite=253;break;case '.':sprite=254;break;case 's':sprite=255;break;default:sprite=i32(ch)+0xc3;break;}}
            if(!vm.bind_sprite(sprite))return false;
            if(style.shadow){
                vm.color=(style.color>>25)<<24;
                #if PORT_TITLE == 14
                const float shadow_step=float(display_scale*2.f);
                vm.script_position.x=float(vm.script_position.x+shadow_step);vm.script_position.y=float(vm.script_position.y+shadow_step);
#else
                vm.script_position.x=float(double(vm.script_position.x)+2);vm.script_position.y=float(double(vm.script_position.y)+2);
#endif
                if(renderer.draw_ascii_sprite(vm)==-2)return false;
                #if PORT_TITLE == 14
                vm.script_position.x=float(vm.script_position.x-shadow_step);vm.script_position.y=float(vm.script_position.y-shadow_step);
#else
                vm.script_position.x=float(double(vm.script_position.x)-2);vm.script_position.y=float(double(vm.script_position.y)-2);
#endif
            }
            vm.color=style.color;if(renderer.draw_ascii_sprite(vm)==-2)return false;
            #if PORT_TITLE == 14
            vm.script_position.x=float(float(display_scale*advance)+vm.script_position.x);
#else
            vm.script_position.x=float(double(advance)+vm.script_position.x);
#endif
        }
    }
#if PORT_TITLE == 14
    // 40be30's core restores full transforms directly after every pass.
    renderer.upload_camera(full);
    // The playfield wrapper40b920 then performs its own40b976 restore.
    if(pass==2)renderer.upload_camera(full);
#else
    renderer.set_camera(full,true);
#endif
    return true;
}
}
