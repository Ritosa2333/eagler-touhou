/* Entry: 0x00516530; symbol: `vbase_destructor'; body bytes: 34 */

/* Library Function - Single Match
    public: void __thiscall std::basic_stringstream<char,struct std::char_traits<char>,class
   std::allocator<char> >::`vbase destructor'(void)
   
   Libraries: Visual Studio 2017 Debug, Visual Studio 2017 Release, Visual Studio 2019 Debug, Visual
   Studio 2019 Release */

void __thiscall
std::basic_stringstream<char,struct_std::char_traits<char>,class_std::allocator<char>_>::
_vbase_destructor_(basic_stringstream<char,struct_std::char_traits<char>,class_std::allocator<char>_>
                   *this)

{
  basic_iostream<char,struct_std::char_traits<char>_>::
  ~basic_iostream<char,struct_std::char_traits<char>_>
            ((basic_iostream<char,struct_std::char_traits<char>_> *)(this + 0x60));
  FUN_00516010((ios_base *)(this + 0x60));
  return;
}

