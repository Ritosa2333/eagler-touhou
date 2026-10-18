/* Entry: 0x0041d960; symbol: FID_conflict:push_back; body bytes: 132 */

/* Library Function - Multiple Matches With Different Base Names
    public: void __thiscall std::basic_string<unsigned short,struct std::char_traits<unsigned
   short>,class std::allocator<unsigned short> >::push_back(unsigned short)
    public: void __thiscall std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class
   std::allocator<wchar_t> >::push_back(wchar_t)
   
   Library: Visual Studio 2019 Release */

void __thiscall FID_conflict_push_back(void *this,undefined2 param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined2 local_8 [2];
  
  uVar1 = *(uint *)((int)this + 0x10);
  if (uVar1 < *(uint *)((int)this + 0x14)) {
    *(uint *)((int)this + 0x10) = uVar1 + 1;
    puVar2 = Myptr((undefined4 *)this);
    FUN_0040ff00((undefined2 *)((int)puVar2 + uVar1 * 2),&param_1);
    local_8[0] = 0;
    FUN_0040ff00((undefined2 *)((int)puVar2 + uVar1 * 2 + 2),local_8);
  }
  else {
    FUN_004184b0(this,1);
  }
  return;
}

