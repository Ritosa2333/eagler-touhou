#include "Scores.hpp"
#include <cstdlib>
namespace th10::browser {
namespace {
// Spell-card difficulty table, original JP/CHS data at 0x4743c0.
constexpr std::int8_t difficulties[]={
    2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,
    0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,
    2,3,0,1,2,3,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,
    1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,
    3,0,1,2,3,0,1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4
};
static_assert(sizeof(difficulties)==110);
}
Scores::Scores(FileSystem& f,Rng& rng,bool chinese):files(f){
    std::strcpy(path,chinese?"scoreth10c.dat":"scoreth10.dat");filename=path;
    current=&data;random=&rng;spell_difficulties=difficulties;search={nodes,dictionary};
    ScoreData::create(*this);
}
Scores::~Scores(){close_and_unlock();ScoreData::destroy(*this);release_scratch();}
void Scores::reload(){ScoreData::destroy(*this);release_scratch();ScoreData::create(*this);}
i32 Scores::save(){const i32 result=data?data->save(*this):-1;release_scratch();return result;}
void Scores::release_scratch(){
    // The original save routine leaves temporary compression buffers behind
    // if opening the destination fails. They are not persistent score data.
    memory.clear(data?data->file:nullptr,data?data->unpacked:nullptr);
}
ScoreData* Scores::allocate_score(){return static_cast<ScoreData*>(std::malloc(sizeof(ScoreData)));}
void Scores::delete_score(ScoreData* score){std::free(score);}
u8* Scores::allocate_bytes(u32 bytes){return memory.allocate(bytes);}
void Scores::release_bytes(void* bytes){memory.release(bytes);}
u8* Scores::read_file(const char* name,u32& length){return static_cast<u8*>(memory.adopt(ResourceFiles{files}.load(name,&length,true)));}
i32 Scores::open(const char* name){close_and_unlock();output=files.host.open(name,true);return output==0xffffffff?-1:0;}
bool Scores::is_open(){return output!=0xffffffff;}
u32 Scores::write(const void* bytes,u32 length){return files.host.write(output,static_cast<const u8*>(bytes),length);}
void Scores::close_and_unlock(){if(output!=0xffffffff){files.host.close(output);output=0xffffffff;}}
}
