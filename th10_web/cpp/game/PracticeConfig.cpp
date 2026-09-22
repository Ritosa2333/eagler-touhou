#include "PracticeConfig.hpp"
#include "PracticeSections.hpp"
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
namespace th10 {
void PracticeConfig::reset(){std::memset(this,0,sizeof(*this));}
bool PracticeConfig::valid()const {
    // Warp 0..5 (None/Portion/Mid/End/NonSpell/Spell); th10 has no frame warp.
    if(mode<0||mode>1||stage<0||stage>6||warp<0||warp>5||phase<0||phase>1||frame<0||dlg<0||dlg>1)return false;
    if(section>=10000){
        // Chapter ids are 10000 + (stage + 1) * 100 + chapter. The chapter
        // totals are THGuiPrac::mChapterSetup's first + second halves.
        constexpr int portions[]{5,5,7,8,6,4,6};
        if((section-10000)/100!=stage+1||(section%100)<1||(section%100)>portions[stage])return false;
    } else if(section<0||u32(section)>=sizeof(practice_sections)/sizeof(*practice_sections)||(section&&practice_sections[section].stage!=stage))return false;
    if(score<0||score>9999999990LL||life<0||life>9||power<0||power>100)return false;
    if(faith<0||faith>999990||faith_bar<0||faith_bar>130||st6_boss9_spd< -1||st6_boss9_spd>160)return false;
    return true;
}
void PracticeConfig::encode(double* w)const {
    const double values[]{double(mode),double(stage),double(warp),double(section),double(phase),double(frame),double(dlg),double(score),double(life),double(power),double(faith),double(faith_bar),double(st6_boss9_spd),double(real_bullet_sprite),1};
    std::copy(values,values+word_count,w);
}
bool PracticeConfig::decode(const double* w,u32 count){
    if(!w||count!=word_count||w[word_count-1]!=1)return false;
    for(u32 i=0;i<count;i++)if(!std::isfinite(w[i])||std::trunc(w[i])!=w[i]||(i!=7&&(w[i]<-2147483648.0||w[i]>2147483647.0)))return false;
    if(w[7]<0||w[7]>9999999990.0)return false;
    PracticeConfig p;
    p.mode=i32(w[0]);p.stage=i32(w[1]);p.warp=i32(w[2]);p.section=i32(w[3]);p.phase=i32(w[4]);p.frame=i32(w[5]);p.dlg=i32(w[6]);p.score=i64(w[7]);
    p.life=i32(w[8]);p.power=i32(w[9]);p.faith=i32(w[10]);p.faith_bar=i32(w[11]);p.st6_boss9_spd=i32(w[12]);p.real_bullet_sprite=i32(w[13]);
    if(!p.valid())return false;*this=p;return true;
}
namespace {
u32 load32(const u8* p){u32 n;std::memcpy(&n,p,4);return n;}
void store32(u8* p,u32 n){std::memcpy(p,&n,4);}
double json_number(const std::string& json,const char* key,double fallback){
    const std::string needle=std::string("\"")+key+"\"";
    size_t position=json.find(needle);
    if(position==std::string::npos||(position=json.find(':',position+needle.size()))==std::string::npos)return fallback;
    const char* begin=json.c_str()+position+1;char* end=nullptr;
    const double value=std::strtod(begin,&end);
    return end==begin?fallback:value;
}
bool json_bool(const std::string& json,const char* key,bool fallback){
    const std::string needle=std::string("\"")+key+"\"";
    size_t position=json.find(needle);
    if(position==std::string::npos||(position=json.find(':',position+needle.size()))==std::string::npos)return fallback;
    position=json.find_first_not_of(" \t\r\n",position+1);
    if(position==std::string::npos)return fallback;
    if(json.compare(position,4,"true")==0)return true;
    if(json.compare(position,5,"false")==0)return false;
    return fallback;
}
}
std::string practice_replay_json(const PracticeConfig& p){
    // THPracParam::GetJson(): section/phase/dlg are only written when set.
    if(!p.valid())return {};
    char buffer[1024];
    int length=std::snprintf(buffer,sizeof(buffer),"{\"version\":\"2.3.0.3\",\"game\":\"th10\",\"mode\":%d,\"stage\":%d",p.mode,p.stage);
    if(length<=0||static_cast<std::size_t>(length)>=sizeof(buffer))return {};
    auto append=[&](const char* fmt,auto... args){
        const int written=std::snprintf(buffer+length,sizeof(buffer)-static_cast<std::size_t>(length),fmt,args...);
        if(written<0||static_cast<std::size_t>(written)>=sizeof(buffer)-static_cast<std::size_t>(length))return false;
        length+=written;return true;
    };
    if(p.section&&!append(",\"section\":%d",p.section))return {};
    if(p.phase&&!append(",\"phase\":%d",p.phase))return {};
    if(p.dlg&&!append(",\"dlg\":true"))return {};
    if(!append(",\"life\":%d,\"power\":%d,\"faith\":%d,\"faith_bar\":%d,\"st6_boss9_spd\":%d,\"score\":%lld,\"real_bullet_sprite\":%s}",
               p.life,p.power,p.faith,p.faith_bar,p.st6_boss9_spd,static_cast<long long>(p.score),
               p.real_bullet_sprite?"true":"false"))return {};
    return std::string(buffer,static_cast<std::size_t>(length));
}
bool practice_replay_parse(const char* json,u32 size,PracticeConfig& out){
    out=PracticeConfig{};
    if(!json||!size)return false;
    const std::string text(json,size);
    // ForceJsonValue(game, "th10") upstream; the version field marks the thprac
    // payload and distinguishes it from arbitrary USER block text.
    if(text.find("\"version\":")==std::string::npos||text.find("\"game\":\"th10\"")==std::string::npos)return false;
    PracticeConfig p;
    // THPracParam::ReadJson() Reset()s (memset) before parsing: missing keys
    // stay zero and never inherit Practice menu defaults.
    p.reset();
    p.mode=i32(json_number(text,"mode",0));
    p.stage=i32(json_number(text,"stage",0));
    p.section=i32(json_number(text,"section",0));
    p.phase=i32(json_number(text,"phase",0));
    p.dlg=json_bool(text,"dlg",false)?1:0;
    p.life=i32(json_number(text,"life",0));
    p.power=i32(json_number(text,"power",0));
    p.faith=i32(json_number(text,"faith",0));
    p.faith_bar=i32(json_number(text,"faith_bar",0));
    p.score=i64(json_number(text,"score",0));
    p.real_bullet_sprite=json_bool(text,"real_bullet_sprite",false)?1:0;
    // GetJsonValue(st6_boss9_spd) leaves the Reset() value when absent; upstream
    // then substitutes -1.
    p.st6_boss9_spd=i32(json_number(text,"st6_boss9_spd",-1));
    // Upstream applies whatever it parsed; this port can only apply configs
    // that pass the same validation the live menu enforces, so anything else
    // degrades to an Original replay instead of a broken startup.
    if(!p.valid())return false;
    out=p;return true;
}
std::vector<u8> practice_replay_block(const PracticeConfig& p){
    const auto json=practice_replay_json(p);
    if(json.empty())return {};
    // Upstream ReplaySaveParam (the non-T6RP/T7RP branch): 'USER', total size
    // aligned to 4, 'PRAC', then the NUL-padded JSON payload.
    u32 paramSize=u32(json.size())+12;
    for(paramSize++;paramSize&3u;paramSize++);
    std::vector<u8> block(paramSize,0);
    std::memcpy(block.data(),"USER",4);
    store32(block.data()+4,paramSize);
    std::memcpy(block.data()+8,"PRAC",4);
    std::memcpy(block.data()+12,json.data(),json.size());
    return block;
}
bool practice_replay_read(const u8* data,u32 size,PracticeConfig& out){
    out=PracticeConfig{};
    // TH10 ReplayHeader::signature and version (Replay.hpp / ReplayFile.cpp).
    if(!data||size<0x68||load32(data)!=0x72303174u)return false;
    const u32 version=data[4]|(u32(data[5])<<8);
    if((version&0xfff)!=5)return false;
    // The encoded header's user_offset (0x0c) is where the plain USER block
    // area starts; blocks are walked exactly like upstream ReplayLoadParam.
    u32 position=load32(data+0xc);
    if(position<0x68||position>size)return false;
    while(position+12<=size){
        if(std::memcmp(data+position,"USER",4)!=0)break;
        const u32 length=load32(data+position+4),number=load32(data+position+8);
        if(length<12||length>size-position)break;
        if(number==(u32(u8('P'))|(u32(u8('R'))<<8)|(u32(u8('A'))<<16)|(u32(u8('C'))<<24))){
            const char* json=reinterpret_cast<const char*>(data+position+12);
            u32 n=0;
            while(n<length-12&&json[n])++n;
            return n&&practice_replay_parse(json,n,out);
        }
        position+=length;
    }
    return false;
}
void practice_replay_menu_reset(PracticeState& p){
    // THGuiRep::State(1): thPracParam.Reset(), mRepStatus=false and
    // mParamStatus=false. mRepParam itself remains untouched.
    p.run.reset();
    p.replay_candidate_valid=false;
}
bool practice_replay_menu_check(PracticeState& p,const u8* replay,u32 size){
    // THGuiRep::State(2): inspect the opened replay into mRepParam without
    // mutating the live run.
    PracticeConfig candidate;
    if(!replay||!practice_replay_read(replay,size,candidate)){
        // CheckReplay() failure Reset()s mRepParam but leaves mParamStatus
        // untouched; the sticky flag is intentional source behavior. A Reset()
        // candidate means Original mode, so a later State(3) stays vanilla.
        p.replay_candidate.reset();
        return false;
    }
    p.replay_candidate=candidate;
    p.replay_candidate_valid=true;
    return true;
}
void practice_replay_menu_activate(PracticeState& p){
    // THGuiRep::State(3): only while mParamStatus holds is mRepParam copied
    // into the live parameters; a Reset() candidate copies as Original mode.
    if(p.replay_candidate_valid){
        p.run=p.replay_candidate;
#ifdef TH_ENABLE_THPRAC
        // th10_rep_power_fix restores the recorded option layout for a practice
        // replay, so the run has to be marked live for apply_practice to use it.
        if(p.run.mode==1)p.active=true;
#endif
    }
}
#ifdef TH_ENABLE_THPRAC
// thprac_th10.cpp:1765-2223. THRepPowerFix rewrites the first recorded stage's
// option_targets/positions/offsets (offset 0x198 into the plain replay payload)
// from a 128-byte table chosen by power and by character*3+shot_type.
#include "PracticePowerFix.inc"
#endif
}
