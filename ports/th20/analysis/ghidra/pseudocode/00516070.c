/* Entry: 0x00516070; symbol: ~basic_iostream<char,struct_std::char_traits<char>_>; body bytes: 82 */

/* Library Function - Single Match
    public: virtual __thiscall std::basic_iostream<char,struct std::char_traits<char>
   >::~basic_iostream<char,struct std::char_traits<char> >(void)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void __thiscall
std::basic_iostream<char,struct_std::char_traits<char>_>::
~basic_iostream<char,struct_std::char_traits<char>_>
          (basic_iostream<char,struct_std::char_traits<char>_> *this)

{
  *(undefined ***)(this + *(int *)(*(int *)(this + -0x60) + 4) + -0x60) = &PTR_LAB_00574e20;
  *(int *)(this + *(int *)(*(int *)(this + -0x60) + 4) + -100) =
       *(int *)(*(int *)(this + -0x60) + 4) + -0x60;
  basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_>::
  ~basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_>
            ((basic_stringbuf<char,struct_std::char_traits<char>,class_std::allocator<char>_> *)
             (this + -0x50));
  basic_istream<char,struct_std::char_traits<char>_>::
  ~basic_istream<char,struct_std::char_traits<char>_>
            ((basic_istream<char,struct_std::char_traits<char>_> *)(this + -0x48));
  return;
}

