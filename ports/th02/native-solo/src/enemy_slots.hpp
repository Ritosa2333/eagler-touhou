#pragma once
#include "core_contract.hpp"
#include "stage_script.hpp"
#include <array>
#include <cstdint>
#include <optional>
#include <stdexcept>
namespace soew {
// Exact 38-byte slot storage: th02_main.asm sub_16C96:16277..16372.
// Spawn alone is NOT an enemy AI implementation. Unwritten bytes stay intact
// on reuse, as in original; caller must perform source-backed invalidation.
class EnemySlots {
public:
    struct Slot {
        std::array<std::uint8_t,38> raw{};
        std::uint16_t word(unsigned offset)const{if(offset+1>=raw.size())throw std::runtime_error("Enemy word outside slot");return static_cast<std::uint16_t>(raw[offset]|(unsigned(raw[offset+1])<<8));}
        void word(unsigned offset,std::uint16_t value){if(offset+1>=raw.size())throw std::runtime_error("Enemy word outside slot");raw[offset]=static_cast<std::uint8_t>(value);raw[offset+1]=static_cast<std::uint8_t>(value>>8);}
    };
    std::array<Slot,25> slots{};
    std::array<unsigned,25> motion_generation{}; // Native presentation, not original slot bytes.
    unsigned high_water=0;
    void begin_frame(unsigned back_page){
        if(back_page>1)throw std::runtime_error("Invalid enemy page");
        high_water=0;const unsigned front=back_page^1;
        for(unsigned i=0;i<slots.size();++i){auto &s=slots[i];if(!s.raw[0xe])continue;
            if(s.raw[0xe]==2){s.raw[0xe]=0;continue;}
            high_water=i+1;s.word(back_page*4,s.word(front*4));s.word(back_page*4+2,s.word(front*4+2));
        }
    }
    std::optional<unsigned> spawn(const StageScript &script,unsigned template_id,unsigned lane,unsigned page,RandomRing &random){
        if(template_id>=script.enemies.size()||lane<1||lane>48||page>1)throw std::runtime_error("Invalid enemy spawn request");
        unsigned index=0;while(index<slots.size()&&slots[index].raw[0x0e]!=0)++index;
        if(index==slots.size())return {};
        ++motion_generation[index];
        auto &s=slots[index];const auto &enemy=script.enemies[template_id];
        const auto y=static_cast<std::uint16_t>(enemy.raw[2]|(unsigned(enemy.raw[3])<<8));
        const auto x=static_cast<std::uint16_t>(16+lane*8);
        s.raw[0x0e]=1;s.word(page*4,x);s.word(page*4+2,y);
        s.word(0x12,0);s.word(0x08,0);s.word(0x0c,static_cast<std::uint16_t>(template_id));s.word(0x0a,0);
        s.raw[0x0f]=random.next8();s.word(0x16,0);s.raw[0x22]=0;s.raw[0x23]=0;
        s.word(0x18,x<208?1:0);s.word(0x1a,0);s.raw[0x1e]=0;s.word(0x14,1);s.raw[0x10]=0;s.word(0x20,0);
        // Original uses strict '< slot', not '<= slot'. In particular slot0
        // alone does not raise a zero high-water mark on its creation tick.
        if(high_water<index)high_water=index+1;
        return index;
    }
};
}
