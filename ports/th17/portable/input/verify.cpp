#include "MotionTrack.hpp"
using touhou::input::MotionTrack;
#define EX(name) extern "C" __attribute__((export_name(name)))
EX("verify") int verify(){
 MotionTrack a,b;float x=0,y=0;
 a.begin(1,true,false,true);a.record(1,false,0,0);a.record(1,true,2.25f,-1.5f);a.record(1,false,0,0);a.record(1,true,0,0);
 a.begin(2,false,false,true);a.record(2,true,-200000,400000);
 auto tail=a.trailer(10);if(tail.empty()||!b.load(tail.data(),tail.size(),10))return 1;
 b.begin(1,false,true,false);if(b.playback(1,x,y)||!b.playback(1,x,y)||x!=2.25f||y!=-1.5f||b.playback(1,x,y)||!b.playback(1,x,y)||x||y)return 2;
 b.begin(2,false,true,false);if(!b.playback(2,x,y)||x!=-200000||y!=400000)return 3;
 if(b.load(tail.data(),tail.size(),8)||b.used())return 4;
 tail[40]^=1;if(b.load(tail.data(),tail.size(),10)||b.used())return 5;tail[40]^=1;
 // A valid checksum must not make impossible counts or duplicate ticks valid.
 auto broken=tail;const std::uint32_t bad=600001;std::memcpy(broken.data()+8,&bad,4);auto length=broken.size()-24;auto hash=MotionTrack::hash(broken.data(),length);std::memcpy(broken.data()+length+20,&hash,4);
 if(b.load(broken.data(),broken.size(),10)||b.used())return 6;
 a.begin(1,true,false,true);if(a.used())return 7;a.record(1,true,0,std::nanf(""));if(!a.invalid||!a.trailer(10).empty())return 8;
 float dx=10,dy=10;touhou::input::limit_vector(dx,dy,4);if(std::abs(dx*dx+dy*dy-16)>0.00001)return 9;
 return 0;
}
