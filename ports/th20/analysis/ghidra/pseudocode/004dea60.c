/* Entry: 0x004dea60; symbol: FUN_004dea60; body bytes: 281 */

undefined4 * __fastcall FUN_004dea60(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_00571ea8;
  param_1[4] = 0;
  FUN_00422d90(param_1 + 5);
  FUN_004aef00(param_1 + 9);
  FUN_004aedf0(param_1 + 0x1c);
  FUN_00425cc0(param_1 + 0x23);
  param_1[0x24] = 0;
  param_1[0x25] = 0;
  param_1[0x26] = 0;
  param_1[0x27] = 0;
  param_1[0x28] = 0;
  param_1[0x29] = 0;
  param_1[0x2a] = 0;
  _memset(param_1 + 0x2b,0,0x80);
  param_1[0x4b] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize NoticeInf\n",ppcVar1);
  DAT_005c5b38 = param_1;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

