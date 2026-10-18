/* Entry: 0x00471480; symbol: FUN_00471480; body bytes: 307 */

undefined4 * __fastcall FUN_00471480(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_0056f80c;
  FUN_004715c0(param_1 + 4);
  param_1[0xcc8] = 0;
  param_1[0xcc9] = 0;
  param_1[0xcca] = 0;
  param_1[0xccb] = 0;
  param_1[0xccc] = 0;
  param_1[0xccd] = 0;
  param_1[0xcce] = 0;
  param_1[0xccf] = 0;
  param_1[0xcd0] = 0;
  FUN_004713c0(param_1 + 0xcd1);
  FUN_00422d90(param_1 + 0xcd2);
  param_1[0xcd6] = 0;
  param_1[0xcd7] = 0;
  param_1[0xcd8] = 0;
  param_1[0xcd9] = 0;
  param_1[0xcda] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize BackGroundInf\n",ppcVar1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

