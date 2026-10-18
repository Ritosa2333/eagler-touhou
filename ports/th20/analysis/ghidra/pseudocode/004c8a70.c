/* Entry: 0x004c8a70; symbol: FUN_004c8a70; body bytes: 122 */

void __fastcall FUN_004c8a70(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_00571214;
  FUN_004d2360((int)param_1);
  FUN_00449370((int)(param_1 + 0x34e));
  FUN_00449370((int)(param_1 + 0x1d5));
  FUN_0048b8b0((int)(param_1 + 0x1be));
  FUN_004c8af0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

