/* Entry: 0x004233b0; symbol: FUN_004233b0; body bytes: 187 */

undefined4 * __fastcall FUN_004233b0(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_0056d744;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  param_1[10] = 0;
  param_1[0xb] = 0;
  FUN_00422d90(param_1 + 0xc);
  param_1[0x10] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize ScreenInf\n",ppcVar1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

