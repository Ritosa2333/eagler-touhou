/* Entry: 0x0049ef90; symbol: FUN_0049ef90; body bytes: 278 */

void __fastcall FUN_0049ef90(undefined4 *param_1)

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
  *param_1 = &PTR_FUN_00570244;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown EndingInf\n",ppcVar1);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  FUN_0049e8c0((void *)param_1[6]);
  param_1[6] = 0;
  param_1[6] = 0;
  FUN_0044c430(DAT_005c0028,0xf);
  FUN_0044c430(DAT_005c0028,0x10);
  FUN_0044c430(DAT_005c0028,0x11);
  FUN_0044c430(DAT_005c0028,0x12);
  if (param_1[5] != 0) {
    FUN_0041f670((LPVOID)param_1[5]);
    param_1[5] = 0;
  }
  param_1[5] = 0;
  DAT_005c49e8 = 0;
  DAT_005ba518 = 0;
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

