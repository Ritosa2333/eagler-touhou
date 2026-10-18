#include "PlayerRecords.hpp"
#include "Binary.hpp"
#include <algorithm>
namespace th09 {
namespace {
void header(u8* p,u32 magic,u16 size,u8 version,const std::array<u8,3>& tail={}){write32(p,magic);write16(p+4,size);write16(p+6,size);p[8]=version;std::copy(tail.begin(),tail.end(),p+9);}
void read_time(PlayTime& t,const u8* p){t={read32(p),read32(p+4),read32(p+8),read32(p+12)};}
void write_time(u8* p,const PlayTime& t){write32(p,t.hours);write32(p+4,t.minutes);write32(p+8,t.seconds);write32(p+12,t.milliseconds);}
bool character_valid(i32 c){return c>=0&&c<16;}
void increment(u32& value){if(signed_bits(value)<999999)++value;}
}
void PlayTime::add(u32 elapsed){hours+=elapsed/3600000;elapsed%=3600000;minutes+=elapsed/60000;elapsed%=60000;seconds+=elapsed/1000;milliseconds+=elapsed%1000;if(milliseconds>999){seconds+=milliseconds/1000;milliseconds%=1000;}if(seconds>59){minutes+=seconds/60;seconds%=60;}if(minutes>59){hours+=minutes/60;minutes%=60;}}
void ScoreEntry::read(const u8* p){std::copy_n(p+9,3,reserved_header.begin());points=read32(p+12);unknown=read32(p+16);character=p[20];difficulty=p[21];rank=p[22];stage=p[23];std::memcpy(name.data(),p+24,9);std::memcpy(date.data(),p+33,10);continues=p[43];}
void ScoreEntry::write(u8* p)const{header(p,fourcc('H','S','C','R'),44,2,reserved_header);write32(p+12,points);write32(p+16,unknown);p[20]=character;p[21]=difficulty;p[22]=rank;p[23]=stage;std::memcpy(p+24,name.data(),9);std::memcpy(p+33,date.data(),10);p[43]=continues;}
void PlayerRecords::reset(){
    container_header={};write16(container_header.data()+4,4);write32(container_header.data()+8,24);profile_reserved={};name_reserved={};application_time={};game_time={};application_clock=game_clock=0;
    music_unlocked={};music_unlocked[0]=1;versus_unlocked={};story_unlocked={};extra_unlocked={};std::fill_n(versus_unlocked.begin(),5,1);std::fill_n(story_unlocked.begin(),5,1);clear_counts={};last_name={};std::fill_n(last_name.begin(),8,' ');
    for(u32 c=0;c<16;++c)for(u32 d=0;d<5;++d)for(u32 r=0;r<5;++r){auto& s=scores[c][d][r];s=ScoreEntry{};s.points=100000-r*20000;s.character=u8(c);s.difficulty=u8(d);s.rank=u8(r);}
}
void PlayerRecords::read_profile(const u8* p){std::copy_n(p+9,3,profile_reserved.begin());read_time(application_time,p+12);read_time(game_time,p+28);std::copy_n(p+44,32,music_unlocked.begin());std::copy_n(p+76,16,versus_unlocked.begin());std::copy_n(p+92,16,story_unlocked.begin());std::copy_n(p+108,16,extra_unlocked.begin());for(u32 c=0;c<16;++c)for(u32 d=0;d<6;++d)clear_counts[c][d]=read32(p+124+(c*6+d)*4);}
void PlayerRecords::write_profile(u8* p)const{header(p,fourcc('P','L','S','T'),508,3,profile_reserved);write_time(p+12,application_time);write_time(p+28,game_time);std::copy(music_unlocked.begin(),music_unlocked.end(),p+44);std::copy(versus_unlocked.begin(),versus_unlocked.end(),p+76);std::copy(story_unlocked.begin(),story_unlocked.end(),p+92);std::copy(extra_unlocked.begin(),extra_unlocked.end(),p+108);for(u32 c=0;c<16;++c)for(u32 d=0;d<6;++d)write32(p+124+(c*6+d)*4,clear_counts[c][d]);}
void PlayerRecords::write_last_name(u8* p)const{header(p,fourcc('L','S','N','M'),24,1,name_reserved);std::memcpy(p+12,last_name.data(),12);}
bool PlayerRecords::load(const u8* p,u32 size){ScoreFile file;return file.decode(p,size)&&load_plain(file.data().data(),u32(file.data().size()));}
bool PlayerRecords::load_plain(const u8* p,u32 size){
    ScoreFile file;if(!file.assign(p,size))return false;PlayerRecords next;std::copy_n(p,24,next.container_header.begin());bool found_name=false;
    for(u32 at=24;at<size;){const u8* item=p+at;const u32 length=read16(item+4),magic=read32(item);const u8 version=item[8];
        if(magic==fourcc('H','S','C','R')&&version==2){if(length!=44||item[20]>=16||item[21]>=5||item[22]>=5)return false;next.scores[item[20]][item[21]][item[22]].read(item);}
        else if(magic==fourcc('P','L','S','T')&&version==3){if(length!=508)return false;next.read_profile(item);}
        else if(magic==fourcc('L','S','N','M')&&version==1&&!found_name){if(length!=24)return false;std::copy_n(item+9,3,next.name_reserved.begin());std::memcpy(next.last_name.data(),item+12,12);found_name=true;}
        at+=length;
    }*this=next;return true;
}
std::vector<u8> PlayerRecords::serialize(Rng* order,u32 time_a,u32 time_b)const{
    std::vector<u8> out(24+12+17600+24+508+28);std::copy(container_header.begin(),container_header.end(),out.begin());write16(out.data()+4,4);write32(out.data()+8,24);write32(out.data()+12,u32(out.size()));write32(out.data()+16,u32(out.size()-24));header(out.data()+24,fourcc('T','H','9','K'),12,1);
    u32 at=36,pending=7,index=0;while(pending){const u32 group=order?order->bounded32(3):index++;if(!(pending&(1u<<group)))continue;pending^=1u<<group;if(group==0){for(const auto& character:scores)for(const auto& difficulty:character)for(const auto& entry:difficulty){entry.write(out.data()+at);at+=44;}}else if(group==1){write_last_name(out.data()+at);at+=24;}else{write_profile(out.data()+at);at+=508;}}
    header(out.data()+at,fourcc('V','R','S','M'),28,1);std::memcpy(out.data()+at+12,"0150a",6);write32(out.data()+at+20,time_a);write32(out.data()+at+24,time_b);return out;
}
std::vector<u8> PlayerRecords::save(Rng& random,u32 time_a,u32 time_b)const{auto bytes=serialize(&random,time_a,time_b);ScoreFile file;if(!file.assign(bytes.data(),u32(bytes.size())))return {};return file.encode(random);}
u32 PlayerRecords::clear_count(i32 c,i32 d)const{if(!character_valid(c)||d>=5)return 0;const auto& counts=clear_counts[c];return d<0?counts[0]|counts[1]|counts[2]|counts[3]:counts[d];}
bool PlayerRecords::cleared(i32 c)const{return character_valid(c)&&(clear_count(c,-1)||clear_count(c,4));}
void PlayerRecords::count_clear(i32 c,i32 d){if(character_valid(c)&&d>=0&&d<5)increment(clear_counts[c][d]);}
void PlayerRecords::count_encounter(i32 c){if(character_valid(c))increment(clear_counts[c][5]);}
void PlayerRecords::unlock_after_ending(i32 c,i32 d,bool count){
    if(count)count_clear(c,d);const auto cleared_total=[&](i32 n){i32 count=0;for(i32 i=0;i<n;++i)count+=cleared(i);return count;};const auto unlock_encountered=[&](i32 n){if(clear_counts[n][5])story_unlocked[n]=1;};
    if(cleared_total(5)>1){unlock_encountered(5);unlock_encountered(7);}if(cleared_total(9)>3){unlock_encountered(6);unlock_encountered(8);}if(cleared_total(9)>8){unlock_encountered(10);unlock_encountered(9);unlock_encountered(11);}if(cleared_total(12)>11)extra_unlocked[12]=1;if(cleared(12))extra_unlocked[13]=1;if(cleared(13)){std::fill_n(story_unlocked.begin(),14,1);std::fill_n(extra_unlocked.begin(),14,1);versus_unlocked[14]=versus_unlocked[15]=1;}
}
i32 PlayerRecords::insert(const ScoreEntry& entry){if(!character_valid(entry.character)||entry.difficulty>=5)return 99;auto& list=scores[entry.character][entry.difficulty];for(i32 rank=0;rank<5;++rank)if(entry.points>=list[rank].points){for(i32 n=4;n>rank;--n){list[n]=list[n-1];list[n].rank=u8(n);}list[rank]=entry;list[rank].rank=u8(rank);return rank;}return 99;}
const ScoreEntry* PlayerRecords::score(i32 c,i32 d,i32 rank)const{return character_valid(c)&&d>=0&&d<5&&rank>=0&&rank<5?&scores[c][d][rank]:nullptr;}
void PlayerRecords::update_application_clock(u32 now){if(now<application_clock)application_clock=now;application_time.add(now-application_clock);application_clock=now;}
void PlayerRecords::update_game_clock(u32 now){if(now<game_clock)game_clock=0;game_time.add(now-game_clock);game_clock=now;}
}
