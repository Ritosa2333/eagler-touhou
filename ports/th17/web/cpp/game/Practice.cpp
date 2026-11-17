#include "Practice.hpp"
#include "PracticeCatalog.hpp"
#include "EclResource.hpp"
#include "SpellCatalog.hpp"
#include <algorithm>
#include <cmath>
#include <utility>

namespace th17 {
namespace {
using std::pair;

const PracticeSectionInfo* section_info(i32 id) {
    if (id<1 || id>i32(std::size(practice_sections))) return nullptr;
    return &practice_sections[id-1];
}

// Retail 1.00b ECL fingerprints in native load order (stage, default, boss, midboss).
constexpr u64 default_ecl=0x233aee8760286e37ull;
constexpr u64 retail_ecl[7][4] = {
    {0x9d7729cbd625a637ull, default_ecl, 0xf988ce9a25722cb2ull, 0xa3a7402b05d18d70ull},
    {0xd7d4134ea6378a92ull, default_ecl, 0x23852bce5980a9ddull, 0xa6db3db6d45f5bfeull},
    {0x5a474e40856f4911ull, default_ecl, 0xeb58a7cee37cea9cull, 0x8c7faeea04d13eaaull},
    {0x1fe26dcd1daf22faull, default_ecl, 0xbb4dbd3605c6bbd1ull, 0x602152abf05ab6c1ull},
    {0x688c45627740c2c8ull, default_ecl, 0x66539790238316ffull, 0x83ca0dcb155a758bull},
    {0xf6c74f31f55d09fcull, default_ecl, 0x69d3a88b60874729ull, 0xee3d8bc96c8641c3ull},
    {0x11cba15980c385f7ull, default_ecl, 0x0dd054f3eb2ae321ull, 0x7b2f9a4e5eb16082ull},
};

u64 fingerprint(const std::vector<u8>& bytes) {
    u64 value=14695981039346656037ull;
    for (u8 byte:bytes) value=(value^byte)*1099511628211ull;
    return value;
}

float NormRad(float angle) {
    constexpr float two_pi=2.f*3.14159265358979323846f;
    return std::remainder(angle,two_pi);
}

class ECLHelper {
    std::array<std::vector<u8>,4>& files;
    u32 file=0, position=0;
public:
    bool valid=true;
    explicit ECLHelper(std::array<std::vector<u8>,4>& buffers):files(buffers){}
    void SetFile(u32 index){file=index;position=0;valid=valid&&index<files.size();}
    void SetPos(u32 offset){position=offset;}
    u32 GetPos()const{return position;}
    void Jump(u32 start,u32 target){
        if (!valid) return;
        const auto size=files[file].size();
        valid=start<size&&target<size&&size-start>=24&&size-target>=16;
    }
    template<class T>ECLHelper& operator<<(T value) {
        static_assert(sizeof(T)==1 || sizeof(T)==2 || sizeof(T)==4);
        if (!valid) return *this;
        auto& bytes=files[file];
        if (position>bytes.size() || sizeof(T)>bytes.size()-position) {
            valid=false;
            return *this;
        }
        std::memcpy(bytes.data()+position,&value,sizeof(T));
        position+=sizeof(T);
        return *this;
    }
    template<class K,class T>ECLHelper& operator<<(pair<K,T> value) {
        SetPos(u32(value.first));
        return *this<<value.second;
    }
};

class Patcher {
    const PracticePatch& thPracParam;
    i32 difficulty;
#include "PracticePatches.inc"
public:
    Patcher(const PracticePatch& patch,i32 rank):thPracParam(patch),difficulty(rank){}
    void apply(ECLHelper& ecl,i32 spell) {
        if (spell>=0) THPatchSP(ecl,spell);
        else if (thPracParam.section>=10000) THStageWarp(ecl,thPracParam.stage,thPracParam.section%100);
        else THPatch(ecl,thPracParam.section);
    }
};
}

i32 practice_phase_count(i32 section,i32 spell)noexcept {
    if (section==TH17_ST6_BOSS11 || (spell>=84&&spell<=87)) return 4;
    if (section==TH17_ST7_END_S10 || spell==100) return 5;
    return 1;
}

i32 practice_bgm(i32 section)noexcept {
    const auto* info=section_info(section);
    return info?info->bgm:0;
}

bool PracticePatch::valid(i32 difficulty,i32 spell)const noexcept {
    if (stage<1||stage>7||difficulty<0||difficulty>4||(stage==7)!=(difficulty==4)) return false;
    if (phase<0||phase>=practice_phase_count(section,spell)) return false;
    if (std::any_of(keiki_offsets.begin(),keiki_offsets.end(),[](i32 v){return v<0||v>7080;})) return false;
    bool empty_slot=false;
    for (i32 spirit:spirits) {
        if (spirit<0||spirit>14||(empty_slot&&spirit)) return false;
        empty_slot=spirit==0;
    }
    if (spell>=0) {
        if (section||dialogue||keep_drops||spirits[0]||SpellCatalog::rank(spell)!=difficulty) return false;
        for (i32 i=0;i<SpellCatalog::groups[stage-1];++i) {
            const auto group=SpellCatalog::group(stage-1,i);
            if (std::find(group.begin(),group.begin()+(stage==7?1:4),spell)!=group.begin()+(stage==7?1:4)) return true;
        }
        return false;
    }
    if (!section) return !dialogue&&!keep_drops;
    if (section>=10000) return !dialogue&&!keep_drops&&section/100==100+stage&&section%100>=1&&section%100<=practice_waves[stage-1];
    const auto* info=section_info(section);
    return info&&info->stage==stage&&*info->names[difficulty][0]&&(!dialogue||info->dialogue)&&(!keep_drops||info->drops);
}

bool patch_practice_ecl(EclProgram& program,const PracticePatch& patch,i32 difficulty,i32 spell,std::string& error) {
    if (!patch) return true;
    if (!patch.valid(difficulty,spell)) {
        error="Invalid TH17 practice parameters";
        return false;
    }
    if (!patch.section&&!patch.phase) return true;
    std::array<std::vector<u8>,4> transaction;
    if (program.files.size()!=transaction.size()) {
        error="TH17 practice requires the retail 1.00b ECL include set";
        return false;
    }
    for (size_t i=0;i<transaction.size();++i) {
        const auto& bytes=program.files[i]->bytes;
        if (fingerprint(bytes)!=retail_ecl[patch.stage-1][i]) {
            error="TH17 practice requires retail 1.00b ECL (include "+std::to_string(i)+")";
            return false;
        }
        transaction[i]=bytes;
    }
    ECLHelper ecl(transaction);
    Patcher(patch,difficulty).apply(ecl,spell);
    if (!ecl.valid) {
        error="TH17 practice ECL write exceeds the validated script";
        return false;
    }
    if(patch.section==TH17_ST7_END_NS2){
        // Purple borrows the first spell's movement bounds, then jumps back
        // into Boss2. Keep its routine and stack while executing that span.
        auto& boss=program.files[2]->subroutines;
        auto source=std::find_if(boss.begin(),boss.end(),[](const auto& sub){return sub.name=="Boss2";});
        if(source==boss.end()||source->offset+16>0x1e14){
            error="TH17 Extra nonspell branch owner missing";
            return false;
        }
        source->branch_offset=0x6600-(source->offset+16);
        source->branch_size=0x6620+24-0x6600;
    }
    // The retail directory already owns the routine offsets. Purple replaces
    // instructions at those offsets before any interpreter binds its pointers.
    for (size_t i=0;i<transaction.size();++i) program.files[i]->bytes.swap(transaction[i]);
    return true;
}
}
