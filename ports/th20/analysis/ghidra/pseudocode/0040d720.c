/* Entry: 0x0040d720; symbol: FUN_0040d720; body bytes: 106 */

void __thiscall FUN_0040d720(void *this,undefined4 *param_1)

{
  undefined4 uVar1;
  uint uVar2;
  char *pcVar3;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040c080(&local_10,8);
  local_10 = *param_1;
  local_c = param_1[1];
  uVar1 = FUN_0040ff90((int)&local_10);
  uVar2 = move<>(uVar1);
  pcVar3 = (char *)FUN_0040c300(&local_10);
  std::basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_>::append
            ((basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> *)this,
             pcVar3,uVar2);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

