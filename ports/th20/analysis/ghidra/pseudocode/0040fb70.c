/* Entry: 0x0040fb70; symbol: append; body bytes: 51 */

/* Library Function - Single Match
    public: class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >
   & __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char>
   >::append(class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >
   const &)
   
   Library: Visual Studio 2019 Release */

basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> * __thiscall
std::basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_>::append
          (basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *this,
          basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *param_1)

{
  uint uVar1;
  char *pcVar2;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *pbVar3;
  
  uVar1 = *(uint *)(param_1 + 0x10);
  pcVar2 = (char *)Myptr((undefined4 *)param_1);
  pbVar3 = append(this,pcVar2,uVar1);
  return pbVar3;
}

