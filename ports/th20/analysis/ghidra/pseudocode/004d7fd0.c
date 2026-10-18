/* Entry: 0x004d7fd0; symbol: FUN_004d7fd0; body bytes: 215 */

void __fastcall FUN_004d7fd0(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_00571798;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown LoadingInf\n",ppcVar1);
  FUN_0040bcf0(param_1 + 4);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  FUN_004d8560();
  FUN_0044c430(DAT_005c0028,1);
  DAT_005c4d2c = 0;
  FUN_004217c0(DAT_005c0698);
  FUN_0044c430(DAT_005c0028,0);
  FUN_0050fc10();
  FUN_00449370((int)(param_1 + 9));
  FUN_0040b980(param_1 + 4);
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

