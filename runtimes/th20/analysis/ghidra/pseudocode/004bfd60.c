/* Entry: 0x004bfd60; symbol: FUN_004bfd60; body bytes: 126 */

void __cdecl FUN_004bfd60(int param_1,float *param_2)

{
  int iVar1;
  void *this;
  undefined1 local_14 [8];
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar1 = FUN_004c0f70(param_1);
  this = (void *)FUN_004c0f50(param_1);
  FUN_00429570(this,local_14,param_2);
  FUN_004c0240(local_14,DAT_0056c8d0);
  local_c = 0;
  *(undefined4 *)(iVar1 + 0xac) = 1;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

