#include "../../web/cpp/sdl/FrameCommandPool.hpp"
#include <array>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <set>

namespace {
struct Command {
 static unsigned copies,moves;
 unsigned identity=0;
 std::vector<uint8_t> bytes;
 explicit Command(unsigned id=0):identity(id),bytes(256,uint8_t(id)){}
 Command(const Command& other):identity(other.identity),bytes(other.bytes){++copies;}
 Command(Command&& other)noexcept:identity(other.identity),bytes(std::move(other.bytes)){++moves;}
 Command& operator=(const Command& other){identity=other.identity;bytes=other.bytes;++copies;return *this;}
 Command& operator=(Command&& other)noexcept{identity=other.identity;bytes=std::move(other.bytes);++moves;return *this;}
};
unsigned Command::copies=0,Command::moves=0;
using Pool=th15::sdl::FrameCommandPool<Command>;
using Snapshot=std::shared_ptr<const Pool::Commands>;
unsigned checks=0;
void check(bool value){assert(value);++checks;}
void equal(const Snapshot& frame,unsigned id){
 check(frame&&frame->size()==3);
 for(const auto& command:*frame){check(command.identity==id);check(command.bytes.size()==256);check(std::all_of(command.bytes.begin(),command.bytes.end(),[id](auto byte){return byte==uint8_t(id);}));}
}
Snapshot publish(Pool& pool,Pool::Commands& current,unsigned id){
 current.resize(3);
 for(auto& command:current){command.identity=id;command.bytes.assign(256,uint8_t(id));}
 const auto* data=current.data();const auto* pixels=current[0].bytes.data();
 const auto copies=Command::copies,moves=Command::moves;
 auto frame=pool.acquire();current.swap(*frame);
 check(frame->data()==data&&(*frame)[0].bytes.data()==pixels);
 check(Command::copies==copies&&Command::moves==moves);
 return frame;
}
}
int main(){
 Pool pool;Pool::Commands current;
 auto first=publish(pool,current,17);const auto* firstStorage=first->data();
 auto second=publish(pool,current,29);equal(first,17);equal(second,29);
 // Even a restored predecessor retained after the forward frame is discarded
 // prevents its storage from being selected by subsequent Draw transactions.
 Snapshot restored=first;first.reset();
 for(unsigned id=30;id<100;++id){auto frame=publish(pool,current,id);equal(frame,id);equal(restored,17);equal(second,29);check(frame->data()!=firstStorage);}
 // An external snapshot remains valid across history/lifecycle pool clearing.
 pool.clear();equal(restored,17);equal(second,29);
 for(unsigned id=100;id<120;++id){auto frame=publish(pool,current,id);equal(frame,id);equal(restored,17);equal(second,29);}
 restored.reset();second.reset();pool.clear();
 // Fill the production prediction window and repeatedly retire only its first
 // frame. Every live frame must remain immutable through thousands of reuses.
 std::array<Snapshot,12> history{};std::array<unsigned,12> identities{};
 std::set<const Pool::Commands*> owners;
 for(unsigned id=1;id<=1200;++id){
  const auto slot=(id-1)%history.size();history[slot].reset();
  history[slot]=publish(pool,current,id);identities[slot]=id;
  owners.insert(history[slot].get());check(owners.size()<=history.size());
  for(unsigned i=0;i<history.size();++i)if(history[i])equal(history[i],identities[i]);
 }
 // Reuse a free slot without allocating or replacing its existing vector.
 for(auto& frame:history)frame.reset();
 auto free=pool.acquire();free->resize(3);const auto* storage=free->data();
 auto* owner=free.get();free.reset();auto reused=pool.acquire();
 check(reused.get()==owner&&reused->data()==storage);
 std::cout<<"PASS presentation frame pool | immutable retained snapshots, zero-copy transfer, lifecycle clear, 1200 frames with 12 reused slots\n";
}
