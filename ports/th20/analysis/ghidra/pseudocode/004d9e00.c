/* Entry: 0x004d9e00; symbol: SetStatus; body bytes: 36 */

/* Library Function - Single Match
    protected: enum Gdiplus::Status __thiscall Gdiplus::Image::SetStatus(enum Gdiplus::Status)const 
   
   Library: Visual Studio 2010 Debug */

Status __thiscall Gdiplus::Image::SetStatus(Image *this,Status param_1)

{
  if (param_1 != 0) {
    *(Status *)(this + 8) = param_1;
  }
  return param_1;
}

