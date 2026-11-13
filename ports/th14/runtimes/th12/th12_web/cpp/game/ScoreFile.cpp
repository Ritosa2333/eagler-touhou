#include "ScoreFile.hpp"
#include "Lzss.hpp"
#include "ResourceCrypt.hpp"
#include <cstring>
namespace th11 {
namespace {
u32 get(const u8* p){return u32(p[0])|u32(p[1])<<8|u32(p[2])<<16|u32(p[3])<<24;}
void put(u8* p,u32 v){for(u32 i=0;i<4;++i)p[i]=u8(v>>(8*i));}
}
#if PORT_TITLE == 14
void ScoreFile::th14_initialize_character(Th14Character& record){
    // Complete retail4567a0. Six ranking bands, then120 spell records.
    record={};auto* p=record.data();put(p,0x15243);put(p+8,0x5298);
    for(u32 difficulty=0;difficulty<6;++difficulty)for(u32 rank=0;rank<10;++rank){
        auto* row=p+0x10+(difficulty*10+rank)*28;
        put(row,100000-rank*10000);row[4]=1;std::memset(row+6,'-',8);
    }
    for(u32 id=0;id<120;++id){
        put(p+0xa08+id*0x9c,id);
        put(p+0xa0c+id*0x9c,id<26?(id+2)%4:id<108?id%4:4);
    }
}
void ScoreFile::th14_initialize_settings(Th14Settings& record,Rng& gameplay){
    // Retail456b20 uses4db510/4db514, leaving the visual RNG untouched.
    record={};auto* p=record.data();put(p,0x15453);put(p+8,0x42c);
    std::memset(p+12,' ',8);
    for(u32 i=0;i<494;++i){const u16 value=gameplay.next16();p[0x50+i*2]=u8(value);p[0x51+i*2]=u8(value>>8);}
}
i32 ScoreFile::th14_insert_score(Th14Character& record,u32 difficulty,u32 score,i32 stage,i32 continues,u64 timestamp,float slowdown){
    // Complete447e00, including unsigned rank comparisons and retained byte15.
    if(difficulty>=6)return -1;auto* rows=record.data()+0x10+difficulty*0x118;
    for(i32 rank=0;rank<10;++rank)if(get(rows+rank*28)<=score){
        auto* row=rows+rank*28;if(rank<9)std::memmove(row+28,row,size_t(9-rank)*28);
        put(row,score);row[4]=u8(stage);row[5]=u8(continues);std::memset(row+6,' ',8);row[14]=0;
        std::memcpy(row+16,&timestamp,8);std::memcpy(row+24,&slowdown,4);return rank;
    }return -1;
}
#endif
#if PORT_TITLE == 16
void ScoreFile::th16_initialize_character(Th16Character& record){
    // Complete4493c0, including the119 spell index/difficulty defaults.
    record={};auto* p=record.data();put(p,0x15243);put(p+8,0x5318);
    for(u32 difficulty=0;difficulty<6;++difficulty)for(u32 rank=0;rank<10;++rank){
        auto* row=p+0x10+(difficulty*10+rank)*32;put(row,100000-rank*10000);row[4]=1;std::memset(row+6,'-',8);
    }
    for(u32 id=0;id<119;++id){put(p+0x960+id*0x9c,id);put(p+0x964+id*0x9c,id<20?id%4:id<106?(id-18)%4:4);}
}
i32 ScoreFile::th16_insert_score(u32 selection,u32 difficulty,u32 score,i32 stage,i32 continues,u64 timestamp,float slowdown,u32 subseason){
    // Own43e250: unsigned ranking,32-byte rows; byte15 remains untouched.
    if(selection>=5||difficulty>=6)return -1;auto* rows=th16_characters[selection].data()+0x10+difficulty*0x140;
    for(i32 rank=0;rank<10;++rank)if(get(rows+rank*32)<=score){
        auto* row=rows+rank*32;if(rank<9)std::memmove(row+32,row,size_t(9-rank)*32);
        put(row,score);row[4]=u8(stage);row[5]=u8(continues);std::memset(row+6,' ',8);row[14]=0;
        std::memcpy(row+16,&timestamp,8);std::memcpy(row+24,&slowdown,4);put(row+28,subseason);return rank;
    }return -1;
}
bool ScoreFile::th16_score_name(u32 selection,u32 difficulty,u32 rank,const char* name){
    if(selection>=5||difficulty>=6||rank>=10||!name)return false;const auto length=std::strlen(name);if(length>8)return false;
    auto* row=th16_characters[selection].data()+0x16+(difficulty*10+rank)*32;
    std::memcpy(row,name,length+1);std::memcpy(th16_last_name.data(),name,length+1);return true;
}
#endif
u32 ScoreFile::checksum(const u8* p,u32 size){u32 sum=0;for(u32 i=8;i<size;++i)sum+=p[i];return sum;}
void ScoreFile::initialize(Rng& rng){
    error.clear();characters={};settings={};
#if PORT_TITLE == 14
    th14_initialize_settings(settings,rng);
    for(auto& character:characters)th14_initialize_character(character);
#else
    put(settings.data(),0x5453);put(settings.data()+8,0x448);std::memset(settings.data()+12,' ',8);
    for(u32 i=0;i<512;++i){const auto value=rng.next16();settings[0x46+i*2]=u8(value);settings[0x47+i*2]=u8(value>>8);}
    for(auto& character:characters){auto* p=character.data();put(p,0x5243);put(p+8,0x68d4);
        for(u32 difficulty=0;difficulty<5;++difficulty)for(u32 rank=0;rank<10;++rank){auto* row=p+16+(difficulty*10+rank)*28;put(row,1000000-rank*100000);row[4]=1;std::memset(row+6,'-',8);}
        for(u32 spell=0;spell<175;++spell){put(p+0x6ec+spell*0x90,spell);put(p+0x6f0+spell*0x90,spell<162?(spell+2)%4:4);}
    }
#endif
#if PORT_TITLE == 16
    for(auto& character:th16_characters)th16_initialize_character(character);
#endif
}
bool ScoreFile::open(const u8* data,u32 size){
    error.clear();auto fail=[&](const char* s){error=s;return false;};
    if(!data||size<24||get(data)!=file_magic||(get(data+8)&65535)!=file_version)return fail("score title or version mismatch");
    const u32 packed=get(data+16),unpacked=get(data+20);
    if(!packed||packed>size-24||unpacked<12||unpacked>2*1024*1024)return fail("invalid score sizes");
    std::vector<u8> decrypted(packed),decoded(unpacked);u32 written=0;Lzss codec;
    if(!resource_crypt(data+24,decrypted.data(),packed,{0xac,0x35,0x10,packed},false)||!codec.decode(decrypted.data(),packed,decoded.data(),unpacked,written)||written!=unpacked)return fail("invalid score compression");
    auto next=characters;auto prefs=settings;u32 cursor=0;bool found=false;
    while(cursor<unpacked){if(unpacked-cursor<12)return fail("truncated score record");const auto* p=decoded.data()+cursor;const u32 kind=get(p),n=get(p+8);
        if(n<12||n>unpacked-cursor)return fail("invalid score record size");
        if(kind==(kPortTitle==14?0x15243u:0x5243u)&&n==character_size){const u32 index=get(p+12);if(index>=7)return fail("invalid score character");if(checksum(p,n)==get(p+4)){std::memcpy(next[index].data(),p,n);found=true;}}
        else if(kind==(kPortTitle==14?0x15453u:0x5453u)&&n==settings_size){if(checksum(p,n)==get(p+4)){std::memcpy(prefs.data(),p,n);found=true;}}
        else return fail("unrecognized score record");cursor+=n;
    }
    if(!found)return fail("score checksum failed");characters=next;settings=prefs;return true;
}
bool ScoreFile::save(std::vector<u8>& out)const{
    std::vector<u8> decoded;decoded.reserve(7*character_size+settings_size);
    for(u32 index=0;index<7;++index){auto record=characters[index];put(record.data()+12,index);put(record.data()+4,checksum(record.data(),u32(record.size())));decoded.insert(decoded.end(),record.begin(),record.end());}
    auto prefs=settings;put(prefs.data()+4,checksum(prefs.data(),u32(prefs.size())));decoded.insert(decoded.end(),prefs.begin(),prefs.end());
    Lzss codec;auto packed=codec.encode(decoded.data(),u32(decoded.size()));const u32 n=u32(packed.size());if(!n)return false;
    out.assign(24+n,0);put(out.data(),file_magic);put(out.data()+4,24+n);put(out.data()+8,file_version);put(out.data()+12,0x100);put(out.data()+16,n);put(out.data()+20,u32(decoded.size()));
    return resource_crypt(packed.data(),out.data()+24,n,{0xac,0x35,0x10,n},true);
}
void ScoreFile::read_records(SpellRecords& spells,ClearRecords& clears)const{
    for(u32 selection=0;selection<7;++selection){const auto* p=characters[selection].data();
        for(u32 id=0;id<spell_count;++id){auto& r=spells.entries[selection][id];const auto* b=p+spell_offset+id*spell_stride;
            std::memcpy(r.name.data(),b,r.name.size());if constexpr(kPortTitle!=14)r.name.back()=0;
            r.captures=signed_bits(get(b+0x80));r.attempts=signed_bits(get(b+(kPortTitle==14?0x88:0x84)));
#if PORT_TITLE == 14
            r.practice_captures=signed_bits(get(b+0x84));r.practice_attempts=signed_bits(get(b+0x8c));
#endif
        }
        if(selection<6){for(u32 d=0;d<5;++d)clears.clears[selection][d]=signed_bits(get(p+clear_count_offset+d*4));
#if PORT_TITLE == 14
            for(u32 d=0;d<5;++d)clears.no_continue_clears[selection][d]=signed_bits(get(p+0x7dc+d*4));
#endif
            for(u32 i=1;i<=practice_slots;++i)clears.stages[selection][i]={p[practice_flags_offset+i*8],p[practice_flags_offset+1+i*8]};}
    }
}
void ScoreFile::write_records(const SpellRecords& spells,const ClearRecords& clears){
    for(u32 selection=0;selection<7;++selection){auto* p=characters[selection].data();
        for(u32 id=0;id<spell_count;++id){const auto& r=spells.entries[selection][id];auto* b=p+spell_offset+id*spell_stride;
            std::memcpy(b,r.name.data(),r.name.size());put(b+0x80,u32(r.captures));put(b+(kPortTitle==14?0x88:0x84),u32(r.attempts));
#if PORT_TITLE == 14
            put(b+0x84,u32(r.practice_captures));put(b+0x8c,u32(r.practice_attempts));
#endif
        }
        if(selection<6){for(u32 d=0;d<5;++d)put(p+clear_count_offset+d*4,u32(clears.clears[selection][d]));
#if PORT_TITLE == 14
            for(u32 d=0;d<5;++d)put(p+0x7dc+d*4,u32(clears.no_continue_clears[selection][d]));
#endif
            for(u32 i=1;i<=practice_slots;++i){p[practice_flags_offset+i*8]=clears.stages[selection][i][0];p[practice_flags_offset+1+i*8]=clears.stages[selection][i][1];}}
    }
}
i32 ScoreFile::high_score(u32 selection,u32 difficulty)const{
#if PORT_TITLE == 16
    return selection<5&&difficulty<6?signed_bits(get(th16_characters[selection].data()+0x10+difficulty*0x140)):0;
#else
    return selection<7&&difficulty<5?signed_bits(get(characters[selection].data()+0x10+difficulty*0x118)):0;
#endif
}
i32 ScoreFile::high_continues(u32 selection,u32 difficulty)const{
#if PORT_TITLE == 16
    return selection<5&&difficulty<6?i8(th16_characters[selection][0x15+difficulty*0x140]):0;
#else
    return selection<7&&difficulty<5?i8(characters[selection][0x15+difficulty*0x118]):0;
#endif
}
i32 ScoreFile::insert_score(u32 selection,u32 difficulty,i32 score,i32 stage,i32 continues,u64 timestamp,float slowdown){
#if PORT_TITLE == 14
    return selection<7?th14_insert_score(characters[selection],difficulty,u32(score),stage,continues,timestamp,slowdown):-1;
#else
    if(selection>=7||difficulty>=5)return -1;
    auto* rows=characters[selection].data()+0x10+difficulty*0x118;
    for(i32 rank=0;rank<10;++rank)if(signed_bits(get(rows+rank*28))<=score){
        auto* row=rows+rank*28;if(rank<9)std::memmove(row+28,row,size_t(9-rank)*28);
        put(row,u32(score));row[4]=u8(stage);row[5]=u8(continues);std::memset(row+6,' ',8);row[14]=0;
        std::memcpy(row+16,&timestamp,8);std::memcpy(row+24,&slowdown,4);return rank;
    }return -1;
#endif
}
bool ScoreFile::score_name(u32 selection,u32 difficulty,u32 rank,const char* name){
    if(selection>=7||difficulty>=5||rank>=10||!name)return false;
    const size_t length=std::strlen(name);if(length>8)return false;
    auto* p=characters[selection].data()+0x16+(difficulty*10+rank)*28;
#if PORT_TITLE == 14
    // Actual44acb1/44acd0 copy the terminating byte and retain the tail.
    std::memcpy(p,name,length+1);std::memcpy(settings.data()+12,name,length+1);return true;
#else
    std::memset(p,' ',8);std::memcpy(p,name,length);p[8]=0;
    std::memset(settings.data()+12,' ',8);std::memcpy(settings.data()+12,name,length);settings[20]=0;return true;
#endif
}
}
