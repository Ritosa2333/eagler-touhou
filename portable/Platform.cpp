#include "Platform.hpp"
#include "Window.hpp"
#include "Audio.hpp"
#include "../Game/Coop/Rollback.h"
#include "../Game/Game.h"
#include "../Game/Practice/Practice.h"
#include "../gameLib/CommonFunction/CommonFunctionSystem.h"
#include "../gameLib/Misc/md5.h"
#include "eagler/input/BrowserGamepad.hpp"
#include "../third_party/imgui/imgui.h"
#include <emscripten/html5.h>
#include <fstream>
#include <sstream>
#include <cmath>
#include <map>
#include <string>
namespace {EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context=0;bool physical[256]{};bool replayViewer=false;double touchX=0,touchY=0;bool touchUnlimited=false;}
namespace th5w {
bool gl::CGLContext::Init(HWND){EmscriptenWebGLContextAttributes attributes;emscripten_webgl_init_context_attributes(&attributes);attributes.alpha=false;attributes.depth=true;attributes.stencil=true;attributes.antialias=false;attributes.majorVersion=1;context=emscripten_webgl_create_context("#canvas",&attributes);if(context<=0)return false;MakeCurrent();return web::graphics::Initialize();}
bool gl::CGLContext::MakeCurrent(){return emscripten_webgl_make_context_current(context)==EMSCRIPTEN_RESULT_SUCCESS;}
bool gl::CGLContext::SwapBuffers(){return true;}
bool CGameWindow::Create(bool,int w,int h){m_width=w;m_height=h;m_pGLContext=new gl::CGLContext;return m_pGLContext->Init(nullptr);}
void CGameWindow::Destroy(){delete m_pGLContext;m_pGLContext=nullptr;if(context)emscripten_webgl_destroy_context(context);context=0;}
void CGameWindow::ToggleFullScreen(){EM_ASM({Module.canvas.requestFullscreen?.();});}
bool CPMDPlayer::LoadPMDData(unsigned char* data,int size){
 if(!CGame::s_pCurGame||!data||size<=0)return false;
 auto& game=*CGame::s_pCurGame;
 for(auto* dat:{&game.m_modDat,&game.m_th5Dat1,&game.m_th5Dat2})for(int i=0;i<dat->NFile();i++)if(dat->GetChildFileContentPointer(i)==data){web::MusicTrack(dat->GetChildFileName(i).c_str());return true;}
 // Music Room owns its additional MUSIC.DAT archive and selects the filename.
 return true;
}
void CCommonFunctionSystem::CreateConsoleWindow(){}
bool CCommonFunctionSystem::GetFileMD5(unsigned char* result,const char* path){FILE* file=fopen(path,"rb");if(!file)return false;MD5_CTX state;MD5Init(&state);unsigned char bytes[8192];size_t n;while((n=fread(bytes,1,sizeof(bytes),file)))MD5Update(&state,bytes,n);fclose(file);MD5Final(result,&state);return true;}
namespace web {
bool ReplayViewer(){return replayViewer;}
void SetKey(int key,bool down){if(key<0||key>=256)return;bool repeated=down&&physical[key];physical[key]=down;
 if(ImGui::GetCurrentContext())ImGui::GetIO().KeysDown[key]=down;
 practice::WindowMessage(nullptr,down?WM_KEYDOWN:WM_KEYUP,key,repeated?(1L<<30):0);
}
void TouchMotion(float& x,float& y,float speed){x=float(touchX);y=float(touchY);touchX=touchY=0;const float length=sqrtf(x*x+y*y);if(!touchUnlimited&&length>speed){x*=speed/length;y*=speed/length;}}
void ClearKeys(){touchX=touchY=0;memset(physical,0,sizeof(physical));practice::WindowMessage(nullptr,WM_KILLFOCUS,0,0);}
namespace {
void ReadKeys(BYTE* keys){memset(keys,0,256);if(replayViewer)return;for(int i=0;i<256;i++)keys[i]=physical[i]?128:0;
 eagler::input::ApplyGamepadVirtualKeys(keys,eagler::input::BrowserGamepadState());
}
}
// Legacy ImGui snapshots KeyMods in NewFrame(). Gameplay sampling may call
// back into the Launcher, so only publish UI keys before that frame begins.
void SampleUIKeys(){if(!ImGui::GetCurrentContext())return;BYTE keys[256];ReadKeys(keys);auto& io=ImGui::GetIO();
 for(int i=0;i<256;i++)io.KeysDown[i]=keys[i]!=0;
 io.KeyShift=keys[VK_SHIFT]||keys[VK_LSHIFT]||keys[VK_RSHIFT];io.KeyCtrl=keys[VK_CONTROL]||keys[VK_LCONTROL]||keys[VK_RCONTROL];
 io.KeyAlt=io.KeySuper=false;
}
void SampleKeys(BYTE* scans){BYTE keys[256];ReadKeys(keys);
 memset(scans,0,256);scans[DIK_UP]=keys[VK_UP];scans[DIK_DOWN]=keys[VK_DOWN];scans[DIK_LEFT]=keys[VK_LEFT];scans[DIK_RIGHT]=keys[VK_RIGHT];scans[DIK_Z]=keys['Z'];scans[DIK_X]=keys['X'];scans[DIK_LSHIFT]=keys[VK_SHIFT]|keys[VK_LSHIFT]|keys[VK_RSHIFT];scans[DIK_LCONTROL]=keys[VK_CONTROL]|keys[VK_LCONTROL]|keys[VK_RCONTROL];scans[DIK_ESCAPE]=keys[VK_ESCAPE];scans[DIK_RETURN]=keys[VK_RETURN];scans[DIK_Q]=keys['Q'];scans[DIK_R]=keys['R'];
}
void MusicTrack(const char* name){if(coop::QueueMusic(name))return;EM_ASM({Module.eaglerAudio?.select(UTF8ToString($0));},name);}
void MusicAction(int action,double value){if(coop::QueueMusicAction(action,value))return;EM_ASM({Module.eaglerAudio?.action($0,$1);},action,value);}
void PlaySound(int index){if(coop::QueueSound(index))return;EM_ASM({Module.eaglerAudio?.sound($0);},index);}
void ReportError(const char* text){EM_ASM({Module.eaglerError?.(UTF8ToString($0));},text);}
}}
namespace {
using Ini=std::map<std::string,std::map<std::string,std::string>>;
Ini ReadIni(const char* path){std::ifstream file(path);Ini data;std::string line,section;while(std::getline(file,line)){if(!line.empty()&&line.back()=='\r')line.pop_back();if(line.size()>1&&line[0]=='['&&line.back()==']')section=line.substr(1,line.size()-2);else{auto at=line.find('=');if(at!=std::string::npos)data[section][line.substr(0,at)]=line.substr(at+1);}}return data;}
}
int GetPrivateProfileIntA(const char* section,const char* key,int fallback,const char* path){auto data=ReadIni(path);auto s=data.find(section);if(s==data.end()||!s->second.count(key))return fallback;char* end;long n=strtol(s->second[key].c_str(),&end,10);return *end?fallback:int(n);}
BOOL WritePrivateProfileStringA(const char* section,const char* key,const char* value,const char* path){auto data=ReadIni(path);data[section][key]=value;std::ofstream file(path);for(const auto& s:data){file<<'['<<s.first<<"]\n";for(const auto& pair:s.second)file<<pair.first<<'='<<pair.second<<'\n';}return file.good();}

extern "C" EMSCRIPTEN_KEEPALIVE void th05_touch_motion(double x,double y,int unlimited){touchX=x*16;touchY=y*16;touchUnlimited=unlimited!=0;}

extern "C" EMSCRIPTEN_KEEPALIVE void th05_viewer(int enabled){replayViewer=enabled!=0;}
