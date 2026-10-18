/* Entry: 0x005160d0; symbol: ~basic_streambuf<char,struct_std::char_traits<char>_>; body bytes: 59 */

/* Library Function - Single Match
    public: virtual __thiscall std::basic_streambuf<char,struct std::char_traits<char>
   >::~basic_streambuf<char,struct std::char_traits<char> >(void)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __thiscall
std::basic_streambuf<char,struct_std::char_traits<char>_>::
~basic_streambuf<char,struct_std::char_traits<char>_>
          (basic_streambuf<char,struct_std::char_traits<char>_> *this)

{
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__00574da0;
  if (*(void **)(this + 0x34) != (void *)0x0) {
    FUN_00516780(*(void **)(this + 0x34),1);
  }
  return;
}

