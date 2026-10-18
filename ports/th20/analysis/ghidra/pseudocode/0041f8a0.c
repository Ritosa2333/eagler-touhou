/* Entry: 0x0041f8a0; symbol: FUN_0041f8a0; body bytes: 261 */

undefined4 * __fastcall FUN_0041f8a0(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_0056d4d0;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  FUN_0040bc20(param_1 + 8,0x3d4,0xc,FUN_0041fcb0);
  param_1[0xb84] = 0;
  param_1[0xb85] = 0;
  param_1[0xb86] = 0;
  param_1[0xb87] = 0;
  param_1[0xb88] = 0;
  param_1[0xb89] = 0;
  param_1[0xb8a] = 0;
  param_1[0xb8b] = 0;
  param_1[0xb8c] = 0;
  param_1[0xb8d] = 0;
  FUN_0040bc20(param_1 + 0xb8e,0x30,4,FUN_0041fb10);
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize InputInf\n",ppcVar1);
  DAT_005b8898 = param_1;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

