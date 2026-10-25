#pragma once
#include <algorithm>
#include <cstdint>
#include <map>
#include <vector>

namespace touhou::sdl::deferred {
// Half-open physical-pixel bounds, not geometry bounds inferred from contents.
struct Rect { std::int64_t left=0,top=0,right=0,bottom=0; };
inline bool empty(Rect a) { return a.left>=a.right||a.top>=a.bottom; }
inline bool contains(Rect a,Rect b) {
 return !empty(b)&&a.left<=b.left&&a.top<=b.top&&a.right>=b.right&&a.bottom>=b.bottom;
}
inline bool overlaps(Rect a,Rect b) {
 return !empty(a)&&!empty(b)&&a.left<b.right&&b.left<a.right&&a.top<b.bottom&&b.top<a.bottom;
}
struct Access { std::uint64_t plane=0;Rect bounds; };
struct Effect {
 // Only Draws can be removed. Clears and external observations are retained.
 bool draw=false,barrier=false;
 std::vector<Access> reads,writes,overwrites;
};
class Liveness {
 std::map<std::uint64_t,std::vector<Rect>> dead;
 bool covered(const Access& a)const {
  if(empty(a.bounds))return false;
  const auto it=dead.find(a.plane);if(it==dead.end())return false;
  for(const auto& r:it->second)if(contains(r,a.bounds))return true;
  return false;
 }
 void read(const Access& a) {
  const auto it=dead.find(a.plane);if(it==dead.end())return;
  auto& regions=it->second;
  // Deliberately forget the WHOLE intersected dead rectangle. Keeping an
  // unnecessary draw is safe; inventing coverage to optimize it is not.
  regions.erase(std::remove_if(regions.begin(),regions.end(),[&](Rect r){return overlaps(r,a.bounds);}),regions.end());
 }
 void overwrite(const Access& a) {
  if(empty(a.bounds))return;
  auto& regions=dead[a.plane];
  for(const auto& r:regions)if(contains(r,a.bounds))return;
  regions.erase(std::remove_if(regions.begin(),regions.end(),[&](Rect r){return contains(a.bounds,r);}),regions.end());
  if(regions.size()<64)regions.push_back(a.bounds); // losing facts only loses optimization
 }
public:
 void reset(){dead.clear();}
 // Traverse a closed submission interval BACKWARDS. Final surfaces are all
 // observable. A retained texture/depth/blend read revives its predecessor.
 bool retain(const Effect& e) {
  if(e.barrier){reset();return true;}
  if(e.draw&&!e.writes.empty()&&std::all_of(e.writes.begin(),e.writes.end(),[&](const Access& a){return covered(a);}))return false;
  for(const auto& a:e.overwrites)overwrite(a);
  for(const auto& a:e.reads)read(a);
  return true;
 }
};
}
