#include <GpuCommandLiveness.hpp>
#include <array>
#include <random>
#include <cstdio>
#include <cstdlib>
using namespace touhou::sdl::deferred;
constexpr unsigned W=16,H=16,N=4;
using Grid=std::array<std::array<unsigned,W*H>,N*2>;
struct Op {Effect e;Rect rect;unsigned target=0,texture=0,seed=0,flags=0;bool clear=false,barrier=false,blend=false,depthTest=false,depthWrite=false;};
unsigned long long checks=0,removed=0,sequences=0;
void need(bool b,const char* s){++checks;if(!b){std::fprintf(stderr,"FAIL %s at %llu\n",s,checks);std::exit(1);}}
Access access(unsigned plane,Rect r){return {plane,r};}
Rect full(){return {0,0,W,H};}
Op clear(unsigned target,Rect rect,unsigned flags,unsigned value){
 Op o;o.target=target;o.rect=rect;o.flags=flags;o.seed=value;o.clear=true;
 if(flags&1)o.e.overwrites.push_back(access(target*2,rect));
 if(flags&2)o.e.overwrites.push_back(access(target*2+1,rect));
 return o;
}
Op draw(unsigned target,unsigned texture,Rect rect,unsigned value,bool blend,bool dt,bool dw){
 Op o;o.target=target;o.texture=texture;o.rect=rect;o.seed=value;o.blend=blend;o.depthTest=dt;o.depthWrite=dw;
 o.e.draw=true;o.e.writes.push_back(access(target*2,rect));
 o.e.reads.push_back(access(target*2,rect)); // conservative even for opaque output
 if(dt)o.e.reads.push_back(access(target*2+1,rect));
 if(dt&&dw)o.e.writes.push_back(access(target*2+1,rect));
 o.e.reads.push_back(access(texture*2,full()));
 return o;
}
void execute(Grid& g,const Op& o){
 if(o.barrier)return;
 for(int y=o.rect.top;y<o.rect.bottom;++y)for(int x=o.rect.left;x<o.rect.right;++x){
  if(x<0||x>=int(W)||y<0||y>=int(H))continue;
  const unsigned i=y*W+x;auto& c=g[o.target*2][i];auto& d=g[o.target*2+1][i];
  if(o.clear){if(o.flags&1)c=o.seed;if(o.flags&2)d=o.seed;continue;}
  // Vary actual coverage WITHIN the declared viewport, including holes and
  // multiple mixed primitives. Do not change RNG state when removing a draw.
  if((i*17+o.seed)%11<3)continue;
  const unsigned z=(o.seed+i*7)%256;
  if(o.depthTest&&z>d)continue;
  const unsigned source=g[o.texture*2][(i+o.seed)% (W*H)];
  c=o.blend?((c*3+source+o.seed)&0xffff):((source^o.seed)&0xffff);
  if(o.depthTest&&o.depthWrite)d=z;
 }
}
void check(const std::vector<Op>& ops,bool negative=false){
 Liveness l;std::vector<bool> keep(ops.size());
 for(size_t i=ops.size();i-->0;){auto e=ops[i].e;if(negative)e.reads.clear();keep[i]=l.retain(e);if(!keep[i])++removed;}
 Grid a{},b{};for(unsigned p=0;p<N*2;++p)for(unsigned i=0;i<W*H;++i)a[p][i]=b[p][i]=(p*123+i)%256;
 bool equal=true;
 for(size_t i=0;i<ops.size();++i){execute(a,ops[i]);if(keep[i])execute(b,ops[i]);if(ops[i].barrier){need(a==b,"barrier observation changed");}}
 equal=a==b;
 need(negative?!equal:equal,negative?"negative control not detected":"final surfaces changed");++sequences;
}
int main(){
 // A read of the first target must stop its earlier draw being erased by a
 // later clear, even though final target pixels themselves no longer need it.
 std::vector<Op> dependence{draw(0,2,full(),19,false,false,false),draw(1,0,full(),7,true,false,false),clear(0,full(),3,0)};
 check(dependence);check(dependence,true);
 check({draw(0,1,full(),9,true,true,true),clear(0,{0,0,8,8},3,7)});
 check({draw(0,1,full(),9,true,true,true),clear(0,full(),1,7),draw(0,1,full(),11,true,true,false)});
 check({draw(0,1,full(),9,true,false,false),clear(0,full(),3,7)});
 std::mt19937 r(0x735139);
 for(unsigned sequence=0;sequence<4000;++sequence){
  std::vector<Op> ops;
  for(unsigned n=0;n<120;++n){
   unsigned target=r()%N,tex=(target+1+r()%(N-1))%N;
   Rect rect{int(r()%8),int(r()%8),int(8+r()%9),int(8+r()%9)};
   if(n%7==0)rect=full();
   if(n%29==0){Op o;o.barrier=o.e.barrier=true;ops.push_back(o);}
   else if(n%5==0){const unsigned flags=1+r()%3,value=r();ops.push_back(clear(target,rect,flags,value));}
   else {const unsigned value=r();const bool blend=r()&1,dt=r()&1,dw=r()&1;ops.push_back(draw(target,tex,rect,value,blend,dt,dw));}
  }check(ops);
 }
 std::fprintf(stderr,"Removed draw count: %llu\n",removed);
 need(removed>0,"no dead work tested");
 std::printf("GPU_LIVENESS {\"sequences\":%llu,\"checks\":%llu,\"removedDraws\":%llu,\"pixels\":false}\n",sequences,checks,removed);
}
