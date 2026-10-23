#pragma once
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace th10::adaptation {
// Disposable native files keep normal open/seek/read/write semantics while a
// controlled input session is armed. No host writer is opened by this owner.
class ValidationWriteOverlay {
    struct Handle {std::string name;std::uint32_t offset=0;bool write=false;};
    std::map<std::string,std::vector<std::uint8_t>> files_;
    std::map<std::uint32_t,Handle> handles_;
    std::uint32_t attempts_=0,overrides_=0,bytes_=0,rejected_=0;
    bool locked_=false;
public:
    void reset(){files_.clear();handles_.clear();attempts_=overrides_=bytes_=rejected_=0;locked_=false;}
    bool lock(bool hostWriterOpen){if(hostWriterOpen||locked_)return false;locked_=true;return true;}
    bool locked() const noexcept{return locked_;}
    bool contains(std::uint32_t id) const{return handles_.count(id)!=0;}
    bool has_file(const std::string& name) const{return locked_&&files_.count(name)!=0;}
    std::size_t handle_count() const{return handles_.size();}
    bool open(std::uint32_t id,const std::string& name,bool write){
        if(!locked_||name.empty()||handles_.count(id))return false;
        if(write){++attempts_;++overrides_;files_[name].clear();}
        else if(!files_.count(name))return false;
        handles_.emplace(id,Handle{name,0,write});return true;
    }
    void close(std::uint32_t id){handles_.erase(id);}
    std::uint32_t size(std::uint32_t id) const{const auto h=handles_.find(id);return h==handles_.end()?~0u:files_.at(h->second.name).size();}
    std::uint32_t seek(std::uint32_t id,std::int32_t offset,std::uint32_t origin){
        auto h=handles_.find(id);if(h==handles_.end()||origin>2)return ~0u;
        const auto base=origin==0?0:origin==1?h->second.offset:size(id);
        const std::int64_t next=std::int64_t(base)+offset;
        if(next<0||next>0xffffffffll){++rejected_;return ~0u;}
        return h->second.offset=static_cast<std::uint32_t>(next);
    }
    std::uint32_t read(std::uint32_t id,std::uint8_t* out,std::uint32_t count){
        auto h=handles_.find(id);if(h==handles_.end())return 0;const auto& bytes=files_.at(h->second.name);
        const auto available=h->second.offset<bytes.size()?bytes.size()-h->second.offset:0;
        const auto amount=std::min<std::size_t>(count,available);
        if(amount)std::memcpy(out,bytes.data()+h->second.offset,amount);h->second.offset+=amount;return amount;
    }
    std::uint32_t write(std::uint32_t id,const std::uint8_t* data,std::uint32_t count){
        auto h=handles_.find(id);if(h==handles_.end()||!h->second.write){++rejected_;return 0;}
        const auto end=std::uint64_t(h->second.offset)+count;
        if(end>64u*1024u*1024u){++rejected_;return 0;}
        auto& bytes=files_[h->second.name];if(bytes.size()<end)bytes.resize(end);
        if(count)std::memcpy(bytes.data()+h->second.offset,data,count);h->second.offset+=count;bytes_+=count;return count;
    }
    void receipt(std::uint32_t* out) const{const std::uint32_t words[]{1,locked_,0,attempts_,overrides_,bytes_,0,rejected_};
        std::copy(std::begin(words),std::end(words),out);}
};
}
