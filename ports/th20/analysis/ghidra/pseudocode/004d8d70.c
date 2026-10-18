/* Entry: 0x004d8d70; symbol: ~Image; body bytes: 33 */

/* Library Function - Single Match
    public: virtual __thiscall Gdiplus::Image::~Image(void)
   
   Library: Visual Studio 2010 Debug */

void __thiscall Gdiplus::Image::~Image(Image *this)

{
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__00571914;
  GdipDisposeImage(*(undefined4 *)(this + 4));
  return;
}

