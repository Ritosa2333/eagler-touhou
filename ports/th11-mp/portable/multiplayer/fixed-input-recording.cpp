// Benchmark-only recording derived from an actual archive. It is NOT an
// authentic recording of the candidate binary. Re-encode via real native
// validation/writer; preserve every input bit and chapter. Runtime build
// identity checks remain enabled and no user's source Replay is overwritten.
#include "../../th11_web/cpp/multiplayer/ReplayArchive.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
using namespace th11::multiplayer;
static void require(bool ok){if(!ok){std::fputs("fixed-input recording validation failed\n",stderr);std::exit(1);}}
int main(int argc,char** argv){
 require(argc==7);auto* f=std::fopen(argv[1],"rb");require(f);require(std::fseek(f,0,SEEK_END)==0);
 const long size=std::ftell(f);require(size>0&&size<=64*1024*1024);std::rewind(f);
 std::vector<unsigned char> data(size);require(std::fread(data.data(),1,data.size(),f)==data.size());std::fclose(f);
 ReplayArchive source;require(source.Load(data.data(),data.size()));auto setup=source.Setup();
 for(unsigned i=0;i<4;++i){char* end=nullptr;const auto value=std::strtoul(argv[3+i],&end,16);require(end&&!*end);setup.build[i]=static_cast<unsigned>(value);}
 ReplayArchive candidate;require(candidate.Begin(setup,source.Timestamp()));unsigned chapter=0;
 for(unsigned frame=0;frame<source.FrameCount();++frame){
  if(chapter+1<source.Info().chapterCount&&frame==source.Info().chapters[chapter+1].firstFrame)++chapter;
  require(candidate.Append(frame,source.Info().chapters[chapter].label,source.FrameAt(frame)->data(),setup.player_count));
 }
 const bool cheat=(source.Info().config.description[28]&2)!=0;
 std::vector<unsigned char> encoded;require(candidate.Encode(encoded,"FIXEDCPU",source.Score(),source.Completed(),cheat));
 ReplayArchive verified;require(verified.Load(encoded.data(),encoded.size()));
 require(verified.FrameCount()==source.FrameCount()&&verified.Info().chapterCount==source.Info().chapterCount);
 for(unsigned i=0;i<source.Info().chapterCount;++i){require(verified.Info().chapters[i].label==source.Info().chapters[i].label);require(verified.Info().chapters[i].firstFrame==source.Info().chapters[i].firstFrame);}
 for(unsigned frame=0;frame<source.FrameCount();++frame)for(unsigned seat=0;seat<setup.player_count;++seat){
  const auto& a=(*source.FrameAt(frame))[seat];const auto& b=(*verified.FrameAt(frame))[seat];require(a==b);
  require(std::memcmp(&a.x,&b.x,4)==0&&std::memcmp(&a.y,&b.y,4)==0);
 }
 require((f=std::fopen(argv[2],"wb")));require(std::fwrite(encoded.data(),1,encoded.size(),f)==encoded.size());require(std::fclose(f)==0);
 std::printf("PASS derived benchmark recording: %u frames x %u seats, every button/analog bit and chapter retained\n",source.FrameCount(),setup.player_count);
}
