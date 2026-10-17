/* Entry: 0x004bfee0; symbol: FUN_004bfee0; body bytes: 191 */

undefined4 * __fastcall FUN_004bfee0(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_00570c88;
  FUN_004bfe80(param_1 + 4);
  param_1[0x3104] = 0;
  FUN_00418a50(param_1 + 0x3105);
  FUN_00418a50(param_1 + 0x310b);
  param_1[0x3111] = 0;
  FUN_00422d90(param_1 + 0x3112);
  param_1[0x3116] = 0;
  param_1[0x3117] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize HitCtrlInf\n",ppcVar1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

