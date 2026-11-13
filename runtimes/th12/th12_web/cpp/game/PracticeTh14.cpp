#include "PortTitle.hpp"
#include "PracticeTh14.hpp"
#if PORT_TITLE == 14
#include "EclResource.hpp"
#include "GameEconomy.hpp"
#include <algorithm>
#include <array>
#include <utility>
namespace th11 {
namespace {
struct Section {i32 id,native,stage,bgm,phases;bool dialogue,drops;i32 ranks;};
#include "PracticeCatalogTh14.inc"
constexpr i32 chapters[]={5,6,6,7,6,4,9};
const Section* section_at(i32 id){
    for(const auto& s:sections)if(s.id==id)return &s;
    return nullptr;
}
i32 spell_phases(i32 spell){
    if(spell>=100&&spell<=103)return 2;
    if(spell>=104&&spell<=107)return 4;
    if(spell==118)return 2;
    return spell==119?5:1;
}
u64 fingerprint(const std::vector<u8>& bytes){
    u64 value=14695981039346656037ull;
    for(const u8 b:bytes)value=(value^b)*1099511628211ull;
    return value;
}
using std::pair;
class ECLHelper {
    std::array<std::vector<u8>,6>& files;
    u32 file=0,pos=0;
public:
    bool valid=true;
    explicit ECLHelper(std::array<std::vector<u8>,6>& f):files(f){}
    void SetFile(u32 n){file=n;pos=0;valid=valid&&n<files.size()&&!files[n].empty();}
    void SetPos(u32 offset){pos=offset;}
    void Jump(u32 from,u32 to){
        if(valid){const auto n=files[file].size();valid=from<=n&&n-from>=24&&to<n&&n-to>=16;}
    }
    template<class T>ECLHelper& operator<<(T v){
        static_assert(sizeof(T)==1||sizeof(T)==2||sizeof(T)==4);
        if(valid){auto& b=files[file];if(pos>b.size()||sizeof(T)>b.size()-pos)valid=false;
            else {std::memcpy(b.data()+pos,&v,sizeof v);pos+=sizeof v;}}
        return *this;
    }
    template<class K,class T>ECLHelper& operator<<(pair<K,T> v){SetPos(u32(v.first));return *this<<v.second;}
};
class Patcher {
    struct Params{bool dlg,keepSpellDrops;i32 phase;float st6bs8_phase;} thPracParam;
#include "PracticePatchesTh14.inc"
public:
    explicit Patcher(const PracticeTh14& p):thPracParam{bool(p.flags&1),bool(p.flags&2),p.phase,float(p.angle)/100.f}{}
    void apply(ECLHelper& h,const PracticeTh14& p,i32 spell){
        if(spell>=0)THPatchSP(h,spell);
        else if(p.section>=10000)THStageWarp(h,p.stage,p.section%100);
        else if(const auto* s=section_at(p.section))THPatch(h,s->native);
    }
};
}
bool PracticeTh14::valid(i32 difficulty,i32 spell)const noexcept{
    if(stage<1||stage>7||difficulty<0||difficulty>4||(stage==7)!=(difficulty==4)||
        flags<0||flags>11||phase<0||angle<-100||angle>100||life<0||life>8||life_fragment<0||life_fragment>2||
        bomb<0||bomb>8||bomb_fragment<0||bomb_fragment>7||power<0||power>400||value<0||value>999990||
        graze<0||graze>999999||score<0||score>999999999||cycle<0||cycle>4)return false;
    if(spell>=0)return !section&&!flags&&phase<spell_phases(spell);
    if((flags&12)&&stage!=4)return false;
    if(!section)return !phase&&!(flags&3);
    if(section>=10000)return !phase&&!(flags&3)&&section/100==100+stage&&section%100>=1&&section%100<=chapters[stage-1];
    const auto* s=section_at(section);
    return s&&s->stage==stage&&(s->ranks&(1<<difficulty))&&phase<s->phases&&
        (!(flags&1)||s->dialogue)&&(!(flags&2)||s->drops);
}
void PracticeTh14::apply(GameEconomy& e)const noexcept{
    e.score_units=score;e.lives=life;e.life_fragments=life_fragment;e.th14_bombs=bomb;e.th14_bomb_fragments=bomb_fragment;
    e.power=std::min(power,e.max_power);e.point_value=value*100;e.max_point_value=std::max(e.max_point_value,e.point_value);
    e.graze=graze;e.th14_life_awards=cycle;
}
i32 PracticeTh14::bgm()const noexcept{const auto* s=section_at(section);return s&&!(flags&1)?s->bgm:0;}
bool patch_practice_th14(EclProgram& program,const PracticeTh14& p,i32 spell,std::string& error){
    if(!p||(!p.section&&!p.phase))return true;
    const auto& hashes=retail_ecl[p.stage-1];const size_t count=p.stage==4?6:4;
    if(program.files.size()!=count){error="TH14 practice requires retail 1.00b ECL includes";return false;}
    std::array<std::vector<u8>,6> buffers;
    std::array<size_t,6> slots{};
    for(size_t i=0;i<count;++i){
        const size_t slot=i;
        slots[i]=slot;
        const auto& b=program.files[slot]->bytes;
        if(fingerprint(b)!=hashes[i]){error="TH14 practice ECL fingerprint mismatch";return false;}
        buffers[i]=b;
    }
    ECLHelper edit(buffers);Patcher(p).apply(edit,p,spell);
    if(!edit.valid){error="TH14 practice ECL write outside retail script";return false;}
    for(size_t i=0;i<count;++i)program.files[slots[i]]->bytes.swap(buffers[i]);
    return true;
}
}
#endif
