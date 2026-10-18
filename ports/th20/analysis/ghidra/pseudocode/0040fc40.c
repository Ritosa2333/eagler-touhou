/* Entry: 0x0040fc40; symbol: append; body bytes: 47 */

/* Library Function - Single Match
    public: class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >
   & __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char>
   >::append(char const * const)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> * __thiscall
std::basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_>::append
          (basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *this,
          char *param_1)

{
  int iVar1;
  uint uVar2;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *pbVar3;
  
  iVar1 = FUN_00411180(param_1);
  uVar2 = move<>(iVar1);
  pbVar3 = append(this,param_1,uVar2);
  return pbVar3;
}

