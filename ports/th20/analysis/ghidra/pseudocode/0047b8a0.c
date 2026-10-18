/* Entry: 0x0047b8a0; symbol: FUN_0047b8a0; body bytes: 386 */

undefined4 * __fastcall FUN_0047b8a0(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_0056fc5c;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  param_1[7] = 0;
  param_1[8] = 0;
  param_1[9] = 0;
  param_1[10] = 0;
  param_1[0xb] = 0;
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  param_1[0xe] = 0;
  param_1[0xf] = 0;
  param_1[0x10] = 0;
  param_1[0x11] = 0;
  param_1[0x12] = 0;
  FUN_004398a0(param_1 + 0x13);
  FUN_004398a0(param_1 + 0x15);
  FUN_0047b790(param_1 + 0x18);
  FUN_0047b7c0(param_1 + 0xa1382);
  FUN_00418a50(param_1 + 0xa1b53);
  FUN_00418a50(param_1 + 0xa1b59);
  param_1[0xa1b5f] = 0;
  param_1[0xa1b60] = 0;
  param_1[0xa1b61] = 0;
  param_1[0xa1b62] = 0;
  param_1[0xa1b63] = 0;
  param_1[0xa1b64] = 0;
  param_1[0xa1b65] = 0;
  param_1[0xa1b66] = 0;
  param_1[0xa1b67] = 0;
  param_1[0xa1b68] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize BulletInf\n",ppcVar1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

