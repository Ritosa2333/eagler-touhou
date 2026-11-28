// 有界原地址视图；同地址的生产上下文共享写入，并在平台回调后核对别名变更。
#pragma once
#include "core/runtime/GameStartEntryContext.hpp"
#include <algorithm>
#include <cstring>
#include <map>
#include <type_traits>
#include <unordered_map>
#include "core/frontend/Replay.hpp"
#include <stdexcept>
namespace kaikeidou {
struct GameStartEntryMemoryBinding {std::uint32_t address;void* data;std::size_t size;};
using GameStartEntryMemoryExtensions=std::vector<GameStartEntryMemoryBinding>;
struct GameStartEntryAccessFault {
 std::uint32_t address=0,bytes=0;bool writing=false;
};
class GameStartEntryMemory {
 struct Binding {std::uint32_t address;std::size_t size;std::uint8_t* data;};
 struct Alias {std::uint32_t address;std::vector<std::uint8_t*> values;std::uint8_t previous;};
 static constexpr std::uint32_t page_bits=12,page_size=1u<<page_bits;
 std::vector<Binding> bindings_;
 std::vector<Alias> aliases_;
 std::vector<std::pair<std::uint64_t,std::uint64_t>> coverage_;
 std::unordered_map<std::uint32_t,std::vector<std::uint32_t>> pages_;
 std::vector<std::uint32_t> visit_marks_;
 std::uint32_t visit_epoch_=0;
 Random& random_;std::uint32_t random_word_;
 bool sealed_=false;
 void rebuild_index(){
  coverage_.clear();pages_.clear();
  std::vector<std::pair<std::uint64_t,std::uint64_t>> intervals;intervals.reserve(bindings_.size());
  for(const auto& b:bindings_)if(b.size)intervals.emplace_back(b.address,std::uint64_t(b.address)+b.size);
  std::sort(intervals.begin(),intervals.end());for(const auto& interval:intervals){
   if(!coverage_.empty()&&interval.first<=coverage_.back().second)coverage_.back().second=std::max(coverage_.back().second,interval.second);
   else coverage_.push_back(interval);
  }
  for(std::uint32_t i=0;i<bindings_.size();++i){const auto& b=bindings_[i];if(!b.size)continue;
   const auto first=std::uint32_t(b.address>>page_bits),last=std::uint32_t((std::uint64_t(b.address)+b.size-1)>>page_bits);
   for(std::uint32_t page=first;page<=last;++page)pages_[page].push_back(i);
  }
  visit_marks_.assign(bindings_.size(),0);visit_epoch_=0;
 }
 void range(std::uint32_t address,std::size_t size,bool writing)const{
  std::uint64_t at=address,end=at+size;
  if(end>0x100000000ull)throw GameStartEntryAccessFault{address,static_cast<std::uint32_t>(size),writing};
  while(at<end){
   auto it=std::upper_bound(coverage_.begin(),coverage_.end(),at,[](std::uint64_t value,const auto& interval){return value<interval.first;});
   if(it==coverage_.begin())throw GameStartEntryAccessFault{static_cast<std::uint32_t>(at),static_cast<std::uint32_t>(end-at),writing};
   --it;
   if(at<it->first||at>=it->second)throw GameStartEntryAccessFault{static_cast<std::uint32_t>(at),static_cast<std::uint32_t>(end-at),writing};
   at=it->second;
  }
 }
 const Binding* find_binding(std::uint64_t at)const{
  const auto found=pages_.find(std::uint32_t(at>>page_bits));if(found==pages_.end())return nullptr;
  for(const auto index:found->second){const auto& b=bindings_[index];if(b.address<=at&&at<std::uint64_t(b.address)+b.size)return &b;}
  return nullptr;
 }
 template<class F>void for_bindings(std::uint64_t address,std::uint64_t size,F&& apply){
  if(!size)return;
  if(++visit_epoch_==0){std::fill(visit_marks_.begin(),visit_marks_.end(),0);++visit_epoch_;}
  const auto end=address+size;const auto first=std::uint32_t(address>>page_bits),last=std::uint32_t((end-1)>>page_bits);
  for(std::uint32_t page=first;page<=last;++page){
   const auto found=pages_.find(page);if(found==pages_.end())continue;
   for(const auto index:found->second){
    if(visit_marks_[index]==visit_epoch_)continue;visit_marks_[index]=visit_epoch_;
    auto& b=bindings_[index];const auto begin=std::max(address,std::uint64_t(b.address)),finish=std::min(end,std::uint64_t(b.address)+b.size);
    if(begin<finish)apply(b,begin,finish);
   }
  }
 }
 void commit_random(){static_assert(std::is_trivially_copyable_v<Random>&&sizeof(Random)==4,"Original Random cursor representation");if(random_.cursor()!=random_word_)std::memcpy(&random_,&random_word_,4);}
 void accept_aliases(){for(auto& a:aliases_)a.previous=*a.values.front();}
public:
 explicit GameStartEntryMemory(Random& random):random_(random),random_word_(random.cursor()){bind(0xa74c08,random_word_);}
 void bind_bytes(std::uint32_t address,void* data,std::size_t size){
  if(sealed_)throw std::logic_error("Game start mapping already sealed");
  if(size)bindings_.push_back({address,size,static_cast<std::uint8_t*>(data)});
 }
 template<class T>void bind(std::uint32_t address,T& value){bind_bytes(address,&value,sizeof(value));}
 void bind_runtime_bytes(std::uint32_t address,void* data,std::size_t size){
  for(const auto& b:bindings_)if(std::uint64_t(address)<std::uint64_t(b.address)+b.size&&std::uint64_t(b.address)<std::uint64_t(address)+size)throw std::logic_error("Game start allocation overlaps owned memory");
  if(size)bindings_.push_back({address,size,static_cast<std::uint8_t*>(data)});
 }
 void seal(){
  std::map<std::uint32_t,std::vector<std::uint8_t*>> overlaps;
  for(std::size_t i=0;i<bindings_.size();++i)for(std::size_t j=0;j<i;++j){
   auto& a=bindings_[i];auto& b=bindings_[j];const auto first=std::max(a.address,b.address);
   const auto last=std::min(std::uint64_t(a.address)+a.size,std::uint64_t(b.address)+b.size);
   for(auto at=std::uint64_t(first);at<last;++at){auto& v=overlaps[static_cast<std::uint32_t>(at)];v.push_back(a.data+at-a.address);v.push_back(b.data+at-b.address);}
  }
  for(auto& p:overlaps){auto& v=p.second;std::sort(v.begin(),v.end());v.erase(std::unique(v.begin(),v.end()),v.end());
   const auto byte=*v.front();for(auto* q:v)if(*q!=byte)throw std::logic_error("Game start initial aliases disagree");aliases_.push_back({p.first,std::move(v),byte});}
  rebuild_index();sealed_=true;
 }
 std::vector<std::array<std::uint32_t,2>> regions()const{
  std::vector<std::array<std::uint32_t,2>> out;out.reserve(coverage_.size());
  for(const auto& interval:coverage_)out.push_back({static_cast<std::uint32_t>(interval.first),static_cast<std::uint32_t>(interval.second-interval.first)});
  return out;
 }
 void reconcile(){
  if(random_.cursor()!=random_word_)random_word_=random_.cursor();
  for(auto& a:aliases_){auto value=a.previous;bool changed=false;
   for(auto* p:a.values)if(*p!=a.previous){if(changed&&*p!=value)throw std::logic_error("Game start callback aliases disagree");value=*p;changed=true;}
   if(changed){for(auto* p:a.values)*p=value;a.previous=value;}
  }
  commit_random();
 }
 std::vector<std::uint8_t> read(std::uint32_t address,std::size_t size)const{
  range(address,size,false);std::vector<std::uint8_t> out(size);std::size_t done=0;
  while(done<size){const auto at=std::uint64_t(address)+done;const auto* b=find_binding(at);
   if(!b)throw GameStartEntryAccessFault{static_cast<std::uint32_t>(at),static_cast<std::uint32_t>(size-done),false};
   const auto count=std::min(size-done,b->size-static_cast<std::size_t>(at-b->address));std::memcpy(out.data()+done,b->data+at-b->address,count);done+=count;
  }
  return out;
 }
 void write(std::uint32_t address,const std::uint8_t* bytes,std::size_t size){
  range(address,size,true);
  for_bindings(address,size,[&](Binding& b,std::uint64_t first,std::uint64_t finish){std::memcpy(b.data+first-b.address,bytes+first-address,static_cast<std::size_t>(finish-first));});
  accept_aliases();commit_random();
 }
 void clear(std::uint32_t address,std::size_t size){
  range(address,size,true);
  for_bindings(address,size,[&](Binding& b,std::uint64_t first,std::uint64_t finish){std::memset(b.data+first-b.address,0,static_cast<std::size_t>(finish-first));});
  accept_aliases();commit_random();
 }
 void write(std::uint32_t a,const std::vector<std::uint8_t>& bytes){write(a,bytes.data(),bytes.size());}
 std::uint8_t byte(std::uint32_t a)const{return read(a,1)[0];}
 std::uint32_t word(std::uint32_t a)const{const auto b=read(a,4);return read_u32(b.data());}
 std::uint64_t quad(std::uint32_t a)const{return std::uint64_t(word(a))|(std::uint64_t(word(a+4))<<32);}
 void word(std::uint32_t a,std::uint32_t v){std::uint8_t b[4];for(unsigned i=0;i<4;++i)b[i]=static_cast<std::uint8_t>(v>>(8*i));write(a,b,4);}
 void quad(std::uint32_t a,std::uint64_t v){std::uint8_t b[8];for(unsigned i=0;i<8;++i)b[i]=static_cast<std::uint8_t>(v>>(8*i));write(a,b,8);}
 void byte(std::uint32_t a,std::uint8_t v){write(a,&v,1);}
};
}
