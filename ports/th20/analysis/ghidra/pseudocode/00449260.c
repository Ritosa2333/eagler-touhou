/* Entry: 0x00449260; symbol: FUN_00449260; body bytes: 138 */

void __fastcall FUN_00449260(int param_1)

{
  void *pvVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005679a0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  pvVar1 = (void *)FUN_00449490((void *)(param_1 + 0x6c8),0);
  FUN_0044a3a0(pvVar1);
  FUN_00449370(param_1 + 0x6000818);
  FUN_004491e0(param_1 + 0x710);
  FUN_00449370(param_1 + 0xdc);
  FUN_0040b980((undefined4 *)(param_1 + 4));
  *unaff_FS_OFFSET = local_10;
  return;
}

