/* Entry: 0x00411660; symbol: resize; body bytes: 70 */

/* Library Function - Single Match
    public: void __thiscall std::basic_string<char,struct std::char_traits<char>,class
   std::allocator<char> >::resize(unsigned int,char)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __thiscall
std::basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_>::resize
          (basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *this,
          uint param_1,char param_2)

{
  uint uVar1;
  
  uVar1 = FUN_00411700((int)this);
  if (uVar1 < param_1) {
    append(this,param_1 - uVar1,param_2);
  }
  else {
    FUN_0040f080(this,param_1);
  }
  return;
}

