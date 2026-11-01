// Offline PMDWin renderer for the original MUSIC.DAT's TH01-04 arrangements.
// This utility runs while preparing content; the browser receives Ogg audio.
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <vector>
template<class T>T function(HMODULE dll,const char* name){auto value=GetProcAddress(dll,name);if(!value){std::fprintf(stderr,"Missing PMDWin export: %s\n",name);std::exit(2);}return reinterpret_cast<T>(value);}
int main(int argc,char** argv){
 if(argc!=4)return 2;
 auto dll=LoadLibraryA(argv[1]);if(!dll)return 3;
 auto init=function<bool(WINAPI*)(char*)>(dll,"pmdwininit");auto rate=function<void(WINAPI*)(int)>(dll,"setpcmrate");
 auto load=function<int(WINAPI*)(char*)>(dll,"music_load");auto start=function<void(WINAPI*)()>(dll,"music_start");
 auto pcm=function<void(WINAPI*)(short*,int)>(dll,"getpcmdata");auto loops=function<int(WINAPI*)()>(dll,"getloopcount");
 auto length=function<bool(WINAPI*)(char*,int*,int*)>(dll,"getlength");
 if(!init(nullptr))return 4;rate(44100);int duration=0,loop=0;length(argv[2],&duration,&loop);
 if(load(argv[2])>2)return 5;start();
 std::vector<short> audio;short block[512*2];int first=-1,second=-1;
 const int maximum=44100*600;
 while(int(audio.size()/2)<maximum){pcm(block,512);audio.insert(audio.end(),block,block+1024);const int count=loops();
  if(count>=1&&first<0)first=int(audio.size()/2);if(count>=2){second=int(audio.size()/2);break;}
  if(count<0)break;
 }
 int end=int(audio.size()/2),begin=0;
 if(first>=0&&second>first){begin=first-(second-first);end=first;}
 if(begin<0)begin=0;
 if(end>=maximum||end<4410)return 6;
 auto out=std::fopen(argv[3],"wb");if(!out)return 7;std::fwrite(audio.data(),4,end,out);std::fclose(out);
 std::printf("{\"frames\":%d,\"loopStart\":%.9f,\"loopEnd\":%.9f,\"lengthMs\":%d,\"loopMs\":%d}\n",end,begin/44100.,end/44100.,duration,loop);return 0;
}
