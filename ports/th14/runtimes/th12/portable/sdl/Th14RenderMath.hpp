#pragma once
#include "Renderer.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstddef>
#include <limits>
namespace touhou::sdl {
#if defined(PORT_TITLE) && PORT_TITLE == 14
// Fixed-function matrix composition rounds every product and partial sum to
// binary32 before the next term. Volatile stores retain that order at -O3.
inline std::array<float,16> th14_multiply(const std::array<float,16>& a,const std::array<float,16>& b){
 std::array<float,16> result{};
 for(int col=0;col<4;++col)for(int row=0;row<4;++row){
  volatile float product=a[row]*b[col*4];volatile float sum=product;
  for(int k=1;k<4;++k){product=a[k*4+row]*b[col*4+k];sum=sum+product;}
  result[col*4+row]=sum;
 }
 return result;
}
inline std::array<float,16> th14_clip(const std::array<float,16>& world,const std::array<float,16>& view,const std::array<float,16>& projection){
 return th14_multiply(projection,th14_multiply(view,world));
}
#endif

#if defined(PORT_TITLE) && PORT_TITLE == 14

// CPU coordinates select topology only. The vertex shader recomputes each
// intersection from the original endpoints using the GPU reciprocal.
struct Th14ClipVertex {float q[4],uv[2],fog;float a[7]{},b[7]{};float edge=-1.f;};
static_assert(sizeof(Th14ClipVertex)==88 && offsetof(Th14ClipVertex,a)==28 &&
              offsetof(Th14ClipVertex,b)==56 && offsetof(Th14ClipVertex,edge)==84);
inline float th14_dot(const std::array<float,16>& m,u32 row,const float* p){
 volatile float product=m[row]*p[0],sum=product;
 for(u32 k=1;k<4;k++){product=m[row+k*4]*(k==3?1.f:p[k]);sum=sum+product;}return sum;
}
inline int th14_clip_world_uv(const State& d,Topology topology,u32 count,const void* data,std::vector<Th14ClipVertex>& output){
 if(!count)return 0;
 if(!data||count>65536||!d.viewport.width||!d.viewport.height||
    (topology!=Topology::Triangles&&topology!=Topology::Strip&&topology!=Topology::Fan))return -1;
 auto supported=[](float x){return std::isfinite(x)&&(x==0.f||std::abs(x)>=std::numeric_limits<float>::min());};
 if(d.pipeline.fog&&(d.pipeline.rangeFog||d.pipeline.fogMode!=FogMode::Linear||
    !supported(d.pipeline.fogNear)||!supported(d.pipeline.fogFar)||d.pipeline.fogNear==d.pipeline.fogFar))return -1;
 const auto clip=th14_clip(d.matrix[0],d.matrix[1],d.matrix[2]),eye=th14_multiply(d.matrix[1],d.matrix[0]);
 const u32 n=vertex_count(topology,count);std::vector<Th14ClipVertex> source(n);bool crosses=false,negativeW=false;
 volatile float range=d.pipeline.fog?d.pipeline.fogFar-d.pipeline.fogNear:1.f;
 volatile float scale=1.f/range,bias=d.pipeline.fog?d.pipeline.fogFar*scale:1.f;
 if(!supported(scale)||!supported(bias))return -1;
 for(u32 i=0;i<n;i++){
  float p[5];std::memcpy(p,static_cast<const u8*>(data)+i*20,20);auto& v=source[i];
  for(float x:p)if(!supported(x))return -1;
  for(u32 row=0;row<4;row++){v.q[row]=th14_dot(clip,row,p);if(!supported(v.q[row]))return -1;}
  if(v.q[2]>v.q[3])return 0;crosses|=v.q[2]<0;negativeW|=v.q[3]<=0;
  for(u32 row=0;row<2;row++){if(d.pipeline.textureTransform){volatile float a=d.matrix[3][row]*p[3],b=d.matrix[3][row+4]*p[4],s=a+b;s=s+d.matrix[3][row+8];v.uv[row]=s;}else v.uv[row]=p[row+3];}
  float z=std::abs(th14_dot(eye,2,p));
  volatile float product=z*scale,fog=bias-product;v.fog=d.pipeline.fog?float(fog):1.f;
  if(!supported(v.uv[0])||!supported(v.uv[1])||!supported(v.fog))return -1;
  std::memcpy(v.a,v.q,28);
 }
 if(!crosses||!negativeW)return 0;
 for(u32 triangle=0;triangle<count;triangle++){
  u32 a,b,c;if(topology==Topology::Triangles){a=triangle*3;b=a+1;c=a+2;}
  else if(topology==Topology::Fan){a=0;b=triangle+1;c=triangle+2;}
  else if(topology==Topology::Strip){a=triangle&1?triangle+1:triangle;b=triangle&1?triangle:triangle+1;c=triangle+2;}
  else return -1;
  std::vector<Th14ClipVertex> polygon{source[a],source[b],source[c]};
  for(u32 plane=0;plane<2;plane++){
   std::vector<Th14ClipVertex> next;int outside=-1;
   auto distance=[&](const Th14ClipVertex& v){volatile float far=v.q[3]-v.q[2];return plane?float(far):v.q[2];};
   for(u32 i=0;i<polygon.size();i++){
    const auto& x=polygon[i];const auto& y=polygon[(i+1)%polygon.size()];const float dx=distance(x),dy=distance(y);
    if(dx>=0)next.push_back(x);else if(outside<0)outside=int(i);
    if((dx>=0)!=(dy>=0)){
     volatile float denominator=dx-dy;
     if(!supported(denominator)||denominator==0.f)return -1;
     volatile float reciprocal=1.f/denominator,wa=-dy*reciprocal,wb=dx*reciprocal;
     if(!supported(reciprocal)||!supported(wa)||!supported(wb))return -1;
     auto mix=[&](float a,float b){volatile float left=a*wa,right=b*wb,sum=left+right;return float(sum);};
     Th14ClipVertex v{};for(u32 j=0;j<4;j++)v.q[j]=mix(x.q[j],y.q[j]);v.q[2]=plane?v.q[3]:0.f;
     for(u32 j=0;j<2;j++)v.uv[j]=mix(x.uv[j],y.uv[j]);v.fog=mix(x.fog,y.fog);std::memcpy(v.a,x.q,28);std::memcpy(v.b,y.q,28);v.edge=-2.f;next.push_back(v);
    }
   }
   // Native solid-fill controls select the diagonal anchored at the
   // surviving vertex with largest projected X, including mirrored strips.
   // Compare binary32 cross-products in binary64 without an extra divide.
   if(outside>=0&&next.size()==4&&!plane){
    u32 root=0;bool found=false;
    for(u32 i=0;i<next.size();i++)if(next[i].edge==-1.f&&next[i].q[3]>0){
     if(!found||double(next[i].q[0])*next[root].q[3]>double(next[root].q[0])*next[i].q[3]){root=i;found=true;}
    }
    if(!found)return -1;std::rotate(next.begin(),next.begin()+root,next.end());
   }
   polygon=std::move(next);if(polygon.size()<3)break;
  }
  for(const auto& v:polygon)if(!(v.q[3]>0))return -1;
  for(u32 i=1;i+1<polygon.size();i++)for(u32 j:{0u,i,i+1})output.push_back(polygon[j]);
 }
 return 1;
}

inline bool th14_primitive_projectable(const float* vertices,u32 stride,u32 first,u32 count,const std::array<float,16>& clip,const Viewport& viewport){
 // Enclose binary32 dot products under reassociation/FMA and flush-to-zero.
 // Eight full float epsilons exceed the seven-operation gamma bound. Keep
 // subnormal inputs, potential overflow and uncertain W/depth boundaries unchanged. Side clipping is eligible only as a complete primitive.
 constexpr double epsilon=0x1p-23,minimum=0x1p-126,maximum=0x1.fffffep127;
 if(!viewport.width||!viewport.height)return false;
 for(float c:clip)if(!std::isfinite(c)||(c!=0&&std::abs(double(c))<minimum))return false;
 for(u32 i=first;i<first+count;++i){float p[3];std::memcpy(p,reinterpret_cast<const u8*>(vertices)+i*stride,12);
  for(float c:p)if(!std::isfinite(c)||(c!=0&&std::abs(double(c))<minimum))return false;
  double q[4]{},error[4]{};
  for(u32 row=0;row<4;++row){double magnitude=0;for(u32 col=0;col<4;++col){const double product=double(clip[col*4+row])*(col==3?1.:double(p[col]));q[row]+=product;magnitude+=std::abs(product);}
   if(!std::isfinite(magnitude)||magnitude>maximum*.25)return false;
   error[row]=8.*epsilon*magnitude+8.*minimum;
  }
  if((std::abs(q[0])+error[0])*std::max(1.,double(float(viewport.width))*.5)>maximum*.25||(std::abs(q[1])+error[1])*std::max(1.,double(float(viewport.height))*.5)>maximum*.25)return false;
  const double lowW=q[3]-error[3];
  if(!(lowW>0&&q[2]-error[2]>0&&q[2]+error[2]<lowW))return false;
  if((std::abs(q[0])+error[0])*std::max(1.,double(float(viewport.width))*.5)/lowW>maximum/1024.||(std::abs(q[1])+error[1])*std::max(1.,double(float(viewport.height))*.5)/lowW>maximum/1024.)return false;
 }
 return true;
}
#endif
}
