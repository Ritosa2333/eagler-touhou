/* Entry: 0x004a2e80; symbol: FUN_004a2e80; body bytes: 312 */

undefined4 * __fastcall FUN_004a2e80(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_005703f8;
  FUN_004a2fc0(param_1 + 4);
  FUN_004141c0(param_1 + 0x2d);
  param_1[0x31] = 0;
  param_1[0x32] = 0;
  param_1[0x33] = 0;
  FUN_00422d90(param_1 + 0x34);
  param_1[0x38] = 0;
  param_1[0x39] = 0;
  param_1[0x3a] = 0;
  param_1[0x3b] = 0;
  param_1[0x3c] = 0;
  param_1[0x3d] = 0;
  param_1[0x3e] = 0;
  param_1[0x3f] = 0;
  param_1[0x40] = 0;
  param_1[0x41] = 0;
  FUN_00418a50(param_1 + 0x42);
  param_1[0x48] = 0;
  param_1[0x49] = 0;
  FUN_00447de0(param_1 + 0x4a);
  param_1[0x4b] = 0;
  param_1[0x4c] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize EnemyCtrlInf\n",ppcVar1);
  FUN_004ab140((int)param_1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

