#include "Ending.hpp"
#include "Localization.hpp"
#include <cstdio>
#include <cstdlib>
namespace th09 {
namespace {
bool utf8_line(const std::vector<u8>& script,u32 from){
    for(u32 at=from;at<script.size()&&script[at]&&script[at]!=10&&script[at]!=13;){
        const u8 first=script[at++];if(first<0x80)continue;
        const u32 count=first>=0xc2&&first<=0xdf?2:first>=0xe0&&first<=0xef?3:first>=0xf0&&first<=0xf4?4:0;
        if(!count||at+count-1>script.size())return false;
        for(u32 n=1;n<count;++n)if((script[at++]&0xc0)!=0x80)return false;
    }
    return true;
}
u32 utf8_unit(u8 first){return first<0x80?1:(first&0xe0)==0xc0?2:(first&0xf0)==0xe0?3:4;}
}
bool Ending::load(const char* path){
    std::string name=path?path:"";const auto slash=name.find_last_of("/\\");if(slash!=std::string::npos)name.erase(0,slash+1);
    std::vector<u8> bytes;if(!resources.read(name.c_str(),bytes)||bytes.empty()){error="Ending resource: "+name;return false;}
    script=std::move(bytes);filename=name;cursor=0;state.line_delay=8;state.line_wait.reset();state.elapsed.reset();return true;
}
bool Ending::initialize(i32 character){
    if(character<0||character>=14){error="Ending character";return false;}
    state={};state.page_wait.previous=0;state.elapsed.reset();state.line_wait.reset();
    std::memset(animations.data(),0,sizeof(animations));finished=false;skip_enabled=true;
    if(!resources.load(AnimationFile::text,"text.anm")){error=resources.error;return false;}
    for(i32 n=0;n<15;++n){if(!resources.start(AnimationFile::text,animations[n],14+n)){error="Ending subtitle animation";return false;}animations[n].pos={64,float(n)*16.f+400.f,0};}
    char name[24];std::snprintf(name,sizeof(name),"end%.2d.end",character);ready=load(name);return ready;
}
bool Ending::number(i32& out){
    if(cursor>=script.size()){error="Ending argument bounds";return false;}const u32 first=cursor;while(cursor<script.size()&&script[cursor])++cursor;
    if(cursor==script.size()){error="Unterminated ending argument";return false;}
    const std::string token(reinterpret_cast<const char*>(script.data()+first),cursor-first);out=i32(std::strtol(token.c_str(),nullptr,10));while(cursor<script.size()&&!script[cursor])++cursor;return true;
}
bool Ending::skip_line(){while(cursor<script.size()&&script[cursor]!=10&&script[cursor]!=13)++cursor;while(cursor<script.size()&&(script[cursor]==10||script[cursor]==13))++cursor;if(cursor>=script.size()){error="Ending missing next line";return false;}return true;}
void Ending::fade(){
    auto& s=state;const i32 frame=s.fade_frame;
    if(!s.fade_mode){s.cover_color=0;return;}
    if(s.fade_mode<1||s.fade_mode>4)return;
    if(frame<s.fade_duration){const u32 alpha=u32(frame*255/s.fade_duration);++s.fade_frame;s.cover_color=((s.fade_mode==1||s.fade_mode==3?255-alpha:alpha)<<24)|(s.fade_mode>=3?0xffffff:0);}
    else if(s.fade_mode==2||s.fade_mode==4)s.cover_color=s.fade_mode==2?0xff000000:0xffffffff;
    else{s.fade_mode=0;s.cover_color=0;}
}
bool Ending::step(const InputFrame& input){
    auto& s=state;fade();const bool advance=(input.pressed&0x1001)||(skip_enabled&&(input.held&0x100));
    if(s.page_wait.current>0){s.page_wait.decrement(1,timing);if(!s.page_lock){if(advance)s.page_wait.reset();}else --s.page_lock;
        if(s.page_wait.current<=0){for(i32 n=0;n<15;++n)animations[n].pendingInterrupt=2;s.line=0;}else goto finish;
    }
    if(s.line_wait.current>0){s.line_wait.decrement(1,timing);if(!s.line_lock){if(advance)s.line_wait.reset();}else --s.line_lock;goto finish;}
    {
        std::string text;
        for(u32 operations=0;operations<4096;++operations){
            if(cursor>=script.size()){error="Ending script bounds";return false;}
            const u8 ch=script[cursor];
            if(!ch||ch==10||ch==13){
                if(!text.empty()){if(s.line<0||s.line>=16){error="Ending subtitle overflow";return false;}output.ending_text(animations[s.line],text.c_str(),s.text_color);animations[s.line].pendingInterrupt=1;}
                while(cursor<script.size()&&(!script[cursor]||script[cursor]==10||script[cursor]==13))++cursor;
                s.line_wait.reset((input.held&0x1001)?s.fast_delay:s.line_delay);s.line_lock=s.fast_delay;++s.line;goto finish;
            }
            if(ch!='@'){
                const u32 unit=Localization::Active()&&utf8_line(script,cursor)?utf8_unit(ch):2;
                if(cursor+unit>script.size()){error="Ending character bounds";return false;}
                text.append(reinterpret_cast<const char*>(script.data()+cursor),unit);cursor+=unit;
                if(text.size()>1024){error="Ending text bounds";return false;}continue;
            }
            if(cursor+1>=script.size()){error="Ending command bounds";return false;}const u8 command=script[++cursor];i32 a=0,b=0,c=0;
            const auto string_argument=[&](){const u32 first=cursor+1;u32 end=first;while(end<script.size()&&script[end])++end;return std::string(reinterpret_cast<const char*>(script.data()+first),end-first);};
            switch(command){
            case '0':case '1':case '2':case '3':s.fade_mode=command-'0'+1;s.fade_frame=0;++cursor;if(!number(a))return false;s.fade_duration=a/2;fade();break;
            case 'F':{if(!resources.load(AnimationFile::staff,"staff01.anm")){error=resources.error;return false;}const auto name=string_argument();if(!load(name.c_str()))return false;text.clear();for(auto& vm:animations)vm.scriptIndex=0;break;}
            case 'R':for(auto& vm:animations)vm.scriptIndex=0;break;
            case 'M':++cursor;if(!number(a))return false;output.ending_music_fade(a);break;
            case 'V':++cursor;if(!number(a)||!number(b))return false;s.scroll=float(a)/float(b);break;
            case 'a':++cursor;if(!number(a)||!number(b)||!number(c))return false;if(a<0||a>=16||!resources.start(AnimationFile::staff,animations[a],b)||!resources.sprite(AnimationFile::staff,animations[a],c)){error="Staff animation";return false;}break;
            case 'b':{const auto name=string_argument();if(!output.ending_picture(name.c_str())){error="Ending picture: "+name;return false;}break;}
            case 'c':++cursor;if(!number(a))return false;s.text_color=u32(a);break;
            case 'm':if(cursor+1>=script.size())return false;output.ending_music(-1);output.ending_music(i8(script[cursor+1]));output.ending_music(i8(script[cursor+1]));break;
            case 'r':++cursor;if(!number(a)||!number(b))return false;s.page_wait.reset(a);s.page_lock=b;s.line_wait.reset();s.line_lock=0;if(!skip_line())return false;goto finish;
            case 's':++cursor;if(!number(a)||!number(b))return false;s.line_delay=a;s.fast_delay=b;break;
            case 'v':++cursor;if(!number(a))return false;s.y=float(a);break;
            case 'w':++cursor;if(!number(a)||!number(b))return false;s.line_wait.reset(a);s.line_lock=b;if(!skip_line())return false;goto finish;
            case 'z':finished=true;return false;
            default:break;
            }
            if(!skip_line())return false;
        }error="Ending command budget";return false;
    }
finish:
    s.elapsed.tick(timing);s.y-=s.scroll;if(s.y<0){s.y=0;s.scroll=0;}return true;
}
bool Ending::update(const InputFrame& input){if(!ready||finished||!error.empty())return false;for(u32 n=0;n<9;++n){if(!step(input))return false;for(i32 i=0;i<15;++i)resources.advance(animations[i]);if(!skip_enabled||!(input.held&0x100))break;}return true;}
void Ending::draw(){output.ending_background(i32(state.x),i32(state.y));for(i32 n=0;n<15;++n)output.ending_sprite(animations[n]);if(state.cover_color&0xff000000)output.ending_cover(state.cover_color);}
}
