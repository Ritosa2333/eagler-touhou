#include "TextRaster.hpp"
namespace th075 {
bool GlyphBank::load(const void* data,std::size_t size){
 error.clear();entries.clear();storage.clear();ByteReader in(data,size);const auto* magic=in.take(4);if(!magic||std::memcmp(magic,"GG75",4)||in.dword()!=1){error="Invalid original glyph bank";return false;}const u32 count=in.dword();if(count>size/20||!in.available(std::size_t(count)*20)){error="Truncated glyph directory";return false;}
 for(u32 i=0;i<count;i++){const u16 code=in.word(),font=in.word(),width=in.word(),height=in.word();i16 left=i16(in.word()),top=i16(in.word());const u16 stride=in.word();in.word();const u32 offset=in.dword();if((stride&&stride<width)||offset<12+count*20||offset>size||std::size_t(stride)*height>size-offset){error="Invalid original glyph payload";entries.clear();return false;}entries.emplace(std::make_pair(font,code),Entry{u16(stride?width:0),u16(stride?height:0),stride,left,top,offset});}
 const auto* p=static_cast<const u8*>(data);storage.assign(p,p+size);return true;
}
bool GlyphBank::glyph(u16 height,u16 code,GlyphOutline& out)const{
 auto found=entries.find({height,code});if(found==entries.end()&&code>=0xff00)found=entries.find({height,u16(code&255)});if(found==entries.end())return false;const auto& e=found->second;out={e.width,e.height,e.stride,e.left,e.top,storage.data()+e.offset};return true;
}
namespace {
bool leading(u8 n,bool chinese){return chinese?(n>=0x81&&n<=0xfe):((n>=0x81&&n<0xa0)||(n>=0xe0&&n<0xff));}
u8 decode(const std::string& text,std::size_t at,u16& code,bool chinese=false){if(at>=text.size()){code=0;return 1;}const u8 first=u8(text[at]);if(leading(first,chinese)){code=u16(first)<<8|(at+1<text.size()?u8(text[at+1]):0);return 2;}code=u16(i16(i8(first)));return 1;}
u8 hex(u8 n){return n>='0'&&n<='9'?u8(n-'0'):n>='a'&&n<='f'?u8(n-'a'+10):n>='A'&&n<='F'?u8(n-'A'+10):0;}
}
bool TextRaster::render(const std::string& text,const TextRasterOptions& o,std::vector<u8>& out){
 error.clear();if(!o.fontHeight||!o.width||!o.height||o.width>4096||o.height>4096||(o.bytesPerPixel!=2&&o.bytesPerPixel!=4)){error="Invalid text surface";return false;}out.assign(std::size_t(o.width)*o.height*o.bytesPerPixel,0);
 i32 x=1,y=1;u8 red=o.red,green=o.green,blue=o.blue;i8 lines=i8(o.lineLimit);std::size_t at=0,limit=text.size();bool chinese=false;
 while(at<limit){if(chineseBank&&(u8(text[at])==1||u8(text[at])==2)){chinese=u8(text[at++])==1;continue;}u16 code;const u8 bytes=decode(text,at,code,chinese);at+=bytes;if(o.bytesPerPixel==4&&code==0)break;
  if(o.bytesPerPixel==4&&code==92){u16 command;const u8 width=decode(text,at,command,chinese);if(width<2){if(command=='c'&&at+7<=text.size()){red=u8(hex(u8(text[at+1]))*16+hex(u8(text[at+2])));green=u8(hex(u8(text[at+3]))*16+hex(u8(text[at+4])));blue=u8(hex(u8(text[at+5]))*16+hex(u8(text[at+6])));at+=7;}else if(command=='e'){red=o.red;green=o.green;blue=o.blue;at++;}else if(command=='n'){x=1;y+=i32(o.fontHeight)+o.lineSpacing;lines=i8(u8(lines)-1);if(lines<1)limit=0;else at++;}}continue;}
  const i32 advance=i32(o.fontHeight/2)*bytes+o.characterSpacing;const bool space=code==32||code==(chinese?0xa1a1:0x8140);
  GlyphOutline glyph;if(!space&&!(chinese?*chineseBank:bank).glyph(u16(o.fontHeight),code,glyph)){error="Missing original glyph "+std::to_string(code)+" at font "+std::to_string(o.fontHeight);return false;}
  if((!space||o.bytesPerPixel==4)&&i32(o.bytesPerPixel==4?o.wrapWidth:o.width)-x<advance){x=o.bytesPerPixel==4?1:0;if(i32(o.height)-y<i32(o.fontHeight)+o.lineSpacing)break;y+=i32(o.fontHeight)+o.lineSpacing;if(o.bytesPerPixel==4){lines=i8(u8(lines)-1);if(lines<1)break;}}
  if(!space)for(u32 column=0;column<glyph.width;column++)for(u32 row=0;row<glyph.height;row++){const u8 coverage=glyph.coverage[row*glyph.stride+column];if(!coverage)continue;const i32 px=x+glyph.left+i32(column),py=y+glyph.top+i32(row);if(px<0||py<0||px>=i32(o.width)||py>=i32(o.height))continue;const std::size_t offset=(std::size_t(py)*o.width+u32(px))*o.bytesPerPixel;
   if(o.bytesPerPixel==4){const i32 distance=glyph.top+i32(row);const auto color=[&](u8 n){return u32(255-((distance*(255-i32(n))/i32(o.fontHeight))&255));};const u32 value=u32(coverage)*0x0e000000|0x03000000|color(red)<<16|color(green)<<8|color(blue);std::memcpy(out.data()+offset,&value,4);}
   else{const u16 value=u16((u16(coverage)*2-1)*0x421|0x8000);std::memcpy(out.data()+offset,&value,2);}
  }x+=advance;
 }return true;
}
}
