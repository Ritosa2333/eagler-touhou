/* Entry: 0x004c22e0; symbol: FUN_004c22e0; body bytes: 330 */

undefined4 * __fastcall FUN_004c22e0(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_00570d40;
  param_1[4] = 0;
  param_1[5] = 0;
  FUN_004c2180(param_1 + 6);
  FUN_00418a50(param_1 + 0x127206);
  FUN_00418a50(param_1 + 0x12720c);
  FUN_00418a50(param_1 + 0x127212);
  param_1[0x127218] = 0;
  param_1[0x127219] = 0;
  param_1[0x12721a] = 0;
  param_1[0x12721b] = 0;
  param_1[0x12721c] = 0;
  param_1[0x12721d] = 0;
  param_1[0x12721e] = 0;
  param_1[0x12721f] = 0;
  param_1[0x127220] = 0;
  param_1[0x127221] = 0;
  FUN_00422e10(param_1 + 0x127222);
  param_1[0x127225] = 0;
  param_1[0x127226] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize ItemInf\n",ppcVar1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

