/* Entry: 0x004116b0; symbol: FID_conflict:resize; body bytes: 70 */

/* Library Function - Multiple Matches With Different Base Names
    public: void __thiscall std::basic_string<unsigned short,struct std::char_traits<unsigned
   short>,class std::allocator<unsigned short> >::resize(unsigned int,unsigned short)
    public: void __thiscall std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class
   std::allocator<wchar_t> >::resize(unsigned int,wchar_t)
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

void __thiscall FID_conflict_resize(void *this,uint param_1,wchar_t param_2)

{
  uint uVar1;
  
  uVar1 = FUN_00411700((int)this);
  if (uVar1 < param_1) {
    FID_conflict_append(this,param_1 - uVar1,param_2);
  }
  else {
    FUN_0040f0c0(this,param_1);
  }
  return;
}

