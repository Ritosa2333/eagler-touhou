#pragma once
#include "Thprac128Source.hpp"
#include "Thprac128SectionData.hpp"
#include <bit>
#include <unordered_set>
#include <stdexcept>
namespace th128::practice {
namespace section_detail {
// The upstream direct-card patch calls all three cards from Boss1A/B/C.
// BossB/C reach those calls one tick before BossA. Let the leader's entry
// synchronization run before the partners register their NEXT phase.
inline void alignDirectThreeBossCard(std::vector<ecl::Program::PracticeRegion>& regions){
    if(regions.size()!=3||regions[0].instructions.size()!=2||regions[0].instructions[0].opcode!=411
        ||regions[1].instructions.size()!=1||regions[2].instructions.size()!=1)return;
    auto& a=regions[0].instructions[1];auto& b=regions[1].instructions[0];auto& c=regions[2].instructions[0];
    const auto card=[](const ecl::Instruction& ins){
        const auto& words=ins.rawWords;
        return ins.time==0&&ins.opcode==11&&ins.parameterCount==1&&ins.parameterMask==0
            &&words.size()==4&&words[0]==12&&words[1]==0x73736f42u&&words[2]==0x64726143u; // BossCard
    };
    if(!card(a)||!card(b)||!card(c))return;
    const auto number=a.rawWords[3];
    if(number<'1'||number>'5'||b.rawWords[3]!=(number|('B'<<8))||c.rawWords[3]!=(number|('C'<<8)))return;
    b.time=1;c.time=1;
}
}
inline bool prepareOwnedSection(const std::unordered_map<std::string,ecl::Program>& sources,
        unsigned stageId,unsigned section,unsigned phase,bool dialogue,ecl::Program& rootOutput,
        std::unordered_map<std::string,ecl::Program>& sourceOutput,std::string& error){
    if(section<1||section>156||section_data::stage[section]!=stageId
        ||phase>(section==156?3u:0u)){error="TH128 section/stage/phase mismatch";return false;}
    ChapterPlan rootPlan;if(!chapterPlan(stageId,1,rootPlan,error))return false;
    // Same complete fixed-source closure as chapter mode. This is a detached identity copy.
    ecl::Program verified;
    if(!prepareOwnedChapter(sources,stageId,1,verified,error))return false;
    auto candidate=sources;std::unordered_set<std::string> changed;
    for(const auto& row:section_data::rows){
        if(row.section!=section||row.dialogue!=unsigned(dialogue)||row.phase!=phase)continue;
        if(row.identity>=source_detail::identities.size()){error="section source table index";return false;}
        const auto& identity=source_detail::identities[row.identity];const std::string name(identity.name);
        const auto found=sources.find(name);if(found==sources.end())continue;
        std::string reason;const auto* actual=source_detail::identify(name,found->second,reason);
        if(actual!=&identity)continue;
        if(!changed.insert(name).second){error="duplicate section source plan";return false;}
        const auto total=std::size(section_data::words);
        if(row.begin>total||row.count>total-row.begin){error="section instruction table bounds";return false;}
        std::size_t cursor=row.begin,limit=row.begin+row.count;
        auto word=[&](){if(cursor>=limit)throw std::runtime_error("section instruction table truncated");return section_data::words[cursor++];};
        std::vector<ecl::Program::PracticeRegion> regions;
        try{
            const auto count=word();if(count>512)throw std::runtime_error("section region budget");
            for(unsigned n=0;n<count;++n){
                ecl::Program::PracticeRegion region;region.sourceOffset=word();region.endOffset=word();const auto instructions=word();
                if(instructions>ecl::Program::maxInstructions)throw std::runtime_error("section instruction budget");
                for(unsigned k=0;k<instructions;++k){
                    ecl::Instruction ins;ins.offset=word();ins.time=std::bit_cast<std::int32_t>(word());ins.opcode=std::uint16_t(word());ins.size=std::uint16_t(word());
                    ins.parameterMask=std::uint16_t(word());ins.rankMask=std::uint8_t(word());ins.parameterCount=std::uint8_t(word());ins.stackOffset=word();
                    const auto words=word();if(words>16379)throw std::runtime_error("section operand budget");
                    for(unsigned i=0;i<words;++i)ins.rawWords.push_back(word());region.instructions.push_back(std::move(ins));
                }
                regions.push_back(std::move(region));
            }
            if(cursor!=limit)throw std::runtime_error("section instruction trailing data");
        }catch(const std::exception& e){error=e.what();return false;}
        section_detail::alignDirectThreeBossCard(regions);
        ecl::Program prepared;
        if(!found->second.withPracticeRegions(found->second.resource().bytes,regions,prepared,error)){error=name+": "+error;return false;}
        candidate[name]=std::move(prepared);
    }
    if(!changed.contains(rootPlan.sourceName)){error="TH128 section has no matching original root plan";return false;}
    rootOutput=candidate.at(rootPlan.sourceName);sourceOutput=std::move(candidate);error.clear();return true;
}
}
