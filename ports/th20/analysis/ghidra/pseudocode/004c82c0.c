/* Entry: 0x004c82c0; symbol: FUN_004c82c0; body bytes: 190 */

undefined4 * __fastcall FUN_004c82c0(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005679a0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[3] = 0;
  param_1[4] = 0;
  FUN_00422e10(param_1 + 5);
  FUN_00422e10(param_1 + 8);
  FUN_00429210(param_1 + 0xb,0.0);
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  param_1[0xe] = 0;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

