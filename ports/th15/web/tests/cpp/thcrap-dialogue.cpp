#include "../../cpp/game/Dialogue.hpp"
#include <cassert>
#include <cmath>
#include <fstream>
#include <iterator>
#include <iostream>
using namespace th15;
// Semantic lane only: synthetic font metrics, not a visual/font-parity claim.
struct Host:DialogueHost {
 u32 next=1,lines=0,rubies=0;
 bool create(i32,i32,u32& h)override{h=next++;return true;}
 bool interrupt(u32,i32,bool=false)override{return true;}
 bool retire(u32& h)override{h=0;return true;}
 bool text_initialize(u32)override{return true;}
 bool text(const DialogueText& t)override{if(t.offset_pixels){rubies++;assert(t.font==2);}else if(t.move)lines++;return true;}
 i32 text_extent(const std::string& s,i32)override{return i32(s.size()*17);}
 bool position(u32,const Vec3&)override{return true;}
 bool depth(u32,float)override{return true;}
 bool balloon_width(u32,i32,float width)override{assert(std::isfinite(width)&&width>=0&&width<10000);return true;}
 bool follow_balloon(DialogueState&)override{return true;}
 bool music(bool)override{return true;}bool fade_music(float)override{return true;}
 bool stage_complete()override{return true;}bool name_banner()override{return true;}
 bool sound(i32)override{return true;}bool clear_field()override{return true;}
};
int main(){
 u32 scripts=0,lines=0,rubies=0;
 for(const char* lang:{"lang_en","lang_zh-hans"})for(int stage=1;stage<=7;stage++)for(char character='a';character<='d';character++){
  char path[128];std::snprintf(path,sizeof path,"/messages/%s/st%02d%c.msg",lang,stage,character);
  std::ifstream f(path,std::ios::binary);assert(f.good());std::vector<u8> bytes{std::istreambuf_iterator<char>(f),{}};
  MessageProgram program;assert(program.open(bytes.data(),u32(bytes.size())));
  for(const auto& script:program.scripts){Host host;DialogueResources resources;resources.character=character-'a';Dialogue dialogue(script,host,resources);assert(dialogue.initialize());
   int status=0;for(int frame=0;frame<10000&&status>=0;frame++)status=dialogue.update({1000,1,1,30,0,stage});
   if(status!=-1){std::cerr<<path<<": "<<dialogue.error<<"\n";return 1;}scripts++;lines+=host.lines;rubies+=host.rubies;
  }
 }
 assert(scripts>=112&&lines>500&&rubies>0);std::cout<<"PASS scripts="<<scripts<<" lines="<<lines<<" ruby="<<rubies<<"\n";
}
