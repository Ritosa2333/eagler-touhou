#pragma once
// Chapter jump plans adapted from thprac TH128, copyright (c) 2022 Ack.
// MIT license retained in licenses/THPRAC-MIT.txt.
// Fixed source: b5beb37a91e322d985c8f89bb6377d55bcf07734, THStageWarp.
// Asset identities/hashes are checked by the owning resource provider before use.
#include "ecl/EclProgram.hpp"
#include <array>
#include <string>
#include <string_view>
#include <vector>
namespace th128::practice {
struct ChapterPlan {
    unsigned stageId = 0;
    unsigned chapter = 0;
    unsigned section = 0;
    std::string sourceName;
    std::size_t sourceBytes = 0;
    std::vector<ecl::Program::PracticeBranch> branches;
};
namespace chapter_detail {
struct Entry { unsigned stage, chapter; std::vector<ecl::Program::PracticeBranch> branches; };
inline const std::array<Entry,126> entries{{
    {1, 1, { }},
    {1, 2, {{0x81d8, 0x8298, 90, 0} }},
    {1, 3, {{0x81d8, 0x82cc, 90, 0} }},
    {1, 4, {{0x81d8, 0x8300, 90, 0} }},
    {1, 5, {{0x81d8, 0x8334, 90, 0} }},
    {1, 6, {{0x81d8, 0x83d0, 90, 0} }},
    {1, 7, {{0x81d8, 0x8404, 90, 0} }},
    {2, 1, { }},
    {2, 2, {{0x8680, 0x877c, 90, 0} }},
    {2, 3, {{0x8680, 0x87b0, 90, 0} }},
    {2, 4, {{0x8680, 0x87e4, 90, 0} }},
    {2, 5, {{0x8680, 0x8838, 90, 0} }},
    {2, 6, {{0x8680, 0x88c0, 90, 0} }},
    {2, 7, {{0x8680, 0x88c0, 90, 0}, {0x615c, 0x6190, 0, 0} }},
    {2, 8, {{0x8680, 0x88c0, 90, 0}, {0x615c, 0x61c4, 0, 0} }},
    {2, 9, {{0x8680, 0x88c0, 90, 0}, {0x615c, 0x61f8, 0, 0} }},
    {3, 1, { }},
    {3, 2, {{0x84b8, 0x85b4, 90, 0} }},
    {3, 3, {{0x84b8, 0x85e8, 90, 0} }},
    {3, 4, {{0x84b8, 0x861c, 90, 0} }},
    {3, 5, {{0x84b8, 0x86c4, 90, 0} }},
    {3, 6, {{0x84b8, 0x86c4, 90, 0}, {0x5528, 0x555c, 0, 0} }},
    {3, 7, {{0x84b8, 0x86c4, 90, 0}, {0x5528, 0x5590, 0, 0} }},
    {4, 1, { }},
    {4, 2, {{0x94a8, 0x95a4, 90, 0} }},
    {4, 3, {{0x94a8, 0x95d8, 90, 0} }},
    {4, 4, {{0x94a8, 0x960c, 90, 0} }},
    {4, 5, {{0x94a8, 0x9694, 90, 0} }},
    {4, 6, {{0x94a8, 0x9694, 90, 0}, {0x6d84, 0x6db8, 0, 0} }},
    {4, 7, {{0x94a8, 0x9694, 90, 0}, {0x6d84, 0x6dec, 0, 0} }},
    {5, 1, { }},
    {5, 2, {{0x7ce4, 0x7de0, 90, 0} }},
    {5, 3, {{0x7ce4, 0x7e14, 90, 0} }},
    {5, 4, {{0x7ce4, 0x7e48, 90, 0} }},
    {5, 5, {{0x7ce4, 0x7e9c, 90, 0} }},
    {5, 6, {{0x7ce4, 0x7f6c, 90, 0} }},
    {5, 7, {{0x7ce4, 0x7f6c, 90, 0}, {0x5408, 0x543c, 0, 0} }},
    {5, 8, {{0x7ce4, 0x7f6c, 90, 0}, {0x5408, 0x5470, 0, 0} }},
    {5, 9, {{0x7ce4, 0x7f6c, 90, 0}, {0x5408, 0x54a4, 0, 0} }},
    {6, 1, { }},
    {6, 2, {{0x7d10, 0x7df8, 90, 0} }},
    {6, 3, {{0x7d10, 0x7e2c, 90, 0} }},
    {6, 4, {{0x7d10, 0x7e60, 90, 0} }},
    {6, 5, {{0x7d10, 0x7e94, 90, 0} }},
    {6, 6, {{0x7d10, 0x7f30, 90, 0} }},
    {6, 7, {{0x7d10, 0x7f30, 90, 0}, {0x5ac4, 0x5ae4, 0, 0} }},
    {7, 1, { }},
    {7, 2, {{0x9afc, 0x9bf8, 90, 0} }},
    {7, 3, {{0x9afc, 0x9c2c, 90, 0} }},
    {7, 4, {{0x9afc, 0x9c60, 90, 0} }},
    {7, 5, {{0x9afc, 0x9c94, 90, 0} }},
    {7, 6, {{0x9afc, 0x9d30, 90, 0} }},
    {7, 7, {{0x9afc, 0x9d30, 90, 0}, {0x7654, 0x7688, 0, 0} }},
    {7, 8, {{0x9afc, 0x9d30, 90, 0}, {0x7654, 0x76bc, 0, 0} }},
    {8, 1, { }},
    {8, 2, {{0x7d54, 0x7e50, 90, 0} }},
    {8, 3, {{0x7d54, 0x7e84, 90, 0} }},
    {8, 4, {{0x7d54, 0x7eb8, 90, 0} }},
    {8, 5, {{0x7d54, 0x7f60, 90, 0} }},
    {8, 6, {{0x7d54, 0x7f60, 90, 0}, {0x4dc4, 0x4df8, 0, 0} }},
    {8, 7, {{0x7d54, 0x7f60, 90, 0}, {0x4dc4, 0x4e2c, 0, 0} }},
    {9, 1, { }},
    {9, 2, {{0x94a8, 0x95a4, 90, 0} }},
    {9, 3, {{0x94a8, 0x95d8, 90, 0} }},
    {9, 4, {{0x94a8, 0x960c, 90, 0} }},
    {9, 5, {{0x94a8, 0x9694, 90, 0} }},
    {9, 6, {{0x94a8, 0x9694, 90, 0}, {0x6d84, 0x6db8, 0, 0} }},
    {9, 7, {{0x94a8, 0x9694, 90, 0}, {0x6d84, 0x6dec, 0, 0} }},
    {10, 1, { }},
    {10, 2, {{0x7b04, 0x7c00, 90, 0} }},
    {10, 3, {{0x7b04, 0x7c34, 90, 0} }},
    {10, 4, {{0x7b04, 0x7c68, 90, 0} }},
    {10, 5, {{0x7b04, 0x7c9c, 90, 0} }},
    {10, 6, {{0x7b04, 0x7d58, 90, 0} }},
    {10, 7, {{0x7b04, 0x7d58, 90, 0}, {0x4600, 0x4634, 0, 0} }},
    {10, 8, {{0x7b04, 0x7d58, 90, 0}, {0x4600, 0x4668, 0, 0} }},
    {11, 1, { }},
    {11, 2, {{0x9464, 0x954c, 90, 0} }},
    {11, 3, {{0x9464, 0x9580, 90, 0} }},
    {11, 4, {{0x9464, 0x95b4, 90, 0} }},
    {11, 5, {{0x9464, 0x9628, 90, 0} }},
    {11, 6, {{0x9464, 0x965c, 90, 0} }},
    {12, 1, { }},
    {12, 2, {{0x9afc, 0x9bf8, 90, 0} }},
    {12, 3, {{0x9afc, 0x9c2c, 90, 0} }},
    {12, 4, {{0x9afc, 0x9c60, 90, 0} }},
    {12, 5, {{0x9afc, 0x9c94, 90, 0} }},
    {12, 6, {{0x9afc, 0x9d30, 90, 0} }},
    {12, 7, {{0x9afc, 0x9d30, 90, 0}, {0x7654, 0x7688, 0, 0} }},
    {12, 8, {{0x9afc, 0x9d30, 90, 0}, {0x7654, 0x76bc, 0, 0} }},
    {13, 1, { }},
    {13, 2, {{0x8408, 0x8504, 90, 0} }},
    {13, 3, {{0x8408, 0x8538, 90, 0} }},
    {13, 4, {{0x8408, 0x856c, 90, 0} }},
    {13, 5, {{0x8408, 0x85c0, 90, 0} }},
    {13, 6, {{0x8408, 0x8690, 90, 0} }},
    {13, 7, {{0x8408, 0x8690, 90, 0}, {0x5b2c, 0x5b60, 0, 0} }},
    {13, 8, {{0x8408, 0x8690, 90, 0}, {0x5b2c, 0x5b94, 0, 0} }},
    {13, 9, {{0x8408, 0x8690, 90, 0}, {0x5b2c, 0x5bc8, 0, 0} }},
    {14, 1, { }},
    {14, 2, {{0x8680, 0x877c, 90, 0} }},
    {14, 3, {{0x8680, 0x87b0, 90, 0} }},
    {14, 4, {{0x8680, 0x87e4, 90, 0} }},
    {14, 5, {{0x8680, 0x8838, 90, 0} }},
    {14, 6, {{0x8680, 0x88c0, 90, 0} }},
    {14, 7, {{0x8680, 0x88c0, 90, 0}, {0x615c, 0x6190, 0, 0} }},
    {14, 8, {{0x8680, 0x88c0, 90, 0}, {0x615c, 0x61c4, 0, 0} }},
    {14, 9, {{0x8680, 0x88c0, 90, 0}, {0x615c, 0x61f8, 0, 0} }},
    {15, 1, { }},
    {15, 2, {{0x7c20, 0x7d1c, 90, 0} }},
    {15, 3, {{0x7c20, 0x7d50, 90, 0} }},
    {15, 4, {{0x7c20, 0x7d84, 90, 0} }},
    {15, 5, {{0x7c20, 0x7db8, 90, 0} }},
    {15, 6, {{0x7c20, 0x7e74, 90, 0} }},
    {15, 7, {{0x7c20, 0x7e74, 90, 0}, {0x471c, 0x4750, 0, 0} }},
    {15, 8, {{0x7c20, 0x7e74, 90, 0}, {0x471c, 0x4784, 0, 0} }},
    {16, 1, { }},
    {16, 2, {{0xd7b8, 0xd8a0, 90, 0} }},
    {16, 3, {{0xd7b8, 0xd8d4, 90, 0} }},
    {16, 4, {{0xd7b8, 0xd908, 90, 0} }},
    {16, 5, {{0xd7b8, 0xd93c, 90, 0} }},
    {16, 6, {{0xd7b8, 0xd9e0, 90, 0} }},
    {16, 7, {{0xd7b8, 0xd9e0, 90, 0}, {0xa99c, 0xa9d0, 0, 0} }},
    {16, 8, {{0xd7b8, 0xd9e0, 90, 0}, {0xa99c, 0xaa04, 0, 0} }},
    {16, 9, {{0xd7b8, 0xd9e0, 90, 0}, {0xa99c, 0xaa38, 0, 0} }},
    {16, 10, {{0xd7b8, 0xd9e0, 90, 0}, {0xa99c, 0xaa6c, 0, 0} }},
}};
struct Asset { const char* name; std::size_t bytes; };
inline constexpr std::array<Asset,16> assets{{
    {"st_a1_1.ecl", 34116},
    {"st_a1_2.ecl", 35148},
    {"st_a1_3.ecl", 34640},
    {"st_a2_2.ecl", 38688},
    {"st_a2_3.ecl", 32760},
    {"st_b1_1.ecl", 32700},
    {"st_b1_2.ecl", 40380},
    {"st_b1_3.ecl", 32748},
    {"st_b2_2.ecl", 38688},
    {"st_b2_3.ecl", 32228},
    {"st_c1_1.ecl", 38632},
    {"st_c1_2.ecl", 40380},
    {"st_c1_3.ecl", 34588},
    {"st_c2_2.ecl", 35148},
    {"st_c2_3.ecl", 32512},
    {"st_ex.ecl", 55916},
}};
}
inline bool chapterPlan(unsigned stageId, unsigned chapter, ChapterPlan& output, std::string& error) {
    if (stageId < 1 || stageId > chapter_detail::assets.size()) {
        error = "TH128 thprac stage is outside 1..16"; return false;
    }
    for (const auto& entry : chapter_detail::entries) {
        if (entry.stage != stageId || entry.chapter != chapter) continue;
        const auto& asset = chapter_detail::assets[stageId-1];
        ChapterPlan candidate{stageId,chapter,10000+stageId*100+chapter,asset.name,asset.bytes,entry.branches};
        output = std::move(candidate); error.clear(); return true;
    }
    error = "TH128 thprac chapter is unavailable for the stage"; return false;
}
inline bool prepareChapter(const ecl::Program& source, std::string_view resourceName,
        const std::vector<std::uint8_t>& expectedSource, unsigned stageId, unsigned chapter,
        ecl::Program& output, std::string& error) {
    ChapterPlan plan;
    if (!chapterPlan(stageId,chapter,plan,error)) return false;
    if (resourceName != plan.sourceName || expectedSource.size() != plan.sourceBytes) {
        error = "TH128 thprac original named resource/size mismatch"; return false;
    }
    return source.withPracticeBranches(expectedSource,plan.branches,output,error);
}
}
