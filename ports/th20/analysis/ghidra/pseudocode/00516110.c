/* Entry: 0x00516110; symbol: ~basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_>; body bytes: 38 */

/* Library Function - Single Match
    public: virtual __thiscall std::basic_stringbuf<char,struct std::char_traits<char>,class
   std::allocator<char> >::~basic_stringbuf<char,struct std::char_traits<char>,class
   std::allocator<char> >(void)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __thiscall
std::basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_>::
~basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_>
          (basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_> *this)

{
  *(undefined ***)this = &PTR__scalar_deleting_destructor__00574de0;
  FUN_0051af90((basic_streambuf<char,struct_std::char_traits<char>_> *)this);
  basic_streambuf<char,struct_std::char_traits<char>_>::
  ~basic_streambuf<char,struct_std::char_traits<char>_>
            ((basic_streambuf<char,struct_std::char_traits<char>_> *)this);
  return;
}

