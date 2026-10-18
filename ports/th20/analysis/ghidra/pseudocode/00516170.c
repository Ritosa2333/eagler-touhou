/* Entry: 0x00516170; symbol: FUN_00516170; body bytes: 212 */

void __fastcall FUN_00516170(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_00574d78;
  std::_Adl_verify_range<char*,char_const*>((char **)"shitdown StoneMenuInf\n",ppcVar1);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  FUN_0044fcd0(param_1 + 0x31);
  FUN_0044fcd0(param_1 + 0x33);
  FUN_0044fcd0(param_1 + 0x34);
  FUN_0044fcd0(param_1 + 0x32);
  FUN_0044c430(DAT_005c0028,0x18);
  FUN_004afa70((int)(param_1 + 0x1d));
  FUN_004afa70((int)(param_1 + 10));
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

