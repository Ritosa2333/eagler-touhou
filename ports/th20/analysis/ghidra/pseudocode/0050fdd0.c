/* Entry: 0x0050fdd0; symbol: FUN_0050fdd0; body bytes: 201 */

undefined4 * __fastcall FUN_0050fdd0(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_00573954;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  FUN_00448b40(param_1 + 7);
  FUN_0040bc20(param_1 + 0x180,0x44,0x12,FUN_0050fd50);
  FUN_0040bc20(param_1 + 0x2b2,0x44,0x12,FUN_0050fd50);
  param_1[0x3e4] = 0;
  param_1[0x3e5] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize SmallScoreInf\n",ppcVar1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

