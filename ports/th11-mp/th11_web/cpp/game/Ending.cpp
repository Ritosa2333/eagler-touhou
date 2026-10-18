#include "Ending.hpp"
#include "MusicCatalog.hpp"
#include <cstdio>
namespace th11 {
namespace {
template<class T>T read(const u8* p){T v;std::memcpy(&v,p,sizeof v);return v;}
bool decode(const u8* p,u32 n,std::string& out){u8 key=0x77,step=7;for(u32 i=0;i<n;++i){const u8 c=p[i]^key;key=u8(key+step);step=u8(step+16);if(!c)return true;out+=char(c);}return false;}
std::string name(const u8* p,u32 n){u32 length=0;while(length<n&&p[length])++length;return std::string(reinterpret_cast<const char*>(p),length);}
}
bool Ending::load_message(const std::string& file,bool staff){
    const auto it=messages.find(file);if(it==messages.end()||it->second.size()<8)return fail("ending MSG missing");
    const auto& bytes=it->second;const auto offset=read<u32>(bytes.data()+4);if(u64(offset)+4>bytes.size())return fail("ending MSG offset invalid");
    message=file;instruction=bytes.data()+offset;end=bytes.data()+bytes.size();
    elapsed.set(0,&animations.rate);time.set(0,&animations.rate);wait.set(0,&animations.rate);flags=staff?2:1;color=0xffffff;line=0;
    if(staff){text_lines.fill(0);images.fill(0);slots.fill(nullptr);}return true;
}
bool Ending::begin(GameResources& source,i32 selection,i32 level,i32 continues){
    error.clear();if(selection<0||selection>=6||level<0||level>3)return fail("invalid ending selection");
    difficulty=level;index=selection+(continues==0?6:0);
    char file[32];std::snprintf(file,sizeof(file),"e%.2d.msg",index);
    for(const std::string msg:{std::string(file),std::string("staff.msg")}){
        auto& bytes=messages[msg];if(!source.read(msg,bytes)||bytes.size()<8)return fail("ending script load failed");
        for(u32 p=read<u32>(bytes.data()+4);u64(p)+4<=bytes.size();){const u32 n=bytes[p+3],op=bytes[p+2];if(u64(p)+4+n>bytes.size())return fail("truncated ending script");
            if(op==7){if(n<5)return fail("invalid ending ANM filename");const auto path=name(bytes.data()+p+8,n-4);if(resources.find(path)==resources.end()&&!source.open_anm(path,resources[path]))return fail("ending ANM load failed");}
            p+=4+n;if(!op)break;
        }
    }
    seen=(scores.settings[0x16+index]==0?1:0)|(scores.settings[0x22]==0?2:0);
    scores.settings[0x16+index]=1;if(difficulty>0)scores.settings[0x16+index]|=0x10;if(index>5)scores.settings[0x22]=1;
    if(!load_message(file))return false;
    for(u32 i=0;i<5;++i){auto* vm=animations.create(text,68+i,0,22);if(!vm)return fail("ending text animation creation");text_lines[i]=vm->id;vm->rectangle_columns=vm->rectangle_rows=16;vm->flags2|=2;}
    active=true;return true;
}
bool Ending::write(u32 slot,const std::string& bytes,u32 ink){
    if(slot>=5||!animations.find(text_lines[slot]))return fail("ending text animation missing");
    text_requests.push_back({text_lines[slot],ink,0,0,0,bytes});return true;
}
bool Ending::update(u32 held,u32 pressed){
    text_requests.clear();sounds.clear();music_request=music_fade=-1;if(!active)return true;
    if(!instruction||instruction+4>end)return fail("ending instruction out of range");
    flags&=~4u;
    if((pressed&0x80001)||(!(flags&2)&&(held&512)&&(flags&1)))time.set(read<u16>(instruction),&animations.rate);
    u32 budget=0;bool blocked=false;
    while(instruction+4<=end&&read<u16>(instruction)<=time.current){
        if(++budget>10000)return fail("ending instruction budget exceeded");
        const auto* p=instruction;const u32 op=p[2],n=p[3];if(p+4+n>end)return fail("truncated ending payload");
        if((op==5||op==6||op==9||op==13||op==14)&&n<4)return fail("invalid ending integer");
        if((op==8||op>=15)&&n<12)return fail("invalid ending image arguments");
        bool jumped=false;
        switch(op){
        case 0:active=false;return true;
        case 1:case 2:break;
        case 3:{
            if(!line)for(u32 i=0;i<5;++i){if(!write(i," ",0xffffff))return false;families.interrupt(text_lines[i],3);}
            std::string text;if(!decode(p+4,n,text))return fail("unterminated ending text");
            if(!write(line,text,color))return false;families.interrupt(text_lines[line],2);line=(line+1)%5;break;
        }
        case 4:for(auto id:text_lines)families.interrupt(id,3);break;
        case 5:case 6:
            if(wait.current<1)wait.set(read<i32>(p+4),&animations.rate);wait.advance(-1);
            if(op==5&&read<i32>(p+4)<0)wait.set(999,&animations.rate);
            if(!(pressed&0x80001)&&wait.current>0){if(!(held&512)||(read<i32>(p+4)>=0&&wait.current%6)){blocked=true;break;}}
            else sounds.push_back(0);
            wait.set(0,&animations.rate);if(op==6)line=0;break;
        case 7:{
            if(n<5)return fail("invalid ending resource command");const i32 slot=read<i32>(p+4);const auto found=resources.find(name(p+8,n-4));
            if(slot<0||slot>=4||found==resources.end())return fail("ending resource slot invalid");slots[slot]=&found->second;
            // Native loading pauses this interpreter until the ANM loader has
            // finished. All resources are ready here; resume on the next tick.
            flags|=4;instruction+=4+n;blocked=true;break;
        }
        case 8:case 15:case 16:case 17:{
            if(op!=8&&difficulty!=i32(op)-14)break;
            const i32 slot=read<i32>(p+4),file=read<i32>(p+8),script=read<i32>(p+12);
            if(slot<0||slot>=16||file<0||file>=4||!slots[file])return fail("ending image slot invalid");
            families.erase(images[slot]);auto* vm=animations.create(*slots[file],i16(script),28+file,22);if(!vm)return fail("ending image creation failed");images[slot]=vm->id;break;
        }
        case 9:color=read<u32>(p+4);break;
        case 10:{auto path=name(p+4,n);const auto slash=path.find_last_of("/\\");if(slash!=std::string::npos)path.erase(0,slash+1);const auto dot=path.find_last_of('.');if(dot!=std::string::npos)path.erase(dot);
            for(const auto& track:music_tracks)if(path==track.file){music_request=track.cue;break;}if(music_request<0)return fail("ending BGM name invalid");scores.settings[0x26+music_request]=1;break;}
        case 11:music_fade=180;flags&=~1u;break;
        case 12:{const auto file=name(p+4,n);for(auto id:text_lines)families.erase(id);if(!load_message(file,true))return false;jumped=true;break;}
        case 13:case 14:fades.start(op==13?0:5,read<i32>(p+4),0,67,&animations.rate);break;
        default:return fail("unsupported ending opcode");
        }
        if(blocked)break;if(!jumped)instruction+=4+n;
    }
    if(!blocked)time.tick();elapsed.tick();++frames;return true;
}
bool Ending::tick(u32 held,u32 pressed){
    // Callback 40f120 returns 6 to restart the chain before its later ANM and
    // fade callbacks. Staff rolls advance to the next multiple of 12.
    std::vector<DialogueText> text;std::vector<i32> audio;i32 music=-1,fade=-1;
    do{
        if(!update(held,pressed))return false;
        text.insert(text.end(),text_requests.begin(),text_requests.end());audio.insert(audio.end(),sounds.begin(),sounds.end());
        if(music_request>=0)music=music_request;if(music_fade>=0)fade=music_fade;
    }while(active&&!(flags&4)&&(flags&2)&&(held&512)&&frames%12);
    text_requests=std::move(text);sounds=std::move(audio);music_request=music;music_fade=fade;
    fades.update();return true;
}
}
