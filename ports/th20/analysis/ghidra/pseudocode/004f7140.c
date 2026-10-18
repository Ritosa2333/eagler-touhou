/* Entry: 0x004f7140; symbol: `scalar_deleting_destructor'; body bytes: 46 */

/* Library Function - Single Match
    public: virtual void * __thiscall std::basic_stringbuf<char,struct std::char_traits<char>,class
   std::allocator<char> >::`scalar deleting destructor'(unsigned int)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void * __thiscall
std::basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_>::
_scalar_deleting_destructor_
          (basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_> *this,
          uint param_1)

{
  FID_conflict__numpunct<wchar_t>((undefined4 *)this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return this;
}

