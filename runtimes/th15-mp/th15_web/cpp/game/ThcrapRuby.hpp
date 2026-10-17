#pragma once
#include <string>
namespace th15 {
// thcrap_tsa/src/layout.cpp: BP_ruby_offset and ruby_offset_half.
// Tabs delimit both parameters so commas inside the strings remain literal.
struct ThcrapRuby {std::string begin,base,annotation;};
inline bool parse_thcrap_ruby(const std::string& text,ThcrapRuby& out){
 if(text.size()<7||text[0]!='|'||text[1]!='\t')return false;
 const auto first=text.find('\t',2);
 if(first==std::string::npos||first+2>=text.size()||text[first+1]!=','||text[first+2]!='\t')return false;
 const auto second=text.find('\t',first+3);
 if(second==std::string::npos||second+1>=text.size()||text[second+1]!=',')return false;
 out={text.substr(2,first-2),text.substr(first+3,second-first-3),text.substr(second+2)};return true;
}
inline int thcrap_ruby_offset(int begin,int base,int annotation){
 return (begin*2+base-annotation+1)/2+4; // TH15-specific +4 at full pixel precision.
}
inline float thcrap_bubble_x(float x,int width,bool right){
 // th06_msg.cpp::box_end: logical 640px screen, 512px text cap, 19px padding.
 const int capped=width<512?width:512;const float length=float(capped+19);
 const float overhang=right?x-length:x+length-640.f;
 return (right?overhang<0:overhang>0)?x-overhang:x;
}
}
