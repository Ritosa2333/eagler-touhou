/* Entry: 0x0049d2f0; symbol: FUN_0049d2f0; body bytes: 190 */

void __fastcall FUN_0049d2f0(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_0056ff94;
  FUN_0040bcf0(param_1 + 10);
  FUN_0049d400();
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown EffectInf\n",ppcVar1);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  FUN_0044c430(DAT_005c0028,8);
  FUN_0044c430(DAT_005c0028,7);
  FUN_0044c430(DAT_005c0028,0x15);
  FUN_0040b980(param_1 + 10);
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

