#pragma once
#include "Platform.hpp"
// Original replay scan codes are part of the TH05 input format.
enum{DIK_ESCAPE=1,DIK_Q=16,DIK_R=19,DIK_RETURN=28,DIK_LCONTROL=29,DIK_LSHIFT=42,DIK_Z=44,DIK_X=45,DIK_UP=200,DIK_DOWN=208,DIK_LEFT=203,DIK_RIGHT=205};
namespace th5w {
class CKeyInputDI {
 BYTE m_keyState[256]{};
public:
 bool Initialize(){return true;}
 bool UpdateKeyState(){web::SampleKeys(m_keyState);return true;}
 unsigned char GetKeyState(int code)const{return code>=0&&code<256?m_keyState[code]>>7:0;}
};
}
