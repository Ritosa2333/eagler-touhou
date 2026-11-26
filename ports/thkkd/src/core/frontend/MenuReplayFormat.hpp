// 正常菜单可达的整数与字符格式；所有参数来自显式原32位栈字。
#pragma once
#include "core/frontend/MenuReplayServices.hpp"
#include <algorithm>
#include <string>
#include <string_view>
namespace kaikeidou {
inline std::uint32_t format_menu_replay_integer(MenuReplayFormatMemory& memory,const MenuReplayFormatRequest& request){
 std::uint32_t cursor=request.format,argument=request.arguments;std::size_t written=0;
 const auto put=[&](std::uint8_t byte){if(written>=request.capacity)throw MenuReplayFormatFault(MenuReplayFormatFaultKind::target_overflow,request.target+static_cast<std::uint32_t>(written),written);memory.byte(request.target+static_cast<std::uint32_t>(written),byte);++written;};
 const auto read_argument=[&](){try{const auto value=memory.argument(argument);argument+=4u;return value;}catch(const MenuReplayFormatFault& fault){throw MenuReplayFormatFault(fault.kind,fault.address,written);}};
 for(;;){auto ch=memory.byte(cursor++);if(ch==0)break;if(ch!='%'){put(ch);continue;}ch=memory.byte(cursor++);if(ch=='%'){put('%');continue;}
  bool left=false,zero=false,extended=false;
  while(ch=='-'||ch=='0'||ch=='+'||ch==' '||ch=='#'){if(ch=='-')left=true;else if(ch=='0')zero=true;else extended=true;ch=memory.byte(cursor++);}
  std::uint32_t width=0;while(ch>='0'&&ch<='9'){const auto digit=std::uint32_t(ch-'0');if(width>0x19999999u)throw MenuReplayFormatFault(MenuReplayFormatFaultKind::unsupported_conversion,cursor-1,written);width=width*10u+digit;ch=memory.byte(cursor++);}
  if(ch=='*'){(void)read_argument();extended=true;ch=memory.byte(cursor++);}
  if(ch=='.'){extended=true;ch=memory.byte(cursor++);if(ch=='*'){(void)read_argument();ch=memory.byte(cursor++);}else while(ch>='0'&&ch<='9')ch=memory.byte(cursor++);}
  unsigned argument_words=1;if(ch=='l'||ch=='h'){const auto length=ch;ch=memory.byte(cursor++);if(ch==length){extended=true;argument_words=length=='l'?2u:1u;ch=memory.byte(cursor++);}if(length=='h'||(length=='l'&&ch=='c'))extended=true;}
  if(ch=='I'){extended=true;ch=memory.byte(cursor++);if(ch=='6'){ch=memory.byte(cursor++);if(ch=='4'){argument_words=2;ch=memory.byte(cursor++);}}}
  if(ch==0)break;
  if(ch!='d'&&ch!='c'){
   const std::string_view conversions="iouxXpsnSCeEfFgGaA";
   if(conversions.find(static_cast<char>(ch))==std::string_view::npos){put(ch);continue;}
   if(std::string_view("eEfFgGaA").find(static_cast<char>(ch))!=std::string_view::npos)argument_words=2;
   for(unsigned i=0;i<argument_words;++i)(void)read_argument();
   throw MenuReplayFormatFault(MenuReplayFormatFaultKind::unsupported_conversion,cursor-1,written);
  }
  const auto value=read_argument();std::string text;
  for(unsigned i=1;i<argument_words;++i)(void)read_argument();
  if(extended)throw MenuReplayFormatFault(MenuReplayFormatFaultKind::unsupported_conversion,cursor-1,written);
  if(ch=='c')text.push_back(static_cast<char>(value&255u));else{const auto signed_value=value>>31?std::int64_t(value)-0x100000000ll:std::int64_t(value);text=std::to_string(signed_value);}
  const auto pad=width>text.size()?width-static_cast<std::uint32_t>(text.size()):0;
  std::size_t begin=0;if(!left&&zero&&ch=='d'&&!text.empty()&&text[0]=='-'){put('-');begin=1;}
  if(!left)for(std::uint32_t i=0;i<pad;++i)put(zero&&ch=='d'?'0':' ');
  for(std::size_t i=begin;i<text.size();++i)put(static_cast<std::uint8_t>(text[i]));
  if(left)for(std::uint32_t i=0;i<pad;++i)put(' ');
 }
 put(0);return static_cast<std::uint32_t>(written-1);
}
}
