#include "Replay.hpp"
#include "Archive.hpp"
#include <algorithm>
#include <cctype>
#include <cstring>
#ifdef TH12_MULTIPLAYER
#include "multiplayer/ReplayArchive.hpp"
#endif

namespace th12host {
namespace {
constexpr u32 Magic=0x72323174,MaxBytes=64*1024*1024;
// Two passes: 4639d0 with (0x5e, 0xe1, 0x800), then (0x7d, 0x3a, 0x40).
constexpr CryptParams outer{0x5e,0xe1,0x800,0xffffffffu},inner{0x7d,0x3a,0x40,0xffffffffu};
template<class T> T get(const u8* p){T v;std::memcpy(&v,p,sizeof v);return v;}
template<class T> void put(u8* p,T v){std::memcpy(p,&v,sizeof v);}
// The replay LZSS (8 KiB dictionary, head 1), stopping at `limit` bytes when
// only the header is wanted. Ok when the end marker or the limit is reached.
bool inflate(const u8* in,u32 size,u8* out,u32 capacity,u32& written,bool prefix){
    u8 dictionary[8192]{};u32 cursor=0,head=1,padding=0;u8 mask=0x80,byte=0;written=0;bool overrun=false;
    auto bit=[&]{if(mask==0x80){if(cursor>=size){if(++padding>2)overrun=true;byte=0;}else byte=in[cursor++];}const u32 b=bool(byte&mask);mask>>=1;if(!mask)mask=0x80;return b;};
    auto bits=[&](u32 n){u32 v=0;while(n--)v=(v<<1)|bit();return v;};
    auto put_byte=[&](u8 v){out[written++]=v;dictionary[head]=v;head=(head+1)&8191;};
    for(;;){
        if(overrun)return false;
        if(bit()){if(written==capacity)return prefix;put_byte(u8(bits(8)));}
        else{
            const u32 offset=bits(13);if(!offset)return !overrun;
            const u32 length=bits(4)+3;
            for(u32 i=0;i<length;++i){if(written==capacity)return prefix;put_byte(dictionary[(offset+i)&8191]);}
        }
    }
}
bool unpack(const std::vector<u8>& file,std::vector<u8>& raw,u32 limit){
    if(file.size()<36||get<u32>(file.data())!=Magic||get<u16>(&file[4])!=4)return false;
    const u32 packed=get<u32>(&file[28]),size=get<u32>(&file[32]);
    if(!packed||packed>file.size()-36||packed>MaxBytes||size<112||size>MaxBytes)return false;
    std::vector<u8> a(packed),b(packed);
    resource_decrypt(&file[36],a.data(),packed,outer);resource_decrypt(a.data(),b.data(),packed,inner);
    const u32 capacity=limit?std::min(limit,size):size;raw.assign(capacity,0);u32 written=0;
    if(!inflate(b.data(),packed,raw.data(),capacity,written,limit!=0))return false;
    return written==capacity;
}
ReplayInfo header_info(const u8* d){
    ReplayInfo i;
    for(int n=0;n<8&&d[n];++n)i.name+=char(d[n]);
    while(!i.name.empty()&&i.name.back()==' ')i.name.pop_back();
    i.practice=d[10]&1;i.timestamp=get<i64>(d+12);i.score_stored=get<i32>(d+20);i.slowdown=get<float>(d+84);
    i.character=i32(get<u32>(d+92));i.shot=i32(get<u32>(d+96));i.difficulty=i32(get<u32>(d+100));i.final_stage=i32(get<u32>(d+104));i.continues=get<i32>(d+108);
    return i;
}
}

std::optional<ReplayInput> Replay::input(i32 number,u32 frame)const{
    const ReplayStage* s=stage(number);if(!s||frame>=s->frames)return std::nullopt;
    const u8* p=&data[s->input_offset+frame*6];return ReplayInput{get<u16>(p),get<u16>(p+2),get<u16>(p+4)};
}
i32 Replay::recorded_fps(i32 number,u32 frame)const{
    const auto* s=stage(number);if(!s||frame>s->frames)return -1;
    if(!frame||!s->fps_count)return 0;
    // 43ba44 reads before 43ba7a advances the FPS pointer at cursors 0,30,...;
    // the displayed cursor already includes the input consumed by that tick.
    return data[s->fps_offset+std::min((frame+28)/30,s->fps_count-1)];
}

bool read_replay_info(const std::vector<u8>& file,ReplayInfo& out){
#ifdef TH12_MULTIPLAYER
    th12::multiplayer::ReplayArchive archive;
    if(!archive.Load(file.data(),file.size()))return false;
    const auto& setup=archive.Setup();out={};out.name=archive.Name();out.timestamp=archive.Timestamp();out.score_stored=int(archive.Score());
    const auto seat=archive.Info().config.recordedPlayer;out.character=int(setup.selections[seat]/2);out.shot=int(setup.selections[seat]%2);out.difficulty=int(setup.difficulty);
    out.final_stage=archive.Completed()?8:int(archive.Info().chapters[archive.Info().chapterCount-1].label);return true;
#else

    std::vector<u8> raw;if(!unpack(file,raw,112))return false;
    out=header_info(raw.data());
    return out.character>=0&&out.character<=2&&out.shot>=0&&out.shot<=1&&out.difficulty>=0&&out.difficulty<=4;
#endif
}
bool parse_replay(const std::vector<u8>& file,Replay& out,std::string& error){
#ifdef TH12_MULTIPLAYER
    th12::multiplayer::ReplayArchive archive;
    if(!archive.Load(file.data(),file.size())||!read_replay_info(file,out.info)){error="Invalid multiplayer replay";return false;}
    out.stages.clear();const auto& info=archive.Info();
    for(unsigned i=0;i<info.chapterCount;++i){ReplayStage stage;stage.number=int(info.chapters[i].label);stage.frames=(i+1<info.chapterCount?info.chapters[i+1].firstFrame:info.frameCount)-info.chapters[i].firstFrame;out.stages.push_back(stage);}
    return true;
#else

    std::vector<u8> d;if(!unpack(file,d,0)){error="not a TH12 1.00b replay";return false;}
    size_t tail=36+get<u32>(&file[28]);
    while(tail+12<=file.size()&&get<u32>(&file[tail])==0x52455355){
        const auto length=get<u32>(&file[tail+4]);if(length<12||length>file.size()-tail){error="invalid replay comment block";return false;}tail+=length;
    }
    Replay r;r.info=header_info(d.data());const u32 count=get<u32>(&d[88]),size=u32(d.size());
    if(count<1||count>7||r.info.character<0||r.info.character>2||r.info.shot<0||r.info.shot>1||r.info.difficulty<0||r.info.difficulty>4){error="bad replay header";return false;}
    u32 at=112;
    for(u32 i=0;i<count;++i){
        if(at>size||size-at<160){error="truncated stage header";return false;}
        const u8* h=&d[at];ReplayStage s;
        s.number=get<u16>(h);s.seed=get<u16>(h+2);s.frames=get<u32>(h+4);s.payload=get<u32>(h+8);
        if(s.number<1||s.number>7||r.stage(s.number)||s.frames<1||s.frames>10000000||s.payload>size-at-160||s.payload<s.frames*6){error="bad stage stream";return false;}
        const u8* last=&d[at+160+(s.frames-1)*6];const bool ended=get<u16>(last)==0xffff&&get<u16>(last+2)==0xffff&&get<u16>(last+4)==0xffff;
        s.input_offset=at+160;s.fps_offset=at+160+s.frames*6;s.fps_count=(s.frames-(ended?1:0)+29)/30;
        if(s.payload-s.frames*6<s.fps_count){error="missing frame-rate samples";return false;}
        auto& in=s.initial;in.score=get<i32>(h+12);in.power=get<i16>(h+16);in.point_value=get<i32>(h+20);
        in.lives=get<i16>(h+24);in.life_fragments=get<i16>(h+26);in.bombs=get<i16>(h+28);in.bomb_fragments=get<i16>(h+30);
        for(int n=0;n<3;++n)in.ufo[size_t(n)]=get<i32>(h+32+n*4);
        in.rank=get<i32>(h+44);in.x=get<i32>(h+48);in.y=get<i32>(h+52);for(int n=0;n<4;++n)in.globals[size_t(n)]=get<i32>(h+56+n*4);
        for(int n=0;n<21;++n)s.spell_times[size_t(n)]=get<i32>(h+76+n*4);
        r.stages.push_back(s);at+=160+s.payload;
    }
    if(at!=size){error="unrecognised replay tail";return false;}
    std::map<i32,u32> stage_frames;for(const auto& s:r.stages)stage_frames[s.number]=s.frames;
    if(!read_replay_extension(file,tail,stage_frames,r.extension,error))return false;
    if(!r.extension.damage.empty()&&(r.info.character!=0||r.info.shot!=0||r.info.practice)){error="Damage-source replay identity mismatch";return false;}
    r.data=std::move(d);out=std::move(r);return true;
#endif
}

std::array<i32,21> default_replay_spell_times(){
    std::array<i32,21> t{};for(i32 i=0;i<20;++i)t[size_t(i)]=i32(u32(-i)*0x21522153u);t[20]=0;return t;
}

void ReplayRecorder::begin_stage(i32 number,i32 seed,const ReplayInitial& initial){
    if(active)finish_stage();
    Stage s;s.number=number;s.seed=seed&0xffff;s.initial=initial;s.spell_times=default_replay_spell_times();active=std::move(s);extension.motion[number]={};
}
void ReplayRecorder::push(u32 held,u32 pressed,u32 released,float fps){
    if(!active||active->inputs.size()>=10000000)return;
    ReplayInput in{u16(held),u16(pressed),u16(released)};if(in.end())return;
    if(active->inputs.size()%30==0)active->fps.push_back(u8(std::clamp(i32(fps+.5f),0,255)));
    active->inputs.push_back(in);
}
void ReplayRecorder::motion(i32 x,i32 y,bool unlimited){
    if(!active||active->inputs.empty())return;
    if(x||y)extension.motion[active->number].push_back({u32(active->inputs.size()-1),x,y});
    extension.unlimited|=unlimited;
}
void ReplayRecorder::finish_stage(bool end){
    if(!active)return;
    if(end)active->inputs.push_back({0xffff,0xffff,0xffff});
    if(!active->inputs.empty())stages.push_back(std::move(*active));
    active.reset();
}
std::vector<u8> ReplayRecorder::encode(const std::string& name,i64 timestamp,i32 score_stored,i32 continues,i32 final_stage,float slowdown){
    finish_stage(true);
    if(stages.empty())return {};
    u32 size=112;for(const auto& s:stages)size+=160+u32(s.inputs.size()*6+s.fps.size());
    std::vector<u8> raw(size,0);u8* d=raw.data();
    std::memset(d,' ',8);for(size_t i=0;i<name.size()&&i<8;++i)d[i]=u8(name[i]);
    d[10]=practice?1:0;put<i64>(d+12,timestamp);put<i32>(d+20,score_stored);std::memcpy(d+24,config.data(),60);
    put<float>(d+84,slowdown);put<u32>(d+88,u32(stages.size()));put<u32>(d+92,u32(character));put<u32>(d+96,u32(shot));put<u32>(d+100,u32(difficulty));
    put<u32>(d+104,u32(std::clamp(final_stage,1,8)));put<i32>(d+108,continues);
    u32 at=112;
    for(const auto& s:stages){
        u8* h=d+at;const auto& in=s.initial;
        put<u16>(h,u16(s.number));put<u16>(h+2,u16(s.seed));put<u32>(h+4,u32(s.inputs.size()));put<u32>(h+8,u32(s.inputs.size()*6+s.fps.size()));
        put<i32>(h+12,in.score);put<i16>(h+16,i16(in.power));put<i32>(h+20,in.point_value);
        put<i16>(h+24,i16(in.lives));put<i16>(h+26,i16(in.life_fragments));put<i16>(h+28,i16(in.bombs));put<i16>(h+30,i16(in.bomb_fragments));
        for(int n=0;n<3;++n)put<i32>(h+32+n*4,in.ufo[size_t(n)]);
        put<i32>(h+44,in.rank);put<i32>(h+48,in.x);put<i32>(h+52,in.y);for(int n=0;n<4;++n)put<i32>(h+56+n*4,in.globals[size_t(n)]);
        for(int n=0;n<21;++n)put<i32>(h+76+n*4,s.spell_times[size_t(n)]);
        u8* p=h+160;for(const auto& i:s.inputs){put<u16>(p,i.held);put<u16>(p+2,i.pressed);put<u16>(p+4,i.released);p+=6;}
        std::memcpy(p,s.fps.data(),s.fps.size());at+=160+u32(s.inputs.size()*6+s.fps.size());
    }
    // A literal-only stream is valid 44c710 input; then 463af0 twice.
    const auto plain=lzss_literals(raw);std::vector<u8> a(plain.size()),b(plain.size());
    resource_encrypt(plain.data(),a.data(),u32(plain.size()),inner);resource_encrypt(a.data(),b.data(),u32(plain.size()),outer);
    // The two USER blocks the original appends: a text summary, then type 1.
    const auto user=[](const std::string& text,u8 type){std::vector<u8> v(text.begin(),text.end());v.push_back(0);const u32 n=u32((12+v.size()+3)&~size_t(3));
        std::vector<u8> block(n,0);put<u32>(block.data(),0x52455355);put<u32>(&block[4],n);block[8]=type;std::copy(v.begin(),v.end(),block.begin()+12);return block;};
    char summary[512];
    std::snprintf(summary,sizeof summary,"TH12 Undefined Fantastic Object\r\nVersion 1.00b\r\nName %-8.8s\r\nCharacter %d/%d\r\nRank %d\r\nStage %d-%d\r\nScore %lld\r\nSlow Rate %.2f%%\r\n",
        name.c_str(),character,shot,difficulty,stages.front().number,std::clamp(final_stage,1,8),(long long)score_stored*10,double(slowdown));
    const auto info=user(summary,0),tail=user("",1);
    std::vector<u8> file(36,0);put<u32>(&file[0],Magic);put<u16>(&file[4],4);put<u32>(&file[12],36+u32(b.size()));put<u32>(&file[16],0x100);put<u32>(&file[28],u32(b.size()));put<u32>(&file[32],size);
    file.insert(file.end(),b.begin(),b.end());file.insert(file.end(),info.begin(),info.end());file.insert(file.end(),tail.begin(),tail.end());
    append_replay_extension(file,extension);return file;
}

std::vector<ReplayEntry> list_replays(const std::string& folder){
    std::vector<ReplayEntry> out(25);std::vector<u8> bytes;
    for(int i=0;i<25;++i){char name[16];std::snprintf(name,sizeof name,
#ifdef TH12_MULTIPLAYER
        "th12_mp_%02d.rpy",
#else
        "th12_%02d.rpy",
#endif
        i+1);out[size_t(i)].file=name;
        ReplayInfo info;if(host::read_file(folder+"/"+name,bytes,MaxBytes)&&read_replay_info(bytes,info))out[size_t(i)].info=info;}
    // 446560: the user files follow in directory order (up to 50).
    for(const auto& name:host::list_files(folder)){
        if(out.size()>=75)break;
        std::string lower=name;for(auto& c:lower)c=char(std::tolower(u8(c)));
#ifdef TH12_MULTIPLAYER
        if(lower.rfind("th12_mp_",0)!=0||lower.size()<13||lower.substr(lower.size()-4)!=".rpy")continue;
        bool numbered=false;for(const auto& fixed:out)if(fixed.file==name){numbered=true;break;}if(numbered)continue;
#else
        if(lower.size()!=15||lower.rfind("th12_ud",0)!=0||lower.substr(11)!=".rpy")continue;
#endif
        ReplayInfo info;if(host::read_file(folder+"/"+name,bytes,MaxBytes)&&read_replay_info(bytes,info))out.push_back({name,info});
    }
    out.resize(75);return out;
}
}
