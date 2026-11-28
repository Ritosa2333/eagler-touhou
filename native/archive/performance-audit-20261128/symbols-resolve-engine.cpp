#include <windows.h>
#include <dbghelp.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#pragma comment(lib,"dbghelp.lib")
int main() {
  const char* image="D:/workspace/eagler/toolchains/flutter-stable/bin/cache/artifacts/engine/windows-x64-release/flutter_windows.dll";
  const char* search="D:/workspace/eagler/toolchains/flutter-stable/bin/cache/artifacts/engine/windows-x64-release";
  HANDLE process=GetCurrentProcess();
  SymSetOptions(SYMOPT_UNDNAME|SYMOPT_LOAD_LINES|SYMOPT_EXACT_SYMBOLS|SYMOPT_FAIL_CRITICAL_ERRORS|SYMOPT_NO_PROMPTS|SYMOPT_IGNORE_NT_SYMPATH);
  if(!SymInitialize(process,search,FALSE)) {printf("ERROR\tinitialize\t%lu\n",GetLastError());return 1;}
  DWORD64 base=SymLoadModuleEx(process,nullptr,image,"flutter_windows",0x180000000ULL,0,nullptr,0);
  if(!base) {printf("ERROR\tload\t%lu\n",GetLastError());SymCleanup(process);return 2;}
  IMAGEHLP_MODULE64 module{};module.SizeOfStruct=sizeof(module);
  if(!SymGetModuleInfo64(process,base,&module)) {printf("ERROR\tinfo\t%lu\n",GetLastError());SymCleanup(process);return 3;}
  printf("META\t%u\t%d\t%d\t%lu\t%s\t%08lx-%04x-%04x-",module.SymType,module.PdbUnmatched,module.DbgUnmatched,module.PdbAge,module.LoadedPdbName,module.PdbSig70.Data1,module.PdbSig70.Data2,module.PdbSig70.Data3);
  for(auto byte:module.PdbSig70.Data4) printf("%02x",byte);
  printf("\n");
  if(module.SymType!=SymPdb || module.PdbUnmatched) {SymCleanup(process);return 4;}
  std::string offset;
  while(std::cin>>offset) {
    DWORD64 address=base+std::strtoull(offset.c_str(),nullptr,16),displacement=0;
    alignas(SYMBOL_INFO) unsigned char storage[sizeof(SYMBOL_INFO)+4096]{};
    auto* symbol=reinterpret_cast<SYMBOL_INFO*>(storage);
    symbol->SizeOfStruct=sizeof(SYMBOL_INFO);symbol->MaxNameLen=4095;
    if(!SymFromAddr(process,address,&displacement,symbol)) {
      printf("MISS\t%s\t%lu\n",offset.c_str(),GetLastError());continue;
    }
    IMAGEHLP_LINE64 line{};line.SizeOfStruct=sizeof(line);DWORD line_displacement=0;
    BOOL has_line=SymGetLineFromAddr64(process,address,&line_displacement,&line);
    printf("SYMBOL\t%s\t%s\t%llu\t%lu\t%s\t%lu\n",offset.c_str(),symbol->Name,displacement,symbol->Size,has_line?line.FileName:"",has_line?line.LineNumber:0);
  }
  SymCleanup(process);return 0;
}
