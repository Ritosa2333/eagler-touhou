#pragma once
#include "Thprac128Chapter.hpp"
#include "game/NativeOrdinaryReplaySave.hpp"
#include <functional>
#include <unordered_map>
namespace th128::practice {
namespace source_detail {
struct Identity {std::string_view name;std::size_t bytes;std::string_view sha256;unsigned branchShift;};
// Original DAT and existing validated Chinese language package only.
// Shift belongs to these exact bytes, not to arbitrary translations.
inline constexpr std::array<Identity,55> identities{{
    {"default.ecl",6900,"e98f8b7209f90e24ac816995177ee5820bf33b99355337531fb79162511041a5",0},
    {"st_a1_1.ecl",34116,"181aa57ef1581ef19340b283550dd140982ea0b1c12290a5f8957eec2a3f7b35",0},
    {"st_a1_1b.ecl",12008,"b485f7afd48f48df5c39f921cb3b39686e9bf5c398e2dbf95ad0efd607cf98b4",0},
    {"st_a1_1b.ecl",12020,"b5329ac044f369bfa0d0ce923b9552629aabe55bdcb9bd16eb996156cc284b3d",0},
    {"st_a1_2.ecl",35148,"30beaf3d3494aa36a5d93941f12bc90bc54f908b15b95026e0b4f063678de339",0},
    {"st_a1_2b.ecl",13212,"7ee4b122acbc9ba386f3ed3bbefc17625bc3e95cb7f941b254a5225297590c63",0},
    {"st_a1_2b.ecl",13232,"0e46322c13eb95b871f9dcc5678750484d58a5e73d8931f659625b071b3ecd87",0},
    {"st_a1_3.ecl",34640,"cc8d94a3f4be66e537d26e511ecf11e25861708696f6d9b6540b7cfa73cce69b",0},
    {"st_a1_3.ecl",34644,"5052aad65645db6c4436c1a3f380795f245be18043aa7591e91d6ef54bf0b13e",4},
    {"st_a1_3b.ecl",71276,"94272483f2eb09ee92cfe4f10e076a6d3066f99858ba8e25bfd70c1a2df39edc",0},
    {"st_a1_3b.ecl",71284,"2346a29c7579d780102254acec3ffce06869970e3c061a41197e283db49012b3",0},
    {"st_a2_2.ecl",38688,"af2e963cb2cf85eb40160fec2a2f05cc38837ac286681594f1c6b66b52f26e83",0},
    {"st_a2_2b.ecl",17456,"cffe0c7b5ad9908234aef961db043afc0dd520dc31994d82e74588b55814b792",0},
    {"st_a2_2b.ecl",17468,"4b2b7a64ab8f8f42d72227018e389c636de30307fef05e7a3c96e752da797dfa",0},
    {"st_a2_3.ecl",32760,"f628914a9bc8a1bf4d5ed2ec0feaadde83cbc462f820e1cce414ff377ddc9c1d",0},
    {"st_a2_3.ecl",32768,"f73497f7b266b18538eb7632e2a1386a0b3ade1600bf628b7a3954f214c2640d",8},
    {"st_a2_3b.ecl",69444,"b40ec8b16a2c332580f75d7b42b17279fd14fbd88ba245269f821683a62f965e",0},
    {"st_a2_3b.ecl",69456,"0d8273668447a1c949f35876f8ad7979583fe9017e001322d941c3f25d55d0a8",0},
    {"st_b1_1.ecl",32700,"9fe6fcdd4426c900ccf186e7a3c767f7fb0a48a14284775721100352b53f45ce",0},
    {"st_b1_1b.ecl",12632,"60605cc6561f1b29f0c216735896db618fe25f4a499bae0302db08e3902ee2ac",0},
    {"st_b1_1b.ecl",12644,"6ec2ef6cc98f005c1a86144a9090f81a2d2f248da1a6ec66c423ecef9e9cefc1",0},
    {"st_b1_2.ecl",40380,"3904696cf5e53a33720cfc541badda3bf81d6c679276b97cec716626381bcfbd",0},
    {"st_b1_2b.ecl",14828,"a3115597ff5478b013042e2afe8f32860cb560cfa230b5f4b28a424d022718f2",0},
    {"st_b1_2b.ecl",14840,"5a74e58a1076e5eaf0235d6e7487a584807bd0769872e32b7baf7b6237d93bcd",0},
    {"st_b1_3.ecl",32748,"feca2a17f81b637bf4bef0f6215df171dbef7441e3804377f95198c94e565189",0},
    {"st_b1_3.ecl",32752,"69bd30bffdaa478daf7440bad246013e4dab4ce3f478f59177325218476bc2be",4},
    {"st_b1_3b.ecl",72828,"609824770ebf44c16b17db121da86619dad73110df0006d3bc1207b0de4dbe32",0},
    {"st_b1_3b.ecl",72832,"5566d9616b6dd5cfcfd97a34f303111b164d6c85180df1f15bd2fb4093cdc901",0},
    {"st_b2_2.ecl",38688,"2f93d419bcb0db7a38d760a9432dcaa26164d17673936b2a2e2883cfbf3c6c00",0},
    {"st_b2_2b.ecl",15136,"783b96c40b88ba23d3d5ad13b6d0409e3fe81996663c09983abec773b5f78239",0},
    {"st_b2_2b.ecl",15148,"1c946080d3b77ecc3dce3611a256a7c2a1dc0860f5a09c363528b26b99c8116f",0},
    {"st_b2_3.ecl",32228,"17117422a719cfc52307e128d31a8afec529fc09e33330c1a9749f039dfcaeac",0},
    {"st_b2_3.ecl",32236,"fd115870262ec9129882507c3692c455e42c162cc706a0473b08679cd27940fa",8},
    {"st_b2_3b.ecl",69412,"02d4746ac09f07ba157fae4a1de82be50c149d14d538b9b8be4371e09f5ebbcd",0},
    {"st_b2_3b.ecl",69412,"31db156ad978d779bc34b1550866a99760f57ea330a887d9bc14a87b2e9d9321",0},
    {"st_c1_1.ecl",38632,"0850280c0a38e5c97dab13eddae695257c60ac6ce840322e88e05bdc38dfa060",0},
    {"st_c1_1b.ecl",9880,"8753cfd5052480f2e55a41391e71543c8f227fc77c91c46aab2cdc7c7e2d428c",0},
    {"st_c1_1b.ecl",9884,"9949d7dc7ecae560deae643ed2964fa293f91f82a77de6a0d2e61122fc72e273",0},
    {"st_c1_2.ecl",40380,"aa68ce64ac2dc439c135c5fb19c49e8075a23aef6429c891c4516b0cfb173de5",0},
    {"st_c1_2b.ecl",16112,"e8e81271b527f2c179f8c82155c384217cd19de3f3d806f524d97ee6f778c81c",0},
    {"st_c1_2b.ecl",16116,"175c5befb626f98fe1c2de0509b4fa0b9875b615d8279a0a25ab1fbb5a434df2",0},
    {"st_c1_3.ecl",34588,"b2dfead6b2875f3cb371b03d2268abfb785a323dade9406c99112a4cae902a24",0},
    {"st_c1_3.ecl",34592,"fadaf13ab74150a9d6b4c1b978f3e594d95e9b35a20281509a94afc45ad93016",4},
    {"st_c1_3b.ecl",70416,"626f29474bb776127c3608f436abe0ed7f6e41984b94ffa2e8a870ce590cf88c",0},
    {"st_c1_3b.ecl",70428,"f1cfd8fec2bf4fe1d482865b4544d8a5997070101e18dfe4dd96fd99115e2bbe",0},
    {"st_c2_2.ecl",35148,"336b4a6685f076649ddf8fa0ca2f09c90cc6e04389078c8de055b95b384cac21",0},
    {"st_c2_2b.ecl",17224,"9654c729721257c6291c648fe843d125243df5c4aea5e238a3f88413ab11cd6b",0},
    {"st_c2_2b.ecl",17240,"32e90b83a4af6a08cbfb1dc7cf897e6aac12179d8bdfdabec951e675400d8b0a",0},
    {"st_c2_3.ecl",32512,"0441e48aef575698ad9af67c47766653421d792670e9b8d25ff836f6da9340f8",0},
    {"st_c2_3.ecl",32516,"d88f8928ec98a7953000557143657a20cd4ec1b54c258972508ec6309e80dfaf",4},
    {"st_c2_3b.ecl",69548,"4c846e21dce7c5accd9a109a5129aec26a79222f9c2afabbe8bf0d9324398092",0},
    {"st_c2_3b.ecl",69548,"5d09702e6f24df32989caa1344f9a76585687ed4c01709096b2d5a5f0cd3c968",0},
    {"st_ex.ecl",55916,"5c65757c0ce6ee28f5f93b99462d087b5cd25c257c541af7bd7b307bdfa08ba5",0},
    {"st_exb.ecl",70688,"1fd96711121617a2ed73184681e692fef5dee9f6a73aed3fe730bd595f9adb90",0},
    {"st_exb.ecl",70700,"a0375e74bc4627a3b20fc923238033e4fbb79e21a13c7b78f65e93aec0f996e8",0},
}};
inline const Identity* identify(std::string_view name,const ecl::Program& program,std::string& error){
    const auto& bytes=program.resource().bytes;
    const auto digest=postrun::ordinaryReplaySHA256(bytes);
    constexpr char hex[]="0123456789abcdef";
    for(const auto& row:identities){
        if(row.name!=name||row.bytes!=bytes.size())continue;
        bool matches=true;
        for(std::size_t i=0;i<digest.size();++i)
            if(hex[digest[i]>>4]!=row.sha256[i*2]||hex[digest[i]&15]!=row.sha256[i*2+1]){matches=false;break;}
        if(matches)return &row;
    }
    error="TH128 thprac unsupported resource name/size/SHA256: "+std::string(name);return nullptr;
}
}
// Verifies the actual reachable sources, then overlays the owned root atomically.
// Full SHA identifies supported bytes; it does not authenticate a publisher.
inline bool prepareOwnedChapter(const std::unordered_map<std::string,ecl::Program>& sources,
        unsigned stageId,unsigned chapter,ecl::Program& output,std::string& error){
    ChapterPlan plan;
    if(!chapterPlan(stageId,chapter,plan,error))return false;
    std::unordered_map<std::string,unsigned> visited;
    const source_detail::Identity* rootIdentity=nullptr;
    std::function<bool(const std::string&,unsigned)> visit=[&](const std::string& name,unsigned depth){
        if(depth>64||visited.size()>64){error="TH128 thprac include ownership budget exceeded";return false;}
        const auto state=visited.find(name);
        if(state!=visited.end()){
            if(state->second==1){error="TH128 thprac include cycle";return false;}
            return true;
        }
        const auto found=sources.find(name);
        if(found==sources.end()){error="TH128 thprac missing include resource: "+name;return false;}
        const auto* identity=source_detail::identify(name,found->second,error);
        if(!identity)return false;
        if(name==plan.sourceName)rootIdentity=identity;
        visited[name]=1;
        for(const auto& include:found->second.resource().eclIncludes)
            if(!visit(include,depth+1))return false;
        visited[name]=2;return true;
    };
    if(!visit(plan.sourceName,0))return false;
    const auto& root=sources.at(plan.sourceName);
    for(auto& branch:plan.branches){
        branch.sourceOffset+=rootIdentity->branchShift;
        branch.targetOffset+=rootIdentity->branchShift;
    }
    return root.withPracticeBranches(root.resource().bytes,plan.branches,output,error);
}
}
