/* Entry: 0x004a3730; symbol: FUN_004a3730; body bytes: 118 */

void __fastcall FUN_004a3730(undefined4 *param_1)

{
  void *pvVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567750;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  if (param_1[1] != 0) {
    pvVar1 = (void *)move<>(param_1[1] + 8);
    FUN_004a2960(*param_1,pvVar1);
  }
  FUN_004a3700(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

