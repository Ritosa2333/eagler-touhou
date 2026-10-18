/* Entry: 0x004e1e90; symbol: FUN_004e1e90; body bytes: 215 */

void __fastcall FUN_004e1e90(undefined4 *param_1)

{
  char **ppcVar1;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  int local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567750;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_005724a0;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown PauseInf\n",ppcVar1);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  for (local_18 = 0; local_18 < 0x19; local_18 = local_18 + 1) {
    puVar2 = (undefined4 *)FUN_00414580(param_1 + 0x3c,local_18);
    FUN_004217c0((undefined4 *)*puVar2);
  }
  DAT_005c60bc = 0;
  FUN_004afa70((int)(param_1 + 0x1f));
  FUN_004afa70((int)(param_1 + 0xc));
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

