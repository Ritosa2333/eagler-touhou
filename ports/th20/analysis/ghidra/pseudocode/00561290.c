/* Entry: 0x00561290; symbol: FUN_00561290; body bytes: 77 */

void __thiscall FUN_00561290(void *this,ushort *param_1)

{
  int iVar1;
  WCHAR local_1c [10];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar1 = ___acrt_GetLocaleInfoEx_16(this,param_1,0x59,local_1c,9);
  if (iVar1 != 0) {
    FUN_00557290((ushort *)local_1c,param_1,9);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

