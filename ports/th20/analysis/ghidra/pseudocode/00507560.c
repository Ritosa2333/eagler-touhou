/* Entry: 0x00507560; symbol: FUN_00507560; body bytes: 327 */

undefined4 * __fastcall FUN_00507560(undefined4 *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056989d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0041fd20(param_1);
  local_8 = 0;
  *param_1 = &PTR_FUN_005732e4;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  param_1[10] = 0;
  param_1[0xb] = 0;
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  param_1[0xe] = 0;
  param_1[0xf] = 0;
  FUN_0040bc20(param_1 + 0x10,0x14,8,FUN_00418a10);
  param_1[0x38] = 0;
  param_1[0x39] = 0;
  FUN_00507330(param_1 + 0x3a);
  param_1[0x92] = 0;
  *(undefined1 *)(param_1 + 0x93) = 0;
  param_1[0x94] = 0;
  param_1[0x95] = 0;
  param_1[0x96] = 0;
  FUN_00507280(param_1 + 0x97);
  _memset(param_1 + 0x98,0,0x100);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

