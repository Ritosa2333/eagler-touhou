#include "ScoreData.hpp"
namespace th10 {
u32 score_checksum(const void* bytes,u32 length) noexcept {u32 sum=0;const auto* data=static_cast<const u8*>(bytes);for(u32 i=8;i<length;i++)sum+=data[i];return sum;}
// 0x42acb0. Seven records include the combined score/spell-card statistics.
void CharacterRecord::initialize(const std::int8_t* difficulties) noexcept {std::memset(this,0,sizeof(*this));header.signature=0x5243;header.length=sizeof(*this);for(auto& difficulty:high_scores){i32 score=1000000;for(auto& entry:difficulty){entry.score=score;entry.stage=1;std::memcpy(entry.name,"--------",9);score-=100000;}}for(u32 i=0;i<110;i++){spells[i].reserved[0]=i;spells[i].reserved[1]=static_cast<i32>(difficulties[i]);}}
// 0x42add0. New save settings consume exactly 512 words from the script RNG.
void ScoreSettings::initialize(Rng& random) noexcept {std::memset(this,0,sizeof(*this));header.signature=0x5453;header.length=sizeof(*this);std::memcpy(last_name,"        ",9);for(auto& word:random_words)word=random.next_word();}
void ScoreData::initialize(ScoreFileEnvironment& env){std::memset(this,0,sizeof(*this));u32 length;file=reinterpret_cast<ScoreFileHeader*>(env.read_file(env.filename,length));settings.initialize(*env.random);for(auto& character:characters)character.initialize(env.spell_difficulties);load(env);}
// 0x42b030. Invalid or unknown chunks retain any records already accepted,
// then replace the outer header so a later save remains possible.
i32 ScoreData::load(ScoreFileEnvironment& env){
    if(file){if(file->signature==0x30314854&&file->version==3){auto* packed=reinterpret_cast<u8*>(file+1);transform_resource(packed,file->packed_bytes,0xac,0x35,16,file->packed_bytes,false,env);unpacked=env.allocate_bytes(file->unpacked_bytes*4);decode_lzss(packed,file->packed_bytes,unpacked,file->unpacked_bytes,env.search.dictionary);i32 remaining=static_cast<i32>(file->unpacked_bytes);auto* chunk=reinterpret_cast<ScoreChunkHeader*>(unpacked);if(remaining<=0)return 0;
        while(remaining>0){if(chunk->signature==0x5243){const auto* record=reinterpret_cast<CharacterRecord*>(chunk);if(!chunk->version&&chunk->checksum==score_checksum(chunk,sizeof(CharacterRecord))&&chunk->length==sizeof(CharacterRecord)&&static_cast<u32>(record->character)<7)std::memcpy(&characters[record->character],record,sizeof(CharacterRecord));}else if(chunk->signature==0x5453){if(!chunk->version&&chunk->checksum==score_checksum(chunk,sizeof(ScoreSettings))&&chunk->length==sizeof(ScoreSettings))std::memcpy(&settings,chunk,sizeof(ScoreSettings));}else break;
            if(!chunk->length||chunk->length>static_cast<u32>(remaining))break;remaining-=chunk->length;chunk=reinterpret_cast<ScoreChunkHeader*>(reinterpret_cast<u8*>(chunk)+chunk->length);if(!remaining)return 0;
        }
    }env.release_bytes(file);file=nullptr;}
    file=reinterpret_cast<ScoreFileHeader*>(env.allocate_bytes(sizeof(ScoreFileHeader)));if(!file)return -1;std::memset(file,0,sizeof(*file));file->signature=0x30314854;file->version=3;file->version_code=0x100;return 0;
}
static void write_score(ScoreFileEnvironment& env,const void* bytes,u32 length){if(env.is_open()&&env.write(bytes,length)!=length)env.close_and_unlock();}
// 0x42b1e0. Chunk checksums include their length and character number, but
// exclude the signature/version/checksum bytes at the front of each chunk.
i32 ScoreData::save(ScoreFileEnvironment& env){
    if(!file)return -1;auto* plain=env.allocate_bytes(0x200000);if(!plain)return -1;std::memcpy(plain,file,sizeof(*file));u32 cursor=sizeof(*file);
    for(i32 i=0;i<7;i++){auto& record=characters[i];if(record.header.signature!=0x5243)continue;record.character=i;record.header.checksum=score_checksum(&record,sizeof(record));std::memcpy(plain+cursor,&record,sizeof(record));cursor+=sizeof(record);}
    settings.header.checksum=score_checksum(&settings,sizeof(settings));std::memcpy(plain+cursor,&settings,sizeof(settings));file->unpacked_bytes=cursor+sizeof(settings)-sizeof(*file);auto* packed=encode_lzss(plain+sizeof(*file),file->unpacked_bytes,file->packed_bytes,env.search,env);file->length=file->packed_bytes+sizeof(*file);transform_resource(packed,file->packed_bytes,0xac,0x35,16,file->packed_bytes,true,env);
    if(env.open(env.filename))return -1;write_score(env,file,sizeof(*file));write_score(env,packed,file->packed_bytes);if(env.is_open())env.close_and_unlock();if(packed)env.release_bytes(packed);env.release_bytes(plain);return 0;
}
void ScoreData::release(ScoreFileEnvironment& env){if(file){env.release_bytes(file);file=nullptr;}if(unpacked){env.release_bytes(unpacked);unpacked=nullptr;}}
void ScoreData::create(ScoreFileEnvironment& env){auto* score=env.allocate_score();if(score)score->initialize(env);*env.current=score;}
void ScoreData::destroy(ScoreFileEnvironment& env){if(auto* score=*env.current){score->release(env);env.delete_score(score);}*env.current=nullptr;}
}
