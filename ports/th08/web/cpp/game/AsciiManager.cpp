// TH08 1.00d display logic. Names and behavior cross-checked against the
// MIT GensokyoClub/th08 reference; original instruction results are the oracle.
#include "AsciiManager.hpp"
#include "Localization.hpp"
#include "Presentation.hpp"
#include "PresentationAudit.hpp"
#include <cmath>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <algorithm>
#include <string>
#include <vector>
namespace th08 {
namespace {
float add(float a,float b){return Scalar::add(a,b);}
float sub(float a,float b){return Scalar::sub(a,b);}
float mul(float a,float b){return Scalar::mul(a,b);}
Extended integer(i32 x){return Extended::from_int(x);}
i32 popup_alpha(const Vec3& player,const Vec3& position){
    const float x=sub(player.x,position.x),y=sub(player.y,position.y);
    const i32 distance=(number(x)*number(x)+number(y)*number(y)).truncate_int();
    return distance>4096?208:distance>1024?80+(distance-1024)*128/3072:80;
}
// thcrap ascii_vpatchf_th07_th08 port: the ASCII overlay font can only draw
// single-byte sprite glyphs, so a translation containing non-ASCII bytes is
// rejected and the original format is kept.
bool single_byte_translation(const char* text){
    if(!text)return false;
    for(const auto* p=reinterpret_cast<const unsigned char*>(text);*p;++p)if(*p>=0x80)return false;
    return true;
}
template<typename T>
bool append_printf_piece(std::string& output,const std::string& specifier,T value){
    const int length=std::snprintf(nullptr,0,specifier.c_str(),value);
    if(length<0||length>4096)return false;
    std::vector<char> buffer(std::size_t(length)+1);
    if(std::snprintf(buffer.data(),buffer.size(),specifier.c_str(),value)!=length)return false;
    output.append(buffer.data(),std::size_t(length));return true;
}
// Re-emits each printf piece separately so %s arguments can be translated and
// malformed specifiers can never reach vsnprintf with a mismatched va_list.
bool format_legacy_ascii(std::string& output,const char* format,va_list args,bool translate_strings){
    if(!format)return false;
    output.clear();
    for(std::size_t index=0;format[index]!='\0';++index){
        if(format[index]!='%'){output.push_back(format[index]);continue;}
        const std::size_t start=index++;
        if(format[index]=='\0')return false;
        if(format[index]=='%'){output.push_back('%');continue;}
        while(std::strchr("-+ #0'",format[index]))++index;
        if(format[index]=='*')return false;
        while(format[index]>='0'&&format[index]<='9')++index;
        if(format[index]=='$')return false;
        if(format[index]=='.'){++index;if(format[index]=='*')return false;while(format[index]>='0'&&format[index]<='9')++index;}
        if(format[index]=='\0'||std::strchr("hljztLI",format[index]))return false;
        const char conversion=format[index];
        const std::string specifier(format+start,index-start+1);
        if(conversion=='d'||conversion=='i'||conversion=='c'){
            if(!append_printf_piece(output,specifier,va_arg(args,int)))return false;
        }else if(std::strchr("uoxX",conversion)){
            if(!append_printf_piece(output,specifier,va_arg(args,unsigned)))return false;
        }else if(std::strchr("fFeEgGaA",conversion)){
            if(!append_printf_piece(output,specifier,va_arg(args,double)))return false;
        }else if(conversion=='s'){
            const char* value=va_arg(args,const char*);
            if(!value)value="(null)";
            const char* translated=translate_strings?Localization::AsciiString(value):value;
            if(translated!=value&&!single_byte_translation(translated))translated=value;
            if(!append_printf_piece(output,specifier,translated))return false;
        }else if(conversion=='p'){
            if(!append_printf_piece(output,specifier,va_arg(args,void*)))return false;
        }else return false;
    }
    return true;
}
// Formats one add_format call through the EAS1 table. Returns false when the
// pack cannot reproduce the call safely; the caller then falls back to the
// exact vanilla vsnprintf path.
bool format_localized_ascii(AsciiManager& manager,const Vec3& source,Vec3& adjusted,
                            char* output,std::size_t capacity,const char* format,va_list args){
    adjusted=source;
    Localization::AsciiEntryView entry{};
    const bool active=Localization::Active();
    const bool known=active&&Localization::LookupAscii(format,entry);
    const char* selected=known&&entry.hasTranslation&&single_byte_translation(entry.text)?entry.text:format;
    std::string formatted;
    va_list copy;va_copy(copy,args);
    const bool ok=format_legacy_ascii(formatted,selected,copy,active);
    va_end(copy);
    if(!ok||formatted.size()>=capacity)return false;
    std::memcpy(output,formatted.c_str(),formatted.size()+1);
    if(known&&entry.hasAlignment){
        // ascii_vpatchf_th07_th08 recenters the translated string on the
        // baseline extent plus the game's alignment offset; the TH08 branch
        // advances each glyph by space_width*scale_x.
        const float char_width=float(manager.state.space_width)*manager.state.scale_x;
        const float baseline_extent=float(std::strlen(entry.baseline))*char_width;
        const float output_extent=float(formatted.size())*char_width;
        adjusted.x=(number(source.x)+number(baseline_extent)/number(2)+number(entry.extraX)-number(output_extent)/number(2)).to_float();
    }
    return true;
}
}
void AsciiManager::start(AnmVm& vm,AnmLoaded& file,i32 script){
    vm.anmFile=&file;vm.scriptIndex=i16(script);executor.start(file,vm,file.scripts[script]);
}
void AsciiManager::set_sprite(AnmVm& vm,i32 sprite,bool initialize){
    if(!state.ascii)return;
    if(initialize){vm.Initialize();vm.anmFile=state.ascii;}
    state.ascii->SetSprite(&vm,sprite);
}
void AsciiManager::direct_sprite(AnmVm& vm,i32 sprite){
    if(state.ascii&&sprite>=0&&u32(sprite)<state.ascii->spriteCount)vm.loadedSprite=&state.ascii->sprites[sprite];
}
void AsciiManager::reset(){
    auto& s=state;
    presentation_state=PresentationState{};
    std::memset(score_popup_previous,0,sizeof(score_popup_previous));for(auto& p:score_popup_previous)p.timer=-2;
#if defined(TH_PRESENTATION_AUDIT)
    std::memset(score_popup_generation,0,sizeof(score_popup_generation));std::memset(time_popup_generation,0,sizeof(time_popup_generation));
#endif
    std::memset(&s.small_score_text,0,sizeof(AnmVm));std::memset(&s.popup_text,0,sizeof(AnmVm));std::memset(&s.large_text,0,sizeof(AnmVm));
    std::memset(s.strings,0,sizeof(s.strings));std::memset(&s.pause,0,sizeof(s.pause));std::memset(&s.retry,0,sizeof(s.retry));
    std::memset(s.score_popups,0,sizeof(s.score_popups));std::memset(s.time_popups,0,sizeof(s.time_popups));
    s.string_count=s.gui=s.selected=s.next_score=s.next_player=s.unused=0;
    // Reset deliberately preserves the next time-popup index and gauge state.
    s.color=0xffffffff;s.scale_x=s.scale_y=1;
    s.small_score_text.anchor=s.popup_text.anchor=3;
    set_sprite(s.small_score_text,0,true);set_sprite(s.popup_text,136,true);set_sprite(s.large_text,32,true);
    s.small_score_text.pos.z=.1f;s.space_width=13;
}
void AsciiManager::snapshot_presentation(const AsciiContext& c){
    if(!presentation_marker.capture())return;
    for(u32 i=0;i<4;++i){presentation_state.boss_markers[i]=state.boss_markers[i].pos;presentation_state.boss_marker_vm[i].capture(state.boss_markers[i]);}
    presentation_state.gauge_vm.capture(state.gauge);presentation_state.human_icon_vm.capture(state.human_icon);presentation_state.youkai_icon_vm.capture(state.youkai_icon);
    presentation_state.cursor_vm.capture(state.cursor);presentation_state.percentage_vm.capture(state.percentage);presentation_state.border_vm.capture(state.border);
    presentation_state.player=c.player;presentation_state.gauge=c.gauge;
    presentation_state.blindness_radius=state.blindness_radius;presentation_state.blindness_color=state.blindness_color;presentation_state.valid=true;
}
void AsciiManager::initialize_vms(const AsciiContext& c){
    if(!state.ascii)return;
    auto& s=state;
    for(const auto pair:{std::pair<AnmVm*,i32>{&s.gauge,5},{&s.youkai_icon,7},{&s.human_icon,6},{&s.cursor,8},{&s.percentage,4},{&s.border,9},
        {&s.boss_markers[0],10},{&s.boss_markers[1],10},{&s.boss_markers[2],10},{&s.boss_markers[3],10}})start(*pair.first,*s.ascii,pair.second);
    s.human_icon.pos.x=(number(s.human_icon.pos.x)-(integer(c.human_limit)*number(56)/number(-10000))).to_float();
    s.youkai_icon.pos.x=(number(s.youkai_icon.pos.x)+(integer(c.youkai_limit)*number(56)/number(10000))).to_float();
    set_gauge_interrupt(s.gauge_interrupt);
}
void AsciiManager::set_gauge_interrupt(i32 interrupt){
    for(auto* vm:{&state.gauge,&state.human_icon,&state.youkai_icon,&state.cursor})vm->pendingInterrupt=i16(interrupt);
    state.gauge_interrupt=interrupt;
}
void AsciiManager::tick_popups(const AsciiContext& c,const FrameTiming& timing){
    if(c.paused||c.retry||c.freeze_popups)return;
    const bool snapshot=popup_marker.capture();
    for(u32 i=0;i<723;++i){auto& p=state.score_popups[i];if(snapshot)score_popup_previous[i]={p.position,p.timer.current,p.in_use,p.characters};if(p.in_use){p.position.y=(number(p.position.y)-number(.5f)*number(timing.rate)).to_float();p.timer.tick(timing);if(p.timer.current>60)p.in_use=0;}}
    for(auto& p:state.time_popups)if(p.in_use){p.timer.tick(timing);if(p.timer.current>90)p.in_use=0;}
}
void AsciiManager::tick_vms(bool demo){
    auto& s=state;
    for(auto* vm:{&s.gauge,&s.human_icon,&s.youkai_icon,&s.cursor,&s.percentage,&s.boss_markers[0],&s.boss_markers[1],&s.boss_markers[2],&s.boss_markers[3],&s.border})executor.execute(*vm);
    if(demo){if(s.demo.scriptIndex==0&&s.ascii)start(s.demo,*s.ascii,11);executor.execute(s.demo);}else s.demo.scriptIndex=0;
    ++s.frame;
}
bool AsciiManager::add_string(const Vec3& position,const char* text,bool software_texturing){
    if(!text||state.string_count>=256||state.string_count<0)return false;
    // Game strings fit the original 64-byte field. Reject overflow instead of
    // reproducing strcpy's overwrite of adjacent game state for external text.
    if(std::strlen(text)>=64)return false;
    auto& next=state.strings[state.string_count++];std::strcpy(next.text,text);
    next.position=position;next.color=state.color;next.scale_x=state.scale_x;next.scale_y=state.scale_y;next.gui=state.gui;next.selected=software_texturing?state.selected:0;return true;
}
i32 AsciiManager::add_format(const Vec3& position,bool software,const char* format,...){
    char buffer[512];va_list args;va_start(args,format);
    Vec3 adjusted=position;
    if(format_localized_ascii(*this,position,adjusted,buffer,sizeof(buffer),format,args)){
        va_end(args);add_string(adjusted,buffer,software);return i32(std::strlen(buffer));
    }
    const i32 length=std::vsnprintf(buffer,sizeof(buffer),format,args);va_end(args);
    if(length<0||length>=i32(sizeof(buffer)))return -1;add_string(position,buffer,software);return length;
}
void AsciiManager::create_score(const Vec3& position,i32 value,u32 color,const AsciiContext& c,bool player){
    auto& s=state;auto& next=player?s.next_player:s.next_score;const i32 capacity=player?3:720;
    if(next>=capacity||next<0)next=0;
    const i32 index=(player?720:0)+next++;auto& p=s.score_popups[index];score_popup_previous[index]={};score_popup_previous[index].timer=-2;p.in_use=1;
#if defined(TH_PRESENTATION_AUDIT)
    score_popup_generation[index]=wrapping_add(score_popup_generation[index],1u);
#endif
    i32 count=0;
    if(value>=0){while(value){p.text[count++]=u8(value%10);value/=10;}}else p.text[count++]=10;
    if(!count)p.text[count++]=0;p.characters=u8(count);p.color=color;p.timer.set(0);p.position=position;
    p.position.x=add(p.position.x,c.arcade_origin.x);p.position.y=add(p.position.y,c.arcade_origin.y);
}
void AsciiManager::create_time(const Vec3& position,i32 value,i32 multiplier,u32 color,const AsciiContext& c,bool familiar){
    auto& s=state;if(s.next_time>=128||s.next_time<0)s.next_time=0;
    const i32 index=s.next_time++;auto& p=s.time_popups[index];p.in_use=1;i32 count=0;
#if defined(TH_PRESENTATION_AUDIT)
    time_popup_generation[index]=wrapping_add(time_popup_generation[index],1u);
#endif
    // Original gameplay caps both quantities; preserve all valid twelve-glyph
    // popups while bounding malformed external values to this record.
    auto append=[&](u8 value){if(count<12)p.text[count++]=value;};
    if(multiplier>0){append(15);while(multiplier){append(u8(multiplier%10));multiplier/=10;}append(14);}
    if(value>0){while(value){append(u8(value%10));value/=10;}}else append(0);append(13);
    p.characters=u8(count);p.color=color;p.timer.set(familiar?88:0);p.position=position;
    p.position.x=((familiar?number(3.5f)*integer(count)+number(c.arcade_origin.x):number(c.arcade_origin.x))+number(p.position.x)).to_float();
    p.position.y=add(p.position.y,c.arcade_origin.y);p.scale_x=s.scale_x;p.scale_y=s.scale_y;
}
void AsciiManager::draw_strings(const AsciiContext& c){
    auto& s=state;AnmVm text_copy;if(presentation::render_only)text_copy=s.large_text;auto& vm=presentation::render_only?text_copy:s.large_text;vm.visible=true;vm.anchor=3;bool gui=true;
    Vec3 presented_player=c.player;if(presentation::active&&presentation_state.valid){const float dx=c.player.x-presentation_state.player.x,dy=c.player.y-presentation_state.player.y;if(dx*dx+dy*dy<16384.0f)presented_player={presentation::lerp_world(presentation_state.player.x,c.player.x),presentation::lerp_world(presentation_state.player.y,c.player.y),presentation::lerp_world(presentation_state.player.z,c.player.z)};}
    auto viewport=[&](bool arcade){auto v=renderer.viewport;v.x=arcade?u32(Scalar::truncate(c.arcade_origin.x)):0;v.y=arcade?u32(Scalar::truncate(c.arcade_origin.y)):0;
        v.width=arcade?u32(Scalar::truncate(c.arcade_size.x)):640;v.height=arcade?u32(Scalar::truncate(c.arcade_size.y)):480;renderer.set_viewport(v);};
    for(i32 i=0;i<s.string_count&&i<256;++i){const auto& str=s.strings[i];vm.pos=str.position;vm.scale={str.scale_x,str.scale_y};const float space=(integer(s.space_width)*number(str.scale_x)).to_float();
        if(gui!=bool(str.gui)){gui=str.gui;viewport(gui);}
        for(const auto* p=reinterpret_cast<const u8*>(str.text);p<reinterpret_cast<const u8*>(str.text)+64&&*p;++p){
            if(*p=='\n'){vm.pos.y=(number(vm.pos.y)+number(16)*number(str.scale_y)).to_float();vm.pos.x=str.position.x;}
            else {if(*p!=' '){direct_sprite(vm,*p+(str.selected?138:-1));vm.color1.d3dColor=str.selected?0xffffffff:str.color;const u32 glyph=u32(p-reinterpret_cast<const u8*>(str.text));TH08_AUDIT_SCOPE(AsciiText,&str,0,glyph);renderer.draw_no_rotation(vm);}vm.pos.x=add(vm.pos.x,space);}
        }
    }
    if(gui)viewport(false);
    for(u32 i=0;i<4;++i){auto& source=s.boss_markers[i];if(source.pos.x<56||source.pos.x>392)continue;AnmVm marker_copy;if(presentation::render_only)marker_copy=source;auto& marker=presentation::render_only?marker_copy:source;
        if(presentation::active&&presentation_state.valid){const auto& before=presentation_state.boss_markers[i];const float dx=source.pos.x-before.x;if(before.x>=56&&before.x<=392&&std::fabs(dx)<128)marker.pos.x=presentation::lerp_world(before.x,source.pos.x);}
        const float distance=std::fabs((number(marker.pos.x)-number(32)-number(presented_player.x)).to_float());direct_sprite(marker,157);
        bool normal=false;
        switch(s.boss_states[i]){
        case 0:normal=true;break;
        case 1:marker.color1.d3dColor=0x80ff4040;break;
        case 2:case 3:case 4:if(s.frame%(1u<<(5-s.boss_states[i]))==0){direct_sprite(marker,158);marker.color1.d3dColor=0xffffffff;}else normal=true;break;
        }
        if(normal){const u8 alpha=distance<64?u8((number(distance)*number(64)/number(64)+number(96)).truncate_int()):160;marker.color1.d3dColor=0xffffff|(u32(alpha)<<24);}
        // The marker alpha is derived from player distance and blink state,
        // rather than an ANM alpha timer.  Its previous submitted color still
        // provides an owner-authored continuous endpoint between logic ticks.
        if(presentation::active&&presentation_state.valid)presentation_state.boss_marker_vm[i].apply(source,marker,presentation::world_alpha,presentation::VisualSample::Opacity,presentation::VisualSample::Opacity);
        TH08_AUDIT_SCOPE(Ascii,&source,source.currentTimeInScript.current,i|(u32(s.boss_states[i])<<8));renderer.draw_no_rotation(marker);
    }
}
void AsciiManager::draw_percentage(const Vec3& position,i32 percentage,u32 color){
    AnmVm local;if(presentation::render_only)local=state.percentage;auto& vm=presentation::render_only?local:state.percentage;const u32 absolute=percentage<0?0u-u32(percentage):u32(percentage);
    const i32 count=4+(percentage<0)+(absolute>=10000?3:absolute>=1000?2:1);
    const float offset=(integer(count)*number(3.5f)-number(3.5f)-number(4)).to_float();
    vm.pos=position;vm.pos.x=sub(vm.pos.x,offset);vm.color1.d3dColor=color;
    u32 digit_index=0;auto digit=[&](i32 sprite,float advance){set_sprite(vm,sprite);TH08_AUDIT_SCOPE(Ascii,&state.percentage,state.percentage.currentTimeInScript.current,0x100u+digit_index++);renderer.draw_no_rotation(vm);vm.pos.x=add(vm.pos.x,advance);};
    if(percentage<0)digit(148,7);
    u32 value=absolute;
    if(absolute>=10000){digit(137,7);digit(136,7);digit(136,7);value=0;}
    else {if(absolute>=1000){digit(136+value/1000,7);value%=1000;}digit(136+value/100,7);value%=100;}
    digit(147,5);vm.scale={.8f,.8f};vm.pos.y=add(vm.pos.y,2);
    digit(136+value/10,5);digit(136+value%10,7);vm.scale={1,1};vm.pos.y=sub(vm.pos.y,2);set_sprite(vm,146);{TH08_AUDIT_SCOPE(Ascii,&state.percentage,state.percentage.currentTimeInScript.current,0x100u+digit_index);renderer.draw_no_rotation(vm);}
}
void AsciiManager::draw_overlays(const AsciiContext& c){
    auto& s=state;overlay.begin(!c.fog_disabled);
    Vec3 presented_player=c.player;float presented_gauge=float(c.gauge);float presented_blindness_radius=s.blindness_radius;u32 presented_blindness_color=s.blindness_color;
    if(presentation::active&&presentation_state.valid){
        const float dx=c.player.x-presentation_state.player.x,dy=c.player.y-presentation_state.player.y;if(dx*dx+dy*dy<16384.0f)presented_player={presentation::lerp_world(presentation_state.player.x,c.player.x),presentation::lerp_world(presentation_state.player.y,c.player.y),presentation::lerp_world(presentation_state.player.z,c.player.z)};
        presented_gauge=presentation::lerp_world(float(presentation_state.gauge),float(c.gauge));
        if(presentation_state.blindness_color&&s.blindness_color){presented_blindness_radius=presentation::lerp_world(presentation_state.blindness_radius,s.blindness_radius);presented_blindness_color=u32(std::clamp(presentation::lerp_world(float(presentation_state.blindness_color&255),float(s.blindness_color&255)),0.0f,255.0f));}
    }
    AnmVm small_copy;if(presentation::render_only)small_copy=s.small_score_text;auto& small=presentation::render_only?small_copy:s.small_score_text;
    for(u32 popup_index=0;popup_index<723;++popup_index){const auto& p=s.score_popups[popup_index];if(!p.in_use)continue;Vec3 position=p.position;
        // Replay/menu shortcuts may execute more than two nested gameplay
        // updates before the next authored Draw.  The snapshot still spans
        // that whole interval.  Slot reuse is rejected by create_score()
        // clearing the previous record, so an arbitrary timer-delta cap only
        // makes a live popup snap during those multi-update intervals.
        if(presentation::active){const auto& before=score_popup_previous[popup_index];if(before.in_use&&before.characters==p.characters&&p.timer.current>=before.timer)position={presentation::lerp_world(before.position.x,p.position.x),presentation::lerp_world(before.position.y,p.position.y),presentation::lerp_world(before.position.z,p.position.z)};}
        small.pos.x=(number(position.x)-integer(p.characters*4)).to_float();small.pos.y=position.y;small.color1.d3dColor=p.color;small.scale={s.scale_x,s.scale_y};
        const i32 alpha=popup_alpha(presented_player,position);for(i32 i=p.characters-1;i>=0;--i){direct_sprite(small,p.text[i]+(p.timer.current<52?0:p.timer.current<56?11:21));small.color1.a=u8(alpha);
            if(small.loadedSprite)small.spriteSize.x=small.loadedSprite->widthPx;
#if defined(TH_PRESENTATION_AUDIT)
            const u32 audit_part=(score_popup_generation[popup_index]<<8)|(u32(i)&0xffu);TH08_AUDIT_SCOPE(AsciiScorePopup,&p,p.timer.current,audit_part);
#endif
            renderer.draw_no_rotation(small);small.pos.x=add(small.pos.x,8);}}
    if(s.blindness_color){
        const u32 color=(presented_blindness_color&255)<<24;
        const auto x=number(presented_player.x)+number(32),y=number(presented_player.y)+number(16),radius=number(presented_blindness_radius);
        OverlayRect rect{32,16,(x-radius+number(renderer.shake.x)).to_float(),464};
        if(rect.right>rect.left)overlay.rectangle(rect,color);
        rect={(x+radius+number(renderer.shake.x)).to_float(),16,416,464};if(rect.right>rect.left)overlay.rectangle(rect,color);
        rect={std::max(32.f,(x-radius+number(renderer.shake.x)).to_float()),16,std::min(416.f,(x+radius+number(renderer.shake.x)).to_float()),(y-radius+number(renderer.shake.y)).to_float()};
        if(rect.bottom>rect.top)overlay.rectangle(rect,color);
        rect.top=(y+radius+number(renderer.shake.y)).to_float();rect.bottom=464;if(rect.bottom>rect.top)overlay.rectangle(rect,color);
        if(c.effects){AnmVm blindness_copy;if(presentation::render_only)blindness_copy=s.blindness;auto& blindness=presentation::render_only?blindness_copy:s.blindness;start(blindness,*c.effects,105);blindness.scale.x=blindness.scale.y=(radius/number(63)).to_float();blindness.pos=presented_player;blindness.pos.x=add(blindness.pos.x,32);blindness.pos.y=add(blindness.pos.y,16);blindness.color1.a=u8(presented_blindness_color);TH08_AUDIT_SCOPE(Ascii,&s.blindness,s.frame,0x200u);renderer.draw_no_rotation(blindness);}
    }
    AnmVm popup_copy;if(presentation::render_only)popup_copy=s.popup_text;auto& popup=presentation::render_only?popup_copy:s.popup_text;
    for(const auto& p:s.time_popups)if(p.in_use){const u32 popup_index=u32(&p-s.time_popups);popup.pos.x=(number(p.position.x)-integer(p.characters)*number(3.5f)).to_float();popup.pos.y=p.position.y;popup.color1.d3dColor=p.color;popup.scale={p.scale_x,p.scale_y};
        const i32 alpha=popup_alpha(presented_player,p.position);for(i32 i=p.characters-1;i>=0;--i){direct_sprite(popup,p.text[i]+136);popup.color1.a=u8(alpha);if(popup.loadedSprite)popup.spriteSize.x=popup.loadedSprite->widthPx;
#if defined(TH_PRESENTATION_AUDIT)
            const u32 audit_part=(time_popup_generation[popup_index]<<8)|(u32(i)&0xffu);TH08_AUDIT_SCOPE(AsciiTimePopup,&p,p.timer.current,audit_part);
#endif
            renderer.draw_no_rotation(popup);popup.pos.x=(number(popup.pos.x)+number(7)*number(p.scale_x)).to_float();}}
    renderer.shake={};
    if(s.gauge.visible){
        AnmVm cursor_copy,percentage_copy,gauge_copy,human_copy,youkai_copy;
        if(presentation::render_only){
            cursor_copy=s.cursor;percentage_copy=s.percentage;gauge_copy=s.gauge;human_copy=s.human_icon;youkai_copy=s.youkai_icon;
            if(presentation::active&&presentation_state.valid){
                using V=presentation::VisualSample;constexpr u32 fields=V::Position|V::Offset|V::Attributes;
                presentation_state.cursor_vm.apply(s.cursor,cursor_copy,presentation::world_alpha,fields);
                presentation_state.percentage_vm.apply(s.percentage,percentage_copy,presentation::world_alpha,fields);
                presentation_state.gauge_vm.apply(s.gauge,gauge_copy,presentation::world_alpha,fields);
                presentation_state.human_icon_vm.apply(s.human_icon,human_copy,presentation::world_alpha,fields);
                presentation_state.youkai_icon_vm.apply(s.youkai_icon,youkai_copy,presentation::world_alpha,fields);
            }
        }
        auto& cursor=presentation::render_only?cursor_copy:s.cursor;auto& percentage_vm=presentation::render_only?percentage_copy:s.percentage;auto& gauge=presentation::render_only?gauge_copy:s.gauge;
        auto& human=presentation::render_only?human_copy:s.human_icon;auto& youkai=presentation::render_only?youkai_copy:s.youkai_icon;
        cursor.pos.x=(number(presented_gauge)*number(112)/number(2)/number(10000)+number(gauge.pos.x)+number(64)).to_float();{TH08_AUDIT_SCOPE(Ascii,&s.cursor,s.cursor.currentTimeInScript.current,0x300u);renderer.draw_2d(cursor,true);}
        percentage_vm.pos.x=(number(presented_gauge)*number(80)/number(2)/number(10000)+number(gauge.pos.x)+number(64)).to_float();percentage_vm.pos.y=sub(cursor.pos.y,7);percentage_vm.pos.z=cursor.pos.z;
        const u32 rgb=c.gauge<=c.human_effects?0x7070ff:c.gauge<=c.human_tint?0xb0b0ff:c.gauge>=c.youkai_effects?0xff7070:c.gauge>=c.youkai_tint?0xffb0b0:0xffffff;
        percentage_vm.color1.d3dColor=(gauge.color1.d3dColor&0xff000000)|rgb;gauge.color1=percentage_vm.color1;
        {TH08_AUDIT_SCOPE(Ascii,&s.gauge,s.gauge.currentTimeInScript.current,0x301u);renderer.draw_no_rotation(gauge);}{TH08_AUDIT_SCOPE(Ascii,&s.human_icon,s.human_icon.currentTimeInScript.current,0x302u);renderer.draw_no_rotation(human);}{TH08_AUDIT_SCOPE(Ascii,&s.youkai_icon,s.youkai_icon.currentTimeInScript.current,0x303u);renderer.draw_no_rotation(youkai);}draw_percentage(percentage_vm.pos,c.gauge,percentage_vm.color1.d3dColor);
        percentage_vm.pos.x=(number(gauge.pos.x)+number(62)-number(14)).to_float();percentage_vm.pos.y=(number(gauge.pos.y)+number(3)+number(8)).to_float();
        i32 divisor=10000000,value=c.point_value,seen=0;
        for(i32 i=0;i<8;++i){seen+=value/divisor;if(seen){set_sprite(percentage_vm,value/divisor+136);TH08_AUDIT_SCOPE(Ascii,&s.percentage,s.percentage.currentTimeInScript.current,0x400u+u32(i));renderer.draw_no_rotation(percentage_vm);percentage_vm.pos.x=add(percentage_vm.pos.x,7);}value%=divisor;divisor/=10;}
    }
}
#if defined(TH_PRESENTATION_AUDIT)
const float* AsciiManager::audit_presentation_sample(uintptr_t object)const{
    static float out[20];std::fill(out,out+20,0.0f);
    const AnmVm* current=nullptr;const presentation::VisualSample* before=nullptr;
    const auto bind=[&](const AnmVm& vm,const presentation::VisualSample& sample){if(object==reinterpret_cast<uintptr_t>(&vm)){current=&vm;before=&sample;}};
    bind(state.gauge,presentation_state.gauge_vm);bind(state.human_icon,presentation_state.human_icon_vm);bind(state.youkai_icon,presentation_state.youkai_icon_vm);
    bind(state.cursor,presentation_state.cursor_vm);bind(state.percentage,presentation_state.percentage_vm);bind(state.border,presentation_state.border_vm);
    if(!current||!before)return out;
    out[0]=before->file?1.0f:0.0f;out[1]=before->file==current->anmFile?1.0f:0.0f;out[2]=before->script_start==current->beginningOfScript?1.0f:0.0f;
    out[3]=float(before->script);out[4]=float(current->scriptIndex);out[5]=before->visible?1.0f:0.0f;out[6]=current->visible?1.0f:0.0f;
    out[7]=float(before->color1.a);out[8]=float(current->color1.a);out[9]=float(before->continuous);out[10]=float(presentation::VisualSample::continuous_fields(*current));
    out[11]=before->matches(*current)?1.0f:0.0f;out[12]=presentation_state.valid?1.0f:0.0f;out[13]=presentation::active?1.0f:0.0f;
    out[14]=presentation::world_alpha;out[15]=float(current->currentTimeInScript.current);out[16]=float(current->interpCurrentTimers[AnmInterp_Alpha1].current);out[17]=float(current->interpEndTimers[AnmInterp_Alpha1].current);
    out[18]=float(presentation_marker.last_epoch&0xffffffu);out[19]=float(presentation::calculation_epoch&0xffffffu);return out;
}
#endif
}
