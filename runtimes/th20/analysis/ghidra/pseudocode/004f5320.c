/* Entry: 0x004f5320; symbol: FUN_004f5320; body bytes: 449 */

void __fastcall FUN_004f5320(undefined4 *param_1)

{
  char **ppcVar1;
  uint uVar2;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567600;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_00572bac;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown PlayerInf\n",ppcVar1);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  FUN_00504950(param_1 + 0x8ad);
  uVar2 = FUN_00474e00(0x5ba568);
  if (uVar2 == 0) {
    if (((uint)param_1[5] >> 8 & 3) == 0) {
      FUN_0044c430(DAT_005c0028,param_1[0x5215] + 9);
      FUN_0044c430(DAT_005c0028,param_1[0x5215] + 10);
      if ((param_1[0x882] != 0) && (param_1[0x882] != 0)) {
        FUN_0041f670((LPVOID)param_1[0x882]);
        param_1[0x882] = 0;
      }
      DAT_005c60f0 = 0;
    }
  }
  else if (((uint)param_1[5] >> 8 & 3) == 0) {
    FUN_004863f0(DAT_005c0028,param_1[0x5215] + 9,'\0');
    FUN_004863f0(DAT_005c0028,param_1[0x5215] + 10,'\0');
    if ((param_1[0x882] != 0) && (param_1[0x882] != 0)) {
      FUN_0041f670((LPVOID)param_1[0x882]);
      param_1[0x882] = 0;
    }
    DAT_005c60f0 = 0;
  }
  FUN_004f52d0((int)(param_1 + 0x891));
  FUN_00449370((int)(param_1 + 10));
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

