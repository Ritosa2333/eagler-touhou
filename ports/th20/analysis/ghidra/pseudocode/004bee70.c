/* Entry: 0x004bee70; symbol: FUN_004bee70; body bytes: 252 */

undefined4 * __fastcall FUN_004bee70(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_00570be4;
  param_1[4] = 0;
  FUN_00422d90(param_1 + 5);
  FUN_004aef00(param_1 + 9);
  FUN_0040bc20(param_1 + 0x1c,4,0xe,FUN_00425cc0);
  param_1[0x2a] = 0;
  param_1[0x2b] = 0;
  param_1[0x2c] = 0;
  param_1[0x2d] = 0;
  param_1[0x2e] = 0;
  _memset(param_1 + 0x2f,0,0x80);
  param_1[0x4f] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize HelpInf\n",ppcVar1);
  DAT_005c4d24 = param_1;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

