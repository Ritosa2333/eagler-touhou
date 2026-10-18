/* Entry: 0x004e69d0; symbol: DelayUpdateFrameTitle; body bytes: 32 */

/* Library Function - Single Match
    public: void __thiscall CFrameWnd::DelayUpdateFrameTitle(void)
   
   Library: Visual Studio 2010 Debug */

void __thiscall CFrameWnd::DelayUpdateFrameTitle(CFrameWnd *this)

{
  *(uint *)(this + 0x25c) = *(uint *)(this + 0x25c) | 2;
  return;
}

