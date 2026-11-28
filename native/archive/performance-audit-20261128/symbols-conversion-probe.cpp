#include <windows.h>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>
#include "flutter/fml/string_conversion.h"
#include "conversion-inputs.h"
__declspec(noinline) std::u16string WindowsUtf16(const std::string& input) {
  if(input.empty()) return {};
  int count=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,input.data(),static_cast<int>(input.size()),nullptr,0);
  if(!count) throw std::runtime_error("UTF-8 validation failed");
  std::u16string result(count,u'\0');
  static_assert(sizeof(wchar_t)==sizeof(char16_t));
  if(MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,input.data(),static_cast<int>(input.size()),reinterpret_cast<wchar_t*>(result.data()),count)!=count) throw std::runtime_error("Conversion failed");
  return result;
}
unsigned long long CpuTime() {
  FILETIME c,e,k,u;GetProcessTimes(GetCurrentProcess(),&c,&e,&k,&u);
  ULARGE_INTEGER a,b;a.LowPart=k.dwLowDateTime;a.HighPart=k.dwHighDateTime;b.LowPart=u.dwLowDateTime;b.HighPart=u.dwHighDateTime;return a.QuadPart+b.QuadPart;
}
int main() {
  SetPriorityClass(GetCurrentProcess(),BELOW_NORMAL_PRIORITY_CLASS);
  for(const auto& input:inputs) {
    if(fml::Utf8ToUtf16(input.first)!=input.second || WindowsUtf16(input.first)!=input.second) return 2;
  }
  printf("{\"correctness\":true,\"strings\":%zu}\n",inputs.size());
  LARGE_INTEGER frequency;QueryPerformanceFrequency(&frequency);
  volatile size_t sink=0;
  const int iterations=1000;
  for(int round=0;round<3;++round) for(int slot=0;slot<2;++slot) {
    bool original=((round+slot)%2)==0;
    LARGE_INTEGER begin,end;QueryPerformanceCounter(&begin);
    auto before=CpuTime();
    for(int i=0;i<iterations;++i) for(const auto& input:inputs) {
      auto value=original?fml::Utf8ToUtf16(input.first):WindowsUtf16(input.first);
      sink=sink+value.size();
    }
    auto after=CpuTime();QueryPerformanceCounter(&end);
    printf("{\"round\":%d,\"method\":\"%s\",\"calls\":%zu,\"wallMs\":%.6f,\"cpuMs\":%.6f}\n",round,original?"pinned-fml-source":"windows-api",inputs.size()*iterations,1000.0*(end.QuadPart-begin.QuadPart)/frequency.QuadPart,(after-before)/10000.0);
    fflush(stdout);
  }
  return sink?0:3;
}
