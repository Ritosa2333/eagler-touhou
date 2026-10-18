/* Entry: 0x004c87c0; symbol: FUN_004c87c0; body bytes: 179 */

undefined4 * __fastcall FUN_004c87c0(undefined4 *param_1)

{
  char **ppcVar1;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0041fd20(param_1);
  *param_1 = &PTR_FUN_00571400;
  FUN_00418a50(param_1 + 4);
  param_1[10] = 0;
  param_1[0xb] = 0x10000;
  FUN_00422e10(param_1 + 0xc);
  FUN_00422e10(param_1 + 0xf);
  param_1[0x12] = 0;
  param_1[0x13] = 0;
  param_1[0x14] = 0;
  param_1[0x15] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize LaserInf\n",ppcVar1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

