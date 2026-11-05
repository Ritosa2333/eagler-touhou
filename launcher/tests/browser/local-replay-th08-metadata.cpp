// Metadata/entry corpus encoded by TH08's real retail codec. The retained Demo
// input is not an authentic stage/spell performance or a golden trace.
#include "ReplayFile.hpp"
#include <fstream>
#include <iterator>
int main(int argc,char** argv){
    if(argc<3||argc>4)return 2;
    std::ifstream input(argv[1],std::ios::binary);std::vector<th08::u8> raw((std::istreambuf_iterator<char>(input)),{});
    th08::ReplayFile file;if(!file.decode(raw.data(),th08::u32(raw.size())))return 3;
    auto bytes=file.decoded();th08::ReplayMetadata meta;std::memcpy(&meta,bytes.data(),sizeof(meta));
    unsigned source=0;while(source<9&&!meta.header.stage_offsets[source])++source;if(source==9)return 4;
    const auto stage=meta.header.stage_offsets[source],timing=meta.header.timing_offsets[source];
    const std::string mode=argc==4?argv[3]:"spell";
    const auto block_end=[&](th08::u32 begin){th08::u32 end=sizeof(meta.header)+meta.header.payload_size;for(const auto* offsets:{meta.header.stage_offsets,meta.header.timing_offsets})for(unsigned i=0;i<9;i++)if(offsets[i]>begin)end=std::min(end,offsets[i]);return end;};
    const std::vector<th08::u8> input_block(bytes.begin()+stage,bytes.begin()+block_end(stage)),timing_block(bytes.begin()+timing,bytes.begin()+block_end(timing));
    for(auto& offset:meta.header.stage_offsets)offset=0;for(auto& offset:meta.header.timing_offsets)offset=0;
    meta.lag=1.23456f;std::memset(meta.player_name,0,8);std::memcpy(meta.player_name,"SC TEST",7);
    if(mode=="spell"){
        meta.header.stage_offsets[0]=stage;meta.header.timing_offsets[0]=timing;
        meta.spell_number=0;meta.spell_score=1234567;meta.difficulty=0;
        std::memset(meta.spell_name,0,48);meta.spell_name[0]=char(0x82);meta.spell_name[1]=char(0xa0);
    }else{
        if(mode!="branches"&&mode!="extra")return 7;
        meta.spell_number=-1;meta.difficulty=mode=="extra"?4:1;meta.spell_score=8765432;
        bytes.resize(sizeof(meta));
        for(unsigned id:(mode=="extra"?std::vector<unsigned>{8}:std::vector<unsigned>{0,1,2,3,4,5,6,7})){
            meta.header.stage_offsets[id]=bytes.size();bytes.insert(bytes.end(),input_block.begin(),input_block.end());
            const th08::u32 score=(id+1)*1111111;std::memcpy(bytes.data()+meta.header.stage_offsets[id],&score,sizeof(score));
            meta.header.timing_offsets[id]=bytes.size();bytes.insert(bytes.end(),timing_block.begin(),timing_block.end());
        }
        meta.header.payload_size=bytes.size()-sizeof(meta.header);
    }
    std::memcpy(bytes.data(),&meta,sizeof(meta));
    th08::ReplayFile encoded;if(!encoded.assign_decoded(bytes.data(),th08::u32(bytes.size())))return 5;
    const auto result=encoded.encode();std::ofstream output(argv[2],std::ios::binary);output.write(reinterpret_cast<const char*>(result.data()),result.size());return output?0:6;
}
