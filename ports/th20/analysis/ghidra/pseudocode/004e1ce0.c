/* Entry: 0x004e1ce0; symbol: FUN_004e1ce0; body bytes: 428 */

undefined4 * __fastcall FUN_004e1ce0(undefined4 *param_1)

{
  char **ppcVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a0b3;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0041fd20(param_1);
  local_8 = 0;
  *param_1 = &PTR_FUN_005724a0;
  FUN_00422d90(param_1 + 4);
  FUN_00422d90(param_1 + 8);
  FUN_004aef00(param_1 + 0xc);
  local_8._0_1_ = 1;
  FUN_004aef00(param_1 + 0x1f);
  local_8 = CONCAT31(local_8._1_3_,2);
  FUN_00425cc0(param_1 + 0x32);
  FUN_00425cc0(param_1 + 0x33);
  param_1[0x34] = 0;
  param_1[0x35] = 0;
  param_1[0x36] = 0;
  param_1[0x37] = 0;
  param_1[0x38] = 0;
  param_1[0x39] = 0;
  param_1[0x3a] = 0;
  param_1[0x3b] = 0;
  param_1[0x3c] = 0;
  _memset(param_1 + 0x3d,0,0xc4);
  param_1[0x6e] = 0;
  param_1[0x6f] = 0;
  *(undefined1 *)(param_1 + 0x70) = 0;
  param_1[0x71] = 0;
  *(undefined8 *)(param_1 + 0x72) = 0;
  _memset(param_1 + 0x74,0,0x100);
  FUN_004e1cb0(param_1 + 0xb4);
  param_1[0xb5] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize PauseInf\n",ppcVar1);
  DAT_005c60bc = param_1;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

