/* Entry: 0x0049ce80; symbol: FUN_0049ce80; body bytes: 211 */

undefined4 * __fastcall FUN_0049ce80(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_0056ff94;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  FUN_0040b780(param_1 + 10);
  param_1[0xe] = 0;
  FUN_0049cdd0(param_1 + 0xf);
  FUN_0049cd70(param_1 + 0x40f);
  param_1[0x4c0f] = 0;
  param_1[0x4c10] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize EffectInf\n",ppcVar1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

