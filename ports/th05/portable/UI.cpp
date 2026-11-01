#include "Platform.hpp"
#include "Graphics.hpp"
#include "../third_party/imgui/imgui.h"
namespace th5w::web {
static GLuint atlas=0;static double previous=0;
void InitializeUI(){auto& io=ImGui::GetIO();io.DisplaySize=ImVec2(640,480);io.BackendPlatformName="uth05-browser";io.BackendRendererName="uth05-gles2";
 const int map[]={VK_TAB,VK_LEFT,VK_RIGHT,VK_UP,VK_DOWN,VK_PRIOR,VK_NEXT,VK_HOME,VK_END,45,VK_DELETE,VK_BACK,VK_SPACE,VK_RETURN,VK_ESCAPE,13,'A','C','V','X','Y','Z'};
 for(int i=0;i<ImGuiKey_COUNT&&i<int(sizeof(map)/sizeof(map[0]));i++)io.KeyMap[i]=map[i];
 unsigned char* data;int w,h;io.Fonts->GetTexDataAsRGBA32(&data,&w,&h);glGenTextures(1,&atlas);glBindTexture(GL_TEXTURE_2D,atlas);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,data);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);io.Fonts->SetTexID((ImTextureID)(uintptr_t)atlas);previous=emscripten_get_now();
}
void ShutdownUI(){if(atlas)glDeleteTextures(1,&atlas);atlas=0;}
void BeginUI(){BYTE scans[256];SampleKeys(scans);double now=emscripten_get_now();ImGui::GetIO().DeltaTime=float((now-previous)/1000);if(ImGui::GetIO().DeltaTime<=0)ImGui::GetIO().DeltaTime=1.f/56;previous=now;}
void EndUI(){auto* data=ImGui::GetDrawData();if(!data)return;
 glPushAttrib(GL_ALL_ATTRIB_BITS);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);glDisable(GL_DEPTH_TEST);glDisable(GL_POLYGON_STIPPLE);glEnable(GL_TEXTURE_2D);glEnable(GL_SCISSOR_TEST);glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);glColorMask(1,1,1,1);
 glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glOrtho(0,640,480,0,-1,1);glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
 for(int list=0;list<data->CmdListsCount;list++){const auto* commands=data->CmdLists[list];for(const auto& cmd:commands->CmdBuffer){if(cmd.UserCallback){if(cmd.UserCallback!=ImDrawCallback_ResetRenderState)cmd.UserCallback(commands,&cmd);continue;}glScissor(int(cmd.ClipRect.x),int(480-cmd.ClipRect.w),int(cmd.ClipRect.z-cmd.ClipRect.x),int(cmd.ClipRect.w-cmd.ClipRect.y));glBindTexture(GL_TEXTURE_2D,(GLuint)(uintptr_t)cmd.TextureId);glBegin(GL_TRIANGLES);for(unsigned n=0;n<cmd.ElemCount;n++){const auto& vertex=commands->VtxBuffer[commands->IdxBuffer[cmd.IdxOffset+n]+cmd.VtxOffset];const auto c=vertex.col;glColor4f((c&255)/255.f,((c>>8)&255)/255.f,((c>>16)&255)/255.f,(c>>24)/255.f);glTexCoord2f(vertex.uv.x,vertex.uv.y);glVertex2f(vertex.pos.x,vertex.pos.y);}glEnd();}}
 glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glPopAttrib();
}
}
