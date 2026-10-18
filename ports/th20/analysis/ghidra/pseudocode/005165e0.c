/* Entry: 0x005165e0; symbol: `scalar_deleting_destructor'; body bytes: 58 */

/* Library Function - Single Match
    public: virtual void * __thiscall std::basic_stringstream<char,struct
   std::char_traits<char>,class std::allocator<char> >::`scalar deleting destructor'(unsigned int)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void * __thiscall
std::basic_stringstream<char,struct_std::char_traits<char>,class_std::allocator<char>_>::
_scalar_deleting_destructor_
          (basic_stringstream<char,struct_std::char_traits<char>,class_std::allocator<char>_> *this,
          uint param_1)

{
  _vbase_destructor_(this + -0x60);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this + -0x60);
  }
  return this + -0x60;
}

