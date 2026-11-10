#pragma once
#include "Platform.hpp"
#include "Graphics.hpp"
namespace th5w {
namespace gl { class CGLContext {
public:
 bool Init(HWND);bool MakeCurrent();bool SwapBuffers();
}; }
class CGameWindow {
public:
 bool m_bActive=true,m_bFullScr=false;HWND m_hWnd=nullptr;
 int m_width=640,m_height=480;gl::CGLContext* m_pGLContext=nullptr;
 bool Create(bool,int,int);void Destroy();void ToggleFullScreen();
};
}
