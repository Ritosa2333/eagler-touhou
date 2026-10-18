/* Entry: 0x00411a70; symbol: FUN_00411a70; body bytes: 138 */

undefined4 * __fastcall FUN_00411a70(undefined4 *param_1)

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
  FUN_00411970(param_1 + 5,param_1);
  param_1[10] = 0;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

