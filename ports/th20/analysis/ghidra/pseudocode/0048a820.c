/* Entry: 0x0048a820; symbol: FUN_0048a820; body bytes: 126 */

void __cdecl FUN_0048a820(void *param_1,int param_2)

{
  void *pvVar1;
  undefined4 uVar2;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040c080(&local_c,4);
  pvVar1 = (void *)move<>(param_1);
  FUN_00489bf0(&local_c,pvVar1);
  for (; param_2 != 0; param_2 = param_2 + -1) {
    FUN_0048be00(&local_c);
  }
  uVar2 = move<>(&local_c);
  FUN_0048a340(param_1,uVar2);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

