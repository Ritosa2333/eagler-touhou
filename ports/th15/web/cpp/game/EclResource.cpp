#include "EclResource.hpp"
#include <algorithm>
namespace th15 {
namespace {
template<class T>T read(const u8* p){T v;std::memcpy(&v,p,sizeof v);return v;}
bool span(u32 off,u32 n,u32 size){return off<=size&&n<=size-off;}
bool name(const u8* b,u32& p,u32 limit,std::string& out){const u32 start=p;while(p<limit&&b[p])++p;if(p==limit)return false;out.assign(reinterpret_cast<const char*>(b+start),p-start);++p;return true;}
}
bool EclResource::open(const u8* source,u32 size){
    if(!source||size<36||read<u32>(source)!=0x54504353||read<u16>(source+4)!=1)return false;
    const u32 header=read<u16>(source+6),count=read<u16>(source+16),table=36+header;if(!span(table,count*4,size))return false;
    EclResource next;
    if(header){u32 p=36;const u32 limit=table;
        if(!span(p,8,limit))return false;
        if(read<u32>(source+p)==0x4d494e41){const u32 n=read<u32>(source+p+4);p+=8;if(n>limit-p)return false;
            for(u32 i=0;i<n;++i){std::string item;if(!name(source,p,limit,item))return false;next.animations.push_back(std::move(item));}
            p=(p+3)&~3u;if(!span(p,8,limit))return false;
            if(read<u32>(source+p)==0x494c4345){const u32 n=read<u32>(source+p+4);p+=8;if(n>limit-p)return false;for(u32 i=0;i<n;++i){std::string item;if(!name(source,p,limit,item))return false;next.includes.push_back(std::move(item));}}
        }
    }
    u32 names=table+count*4;std::vector<u32> offsets;offsets.reserve(count);for(u32 i=0;i<count;++i){const u32 off=read<u32>(source+table+i*4);if(!span(off,16,size)||off<names||std::memcmp(source+off,"ECLH",4))return false;offsets.push_back(off);}
    const u32 names_end=offsets.empty()?size:*std::min_element(offsets.begin(),offsets.end());
    for(u32 i=0;i<count;++i){EclSubroutine sub;if(!name(source,names,names_end,sub.name))return false;sub.offset=offsets[i];u32 end=size;for(u32 other:offsets)if(other>sub.offset)end=std::min(end,other);sub.size=end-sub.offset;
        for(u32 p=sub.offset+16;p<end;){if(!span(p,16,end))return false;const u32 length=read<u16>(source+p+6);if(length<16||length%4||!span(p,length,end))return false;p+=length;}
        next.subroutines.push_back(std::move(sub));
    }
    next.bytes.assign(source,source+size);*this=std::move(next);return true;
}
i32 EclProgram::attach(const u8* data,u32 size,EclResourceProvider* provider){
    if(files.size()>=32){error="ECL include limit exceeded";return -1;}
    auto file=std::make_unique<EclResource>();if(!file->open(data,size)){error="Invalid ECL resource";return -1;}
    const i32 result=files.size();const auto* current=file.get();const bool first=files.empty();files.push_back(std::move(file));
    for(u32 i=0;i<current->subroutines.size();++i){Definition def{current,i};if(first)definitions.push_back(def);else{const auto& key=current->subroutines[i].name;auto at=definitions.begin();while(at!=definitions.end()&&std::strcmp(key.c_str(),at->file->subroutines[at->index].name.c_str())>0)++at;definitions.insert(at,def);}}
    if(provider){for(u32 i=0;i<current->animations.size();++i){if(!provider->animation(i+10,current->animations[i])){error="Missing animation: "+current->animations[i];return result;}}for(const auto& name:current->includes)load(name,*provider);}
    return result;
}
bool EclProgram::load(const std::string& name,EclResourceProvider& provider){std::vector<u8> data;if(!provider.read(name,data)){error="Missing ECL: "+name;return false;}return attach(data.data(),data.size(),&provider)>=0;}
i32 EclProgram::find_index(const char* name)const noexcept{if(!name)return -1;i32 low=0,high=i32(definitions.size())-1;while(low<=high){const i32 mid=low+(high-low)/2;const auto& sub=subroutine(mid);const int order=std::strcmp(name,sub.name.c_str());if(!order)return mid;if(order<0)high=mid-1;else low=mid+1;}return -1;}
const EclInstruction* EclProgram::find(const char* name)const noexcept{const i32 index=find_index(name);if(index<0)return nullptr;const auto& def=definitions[index];return reinterpret_cast<const EclInstruction*>(def.file->bytes.data()+subroutine(index).offset+16);}
}
