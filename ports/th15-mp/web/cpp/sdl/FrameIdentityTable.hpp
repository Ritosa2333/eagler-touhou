#pragma once
#include <cstdint>
#include <cstdlib>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace th15::sdl {
// Presentation identities are numeric; the table never owns or dereferences a
// VM. Generation-tagged flat storage avoids per-Draw node allocation. The hash
// affects lookup placement only, never authored command/sample order.
// Narrow Epoch is for wrap regression tests; production uses uint32_t.
template<class Key,class Value,class Epoch=std::uint32_t> class FrameIdentityTable {
 static_assert(std::is_integral_v<Epoch>&&std::is_unsigned_v<Epoch>&&!std::is_same_v<Epoch,bool>);
 static_assert(std::tuple_size_v<Key> == 3||std::tuple_size_v<Key> == 4);
 struct Entry {Key key{};Value value{};Epoch epoch=0;};
 std::vector<Entry> entries;
 std::size_t used=0;
 Epoch epoch=1;
 static std::uint32_t hash(const Key& key)noexcept{
  const auto owner=std::uint64_t(std::get<0>(key)),generation=std::uint64_t(std::get<1>(key));
  std::uint32_t h=std::uint32_t(owner)^std::uint32_t(owner>>32);
  h^=std::uint32_t(generation)*0x9e3779b9u;
  h^=std::uint32_t(generation>>32)*0x85ebca6bu;
  h^=std::uint32_t(std::get<2>(key))*0xc2b2ae35u;
  if constexpr(std::tuple_size_v<Key> == 4)h^=std::uint32_t(std::get<3>(key))*0x27d4eb2du;
  h^=h>>16;h*=0x7feb352du;h^=h>>15;h*=0x846ca68bu;return h^(h>>16);
 }
 std::size_t locate(const Key& key)const noexcept{
  const auto mask=entries.size()-1;auto slot=std::size_t(hash(key))&mask;
  while(entries[slot].epoch==epoch&&entries[slot].key!=key)slot=(slot+1)&mask;
  return slot;
 }
 void grow(){
  // Power-of-two capacity, load <= 1/2: every probe has an empty terminator.
  const auto capacity=entries.empty()?64:entries.size()*2;
  if(capacity<entries.size()||capacity>entries.max_size())std::abort();
  auto old=std::move(entries);entries=std::vector<Entry>(capacity);
  for(const auto& entry:old)if(entry.epoch==epoch)entries[locate(entry.key)]=entry;
 }
public:
 void clear()noexcept{
  used=0;
  if(++epoch==0){for(auto& entry:entries)entry.epoch=0;epoch=1;}
 }
 const Value* find(const Key& key)const noexcept{
  if(entries.empty())return nullptr;const auto& entry=entries[locate(key)];
  return entry.epoch==epoch?&entry.value:nullptr;
 }
 std::pair<Value*,bool> emplace(const Key& key){
  if(entries.empty())grow();auto slot=locate(key);
  if(entries[slot].epoch==epoch)return {&entries[slot].value,false};
  if(used==entries.size()/2){grow();slot=locate(key);}
  auto& entry=entries[slot];entry.key=key;entry.value={};entry.epoch=epoch;++used;
  return {&entry.value,true};
 }
 Value& operator[](const Key& key){return *emplace(key).first;}
};
}
