#include "ArchiveStreams.hpp"
namespace th10 {
namespace {
void copy_forward(u8* output,const u8* input,u32 count){u32 i=0;for(;count-i>=4;i+=4){u32 value;std::memcpy(&value,input+i,4);std::memcpy(output+i,&value,4);}for(;i<count;++i)output[i]=input[i];}
}
void ArchiveFileStream::close(ArchiveStreamEnvironment& env){if(handle!=0xffffffffu){env.close_file(handle);handle=0xffffffff;access=0;}}
void ArchiveFileStream::full_path(const char* input,char* output,ArchiveStreamEnvironment& env){
    if(std::strchr(input,':')){do{*output++=*input;}while(*input++);return;}
    env.module_filename(output,260);char* end=std::strrchr(output,'\\');if(end)end[1]=0;else *output=0;
    copy_forward(reinterpret_cast<u8*>(output+std::strlen(output)),reinterpret_cast<const u8*>(input),std::strlen(input)+1);
}
bool ArchiveFileStream::open(const char* name,const char* mode,ArchiveStreamEnvironment& env){
    close(env);while(*mode&&*mode!='r'&&*mode!='w'&&*mode!='a')++mode;if(!*mode)return false;
    const bool append=*mode=='a';u32 creation;
    if(*mode=='r'){access=0x80000000;creation=3;}else{if(*mode=='w')env.delete_file(name);access=0x40000000;creation=append?4:2;}
    // Game archive names are bounded by MAX_PATH at their call sites.
    char path[520]{};full_path(name,path,env);handle=env.open_file(path,access,creation);
    if(handle==0xffffffffu)return false;if(append)env.seek_file(handle,0,2);return true;
}
u32 ArchiveFileStream::read(u8* output,u32 length,ArchiveStreamEnvironment& env){if(access!=0x80000000u)return 0;u32 actual=0;env.read_file(handle,output,length,&actual);return actual;}
bool ArchiveFileStream::write(const u8* input,u32 length,ArchiveStreamEnvironment& env){if(access!=0x40000000u)return false;u32 actual=0;env.write_file(handle,input,length,&actual);return actual==length;}
u32 ArchiveFileStream::position(ArchiveStreamEnvironment& env){return handle==0xffffffffu?0:env.seek_file(handle,0,1);}
u32 ArchiveFileStream::size(ArchiveStreamEnvironment& env){return handle==0xffffffffu?0:env.file_size(handle);}
bool ArchiveFileStream::seek(i32 offset,u32 origin,ArchiveStreamEnvironment& env){if(handle==0xffffffffu)return false;env.seek_file(handle,offset,origin);return true;}
u8* ArchiveFileStream::read_all(u32 limit,ArchiveStreamEnvironment& env){
    if(access!=0x80000000u)return nullptr;const u32 length=size(env);if(length>limit)return nullptr;
    auto* bytes=env.allocate_bytes(length);if(!bytes)return nullptr;const u32 saved=position(env);
    if(!seek(saved,0,env))return nullptr;if(!read(bytes,length,env)){env.release_bytes(bytes);return nullptr;}seek(saved,0,env);return bytes;
}
void ArchiveMemoryStream::close(CodecMemory& env){if(bytes)env.release_bytes(bytes);bytes=nullptr;length=0;cursor=nullptr;}
bool ArchiveMemoryStream::seek(i32 offset,u32 origin) noexcept {
    const u32 delta=static_cast<u32>(offset),begin=reinterpret_cast<uintptr_t>(bytes),current=reinterpret_cast<uintptr_t>(cursor);
    u32 next;if(origin==0){if(offset<0||delta>=length)return false;next=begin+delta;}
    else if(origin==1){if(static_cast<i32>(begin+length-current)<=offset)return false;next=current+delta;}
    else if(origin==2){if(offset>0||(offset<0?0u-delta:delta)>=length)return false;next=begin+length+delta;}
    else return false;cursor=reinterpret_cast<u8*>(next);return true;
}
u32 ArchiveMemoryStream::read(u8* output,u32 count) noexcept {
    const u32 remaining=static_cast<u32>(reinterpret_cast<uintptr_t>(bytes))+length-static_cast<u32>(reinterpret_cast<uintptr_t>(cursor));
    if(count>remaining)count=remaining;if(!count)return 0;copy_forward(output,cursor,count);
    cursor=reinterpret_cast<u8*>(reinterpret_cast<uintptr_t>(cursor)+count);return count;
}
bool ArchiveMemoryStream::open_resource(const char* name,ArchiveStreamEnvironment& env){
    close(env);const u32 resource=env.find_resource(name);if(!resource){close(env);return true;}
    const u32 loaded=env.load_resource(resource);if(!loaded){close(env);return true;}
    const u8* data=env.lock_resource(loaded);if(!data){env.free_resource(0);close(env);return true;}
    length=env.resource_size(resource);bytes=env.allocate_bytes(length);if(!bytes){close(env);return true;}
    copy_forward(bytes,data,length);cursor=bytes;env.free_resource(loaded);return true;
}
}
