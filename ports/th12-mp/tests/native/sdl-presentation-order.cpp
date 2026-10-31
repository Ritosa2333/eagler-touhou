// Header-only behavioral checks for the native frame lookup and priority buckets.
#include "PresentationOrder.hpp"
#include <cassert>
#include <numeric>
#include <random>
#include <cstdio>
using namespace th12host;
struct Record {unsigned id;};
int main(){
 FrameLookup index;Record repeated[]{{0},{1},{0},{0xffffffffu}};index.build(repeated,4);
 assert(index.find(0)==0&&index.find(1)==1&&index.find(0xffffffffu)==3&&index.find(2)==FrameLookup::missing);
 Record newer[]{{2},{1}};index.build(newer,2);assert(index.find(0)==FrameLookup::missing&&index.find(1)==1&&index.find(2)==0);
 index.build(newer,0);assert(index.find(1)==FrameLookup::missing);
 std::mt19937 random(91);PriorityBuckets buckets;std::vector<unsigned> indices(3000),expected,priority(3000),order(3000);
 for(unsigned frame=0;frame<60;frame++){
  std::iota(indices.begin(),indices.end(),0);std::shuffle(indices.begin(),indices.end(),random);
  for(unsigned i=0;i<indices.size();i++){priority[i]=frame%10?random()%101:random()%150;order[i]=random()%600;}
  expected=indices;std::sort(expected.begin(),expected.end(),[&](auto a,auto b){return priority[a]!=priority[b]?priority[a]<priority[b]:order[a]!=order[b]?order[a]<order[b]:a<b;});
  buckets.sort(indices,[&](auto i){return priority[i];},[&](auto i){return order[i];});assert(indices==expected);
 }
 for(unsigned frame=0;frame<100;frame++){
  std::vector<Record> records;for(unsigned i=0;i<3000;i++)records.push_back({random()});index.build(records.data(),records.size());
  for(unsigned i=0;i<records.size();i++){unsigned expected=i;for(unsigned n=0;n<i;n++)if(records[n].id==records[i].id){expected=n;break;}assert(index.find(records[i].id)==expected);}
 }
 std::puts("priority buckets and epoch lookup match stable ordering, fallback, duplicate IDs and frame resets");
}
