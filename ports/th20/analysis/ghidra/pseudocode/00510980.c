/* Entry: 0x00510980; symbol: FUN_00510980; body bytes: 244 */

undefined4 * __fastcall FUN_00510980(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_00573ec4;
  param_1[4] = 0;
  FUN_00425cc0(param_1 + 5);
  FUN_00425cc0(param_1 + 6);
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  param_1[10] = 0;
  FUN_00422d90(param_1 + 0xb);
  FUN_00422d90(param_1 + 0xf);
  param_1[0x13] = 0;
  _memset(param_1 + 0x14,0,0x40);
  param_1[0x24] = 0;
  param_1[0x25] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize StageClearInf\n",ppcVar1);
  DAT_005c6114 = param_1;
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

