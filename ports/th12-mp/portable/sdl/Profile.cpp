#include "Profile.hpp"
#include "Archive.hpp"
#include <algorithm>
#include <cstring>

namespace th12host {
const std::array<const char*,18> native_music_ids{"th12_01","th12_00","th12_02","th12_04","th12_05","th12_07","th12_08","th12_09","th12_10",
    "th12_13","th12_14","th12_16","th12_17","th12_18","th12_19","th12_20","th12_21","th10_17"};
namespace {
constexpr u32 RecordSize=0x45f4,StatusSize=0x448,MaxBytes=2*1024*1024;
constexpr u16 CR=0x5243,ST=0x5453;
u32 get32(const u8* p){u32 v;std::memcpy(&v,p,4);return v;}
u16 get16(const u8* p){u16 v;std::memcpy(&v,p,2);return v;}
void put32(u8* p,u32 v){std::memcpy(p,&v,4);}
void put16(u8* p,u16 v){std::memcpy(p,&v,2);}
u32 bounded(i64 v,u32 max){return u32(std::clamp<i64>(v,0,max));}
u32 checksum(const std::vector<u8>& b){u32 t=0;for(size_t i=8;i<b.size();++i)t+=b[i];return t;}
void finish(std::vector<u8>& b){put32(&b[4],checksum(b));}
void write_text(u8* at,size_t length,const std::string& text,u8 pad=0,size_t max=0){
    if(!max)max=length-1;std::memset(at,pad,length);size_t n=0;for(char c:text){if(!c||n>=max)break;at[n++]=u8(c);}if(n<length)at[n]=0;
}
// Retail difficulty of each of the 113 spell records (native-profile difficultyOf).
u32 card_difficulty(i32 id){return id>=100?4:id<2?u32(id+2):id>=54&&id<=56?u32(id-53):id>=93&&id<=95?u32(id-92):id<54?u32((id-2)%4):id<77?u32((id-57)%4):id<93?u32((id-77)%4):u32((id-96)%4);}
// 4639d0 / 463af0: 16-byte blocks, rolling XOR key 0xac + 0x35.
constexpr CryptParams score_cipher{0xac,0x35,16,0xffffffffu};
}

void Profile::fresh(){
    // createNativeConfig
    config.fill(0);put32(&config[0],0x120001);const i16 joy[9]{0,1,2,4,-1,-1,-1,-1,3};for(int i=0;i<9;++i)put16(&config[4+i*2],u16(joy[i]));
    put16(&config[0x16],600);put16(&config[0x18],600);config[0x1b]=config[0x1c]=1;config[0x1f]=config[0x23]=2;config[0x20]=100;config[0x21]=80;
    put32(&config[0x28],0x80000000u);put32(&config[0x2c],0x80000000u);put32(&config[0x38],0x100);
    // createNativeScore
    header.fill(0);put32(&header[0],0x31324854);put16(&header[8],2);put32(&header[12],0x100);
    for(u32 player=0;player<7;++player){
        auto& b=records[player];b.assign(RecordSize,0);put16(&b[0],CR);put16(&b[2],2);put32(&b[8],RecordSize);put32(&b[12],player);
        for(int d=0;d<5;++d)for(int r=0;r<10;++r){u8* at=&b[0x10+(d*10+r)*28];put32(at,u32(100000-r*10000));at[4]=1;write_text(at+6,9,"--------");}
        for(int id=0;id<113;++id){u8* at=&b[0x664+id*0x90];put32(at+0x88,u32(id));put32(at+0x8c,card_difficulty(id));}
        finish(b);
    }
    status.assign(StatusSize,0);put16(&status[0],ST);put16(&status[2],2);put32(&status[8],StatusSize);write_text(&status[12],9,"        ",32,8);
    // 43cf29..43cf77: 512 reserved words from the game's 16-bit RNG; seed 0.
    u32 seed=0;for(u32 at=0x46;at<0x446;at+=2){const u32 v=((seed^0x9630)-0x6553)&65535;seed=((v<<2)|(v>>14))&65535;put16(&status[at],u16(seed));}
    finish(status);
}

bool Profile::load(const std::string& dir){
    folder=dir;fresh();error.clear();std::vector<u8> bytes;
    if(host::read_file(dir+"/th12.cfg",bytes,4096)){
        bool ok=bytes.size()==60&&get32(bytes.data())==0x120001;
        // The six mode checks in 42fefa..42ff2c.
        const std::pair<int,int> modes[6]{{0x1a,2},{0x1b,3},{0x1c,2},{0x1d,4},{0x1e,3},{0x1f,3}};for(const auto& [at,limit]:modes)ok=ok&&bytes[size_t(at)]<limit;
        if(ok)std::copy(bytes.begin(),bytes.end(),config.begin());else{config_read_only=true;host::log("th12.cfg is not a TH12 config; left untouched");}
    }
    if(host::read_file(dir+"/scoreth12.dat",bytes,MaxBytes)&&!decode_score(bytes)){score_read_only=true;host::log("scoreth12.dat: %s; left untouched",error.c_str());}
    return true;
}
bool Profile::decode_score(const std::vector<u8>& bytes){
    if(bytes.size()<24||get32(bytes.data())!=0x31324854||get16(&bytes[8])!=2){error="not a TH12 score file";return false;}
    const u32 packed=get32(&bytes[16]),size=get32(&bytes[20]);
    if(!packed||packed>bytes.size()-24||get32(&bytes[4])!=packed+24||size<12||size>MaxBytes){error="bad score container";return false;}
    std::vector<u8> decoded(packed),raw(size);resource_decrypt(&bytes[24],decoded.data(),packed,score_cipher);
    u32 written=0;if(!lzss_decode(decoded.data(),packed,raw.data(),size,written)||written!=size){error="bad score data";return false;}
    auto next=records;std::vector<u8> next_status=status;
    for(size_t at=0;at<raw.size();){
        if(at+12>raw.size()){error="truncated score record";return false;}
        const u16 type=get16(&raw[at]),version=get16(&raw[at+2]);const u32 length=get32(&raw[at+8]);
        if(version!=2||!((type==CR&&length==RecordSize)||(type==ST&&length==StatusSize))||length>raw.size()-at){error="unknown score record";return false;}
        std::vector<u8> record(raw.begin()+i64(at),raw.begin()+i64(at+length));
        if(get32(&record[4])!=checksum(record)){error="score record checksum";return false;}
        // The native loader takes the last record of each kind.
        if(type==CR){const u32 player=get32(&record[12]);if(player>=7){error="bad score player";return false;}next[player]=std::move(record);}
        else next_status=std::move(record);
        at+=length;
    }
    std::copy_n(bytes.begin(),24,header.begin());records=std::move(next);status=std::move(next_status);return true;
}
std::vector<u8> Profile::encode_score()const{
    std::vector<u8> raw;for(const auto& r:records)raw.insert(raw.end(),r.begin(),r.end());raw.insert(raw.end(),status.begin(),status.end());
    // A literal-only stream is valid 44c710 input.
    const auto plain=lzss_literals(raw);std::vector<u8> out(24+plain.size());std::copy(header.begin(),header.end(),out.begin());
    resource_encrypt(plain.data(),&out[24],u32(plain.size()),score_cipher);
    put32(&out[4],u32(24+plain.size()));put32(&out[16],u32(plain.size()));put32(&out[20],u32(raw.size()));return out;
}
bool Profile::save(){
    if(folder.empty())return false;bool ok=true;
    for(auto& r:records)finish(r);finish(status);
    if(!config_read_only)ok&=host::write_file(folder+"/th12.cfg",config.data(),config.size());
    if(!score_read_only)ok&=host::write_file(folder+"/scoreth12.dat",encode_score());
    if(!ok)host::log("profile: saving failed");return ok;
}

void Profile::set_volumes(i32 bgm,i32 se){config[0x20]=u8(std::clamp(bgm,0,100));config[0x21]=u8(std::clamp(se,0,100));}
std::array<i32,5> Profile::joy_buttons()const{
    static constexpr int offsets[5]{4,6,8,10,20};static constexpr i32 defaults[5]{0,1,2,4,3};std::array<i32,5> out{};
    for(int i=0;i<5;++i){const i32 v=i16(get16(&config[size_t(offsets[i])]));out[size_t(i)]=v>=-1&&v<32?v:defaults[i];}return out;
}
void Profile::set_joy_buttons(const std::array<i32,5>& b){static constexpr int offsets[5]{4,6,8,10,20};for(int i=0;i<5;++i)put16(&config[size_t(offsets[i])],u16(i16(b[size_t(i)])));}

bool Profile::can_extra(i32 c,i32 s)const{return c>=0&&c<3&&s>=0&&s<2&&(status[size_t(0x16+6+c*2+s)]&0x10);}
bool Profile::any_extra()const{for(int c=0;c<3;++c)for(int s=0;s<2;++s)if(can_extra(c,s))return true;return false;}
bool Profile::cleared(i32 c,i32 s,i32 d)const{if(c<0||c>2||s<0||s>1||d<0||d>4)return false;return get32(&records[size_t(c*2+s)][size_t(0x590+d*4)])>0;}
const u8* Profile::practice_entry(i32 player,i32 d,i32 stage)const{return &records[size_t(player)][size_t(0x5a4+(d*6+stage-1)*8)];}
u8* Profile::practice_entry(i32 player,i32 d,i32 stage){return &records[size_t(player)][size_t(0x5a4+(d*6+stage-1)*8)];}
bool Profile::can_practice(i32 c,i32 s,i32 d,i32 stage)const{if(c<0||c>2||s<0||s>1||d<0||d>3||stage<1||stage>6)return false;return practice_entry(c*2+s,d,stage)[5]!=0;}
i32 Profile::practice_score(i32 c,i32 s,i32 d,i32 stage)const{if(c<0||c>2||s<0||s>1||d<0||d>3||stage<1||stage>6)return 0;return i32(get32(practice_entry(c*2+s,d,stage)));}
bool Profile::music_heard(const std::string& id)const{for(size_t i=0;i<native_music_ids.size();++i)if(id==native_music_ids[i])return status[0x26+i]!=0;return false;}
std::string Profile::last_name()const{std::string s;for(int i=12;i<21&&status[size_t(i)];++i)s+=char(status[size_t(i)]);while(!s.empty()&&s.back()==' ')s.pop_back();return s;}
void Profile::unlock_music(const std::string& id){for(size_t i=0;i<native_music_ids.size();++i)if(id==native_music_ids[i])status[0x26+i]=1;}
void Profile::record_play_start(i32 c,i32 s){if(c<0||c>2||s<0||s>1)return;u8* at=&records[size_t(c*2+s)][0x588];put32(at,std::min<u32>(99999,get32(at)+1));}
void Profile::record_play_frame(i32 c,i32 s){if(c<0||c>2||s<0||s>1)return;u8* at=&records[size_t(c*2+s)][0x58c];put32(at,std::min<u32>(0xcdfe5ff,get32(at)+1));}
// Retail 420a64 unlocks a Practice stage after clearing it.
void Profile::record_stage_clear(i32 c,i32 s,i32 d,i32 stage,i64 score){
    if(c<0||c>2||s<0||s>1||d<0||d>3||stage<1||stage>6)return;u8* e=practice_entry(c*2+s,d,stage);
    e[4]=e[5]=1;put32(e,std::max<u32>(get32(e),bounded(score/10,999999999)));
}
void Profile::record_clear(i32 c,i32 s,i32 d,i32 continues,i32 stage){
    if(c<0||c>2||s<0||s>1||d<0||d>4)return;u8* at=&records[size_t(c*2+s)][size_t(0x590+d*4)];put32(at,std::min<u32>(99999,get32(at)+1));
    if(stage==6){const i32 ending=c*2+s+(continues?0:6);status[size_t(0x16+ending)]|=u8(1|(d>=1?0x10:0));if(ending>=6)status[0x22]=1;}
}
std::vector<i32> Profile::spell_history()const{
    std::vector<i32> words(1792,0);
    for(int id=0;id<113;++id)for(int player=0;player<7;++player){const u8* at=&records[size_t(player)][size_t(0x664+id*0x90)];
        words[size_t(id*14+player*2)]=i32(std::min<u32>(99999,get32(at+0x84)));words[size_t(id*14+player*2+1)]=i32(std::min<u32>(99999,get32(at+0x80)));}
    return words;
}
void Profile::set_spell_history(const std::vector<i32>& words){
    if(words.size()!=1792)return;
    for(int id=0;id<113;++id)for(int player=0;player<7;++player){u8* at=&records[size_t(player)][size_t(0x664+id*0x90)];
        put32(at+0x84,bounded(words[size_t(id*14+player*2)],99999));put32(at+0x80,bounded(words[size_t(id*14+player*2+1)],99999));}
}
std::array<Profile::RankingRow,10> Profile::rankings(i32 c,i32 s,i32 d)const{
    std::array<RankingRow,10> rows{};if(c<0||c>2||s<0||s>1||d<0||d>4)return rows;
    for(int r=0;r<10;++r){const u8* at=&records[size_t(c*2+s)][size_t(0x10+(d*10+r)*28)];auto& row=rows[size_t(r)];
        row.score_stored=i32(get32(at));row.final_stage=at[4];row.continues=at[5];for(int i=6;i<15&&at[i];++i)row.name+=char(at[i]);
        std::memcpy(&row.timestamp,at+16,8);std::memcpy(&row.slowdown,at+24,4);}
    return rows;
}
i32 Profile::ranking_place(i32 c,i32 s,i32 d,i64 score)const{
    const auto rows=rankings(c,s,d);for(int r=0;r<10;++r)if(score>=rows[size_t(r)].score())return r;return -1;
}
void Profile::insert_ranking(i32 c,i32 s,i32 d,i32 rank,const RankingRow& row){
    if(c<0||c>2||s<0||s>1||d<0||d>4||rank<0||rank>9)return;u8* base=&records[size_t(c*2+s)][size_t(0x10+d*10*28)];
    std::memmove(base+(rank+1)*28,base+rank*28,size_t(9-rank)*28);u8* at=base+rank*28;std::memset(at,0,28);
    put32(at,u32(row.score_stored));at[4]=u8(std::clamp(row.final_stage,0,8));at[5]=u8(std::clamp(row.continues,0,255));
    write_text(at+6,9,row.name.empty()?std::string("        "):row.name);std::memcpy(at+16,&row.timestamp,8);std::memcpy(at+24,&row.slowdown,4);
}
void Profile::set_last_name(const std::string& name){write_text(&status[12],9,name,0,8);}
Profile::Totals Profile::totals(i32 c,i32 s,i32 d)const{
    Totals t;if(c<0||c>2||s<0||s>1||d<0||d>4)return t;const auto& r=records[size_t(c*2+s)];
    t.play_count=get32(&r[0x588]);t.play_frames=get32(&r[0x58c]);t.clear_count=get32(&r[size_t(0x590+d*4)]);return t;
}
std::string Profile::spell_title(i32 card,i32 player)const{
    std::string t;if(card<0||card>=113||player<0||player>6)return t;const u8* at=&records[size_t(player)][size_t(0x664+card*0x90)];
    for(int i=0;i<128&&at[i];++i)t+=char(at[i]);return t;
}
void Profile::set_spell_title(i32 card,i32 player,const std::string& cp932){
    if(card<0||card>=113||player<0||player>6||cp932.empty())return;write_text(&records[size_t(player)][size_t(0x664+card*0x90)],128,cp932);
}
void Profile::record_practice_score(i32 c,i32 s,i32 d,i32 stage,i64 score){
    if(c<0||c>2||s<0||s>1||d<0||d>3||stage<1||stage>6)return;u8* e=practice_entry(c*2+s,d,stage);
    put32(e,std::max<u32>(get32(e),bounded(score/10,999999999)));
}
}
