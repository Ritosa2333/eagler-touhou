/* Entry: 0x004497d0; symbol: FUN_004497d0; body bytes: 150 */

void __fastcall FUN_004497d0(int param_1)

{
  undefined4 uVar1;
  void *pvVar2;
  int *unaff_FS_OFFSET;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056808d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uVar1 = FUN_0040c310(&DAT_005c0240,9);
  FUN_0040b730(&local_18,uVar1);
  local_8 = 0;
  pvVar2 = (void *)FUN_00449490((void *)(param_1 + 0x6c8),0);
  FUN_00449870(pvVar2);
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

