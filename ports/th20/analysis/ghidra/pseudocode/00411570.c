/* Entry: 0x00411570; symbol: SetNativeImage; body bytes: 22 */

/* Library Function - Single Match
    protected: void __thiscall Gdiplus::Image::SetNativeImage(class Gdiplus::GpImage *)
   
   Library: Visual Studio 2010 Debug */

void __thiscall Gdiplus::Image::SetNativeImage(Image *this,GpImage *param_1)

{
  *(GpImage **)(this + 4) = param_1;
  return;
}

