/* Entry: 0x00428810; symbol: FUN_00428810; body bytes: 120 */

void __fastcall FUN_00428810(int *param_1)

{
  int iVar1;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  param_1[5] = 0;
  if (*param_1 != 0) {
    iVar1 = (**(code **)(*(int *)*param_1 + 0x24))(*param_1,&local_c);
    param_1[5] = iVar1;
    param_1[5] = local_c & 1;
    (**(code **)(*(int *)*param_1 + 0x48))(*param_1);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

