/* Entry: 0x004abb80; symbol: FUN_004abb80; body bytes: 260 */

undefined4 * __fastcall FUN_004abb80(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_005705a4;
  param_1[4] = 0;
  *(undefined8 *)(param_1 + 6) = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  *(undefined8 *)(param_1 + 10) = 0;
  *(undefined8 *)(param_1 + 0xc) = 0;
  param_1[0xe] = 0;
  *(undefined8 *)(param_1 + 0x10) = 0;
  *(undefined8 *)(param_1 + 0x12) = 0;
  FUN_0040bc20(param_1 + 0x14,8,8,FUN_004abb60);
  FUN_0040bc20(param_1 + 0x24,8,8,FUN_004abb60);
  FUN_004abb60(param_1 + 0x34);
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize FPSInf\n",ppcVar1);
  DAT_005c4a00 = param_1;
  FUN_00451520(0);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

