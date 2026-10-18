/* Entry: 0x00486bb0; symbol: FUN_00486bb0; body bytes: 348 */

undefined4 * __fastcall FUN_00486bb0(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_0056fd88;
  FUN_00425cc0(param_1 + 4);
  FUN_0040bc20(param_1 + 5,4,3,FUN_00425cc0);
  FUN_00425cc0(param_1 + 8);
  FUN_00422d90(param_1 + 9);
  _memset(param_1 + 0xd,0,0x40);
  param_1[0x1d] = 0;
  param_1[0x1e] = 0;
  param_1[0x1f] = 0;
  param_1[0x20] = 0;
  param_1[0x21] = 0;
  param_1[0x22] = 0;
  param_1[0x23] = 0;
  param_1[0x24] = 0;
  *(undefined8 *)(param_1 + 0x26) = 0;
  *(undefined8 *)(param_1 + 0x28) = 0;
  param_1[0x2a] = 0;
  FUN_00422e10(param_1 + 0x2b);
  param_1[0x2e] = 0;
  param_1[0x2f] = 0;
  param_1[0x30] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize CardInf\n",ppcVar1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

