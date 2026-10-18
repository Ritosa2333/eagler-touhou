/* Entry: 0x004d8830; symbol: GdiplusStartupInput; body bytes: 52 */

/* Library Function - Single Match
    public: __thiscall Gdiplus::GdiplusStartupInput::GdiplusStartupInput(void (__stdcall*)(enum
   Gdiplus::DebugEventLevel,char *),int,int)
   
   Library: Visual Studio 2010 Debug */

GdiplusStartupInput * __thiscall
Gdiplus::GdiplusStartupInput::GdiplusStartupInput
          (GdiplusStartupInput *this,_func_void_DebugEventLevel_char_ptr *param_1,int param_2,
          int param_3)

{
  *(undefined4 *)this = 1;
  *(_func_void_DebugEventLevel_char_ptr **)(this + 4) = param_1;
  *(int *)(this + 8) = param_2;
  *(int *)(this + 0xc) = param_3;
  return this;
}

