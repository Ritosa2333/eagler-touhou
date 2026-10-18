/* Entry: 0x00477a60; symbol: FUN_00477a60; body bytes: 118 */

void __fastcall FUN_00477a60(undefined4 *param_1)

{
  int iVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_0056fa78;
  FUN_0044fcd0(param_1 + 0x1c);
  FUN_0044fcd0(param_1 + 0x1d);
  iVar1 = FUN_00478040(0);
  *(undefined4 *)(iVar1 + 0x14) = 0;
  iVar1 = FUN_00478040(0);
  *(undefined4 *)(iVar1 + 0x18) = 0;
  *unaff_FS_OFFSET = local_10;
  return;
}

