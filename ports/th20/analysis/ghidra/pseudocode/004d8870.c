/* Entry: 0x004d8870; symbol: Image; body bytes: 46 */

/* Library Function - Single Match
    protected: __thiscall Gdiplus::Image::Image(class Gdiplus::GpImage *,enum Gdiplus::Status)
   
   Library: Visual Studio 2010 Debug */

Image * __thiscall Gdiplus::Image::Image(Image *this,GpImage *param_1,Status param_2)

{
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__00571914;
  SetNativeImage(this,param_1);
  *(Status *)(this + 8) = param_2;
  return this;
}

