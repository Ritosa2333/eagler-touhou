#include "Ending.hpp"
#include "TextRaster.hpp"
#include <algorithm>
#include <cstring>

namespace th12host {
namespace {
// 420e40: CP932 bytes XOR a rolling key 0x77, step 7 rising by 16.
bool decode_text(const u8* p,size_t n,std::u32string& out){
    std::string bytes;u8 key=0x77,step=7;
    for(size_t i=0;i<n;++i){const u8 b=u8(p[i]^key);key=u8(key+step);step=u8(step+16);if(!b){out=decode_cp932(bytes);return true;}bytes+=char(b);}
    return false;
}
std::string c_string(const u8* p,size_t n){std::string s;for(size_t i=0;i<n&&p[i];++i)s+=char(p[i]);return s;}
std::string stem(const std::string& path){std::string s=path.substr(path.find_last_of('/')==std::string::npos?0:path.find_last_of('/')+1);const auto dot=s.find('.');return dot==std::string::npos?s:s.substr(0,dot);}
}

bool decode_ending_message(const std::vector<u8>& d,std::vector<EndingCommand>& out,std::string& error){
    const auto word=[&](size_t at){u32 v;std::memcpy(&v,&d[at],4);return v;};
    if(d.size()<12||word(0)!=1||word(4)!=12){error="ending MSG header";return false;}
    out.clear();size_t cursor=12;bool terminated=false;
    while(cursor+4<=d.size()){
        EndingCommand c;std::memcpy(&c.time,&d[cursor],2);c.op=d[cursor+2];const u8 length=d[cursor+3];const size_t end=cursor+4+length;
        if(end>d.size()){error="ending MSG command past the end";return false;}
        const u8* payload=&d[cursor+4];
        if(length%4==0)for(size_t i=0;i<length;i+=4){i32 v;std::memcpy(&v,payload+i,4);c.ints.push_back(v);}
        if(c.op==3&&!decode_text(payload,length,c.text)){error="ending text without terminator";return false;}
        if(c.op==7){if(length<5){error="ending bank command";return false;}c.ints.assign(1,0);std::memcpy(&c.ints[0],payload,4);c.file=c_string(payload+4,length-4);}
        if((c.op==8||(c.op>=15&&c.op<=17))&&length!=12){error="ending animation command";return false;}
        if((c.op==5||c.op==6||c.op==9||c.op==13||c.op==14)&&length!=4){error="ending command size";return false;}
        if(c.op==10||c.op==12)c.file=c_string(payload,length);
        out.push_back(std::move(c));cursor=end;
        if(out.back().op==0){terminated=true;break;}
    }
    if(!terminated){error="ending MSG has no end";return false;}
    return true;
}

// 411230..41135a: text.anm 68..72, 512x17 lines at (64, 384+16*i).
EndingTimeline::EndingTimeline(AnimationSampler& s,i32 tb,std::vector<EndingCommand> ending,std::vector<EndingCommand> credits,i32 d,bool seen_,Hooks h)
    :sampler(s),text_bank(tb),commands(std::move(ending)),staff(std::move(credits)),difficulty(d),seen(seen_),hooks(std::move(h)){
    lines.resize(5);
    for(i32 i=0;i<5;++i){text.push_back(std::make_unique<MenuAnimation>(sampler,sampler.files(text_bank),text_bank,68+i));set_line(i,U" ",0xffffff);}
}
void EndingTimeline::set_line(i32 slot,const std::u32string& t,u32 rgb){
    if(slot<0||slot>=i32(text.size()))return;lines[size_t(slot)]={t,rgb,512,17};text[size_t(slot)]->set_text(68+slot,lines[size_t(slot)]);
}
bool EndingTimeline::release_wait(u32 held,u32 pressed){
    // 41159d counts down on every visit; 4115b2 pins a negative op 5 wait at 999.
    auto& w=*wait;w.left=w.indefinite?999:w.left-1;
    // 4115dd: Ctrl skips only an ending already seen, every sixth frame.
    const bool confirm=pressed&0x80001,skip=seen&&!w.indefinite&&(held&512)&&w.left%6==0;
    if(w.left>0&&!confirm&&!skip)return false;
    if(confirm||w.left<=0)hooks.sound(0); // 411616 / 4116a9
    if(w.clear)line_slot=0;wait.reset();return true;
}
void EndingTimeline::tick(u32 held,u32 pressed){
    for(auto& [slot,vm]:animations)vm->tick();for(auto& vm:text)vm->tick();
    if(fade)++fade->age;
    if(done)return;
    if(wait&&!release_wait(held,pressed))return;
    for(i32 budget=0;budget<1024&&pc<commands.size();++budget){
        const auto c=commands[pc];if(c.time>clock)break;++pc;
        switch(c.op){
        case 0:done=true;return;
        case 3:{
            // 411467: slot 0 clears all five lines; 411554 wraps after five.
            if(line_slot==0)for(i32 i=0;i<i32(text.size());++i){set_line(i,U" ",0xffffff);text[size_t(i)]->interrupt(3,true,true);}
            const i32 slot=line_slot;set_line(slot,c.text,color);if(slot<i32(text.size()))text[size_t(slot)]->interrupt(2,true,true);
            line_slot=(slot+1)%5;break;}
        case 4:for(auto& vm:text)vm->interrupt(3,true,true);break;
        case 5:case 6:
            wait=Wait{c.ints.empty()?0:c.ints[0],c.op==5&&!c.ints.empty()&&c.ints[0]<0,c.op==6};
            if(!release_wait(held,pressed))return;break;
        case 7:{const i32 bank=hooks.bank(c.file);if(bank<0){error="missing "+c.file;done=true;return;}
            const i32 slot=c.ints.empty()?0:c.ints[0];bool found=false;for(auto& b:banks)if(b.first==slot){b.second=bank;found=true;}if(!found)banks.emplace_back(slot,bank);break;}
        case 8:case 15:case 16:case 17:{
            if(c.op>=15&&c.op-14!=difficulty)break;
            const i32 slot=c.ints[0],bank_slot=c.ints[1],script=c.ints[2];i32 bank=-1;for(const auto& b:banks)if(b.first==bank_slot)bank=b.second;
            const auto file=bank>=0?sampler.files(bank):nullptr;if(!file||!file->script(script)){error="ending animation reference";done=true;return;}
            auto vm=std::make_unique<MenuAnimation>(sampler,file,bank,script);bool replaced=false;
            for(auto& a:animations)if(a.first==slot){a.second=std::move(vm);replaced=true;}
            if(!replaced)animations.emplace_back(slot,std::move(vm));break;}
        case 9:{const u32 v=u32(c.ints[0]);color=((v&255)<<16)|(v&0xff00)|((v>>16)&255);break;}
        case 10:hooks.music(stem(c.file));break;
        case 11:hooks.fade_music();break;
        case 12:
            // The ending hands over to staff.msg; staff has no MSG text.
            commands=staff;in_credits=true;pc=0;clock=0;line_slot=0;text.clear();animations.clear();banks.clear();color=0xffffff;fade.reset();break;
        case 13:case 14:fade=Fade{std::max(1,c.ints[0]),0,c.op==14};break;
        default:break;
        }
    }
    ++clock;
}
void EndingTimeline::poses(std::vector<Appearance>& out)const{
    for(const auto& [slot,vm]:animations)vm->poses(out);for(const auto& vm:text)vm->poses(out);
}
// 452350: a 640x480 black rectangle with an integer alpha.
float EndingTimeline::shade()const{
    if(!fade)return 0;const float v=std::min(1.f,float(fade->age)/float(fade->frames));return float(i32(255*(fade->out?v:1-v)))/255.f;
}
}
