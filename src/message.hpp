// TH12 MSG control VM. Evidence: original 41fcc0 and 420802.
// Presentation commands are emitted; rendering does not advance these timers.
#pragma once
#include <cstdint>
#include <cstring>
#include <vector>
#include <array>
#include <string>
#include <utility>
#include <cstdlib>
namespace message_system {
using Byte=uint8_t;
template<class T>inline T read(const Byte* p){T v;std::memcpy(&v,p,sizeof v);return v;}
struct Entry {uint32_t offset=0,flags=0;};
struct File {
  std::vector<Byte> data;std::vector<Entry> entries;
  bool load(const Byte* p,int size){
    if(size<12)return false;const uint32_t count=read<uint32_t>(p);
    if(!count||count>256||count>uint32_t(size-4)/8)return false;
    std::vector<Entry> parsed;for(uint32_t i=0;i<count;i++){
      Entry entry{read<uint32_t>(p+4+i*8),read<uint32_t>(p+8+i*8)};
      if(entry.offset<4+count*8||entry.offset>uint32_t(size)-4)return false;
      const uint32_t end=i+1<count?read<uint32_t>(p+12+i*8):uint32_t(size);
      if(end<=entry.offset||end>uint32_t(size))return false;
      bool terminated=false;for(uint32_t at=entry.offset;at+4<=end;){const uint32_t length=4+p[at+3];if(length>end-at)return false;if(p[at+2]==0){terminated=true;break;}at+=length;}
      if(!terminated)return false;parsed.push_back(entry);
    }
    data.assign(p,p+size);entries=std::move(parsed);return true;
  }
};
inline std::string decrypt(const Byte* p,size_t size){std::string result;uint8_t key=0x77,step=7;for(size_t i=0;i<size;i++){const uint8_t c=p[i]^key;key+=step;step+=0x10;if(!c)break;result.push_back(char(c));}return result;}
struct Command {int op=0;const Byte* payload=nullptr;size_t size=0;int integer(size_t at=0)const{return at+4<=size?read<int32_t>(payload+at):0;}};
struct TextRecord {
  std::string bytes;int offset=0,spacing=0,style=0;uint32_t color=0xf8f08f;
};
struct TextAction {int slot=0,interrupt=0;TextRecord record;};
struct VM {
  const File* file=nullptr;uint32_t pc=0;int entry=-1,clock=0,previousClock=-1,wait=0,previousWait=-1,typingDelay=0;
  uint32_t flags=0;bool active=false,failed=false;
  // Native 94/98 track alternating modern text lines and initial clearing.
  int line=0,textInitialized=0,speaker=0,age=0;std::array<std::string,2> text{};
  // Four independent @R texture rectangles: normal lines0/1 and formatted
  // lines2/3. Native formatted op17 does not toggle +94 or clear +98.
  std::array<TextRecord,4> textRecords{};std::vector<TextAction> textActions;
  void writeText(int slot,std::string bytes,int offset,int spacing,int style,int interrupt){
    static constexpr uint32_t colors[3]={0xf8f08f,0x8088ff,0xd8d8d8};
    textRecords[slot]={std::move(bytes),offset,spacing,style,colors[speaker]};
    textActions.push_back({slot,interrupt,textRecords[slot]});
  }
  bool modernText(const Command& command){
    const int style=(flags>>1)&1;
    if(line==0&&!textInitialized){
      for(int slot=0;slot<4;slot++)writeText(slot," ",0,slot>=2?1:0,style,3);
      text={};textInitialized=1;
    }
    std::string bytes=decrypt(command.payload,command.size);text[line]=bytes;
    if(!bytes.empty()&&bytes[0]=='|'){
      const size_t first=bytes.find(',',1),second=first==std::string::npos?first:bytes.find(',',first+1);
      if(first==std::string::npos||second==std::string::npos){active=false;failed=true;return false;}
      const int offset=int(std::strtol(bytes.c_str()+1,nullptr,10)),spacing=int(std::strtol(bytes.c_str()+first+1,nullptr,10));
      writeText(line+2,bytes.substr(second+1),offset,spacing,2,2);
    }else{
      writeText(line,std::move(bytes),0,0,style,2);
      if(line==0)line=1;else line=textInitialized=0;
    }return true;
  }
  void reset(){*this={};}
  bool start(const File& source,int index){if(index<0||size_t(index)>=source.entries.size())return false;reset();file=&source;entry=index;pc=source.entries[index].offset;active=true;return true;}
  template<class Emit> void tick(uint32_t held,uint32_t pressed,Emit emit){
    if(!active||!file)return;if(typingDelay>0)--typingDelay;
    if((flags&1)&&(held&0x200)){previousClock=clock;clock=read<uint16_t>(file->data.data()+pc);previousClock=clock-1;}
    for(int budget=0;budget<4096;budget++){
      if(pc+4>file->data.size()){active=false;failed=true;return;}
      const auto* p=file->data.data()+pc;const uint32_t length=4+p[3];if(length>file->data.size()-pc){active=false;failed=true;return;}
      if(clock<read<uint16_t>(p)){previousClock=clock;++clock;++age;return;}
      const Command command{p[2],p+4,p[3]};
      textActions.clear();
      if(command.op==0){active=false;emit(command);++age;return;}
      if(command.op==10){if(command.size)flags=(flags&~1u)|(command.payload[0]&1u);}
      else if(command.op==11){
        if(wait<=0){wait=command.integer();previousWait=wait-1;}
        previousWait=wait;--wait;
        if(!(pressed&0x80001)&&wait>0&&!((flags&1)&&(held&0x200))){++age;return;}
        wait=0;previousWait=-1;line=textInitialized=0;
      }else if(command.op==12)typingDelay=1;
      else if(command.op==26)flags|=2;
      else if(command.op>=7&&command.op<=9){speaker=command.op-7;if(command.op==9)flags|=2u;else flags&=~2u;line=textInitialized=0;}
      else if(command.op==15||command.op==16){const int slot=command.op-15;text[slot]=decrypt(command.payload,command.size);writeText(slot,text[slot],0,0,(flags>>1)&1,2);}
      else if(command.op==17){if(!modernText(command))return;}
      emit(command);pc+=length;
      if(budget==4095){active=false;failed=true;}
    }
  }
};
}
