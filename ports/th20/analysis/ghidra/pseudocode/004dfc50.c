/* Entry: 0x004dfc50; symbol: FUN_004dfc50; body bytes: 189 */

undefined4 * __fastcall FUN_004dfc50(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_00572090;
  param_1[4] = 0;
  FUN_004aef00(param_1 + 5);
  FUN_00422d90(param_1 + 0x18);
  FUN_00422e10(param_1 + 0x1c);
  FUN_00422d90(param_1 + 0x1f);
  FUN_00422d90(param_1 + 0x23);
  param_1[0x27] = 0;
  param_1[0x28] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize OptionInf\n",ppcVar1);
  DAT_005c60b8 = param_1;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

