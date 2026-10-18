/* Entry: 0x004afaa0; symbol: FUN_004afaa0; body bytes: 336 */

void __fastcall FUN_004afaa0(undefined4 *param_1)

{
  char **ppcVar1;
  int iVar2;
  undefined4 *puVar3;
  int *unaff_FS_OFFSET;
  int local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00568060;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_0057062c;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown FrontInf\n",ppcVar1);
  FUN_004b6220(param_1);
  iVar2 = FUN_0051b960();
  FUN_005198d0(iVar2);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[0x54]);
  param_1[2] = 0;
  FUN_0044fcd0(param_1 + 0x3e);
  FUN_0044fcd0(param_1 + 0x44);
  for (local_18 = 0; local_18 < 10; local_18 = local_18 + 1) {
    puVar3 = (undefined4 *)FUN_00414580(param_1 + 0x25,local_18);
    FUN_0044fcd0(puVar3);
  }
  puVar3 = (undefined4 *)FUN_00414580(param_1 + 0x12,0);
  FUN_0044fcd0(puVar3);
  puVar3 = (undefined4 *)FUN_00414580(param_1 + 0x12,1);
  FUN_0044fcd0(puVar3);
  FUN_004863f0(DAT_005c0028,5,'\0');
  DAT_005c06a4 = 0;
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

