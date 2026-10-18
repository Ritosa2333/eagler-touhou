/* Entry: 0x004c8af0; symbol: FUN_004c8af0; body bytes: 108 */

void __fastcall FUN_004c8af0(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_005710bc;
  if (param_1[0x1b6] != 0) {
    FUN_00413b70(DAT_005b8894,(char **)param_1[0x1b6]);
    param_1[0x1b6] = 0;
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

