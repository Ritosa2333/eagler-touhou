/* Entry: 0x0040fd00; symbol: FID_conflict:append; body bytes: 150 */

/* Library Function - Multiple Matches With Different Base Names
    public: class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class
   std::allocator<unsigned short> > & __thiscall std::basic_string<unsigned short,struct
   std::char_traits<unsigned short>,class std::allocator<unsigned short> >::append(unsigned
   int,unsigned short)
    public: class std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class
   std::allocator<wchar_t> > & __thiscall std::basic_string<wchar_t,struct
   std::char_traits<wchar_t>,class std::allocator<wchar_t> >::append(unsigned int,wchar_t)
   
   Library: Visual Studio 2019 Release */

undefined4 * __thiscall FID_conflict_append(void *this,uint param_1,wchar_t param_2)

{
  int iVar1;
  undefined4 *puVar2;
  undefined2 local_8;
  byte local_5;
  
  iVar1 = *(int *)((int)this + 0x10);
  if ((uint)(*(int *)((int)this + 0x14) - iVar1) < param_1) {
    this = (void *)FUN_0040d370(this,param_1,(uint)local_5,param_1,param_2);
  }
  else {
    *(uint *)((int)this + 0x10) = iVar1 + param_1;
    puVar2 = Myptr((undefined4 *)this);
    FUN_0040fec0((wchar_t *)((int)puVar2 + iVar1 * 2),param_1,param_2);
    local_8 = 0;
    FUN_0040ff00((undefined2 *)((int)puVar2 + (iVar1 + param_1) * 2),&local_8);
  }
  return (undefined4 *)this;
}

