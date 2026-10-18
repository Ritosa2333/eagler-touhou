/* Entry: 0x0050fea0; symbol: FUN_0050fea0; body bytes: 132 */

void __fastcall FUN_0050fea0(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_00573954;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown SmallScoreInf\n",ppcVar1);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  FUN_00449370((int)(param_1 + 7));
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

