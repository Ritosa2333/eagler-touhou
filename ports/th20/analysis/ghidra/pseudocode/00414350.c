/* Entry: 0x00414350; symbol: FUN_00414350; body bytes: 118 */

void __fastcall FUN_00414350(undefined4 *param_1)

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
    FUN_00413ca0(*param_1,pvVar1);
  }
  FUN_004142e0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

