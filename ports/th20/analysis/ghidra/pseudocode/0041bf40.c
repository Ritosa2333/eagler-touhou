/* Entry: 0x0041bf40; symbol: FID_conflict:assign; body bytes: 120 */

/* Library Function - Multiple Matches With Different Base Names
    public: class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class
   std::allocator<unsigned short> > & __thiscall std::basic_string<unsigned short,struct
   std::char_traits<unsigned short>,class std::allocator<unsigned short> >::assign(unsigned short
   const * const,unsigned int)
    public: class std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class
   std::allocator<wchar_t> > & __thiscall std::basic_string<wchar_t,struct
   std::char_traits<wchar_t>,class std::allocator<wchar_t> >::assign(wchar_t const * const,unsigned
   int)
   
   Library: Visual Studio 2019 Release */

undefined4 * __thiscall FID_conflict_assign(void *this,uint *param_1,uint *param_2)

{
  uint *puVar1;
  undefined2 local_8;
  byte local_5;
  
  if (*(uint **)((int)this + 0x14) < param_2) {
    this = (void *)FUN_004182a0(this,(uint)param_2,(uint)local_5,param_1);
  }
  else {
    puVar1 = Myptr((undefined4 *)this);
    *(uint **)((int)this + 0x10) = param_2;
    FUN_0041d210(puVar1,param_1,param_2);
    local_8 = 0;
    FUN_0040ff00((undefined2 *)((int)puVar1 + (int)param_2 * 2),&local_8);
  }
  return (undefined4 *)this;
}

