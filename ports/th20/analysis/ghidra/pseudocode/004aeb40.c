/* Entry: 0x004aeb40; symbol: FUN_004aeb40; body bytes: 159 */

undefined4 * __fastcall FUN_004aeb40(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[3] = 0;
  FUN_0040bc20(param_1 + 4,8,4,FUN_004aece0);
  FUN_0040bc20(param_1 + 0xc,4,7,FUN_00425cc0);
  param_1[0x13] = 0;
  param_1[0x14] = 0;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

