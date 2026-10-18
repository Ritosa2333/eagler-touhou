/* Entry: 0x004aeda0; symbol: FUN_004aeda0; body bytes: 75 */

undefined4 __fastcall FUN_004aeda0(undefined4 param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040bc20(param_1,4,3,FUN_00425cc0);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

