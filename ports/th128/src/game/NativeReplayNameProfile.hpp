#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <span>
#include <stdexcept>
namespace th128::record {
class NativeReplayNameProfile {
    std::array<std::uint8_t,9> name_{32,32,32,32,32,32,32,32,0},nextName_{};
    std::uint64_t revision_=0,nextRevision_=0;
    unsigned stagedSerial_=0;
    bool bound_=false,staged_=false;
    struct Packet {std::uint64_t revision=0;unsigned serial=0;std::array<std::uint8_t,9> name{};};
    static Packet decode(std::span<const std::uint8_t> bytes){
        if(bytes.size()!=32)throw std::runtime_error("last-name profile packet size rejected");
        const auto word=[&](unsigned offset){return unsigned(bytes[offset])|(unsigned(bytes[offset+1])<<8)|(unsigned(bytes[offset+2])<<16)|(unsigned(bytes[offset+3])<<24);};
        if(word(0)!=0x314c4e50u||word(4)!=1||bytes[29]||bytes[30]||bytes[31])
            throw std::runtime_error("last-name profile field protocol rejected");
        Packet result;result.revision=(std::uint64_t(word(12))<<32)|word(8);result.serial=word(16);
        if(result.revision>((std::uint64_t(1)<<53)-1))throw std::runtime_error("last-name profile revision is not exactly representable");
        std::copy_n(bytes.begin()+20,9,result.name.begin());
        if(result.name[8]!=0)throw std::runtime_error("last-name profile native terminator rejected");
        for(unsigned index=0;index<8;++index)if(result.name[index]<32||result.name[index]>126)
            throw std::runtime_error("last-name profile native glyph rejected");
        if(result.revision==0&&std::any_of(result.name.begin(),result.name.begin()+8,[](auto value){return value!=32;}))
            throw std::runtime_error("absent last-name profile must retain the original blank field");
        return result;
    }
public:
    void bind(std::span<const std::uint8_t> bytes){
        const auto observed=decode(bytes);
        if(bound_||observed.serial)throw std::runtime_error("last-name profile initial owner already bound or has a save serial");
        name_=observed.name;revision_=observed.revision;bound_=true;
    }
    void prepare(std::span<const std::uint8_t> bytes,unsigned serial,const std::array<std::uint8_t,8>& name){
        const auto observed=decode(bytes);
        if(!bound_||staged_||!serial||observed.serial!=serial||revision_==((std::uint64_t(1)<<53)-1)
           ||observed.revision!=revision_+1||!std::equal(name.begin(),name.end(),observed.name.begin()))
            throw std::runtime_error("last-name profile durable receipt identity rejected");
        nextName_=observed.name;nextRevision_=observed.revision;stagedSerial_=serial;staged_=true;
    }
    void accept(unsigned serial){
        if(!bound_||!staged_||!serial||serial!=stagedSerial_)throw std::runtime_error("last-name profile full durable receipt missing");
        name_=nextName_;revision_=nextRevision_;staged_=false;stagedSerial_=0;
    }
    bool bound()const{return bound_;}
    std::uint64_t revision()const{return revision_;}
    const auto& name()const{return name_;}
};
}
