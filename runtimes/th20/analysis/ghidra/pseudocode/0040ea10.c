/* Entry: 0x0040ea10; symbol: operator+=; body bytes: 29 */

/* Library Function - Single Match
    public: class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >
   & __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char>
   >::operator+=(char)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> * __thiscall
std::basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_>::operator+=
          (basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *this,
          char param_1)

{
  push_back(this,param_1);
  return this;
}

