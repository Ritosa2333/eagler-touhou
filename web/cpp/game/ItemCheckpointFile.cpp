#include "ItemCheckpoint.hpp"
#include "AnmFile.hpp"
namespace th15 {
namespace {constexpr u32 stride=0xc88;u32 word(const u8* p,u32 at){u32 value;std::memcpy(&value,p+at,4);return value;}void put(u8* p,u32 at,u32 value){std::memcpy(p+at,&value,4);}}
bool ItemCheckpoint::write_file(std::vector<u8>& out,AnmManager& animations){
    error.clear();out.clear();if(!available){error="No item checkpoint to serialize";return false;}AnmFile codec;
    auto slot=[&](const Slot& s)->bool{const u32 at=out.size();out.resize(at+stride,0);std::memcpy(out.data()+at+0xc20,&s.state,sizeof s.state);if(s.arrow_ended&&!s.arrow)put(out.data()+at,0x618+0x34,0xffffffffu);
        for(u32 i=0;i<2;i++){const auto* vm=i?s.arrow.get():s.body.get();if(!vm)continue;AnmFile::Block record;if(!codec.write(*vm,animations,record,true)){error=codec.error;return false;}std::memcpy(out.data()+at+(i?0x618:0x10),record.data(),record.size());}
        for(const auto* vm:{s.body.get(),s.arrow.get()})if(vm&&vm->geometry.allocation_bytes){AnmFile::Block record;if(!codec.write(*vm,animations,record,true)){error=codec.error;return false;}out.insert(out.end(),record.begin(),record.end());if(!codec.write_geometry(*vm,animations,out,{})){error=codec.error;return false;}}
        return true;};
    for(u32 group=0;group<2;group++){const u32 at=out.size();out.resize(at+4);u32 count=0;const u32 begin=group?ItemManager::ordinary_count:0,end=group?ItemManager::pool_size:ItemManager::ordinary_count;for(u32 i=begin;i<end;i++)if(slots[i].state.state){if(!slot(slots[i])){out.clear();return false;}count++;}put(out.data(),at,count);}
    return true;
}
bool ItemCheckpoint::read_file(const u8* data,u32 size,u32& consumed,AnmManager& animations){
    error.clear();consumed=0;if(!data||size<8){error="Truncated item checkpoint counts";return false;}std::vector<Slot> next(ItemManager::pool_size);AnmFile codec;u32 offset=0;
    auto animation=[&](const u8* fixed,std::unique_ptr<AnmVm>& vm)->bool{
        // Uninitialized cancellation-item VMs have never been bound. Keep that
        // absence rather than binding bullet script zero during a file load.
        if(word(fixed,0x18)==0&&word(fixed,0x28)==0&&word(fixed,0x30)==0&&(word(fixed,0x34)==0||word(fixed,0x34)==0xffffffffu)&&word(fixed,0x5c8)==0){vm.reset();return true;}
        const bool geometry=word(fixed,0x5c8)!=0;const auto* record=geometry?data+offset:fixed;const u32 bytes=geometry?size-offset:0x608;vm=std::make_unique<AnmVm>();if(!codec.read(record,bytes,*vm,animations,true)){error=codec.error;return false;}
        if(geometry){u32 used=0;if(!codec.read_geometry(record,record+0x608,bytes-0x608,*vm,animations,used,{})){error=codec.error;return false;}offset+=0x608+used;}return true;};
    for(u32 group=0;group<2;group++){if(size-offset<4){error="Missing item checkpoint group count";return false;}const u32 count=word(data,offset);offset+=4;const u32 begin=group?ItemManager::ordinary_count:0,limit=group?ItemManager::pool_size-ItemManager::ordinary_count:ItemManager::ordinary_count;if(count>limit){error="Item checkpoint count exceeds pool capacity";return false;}
        for(u32 i=0;i<count;i++){if(size-offset<stride){error="Truncated item checkpoint slot";return false;}const auto* record=data+offset;offset+=stride;auto& s=next[begin+i];std::memcpy(&s.state,record+0xc20,sizeof s.state);s.arrow_ended=signed_bits(word(record,0x618+0x34))<0;if(!animation(record+0x10,s.body)||!animation(record+0x618,s.arrow))return false;}
    }
    slots=std::move(next);available=true;consumed=offset;return true;
}
}
