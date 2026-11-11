#include "SpellCatalog.hpp"
#include "EclResource.hpp"
#include "TitleSpellPracticeMenu.hpp"

#include <algorithm>
#include <cstdio>
#include <set>

namespace th13 {
namespace {
// VA 004bbd30, used by 00412970/00456b50. This is the existing score-slot
// rank contract, shared by storage and the runtime metadata parser.
constexpr std::array<u8, 127> kDifficulties = {
    0,1,2,3, 0,1,2,3, 0,1,2,3, 2,3,
    0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 2,3,
    0,1,2,3, 0,1,2,3, 0,1,2,3, 1,2,3,
    0,1,2,3, 0,1,2,3, 0,1,2,3, 1,2,3,
    0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3,
    0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3, 0,1,2,3,
    4,4,4,4,4,4,4,4,4,4,4,4,4,
    5,5,5,5,5,5,5,5
};

bool is_spell(u16 opcode) {
    return opcode == 522 || opcode == 528 || opcode == 537 ||
           opcode == 538 || opcode == 539;
}

bool decode_name(const EclInstruction& instruction, std::string& out) {
    if (instruction.length < 36) return false;
    const u32 size = instruction.argument<u32>(3);
    if (size > u32(instruction.length - 32)) return false;
    const u8* bytes = reinterpret_cast<const u8*>(&instruction) + 32;
    u8 key = 0x77, increment = 7;
    out.clear();
    for (u32 i = 0; i < size; ++i) {
        const u8 value = bytes[i] ^ key;
        if (value == 0) return true;
        out.push_back(char(value));
        key = u8(key + increment);
        increment = u8(increment + 0x10);
    }
    return false;
}

i32 first_id(const std::array<SpellCatalogVariant, 5>& family) {
    i32 result = 127;
    for (const auto& variant : family)
        if (variant.id >= 0) result = std::min(result, variant.id);
    return result;
}
} // namespace

i32 SpellCatalog::difficulty(i32 id) {
    return id >= 0 && id < i32(kDifficulties.size()) ? kDifficulties[size_t(id)] : -1;
}

bool SpellCatalog::load(const Reader& reader) {
    SpellCatalog next;
    for (i32 stage = 1; stage <= 7; ++stage) {
        if (!next.load_stage(stage, reader)) {
            error = next.error;
            return false;
        }
    }
    *this = std::move(next);
    return true;
}

bool SpellCatalog::load_stage(i32 stage, const Reader& reader) {
    error.clear();
    if (stage < 1 || stage > 7 || !reader) {
        error = "Invalid spell catalog stage or reader";
        return false;
    }
    std::vector<Family> families;
    std::set<std::string> visited;
    std::function<bool(const std::string&)> scan = [&](const std::string& name) {
        if (visited.count(name)) return true;
        if (visited.size() >= 32) {
            error = "Spell catalog ECL include limit exceeded";
            return false;
        }
        visited.insert(name);
        std::vector<u8> bytes;
        EclResource resource;
        if (!reader(name, bytes) || !resource.open(bytes.data(), u32(bytes.size()))) {
            error = "Spell catalog ECL unavailable: " + name;
            return false;
        }
        for (const auto& sub : resource.subroutines) {
            Family family{};
            for (u32 at = sub.offset + 16; at < sub.offset + sub.size;) {
                const auto& instruction = *reinterpret_cast<const EclInstruction*>(resource.bytes.data() + at);
                at += instruction.length;
                if (!is_spell(instruction.opcode)) continue;
                if (instruction.length < 36 || (instruction.parameter_mask & 1u)) {
                    error = "Spell catalog requires a literal spell id: " + name;
                    return false;
                }
                std::string spell_name;
                if (!decode_name(instruction, spell_name)) {
                    error = "Spell catalog name outside ECL instruction: " + name;
                    return false;
                }
                const i32 base = instruction.argument<i32>();
                for (i32 rank = 0; rank < 6; ++rank) {
                    if (!(instruction.rank_mask & (1u << rank))) continue;
                    i32 id = base;
                    if (instruction.opcode >= 537) {
                        if (rank >= 4) continue;
                        id += rank - (instruction.opcode - 537);
                    }
                    // A subroutine may be invoked for only some ranks even if its
                    // instructions use mask 0xff. The intrinsic table excludes
                    // those unreachable combinations (e.g. stage3 midboss E/N).
                    if (difficulty(id) != rank) continue;
                    const i32 row = stage == 7 ? (rank == 4 ? 0 : 1) : (rank == 5 ? 4 : rank);
                    if (row < 0 || row >= 5 || (stage != 7 && rank == 4)) continue;
                    auto& entry = family[size_t(row)];
                    if (entry.id >= 0 && (entry.id != id || entry.name != spell_name)) {
                        error = "Conflicting spell variants in ECL subroutine: " + sub.name;
                        return false;
                    }
                    entry = {id, rank, spell_name};
                }
            }
            if (first_id(family) < 127) families.push_back(std::move(family));
        }
        for (const auto& include : resource.includes)
            if (!scan(include)) return false;
        return true;
    };
    char name[20];
    std::snprintf(name, sizeof name, "st%02d.ecl", stage);
    if (!scan(name)) return false;
    std::sort(families.begin(), families.end(), [](const Family& left, const Family& right) {
        return first_id(left) < first_id(right);
    });
    if (families.size() != size_t(kSpellPracticeSlots[stage - 1])) {
        error = "Spell catalog family count does not match stage";
        return false;
    }
    std::set<i32> ids;
    for (const auto& family : families) {
        for (const auto& entry : family) {
            if (entry.id >= 0 && !ids.insert(entry.id).second) {
                error = "Duplicate spell catalog id";
                return false;
            }
        }
    }
    stages_[size_t(stage - 1)] = std::move(families);
    return true;
}

const SpellCatalog::Family* SpellCatalog::family(i32 stage, i32 slot) const {
    if (stage < 1 || stage > 7 || slot < 0) return nullptr;
    const auto& families = stages_[size_t(stage - 1)];
    return slot < i32(families.size()) ? &families[size_t(slot)] : nullptr;
}

i32 SpellCatalog::spell_count(i32 stage) const {
    return stage >= 1 && stage <= 7 ? i32(stages_[size_t(stage - 1)].size()) : 0;
}

const SpellCatalogVariant* SpellCatalog::variant(i32 stage, i32 slot, i32 rank) const {
    const auto* entries = family(stage, slot);
    if (!entries || rank < 0 || rank >= 5 || (*entries)[size_t(rank)].id < 0) return nullptr;
    return &(*entries)[size_t(rank)];
}

bool SpellCatalog::spell_unlocked(i32 stage, i32 slot, const Check& encountered) const {
    const auto* entries = family(stage, slot);
    if (!entries || !encountered) return false;
    for (const auto& entry : *entries)
        if (entry.id >= 0 && encountered(entry.id)) return true;
    return false;
}

bool SpellCatalog::variant_available(i32 stage, i32 slot, i32 rank,
                                    const Check& encountered, const Check& captured) const {
    const auto* entry = variant(stage, slot, rank);
    if (!entry || !encountered) return false;
    if (encountered(entry->id)) return true;
    if (entry->rank != 5 || !captured) return false;
    // 00456b50: Overdrive also opens after every preceding ordinary/Extra
    // variant has a capture in either game mode or spell practice.
    bool preceding = false;
    for (const auto& other : *family(stage, slot)) {
        if (other.id < 0 || other.rank >= 5) continue;
        preceding = true;
        if (!captured(other.id)) return false;
    }
    return preceding;
}

std::string SpellCatalog::spell_name(i32 stage, i32 slot, const Check& encountered) const {
    const auto* entries = family(stage, slot);
    if (!entries || !encountered) return {};
    for (const auto& entry : *entries)
        if (entry.id >= 0 && encountered(entry.id)) return entry.name;
    return {};
}

} // namespace th13
