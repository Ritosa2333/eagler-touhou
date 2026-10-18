/* Entry: 0x004a3ce0; symbol: FUN_004a3ce0; body bytes: 83 */

void __fastcall FUN_004a3ce0(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_005703b0;
  FUN_004973c0((int)param_1);
  FUN_0048b8d0((int)(param_1 + 4));
  *unaff_FS_OFFSET = local_10;
  return;
}

