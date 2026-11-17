#pragma once
#include "Thprac128Chapter.hpp"
#include "game/NativeOrdinaryReplaySave.hpp"
namespace th128::practice {
// Exact original ECL bytes from independently pinned DAT inventory864.
// Supported resource consistency, not publisher identity authentication.
inline bool originalChapterSource(unsigned stageId,const ecl::Program& program,std::string& error) {
    static constexpr std::array<std::string_view,16> hashes{{
        "181aa57ef1581ef19340b283550dd140982ea0b1c12290a5f8957eec2a3f7b35",
        "30beaf3d3494aa36a5d93941f12bc90bc54f908b15b95026e0b4f063678de339",
        "cc8d94a3f4be66e537d26e511ecf11e25861708696f6d9b6540b7cfa73cce69b",
        "af2e963cb2cf85eb40160fec2a2f05cc38837ac286681594f1c6b66b52f26e83",
        "f628914a9bc8a1bf4d5ed2ec0feaadde83cbc462f820e1cce414ff377ddc9c1d",
        "9fe6fcdd4426c900ccf186e7a3c767f7fb0a48a14284775721100352b53f45ce",
        "3904696cf5e53a33720cfc541badda3bf81d6c679276b97cec716626381bcfbd",
        "feca2a17f81b637bf4bef0f6215df171dbef7441e3804377f95198c94e565189",
        "2f93d419bcb0db7a38d760a9432dcaa26164d17673936b2a2e2883cfbf3c6c00",
        "17117422a719cfc52307e128d31a8afec529fc09e33330c1a9749f039dfcaeac",
        "0850280c0a38e5c97dab13eddae695257c60ac6ce840322e88e05bdc38dfa060",
        "aa68ce64ac2dc439c135c5fb19c49e8075a23aef6429c891c4516b0cfb173de5",
        "b2dfead6b2875f3cb371b03d2268abfb785a323dade9406c99112a4cae902a24",
        "336b4a6685f076649ddf8fa0ca2f09c90cc6e04389078c8de055b95b384cac21",
        "0441e48aef575698ad9af67c47766653421d792670e9b8d25ff836f6da9340f8",
        "5c65757c0ce6ee28f5f93b99462d087b5cd25c257c541af7bd7b307bdfa08ba5",
    }};
    ChapterPlan plan;
    if(!chapterPlan(stageId,1,plan,error))return false;
    const auto& bytes=program.resource().bytes;
    if(bytes.size()!=plan.sourceBytes){error="TH128 thprac original source size mismatch";return false;}
    const auto digest=postrun::ordinaryReplaySHA256(bytes);
    constexpr char hex[]="0123456789abcdef";
    const auto expected=hashes[stageId-1];
    for(std::size_t i=0;i<digest.size();++i)
        if(hex[digest[i]>>4]!=expected[i*2]||hex[digest[i]&15]!=expected[i*2+1]){
            error="TH128 thprac original source SHA256 mismatch";return false;
        }
    error.clear();return true;
}
}
