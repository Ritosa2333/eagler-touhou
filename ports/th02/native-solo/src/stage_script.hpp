#pragma once
#include <array>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>
namespace soew {
// Structural translation of ReC98/th02_main.asm:25610..25855 only.
// Opaque template/path bytes are NOT an implemented enemy behavior interpreter.
struct StageScript {
    struct Path {std::size_t file_offset=0;std::vector<std::uint8_t> bytes;};
    struct EnemyTemplate {
        std::size_t file_offset=0;
        std::array<std::uint8_t,28> raw{};
        std::uint8_t path_slot=0;
        bool path_reference=false;
        std::uint8_t ignored_reference_byte=0;
    };
    struct Formation {
        std::uint16_t trigger=0;
        std::array<std::uint8_t,48> selectors{}; // 0xff = no spawn, source order preserved
    };
    std::vector<std::uint8_t> map;
    std::vector<EnemyTemplate> enemies;
    std::vector<Path> paths;
    std::vector<Formation> formations;
    std::uint16_t declared_formations=0;
    std::size_t timeline_offset=0,trailing_bytes=0;
    bool complete()const{return formations.size()==declared_formations;}
    bool known_stage4_short_timeline=false;
    bool stage4_compatibility=false;
    void enable_stage4_compatibility(){
        if(!known_stage4_short_timeline)throw std::runtime_error("Stage 4 compatibility requires the verified original DT1");
        stage4_compatibility=true;
    }
    bool has_missing_formations(std::size_t cursor)const{
        return !stage4_compatibility&&cursor<declared_formations;
    }
    static StageScript decode(std::span<const std::byte> file,bool allow_incomplete_timeline=false){
        std::size_t cursor=0;
        const auto need=[&](std::size_t count){if(cursor>file.size()||count>file.size()-cursor)throw std::runtime_error("Truncated DT1 structure");};
        const auto u8=[&](){need(1);return std::to_integer<std::uint8_t>(file[cursor++]);};
        const auto u16=[&](){const unsigned lo=u8(),hi=u8();return static_cast<std::uint16_t>(lo|(hi<<8));};
        if(file.size()<6||file.size()>65539)throw std::runtime_error("Unsupported DT1 size");
        const auto payload=u16();const auto high=u16();if(high||payload!=file.size()-4)throw std::runtime_error("DT1 payload length mismatch");
        StageScript out;const auto map_count=u16();if(map_count==0||map_count>320)throw std::runtime_error("DT1 map capacity exceeded");
        for(unsigned i=0;i<map_count;++i){const auto section=u8();if(section>=16)throw std::runtime_error("DT1 section index out of range");out.map.push_back(section);}
        const auto templates=u16();if(templates>255)throw std::runtime_error("Unsupported DT1 template count");
        for(unsigned i=0;i<templates;++i){
            need(29);EnemyTemplate enemy;enemy.file_offset=cursor;for(auto &v:enemy.raw)v=u8();
            const auto first=u8();
            if(first==0xfe){
                enemy.path_reference=true;enemy.path_slot=u8();enemy.ignored_reference_byte=u8();
                // Original references a 64-byte path slot, NOT a template ID.
                if(enemy.path_slot>=out.paths.size())throw std::runtime_error("Uninitialized DT1 path reference");
            }else{
                if(out.paths.size()>=256)throw std::runtime_error("DT1 path slot overflow");
                enemy.path_slot=static_cast<std::uint8_t>(out.paths.size());Path path;path.file_offset=cursor-1;path.bytes.push_back(first);
                // Original copies first byte before checking NEXT byte for FF.
                for(;;){const auto next=u8();if(next==0xff)break;if(path.bytes.size()==64)throw std::runtime_error("DT1 path exceeds original slot");path.bytes.push_back(next);}
                out.paths.push_back(std::move(path));
            }
            out.enemies.push_back(enemy);
        }
        out.declared_formations=u16();out.timeline_offset=cursor;
        const auto available=(file.size()-cursor)/50;
        const auto rows=std::min<std::size_t>(available,out.declared_formations);
        if(rows<out.declared_formations&&!allow_incomplete_timeline)throw std::runtime_error("Incomplete DT1 timeline (no synthesized rows)");
        for(std::size_t i=0;i<rows;++i){Formation row;row.trigger=u16();for(auto &id:row.selectors){id=u8();if(id!=0xff&&id>=out.enemies.size())throw std::runtime_error("DT1 formation template index out of range");}out.formations.push_back(row);}
        out.trailing_bytes=file.size()-cursor;
        if(out.trailing_bytes)throw std::runtime_error("Unclassified DT1 trailing/partial timeline bytes");
        if(file.size()==14025&&out.declared_formations==270&&rows==266){
            std::uint64_t fingerprint=14695981039346656037ull;
            for(auto byte:file){fingerprint^=std::to_integer<unsigned>(byte);fingerprint*=1099511628211ull;}
            out.known_stage4_short_timeline=fingerprint==0x540408fc1652f07eull;
        }
        return out;
    }
};
}
