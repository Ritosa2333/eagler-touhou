#include "ScoreFile.hpp"
#include "ProgramVersion.hpp"
#include "ResourceCrypt.hpp"
#include <algorithm>
namespace th08 {
namespace {
template<class T>T object(const u8* source){T value;std::memcpy(&value,source,sizeof(T));return value;}
u8 rotate(u8 value){return u8((value<<3)|(value>>5));}
}
bool ScoreFile::valid() const {
    if(bytes.size()<28)return false;
    auto h=object<ScoreHeader>(bytes.data());
    if(h.header_size!=28||h.version!=1||h.total_size!=bytes.size()||h.payload_size!=bytes.size()-28)return false;
    bool found=false;u8 version=0;
    for(u32 p=28;p<bytes.size();){
        if(bytes.size()-p<sizeof(ScoreChapter))return false;
        auto chapter=object<ScoreChapter>(bytes.data()+p);
        if(chapter.length<sizeof(ScoreChapter)||chapter.length>bytes.size()-p)return false;
        if(chapter.magic==fourcc('T','H','8','K')){found=true;version=chapter.version;}
        if(chapter.magic==fourcc('V','R','S','M')&&chapter.version==1){
            if(chapter.length<sizeof(VersionRecord))return false;
            auto v=object<VersionRecord>(bytes.data()+p);if(!compatible_program(v.version,v.exe_size,v.exe_checksum))return false;
        }
        p+=chapter.length;
    }
    return found&&version==1;
}
bool ScoreFile::assign_decoded(const u8* data,u32 size){bytes.assign(data,data+size);if(valid())return true;bytes.clear();return false;}
bool ScoreFile::decode(const u8* data,u32 size){
    bytes.clear();if(size<28||size>4*1024*1024)return false;
    std::vector<u8> decrypted(size);resource_crypt(data,decrypted.data(),size,{0x59,0x79,0x100,0xc00},false);
    u8 key=0;u16 checksum=0;
    for(u32 p=2;p<size;++p){key=rotate(u8(key+decrypted[p-1]));decrypted[p]^=key;if(p>=4)checksum=u16(checksum+decrypted[p]);}
    auto h=object<ScoreHeader>(decrypted.data());
    if(h.checksum!=checksum||h.version!=1||h.header_size!=28||h.payload_size>0xa0000||h.total_size!=h.payload_size+28||h.compressed_size>size-28)return false;
    bytes.resize(h.total_size);std::memcpy(bytes.data(),decrypted.data(),28);u32 written=0;
    if(!codec.decode(decrypted.data()+28,h.compressed_size,bytes.data()+28,h.payload_size,written)||written!=h.payload_size||!valid()){bytes.clear();return false;}
    return true;
}
std::vector<u8> ScoreFile::encode(Rng& rng){
    if(!valid())return {};
    auto packed=codec.encode(bytes.data()+28,bytes.size()-28);auto h=object<ScoreHeader>(bytes.data());
    h.compressed_size=packed.size();h.random1=rng.bounded16(256);h.random2=rng.bounded16(256);h.version=1;h.checksum=0;
    std::vector<u8> output(28+packed.size());std::memcpy(output.data(),&h,28);std::memcpy(output.data()+28,packed.data(),packed.size());
    for(u32 i=4;i<output.size();++i)h.checksum=u16(h.checksum+output[i]);std::memcpy(output.data(),&h,28);
    u8 key=output[1];
    for(u32 i=2;i<output.size();++i){const u8 plain=output[i];key=rotate(key);output[i]^=key;key=u8(key+plain);}
    std::vector<u8> encrypted(output.size());resource_crypt(output.data(),encrypted.data(),output.size(),{0x59,0x79,0x100,0xc00},true);return encrypted;
}
bool ScoreFile::copy_chapter(u32 magic,u8 version,void* output,u32 size,bool first) const {
    bool found=false;
    for(u32 p=28;p<bytes.size();){auto h=object<ScoreChapter>(bytes.data()+p);if(h.magic==magic&&h.version==version&&h.length>=size){std::memcpy(output,bytes.data()+p,size);found=true;if(first)return true;}p+=h.length;}
    return found;
}
void ScoreFile::spells(SpellRecord* out) const {
    for(u32 p=28;p<bytes.size();){auto h=object<ScoreChapter>(bytes.data()+p);if(h.magic==fourcc('C','A','T','K')&&h.version==3&&h.length>=sizeof(SpellRecord)){
        auto record=object<SpellRecord>(bytes.data()+p);if(record.number>=spell_count)break;out[record.number]=record;
    }p+=h.length;}
}
void ScoreFile::clears(ClearRecord* out) const {
    for(u32 i=0;i<=shot_count;++i){out[i]={};auto& r=out[i];r.header={fourcc('C','L','R','D'),sizeof(r),sizeof(r),4,0,0};r.shot=i;for(u32 d=0;d<5;++d)r.without_retries[d]=r.with_retries[d]=1;}
    for(u32 p=28;p<bytes.size();){auto h=object<ScoreChapter>(bytes.data()+p);if(h.magic==fourcc('C','L','R','D')&&h.version==4&&h.length>=sizeof(ClearRecord)){
        auto record=object<ClearRecord>(bytes.data()+p);if(record.shot>shot_count)break;out[record.shot]=record;
    }p+=h.length;}
}
void ScoreFile::practice(PracticeRecord* out) const {
    for(u32 i=0;i<shot_count;++i){out[i]={};auto& r=out[i];r.header={fourcc('P','S','C','R'),sizeof(r),sizeof(r),2,0,0};r.shot=i;}
    for(u32 p=28;p<bytes.size();){auto h=object<ScoreChapter>(bytes.data()+p);if(h.magic==fourcc('P','S','C','R')&&h.version==2&&h.length>=sizeof(PracticeRecord)){
        auto record=object<PracticeRecord>(bytes.data()+p);if(record.shot>=shot_count)break;out[record.shot]=record;
    }p+=h.length;}
}
std::vector<HighScore> ScoreFile::high_scores(u32 character,u32 difficulty) const {
    std::vector<HighScore> result;
    for(u32 p=28;p<bytes.size();){auto h=object<ScoreChapter>(bytes.data()+p);if(h.magic==fourcc('H','S','C','R')&&h.version==4&&h.length>=sizeof(HighScore)){
        auto record=object<HighScore>(bytes.data()+p);
        if(record.character==character&&record.difficulty==difficulty){auto i=result.begin();while(i!=result.end()&&i->score>record.score)++i;result.insert(i,record);}
    }p+=h.length;}return result;
}
u32 ScoreFile::high_score(u32 character,u32 difficulty,u8& retries) const {
    auto scores=high_scores(character,difficulty);retries=scores.empty()?0:u8(scores[0].retries);return scores.empty()?100000:std::max(100000u,scores[0].score);
}
}
