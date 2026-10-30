#include "RecordStore.hpp"
#include <algorithm>
namespace th17 {
namespace {
template<class T>T read(const u8* p,u32 at){T value;std::memcpy(&value,p+at,sizeof value);return value;}
template<class T>void write(u8* p,u32 at,T value){std::memcpy(p+at,&value,sizeof value);}
}
bool RecordStore::fail(const char* reason){if(error.empty())error=reason;return false;}
void RecordStore::decode(){
    for(u32 loadout=0;loadout<10;loadout++){
        const auto* block=file.characters[loadout].data();
        for(u32 d=0;d<6;d++)for(u32 row=0;row<10;row++)std::memcpy(&scores[loadout][d][row],block+0x10+(d*10+row)*32,32);
        for(u32 id=0;id<101;id++){
            const u32 at=0x8d0+id*0x9c;auto& spell=loadout<9?spells.loadouts[loadout][id]:spells.aggregate[id];
            u32 length=0;while(length<128&&block[at+length])length++;spell.name.assign(reinterpret_cast<const char*>(block+at),length);
            for(u32 mode=0;mode<2;mode++){spell.captures[mode]=read<i32>(block,at+0x80+mode*4);spell.attempts[mode]=read<i32>(block,at+0x88+mode*4);}
            spell_difficulty[loadout][id]=read<i32>(block,at+0x94);if(loadout<9)game.spell_best[loadout][id]=read<i32>(block,at+0x98);
        }
        play_centiseconds[loadout]=read<u64>(block,0x4660);
        if(loadout<9){
            game.starts[loadout]=read<i32>(block,0x465c);
            for(u32 d=0;d<5;d++){
                game.clears[loadout][d]=read<i32>(block,0x4668+d*4);game.title.cleared[loadout][d]=read<i32>(block,0x4684+d*4);
                game.high_scores[loadout][d]={scores[loadout][d][0].score,scores[loadout][d][0].continues};
                for(u32 stage=0;stage<8;stage++){const u32 at=0x4698+(d*8+stage)*8;game.practice_scores[loadout][d][stage]=read<i32>(block,at);game.stage_cleared[loadout][d][stage]=block[at+4];game.title.stage_available[loadout][d][stage]=block[at+13];}
            }
        }
    }
    const auto* settings=file.settings.data();std::memcpy(name.data(),settings+12,9);std::copy_n(settings+0x16,16,endings.begin());std::copy_n(settings+0x26,18,music.begin());std::copy_n(settings+0x50,40,trophies.begin());total_play_centiseconds=read<u64>(settings,0x48);
}
void RecordStore::encode(){
    for(u32 loadout=0;loadout<10;loadout++){
        auto* block=file.characters[loadout].data();
        for(u32 d=0;d<6;d++)for(u32 row=0;row<10;row++)std::memcpy(block+0x10+(d*10+row)*32,&scores[loadout][d][row],32);
        for(u32 id=0;id<101;id++){
            const u32 at=0x8d0+id*0x9c;const auto& spell=loadout<9?spells.loadouts[loadout][id]:spells.aggregate[id];
            const u32 length=std::min<u32>(127,spell.name.size());std::memcpy(block+at,spell.name.data(),length);block[at+length]=0;
            for(u32 mode=0;mode<2;mode++){write(block,at+0x80+mode*4,spell.captures[mode]);write(block,at+0x88+mode*4,spell.attempts[mode]);}
            write(block,at+0x94,spell_difficulty[loadout][id]);if(loadout<9)write(block,at+0x98,game.spell_best[loadout][id]);
        }
        write(block,0x4660,play_centiseconds[loadout]);
        if(loadout<9){write(block,0x465c,game.starts[loadout]);for(u32 d=0;d<5;d++){
            write(block,0x4668+d*4,game.clears[loadout][d]);write(block,0x4684+d*4,game.title.cleared[loadout][d]);
            for(u32 stage=0;stage<8;stage++){const u32 at=0x4698+(d*8+stage)*8;write(block,at,game.practice_scores[loadout][d][stage]);block[at+4]=game.stage_cleared[loadout][d][stage];block[at+13]=game.title.stage_available[loadout][d][stage];}
        }}
    }
    auto* settings=file.settings.data();std::memcpy(settings+12,name.data(),9);std::copy(endings.begin(),endings.end(),settings+0x16);std::copy(music.begin(),music.end(),settings+0x26);std::copy(trophies.begin(),trophies.end(),settings+0x50);write(settings,0x48,total_play_centiseconds);
}
void RecordStore::reset(Rng& random){error.clear();file.reset(random);decode();}
bool RecordStore::open(const u8* bytes,u32 size){encode();if(!file.open(bytes,size))return fail(file.error.c_str());decode();error.clear();return true;}
bool RecordStore::save(std::vector<u8>& bytes){if(!error.empty())return false;encode();return file.save(bytes)||fail(file.error.c_str());}
bool RecordStore::import_blocks(const u8* characters,const u8* settings){if(!characters||!settings)return fail("Missing native score blocks");std::memcpy(file.characters.data(),characters,ScoreFile::character_size*10);std::memcpy(file.settings.data(),settings,ScoreFile::settings_size);decode();return true;}
i32 RecordStore::insert_score(i32 loadout,i32 difficulty,const RunScoreSubmission& value){
    if(u32(loadout)>=9||u32(difficulty)>=6){fail("Invalid score ranking selection");return -1;}auto& rows=scores[loadout][difficulty];i32 slot=0;while(slot<10&&u32(rows[slot].score)>u32(value.score))slot++;if(slot==10)return -1;
    for(i32 row=9;row>slot;row--)rows[row]=rows[row-1];auto& row=rows[slot];row.score=value.score;row.stage=u8(value.stage);row.continues=i8(value.continues);row.name.fill(' ');row.name[8]=0;
    row.details[0]=value.timestamp[0];row.details[1]=value.timestamp[1];row.details[2]=signed_bits(float_to_bits(value.slowdown));
    if(difficulty<5)game.high_scores[loadout][difficulty]={rows[0].score,rows[0].continues};return slot;
}
bool RecordStore::score_name(i32 loadout,i32 difficulty,i32 slot,const std::array<char,9>& value){if(u32(loadout)>=9||u32(difficulty)>=6||u32(slot)>=10||value[8])return fail("Invalid registered score name");scores[loadout][difficulty][slot].name=name=value;return true;}
bool RecordStore::add_play_time(i32 loadout,u64 duration){if(u32(loadout)>=9)return fail("Invalid play-time loadout");play_centiseconds[loadout]+=duration;total_play_centiseconds+=duration;return true;}
bool RecordStore::unlock_music(i32 track){if(u32(track)>=18)return fail("Invalid unlocked music");music[track]=1;return true;}
bool RecordStore::trophy(i32 id){if(u32(id)>=40)return fail("Invalid trophy");trophies[id]=1;return true;}
bool RecordStore::unlock_all(){
    encode();auto* base=reinterpret_cast<u8*>(file.characters.data());std::fill_n(file.settings.data()+0x16,16,u8(0x11));
    for(u32 id=0;id<101;id++){const u32 at=9*ScoreFile::character_size+0x958+id*0x9c;const i32 value=read<i32>(base,at);if(value<99999)write(base,at,wrapping_add(value,1));}
    for(u32 loadout=0;loadout<9;loadout++)for(u32 d=0;d<6;d++){
        const u32 start=loadout*ScoreFile::character_size;for(u32 at:{0x4668u+d*4,0x4684u+d*4})write(base,start+at,wrapping_add(read<i32>(base,start+at),1));
        for(u32 stage=1;stage<=8;stage++)base[start+0x4698+(d*8+stage)*8+5]=1;
    }
    decode();return true;
}
}
