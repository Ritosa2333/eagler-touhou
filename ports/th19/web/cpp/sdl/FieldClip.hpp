#pragma once
#include <array>
#include <algorithm>
#include <cstddef>

namespace th19::sdl {
// A convex triangle/quad gains at most four vertices against the field rect.
// Keep the native clipping order and UV arithmetic, without per-edge heaps.
template<class Vertex> struct FieldPolygon {
 std::array<Vertex,12> vertices{};
 std::size_t count=0;
};
template<class Vertex>
FieldPolygon<Vertex> clipFieldPolygon(const Vertex* input,std::size_t count,int side){
 FieldPolygon<Vertex> polygon;polygon.count=count;
 std::copy_n(input,count,polygon.vertices.begin());
 const float left=side?656.f:32.f,right=left+592.f;
 const float bounds[4]={left,right,32.f,928.f};
 unsigned common=15,combined=0;
 for(std::size_t i=0;i<count;++i){const auto&p=input[i].position;
  const unsigned mask=(p.x<left?1u:0u)|(p.x>right?2u:0u)|(p.y<32?4u:0u)|(p.y>928?8u:0u);
  common&=mask;combined|=mask;
 }
 if(common){polygon.count=0;return polygon;}
 if(!combined)return polygon;
 for(int edge=0;edge<4&&polygon.count;++edge){
  FieldPolygon<Vertex> result;
  auto distance=[&](const Vertex&v){const float value=edge<2?v.position.x:v.position.y;return (edge==0||edge==2)?value-bounds[edge]:bounds[edge]-value;};
  auto previous=polygon.vertices[polygon.count-1];float dp=distance(previous);
  for(std::size_t i=0;i<polygon.count;++i){const auto&current=polygon.vertices[i];const float dc=distance(current);
   if((dp>=0)!=(dc>=0)){const float t=dp/(dp-dc);auto v=previous;
    v.position.x+=(current.position.x-previous.position.x)*t;
    v.position.y+=(current.position.y-previous.position.y)*t;
    v.uv.x+=(current.uv.x-previous.uv.x)*t;v.uv.y+=(current.uv.y-previous.uv.y)*t;
    result.vertices[result.count++]=v;
   }
   if(dc>=0)result.vertices[result.count++]=current;
   previous=current;dp=dc;
  }
  polygon=result;
 }
 return polygon;
}
}
