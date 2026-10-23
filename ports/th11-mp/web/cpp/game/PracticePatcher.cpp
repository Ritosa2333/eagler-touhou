// thprac TH11 source adapter (MIT); see PracticePatches.inc and license.
#include "PracticePatcher.hpp"
#include "PracticeSections.hpp"
#include "PracticeSiteChecks.hpp"
#include "PracticeWave.hpp"
#include "EclResource.hpp"
#include <cstring>
#include <utility>
namespace th11 {
namespace {
using std::pair;
class VFile {
protected:
    u8* data=nullptr;size_t size=0,position=0;
public:
    bool valid=true;
    void SetFile(void* bytes,size_t length){data=static_cast<u8*>(bytes);size=length;position=0;}
    void SetPos(size_t offset){position=offset;}
    template<class T>VFile& operator<<(T value){static_assert(sizeof(T)==1||sizeof(T)==2||sizeof(T)==4,"source patch word width");if(!data||position>size||sizeof(T)>size-position){valid=false;return *this;}std::memcpy(data+position,&value,sizeof(T));position+=sizeof(T);return *this;}
    template<class K,class T>VFile& operator<<(pair<K,T> value){SetPos(size_t(value.first));return *this<<value.second;}
    template<class T>VFile& operator>>(T& value){value={};if(!data||position>size||sizeof(T)>size-position){valid=false;return *this;}std::memcpy(&value,data+position,sizeof(T));position+=sizeof(T);return *this;}
};
class ECLHelper:public VFile {
    std::vector<std::vector<u8>>& files;
    unsigned current=0;
public:
    explicit ECLHelper(std::vector<std::vector<u8>>& f):files(f){SetFile(0);}
    void SetFile(unsigned ordinal){if(ordinal>=files.size()){valid=false;VFile::SetFile(nullptr,0);return;}current=ordinal;VFile::SetFile(files[ordinal].data(),files[ordinal].size());}
    size_t AppendWave(bool first_wave){if(!valid)return 0;auto& bytes=files[current];const auto offset=bytes.size();bytes.insert(bytes.end(),std::begin(practice_wave),std::end(practice_wave));if(first_wave)bytes[offset+16]=0;VFile::SetFile(bytes.data(),bytes.size());return offset;}
};
class Patcher {
    const PracticeConfig& thPracParam;
    std::vector<u8>& std_data;
    std::vector<u8>& anm_data;
    i32& stage_section;
    ECLHelper ecl;
    // Generated upstream helper-local VFiles contribute to the transaction.
    // Their bounds status must not be lost when returning from a helper.
    bool auxiliary_valid=true;
#include "PracticePatches.inc"
public:
    Patcher(PracticeBuffers& b,const PracticeConfig& p):thPracParam(p),std_data(b.scene),anm_data(b.background),stage_section(b.stage_section),ecl(b.ecl){}
    bool apply(){if(thPracParam.section>=10000)THStageWarp(ecl,(thPracParam.section-10000)/100,thPracParam.section%100);else THPatch(ecl,thPracParam.section);return ecl.valid&&auxiliary_valid;}
};
}
bool patch_practice_buffers(PracticeBuffers& output,const PracticeConfig& config,std::string& error){
    error.clear();if(!config.valid()){error="Invalid TH11 practice parameters";return false;}
    if(config.mode!=1||!config.section)return true;
    // Check only upstream patch-site ranges, not a whole-file hash. Unrelated
    // translated text may differ; instruction operands at a patch site may not.
    for(const auto& site:practice_site_checks){
        if(site.stage!=config.stage)continue;
        if(site.file>=0&&size_t(site.file)>=output.ecl.size()){error="TH11 practice source ECL ordinal mismatch";return false;}
        const auto& bytes=site.file==-1?output.scene:site.file==-2?output.background:output.ecl[site.file];
        if(site.offset>bytes.size()||site.length>bytes.size()-site.offset){error="TH11 practice source site outside resource";return false;}
        u32 crc=~0u;for(u32 i=0;i<site.length;++i){crc^=bytes[site.offset+i];for(int bit=0;bit<8;++bit)crc=(crc>>1)^(0xedb88320u&u32(-i32(crc&1)));}
#ifndef TH11_PRACTICE_SITE_GENERATION
        if(~crc!=site.crc){error="TH11 practice source site mismatch: stage "+std::to_string(site.stage+1)+", file "+std::to_string(site.file)+", offset "+std::to_string(site.offset);return false;}
#endif
    }
    // Verify the original resource before rewriting instruction bytes. The
    // native THPrac hook runs before pointer binding, not before decoding the
    // directory: shorter injected jumps leave unreachable original operands.
    // Those operands must not be reparsed as an independent instruction stream.
    std::vector<EclResource> verified(output.ecl.size());
    for(size_t i=0;i<output.ecl.size();++i)
        if(!verified[i].open(output.ecl[i].data(),u32(output.ecl[i].size()))){error="Invalid source ECL for TH11 practice patch";return false;}
    // Never publish half of a section patch when a later file/range fails.
    PracticeBuffers transaction=output;
    if(!Patcher(transaction,config).apply()){error="TH11 practice patch exceeds its owned source buffers";return false;}
    for(size_t i=0;i<verified.size();++i){
        const auto& before=output.ecl[i];const auto& after=transaction.ecl[i];
        std::vector<bool> instruction_byte(before.size(),false),boundary(after.size(),false);
        if(after.size()!=before.size()){
            if(config.stage!=4||config.section!=TH11_ST5_MID3||i!=2||(config.phase!=2&&config.phase!=3)||after.size()!=before.size()+sizeof(practice_wave)){error="Unexpected TH11 practice ECL extension";return false;}
            for(size_t n=0;n<sizeof(practice_wave);++n)if(after[before.size()+n]!=(n==16&&config.phase==2?0:practice_wave[n])){error="TH11 purple wave payload mismatch";return false;}
            for(size_t p=before.size();p<after.size();){
                if(after.size()-p<16){error="Truncated purple wave instruction";return false;}
                u16 length;std::memcpy(&length,after.data()+p+6,2);
                if(length<16||length%4||length>after.size()-p){error="Invalid purple wave instruction length";return false;}
                boundary[p]=true;p+=length;
            }
        }
        for(const auto& sub:verified[i].subroutines){
            for(u32 p=sub.offset+16;p<sub.offset+sub.size;){
                boundary[p]=true;u16 length;std::memcpy(&length,before.data()+p+6,2);
                for(u32 j=p;j<p+length;++j)instruction_byte[j]=true;
                p+=length;
            }
        }
        for(size_t p=0;p<before.size();++p)if(before[p]!=after[p]&&!instruction_byte[p]){error="TH11 practice patch changed an ECL directory/header";return false;}
        for(size_t p=0;p+24<=before.size();++p)if(boundary[p]){
            u16 opcode,length;std::memcpy(&opcode,after.data()+p+4,2);std::memcpy(&length,after.data()+p+6,2);
            if(opcode==12&&length==24&&std::memcmp(before.data()+p,after.data()+p,24)){
                i32 delta;std::memcpy(&delta,after.data()+p+16,4);const i64 target=i64(p)+delta;
                if(target<0||target>=i64(boundary.size())||!boundary[size_t(target)]){error="TH11 practice jump does not target a source instruction";return false;}
            }
        }
    }
    output=std::move(transaction);return true;
}
}
