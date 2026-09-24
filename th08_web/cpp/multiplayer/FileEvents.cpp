#include "FileEvents.hpp"
#include <algorithm>
namespace th08::multiplayer {
void FileEvents::Reset(u32 first){frames.clear();open=Invalid;next=next_commit=first;committed=0;pending_bytes=0;digest=2166136261u;failed=false;}
bool FileEvents::BeginFrame(u32 frame){
    if(failed||IsOpen()||frame!=next||frame==Invalid||frames.size()>=Capacity)return Fail();
    frames.push_back({frame,{}});open=frame;return true;
}
bool FileEvents::Write(const std::string& path,const u8* bytes,u32 size){
    if(failed||!IsOpen()||path.empty()||path.size()>=256||path.find("..")!=std::string::npos||
       path[0]=='/'||path.find(':')!=std::string::npos||(!bytes&&size)||size>MaxBytes||
       pending_bytes>MaxBytes-size||frames.back().writes.size()>=32)return Fail();
    WriteRecord record;record.path=path;if(size)record.bytes.assign(bytes,bytes+size);
    frames.back().writes.push_back(std::move(record));pending_bytes+=size;return true;
}
bool FileEvents::EndFrame(){if(failed||!IsOpen())return false;open=Invalid;++next;return true;}
bool FileEvents::DiscardFrom(u32 frame){
    if(failed||IsOpen()||frame<next_commit||frame>next)return false;
    while(!frames.empty()&&frames.back().number>=frame){for(const auto& w:frames.back().writes)pending_bytes-=w.bytes.size();frames.pop_back();}
    next=frame;return true;
}
const std::vector<u8>* FileEvents::Pending(const std::string& path)const{
    for(auto f=frames.rbegin();f!=frames.rend();++f)
        for(auto w=f->writes.rbegin();w!=f->writes.rend();++w)if(w->path==path)return &w->bytes;
    return nullptr;
}
bool FileEvents::CommitThrough(u32 confirmed,u32 simulated,FileEventOutput& output){
    if(failed||IsOpen())return false;
    if(confirmed==Invalid||simulated==Invalid)return true;
    const auto end=std::min(confirmed,simulated);
    while(next_commit<=end){
        if(frames.empty()||frames.front().number!=next_commit)return Fail();
        for(const auto& w:frames.front().writes){
            if(!output.apply_file_event(w.path.c_str(),w.bytes.data(),u32(w.bytes.size())))return Fail();
            for(const auto c:w.path){digest^=u8(c);digest*=16777619u;}
            for(const auto c:w.bytes){digest^=c;digest*=16777619u;}
            ++committed;pending_bytes-=w.bytes.size();
        }
        frames.pop_front();++next_commit;
    }
    return true;
}
}
