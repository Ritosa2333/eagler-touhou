/* Entry: 0x0051aa00; symbol: FUN_0051aa00; body bytes: 307 */

void __thiscall FUN_0051aa00(void *this,uint *param_1,uint param_2,uint param_3)

{
  undefined4 uVar1;
  uint *puVar2;
  uint *local_18;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (0x7fffffff < param_2) {
    __scrt_throw_std_bad_alloc();
  }
  if ((param_2 == 0) || ((param_3 & 6) == 6)) {
    *(undefined4 *)((int)this + 0x38) = 0;
    param_3 = param_3 & 0xffffffdf;
  }
  else {
    local_c = param_2;
    uVar1 = FUN_0040c7c0((allocator<char> *)((int)this + 0x40),&local_c);
    puVar2 = (uint *)move<>(uVar1);
    FUN_0040ffd0(puVar2,param_1,param_2);
    *(uint *)((int)this + 0x38) = (int)puVar2 + local_c;
    if ((param_3 & 4) == 0) {
      std::basic_streambuf<char,struct_std::char_traits<char>_>::setg
                ((basic_streambuf<char,struct_std::char_traits<char>_> *)this,(char *)puVar2,
                 (char *)puVar2,*(char **)((int)this + 0x38));
    }
    if ((param_3 & 2) == 0) {
      local_18 = puVar2;
      if ((param_3 & 0x18) != 0) {
        local_18 = *(uint **)((int)this + 0x38);
      }
      FUN_0051ca70(this,puVar2,(int)local_18,*(int *)((int)this + 0x38));
      if ((param_3 & 4) != 0) {
        std::basic_streambuf<char,struct_std::char_traits<char>_>::setg
                  ((basic_streambuf<char,struct_std::char_traits<char>_> *)this,(char *)puVar2,
                   (char *)puVar2,(char *)puVar2);
      }
    }
    param_3 = param_3 & 0xffffffdf | 1;
  }
  *(uint *)((int)this + 0x3c) = param_3;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

