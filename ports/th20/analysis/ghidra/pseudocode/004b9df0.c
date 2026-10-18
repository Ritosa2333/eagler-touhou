/* Entry: 0x004b9df0; symbol: FUN_004b9df0; body bytes: 245 */

undefined4 * __fastcall FUN_004b9df0(undefined4 *param_1)

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
  *param_1 = &PTR__scalar_deleting_destructor__00570b04;
  FUN_00422d90(param_1 + 4);
  FUN_00422d90(param_1 + 8);
  param_1[0xc] = 0;
  param_1[0xd] = 0;
  FUN_004b9c10(param_1 + 0xe);
  param_1[0x3a] = 0;
  param_1[0x3b] = 0;
  param_1[0x3c] = 0;
  param_1[0x3d] = 0;
  *(undefined8 *)(param_1 + 0x3e) = 0;
  *(undefined8 *)(param_1 + 0x40) = 0;
  param_1[0x42] = 0;
  param_1[0x43] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"initialize GameTaskInf\n",ppcVar1);
  *unaff_FS_OFFSET = local_10;
  return param_1;
}

