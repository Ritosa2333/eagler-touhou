#include "LegacyTextFormat.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
namespace th15 {
std::string legacy_decimal_tenth(float value){
 if(!std::isfinite(value))return std::signbit(value)?"-1.$":"1.$";
 double number=value;const double scaled=std::fabs(number)*10.;if(scaled-std::floor(scaled)==.5)number=std::nextafter(number,std::signbit(value)?-std::numeric_limits<double>::infinity():std::numeric_limits<double>::infinity());
 char line[128];
 if(std::fabs(number)>=1e17){
  // VC2012 retains at most 17 significant decimal digits and pads the
  // remaining integer places with zero, even for a promoted SSE float.
  std::snprintf(line,sizeof line,"%.16e",number);std::string digits;const char* p=line;const bool negative=*p=='-';if(negative)p++;for(;*p&&*p!='e';p++)if(*p!='.')digits+=*p;const i32 places=std::atoi(p+1)+1;if(places>i32(digits.size()))digits.append(u32(places-digits.size()),'0');return (negative?"-":"")+digits+".0";
 }
 std::snprintf(line,sizeof line,"%2.1f",number);return line;
}
}
