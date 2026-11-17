#pragma once
#include <vector>
#include <fstream>
#include <cstdint>
#include <cstdlib>
namespace th5w::web {
struct Bitmap {int width=0,height=0;std::vector<unsigned char> rgba;};
inline Bitmap ReadBitmap(const char* path){
 std::ifstream file(path,std::ios::binary);std::vector<unsigned char> b((std::istreambuf_iterator<char>(file)),{});Bitmap out;
 auto u16=[&](size_t at)->uint16_t{return b[at]|b[at+1]<<8;};auto u32=[&](size_t at)->uint32_t{return u16(at)|uint32_t(u16(at+2))<<16;};
 if(b.size()<54||b[0]!='B'||b[1]!='M'||u32(14)<40||u32(30)!=0)return out;
 int w=int(u32(18)),signedHeight=int(u32(22));unsigned bits=u16(28);size_t offset=u32(10),palette=14+u32(14);
 if(w<=0||w>8192||!signedHeight||signedHeight< -8192||signedHeight>8192||(bits!=1&&bits!=4&&bits!=8&&bits!=24&&bits!=32))return out;
 int h=abs(signedHeight);size_t stride=((size_t(w)*bits+31)/32)*4;
 if(offset>b.size()||stride*size_t(h)>b.size()-offset||bits<=8&&(palette+(size_t(1)<<bits)*4>offset))return out;
 out.width=w;out.height=h;out.rgba.resize(w*h*4);
 for(int y=0;y<h;y++)for(int x=0;x<w;x++){
  const auto* row=b.data()+offset+(signedHeight>0?y:h-1-y)*stride;auto* d=out.rgba.data()+(y*w+x)*4;const unsigned char* c;
  if(bits<=8){unsigned n=bits==1?(row[x/8]>>(7-x%8))&1:bits==4?(row[x/2]>>((1-x%2)*4))&15:row[x];c=b.data()+palette+n*4;}else c=row+x*(bits/8);
  d[0]=c[2];d[1]=c[1];d[2]=c[0];d[3]=255;
 }
 return out;
}
}
