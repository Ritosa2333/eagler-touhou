/* Entry: 0x00532970; symbol: FUN_00532970; body bytes: 274 */

void __fastcall FUN_00532970(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_00576228;
  FUN_00532740((void *)param_1[10]);
  param_1[10] = 0;
  FUN_00532740((void *)param_1[0xb]);
  param_1[0xb] = 0;
  FUN_00532740((void *)param_1[0xc]);
  param_1[0xc] = 0;
  FUN_00532740((void *)param_1[0xd]);
  param_1[0xd] = 0;
  FUN_004a2800((void *)param_1[0x10]);
  param_1[0x10] = 0;
  std::_Adl_verify_range<char*,char_const*>((char **)"shitdown WeaponStoneInf\n",ppcVar1);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  FUN_0044c430(DAT_005c0028,0x17);
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

