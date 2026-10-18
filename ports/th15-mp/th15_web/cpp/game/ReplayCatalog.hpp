#pragma once
#include "Replay.hpp"
#include <memory>
namespace th15 {
struct ReplayCatalogEntry {std::shared_ptr<Replay> file;std::string filename;};
class ReplayCatalog {
 std::array<ReplayCatalogEntry,100> entries;
public:
 void clear(){entries={};}
 bool set(u32 index,const std::string& name,const u8* bytes,u32 size){if(index>=entries.size()||name.size()>255)return false;auto replay=std::make_shared<Replay>();if(!replay->open(bytes,size)){entries[index]={};return false;}entries[index]={std::move(replay),name};return true;}
 const ReplayCatalogEntry* entry(u32 index)const noexcept{return index<entries.size()&&entries[index].file?&entries[index]:nullptr;}
};
}
