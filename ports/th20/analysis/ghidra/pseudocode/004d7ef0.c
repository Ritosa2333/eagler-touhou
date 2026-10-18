/* Entry: 0x004d7ef0; symbol: FUN_004d7ef0; body bytes: 207 */

undefined4 * __fastcall FUN_004d7ef0(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_00571798;
  FUN_0040b780(param_1 + 4);
  param_1[8] = 0;
  FUN_00448b40(param_1 + 9);
  FUN_00425cc0(param_1 + 0x182);
  param_1[0x183] = 0;
  param_1[0x184] = 0;
  param_1[0x185] = 0;
  param_1[0x186] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize LoadingInf\n",ppcVar1);
  DAT_005c4d2c = param_1;
  param_1[1] = param_1[1] | 2;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

