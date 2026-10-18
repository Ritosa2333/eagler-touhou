/* Entry: 0x00471980; symbol: FUN_00471980; body bytes: 691 */

void __fastcall FUN_00471980(undefined4 *param_1)

{
  char **ppcVar1;
  uint uVar2;
  int *unaff_FS_OFFSET;
  int local_20;
  int local_1c;
  int local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005674f0;
  local_10 = *unaff_FS_OFFSET;
  ppcVar1 = (char **)(DAT_005b25c0 ^ (uint)&stack0xfffffffc);
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR_FUN_0056f80c;
  std::_Adl_verify_range<char*,char_const*>((char **)"shutdown BackGroundInf\n",ppcVar1);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[2]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[3]);
  FUN_004124b0(DAT_005b66d8,(void *)param_1[0xcda]);
  if (param_1[0xcc8] != 0) {
    local_20 = param_1[0xcc8];
    for (local_1c = 0; local_1c < *(short *)(param_1[0xcc9] + 2); local_1c = local_1c + 1) {
      FUN_0044c240(local_20);
      local_20 = local_20 + 0x5e4;
    }
    if (param_1[0xcc8] != 0) {
      FUN_0041f670((LPVOID)param_1[0xcc8]);
      param_1[0xcc8] = 0;
    }
  }
  for (local_18 = 0; local_18 < 8; local_18 = local_18 + 1) {
    FUN_0044c240((int)(param_1 + local_18 * 0x179 + 0xe1));
  }
  if ((param_1[0xcc9] != 0) && (param_1[0xcc9] != 0)) {
    FUN_0041f670((LPVOID)param_1[0xcc9]);
    param_1[0xcc9] = 0;
  }
  if ((param_1[0xcd8] != 0) && (param_1[0xcd8] != 0)) {
    FUN_0041f670((LPVOID)param_1[0xcd8]);
    param_1[0xcd8] = 0;
  }
  param_1[0xcd8] = 0;
  if (param_1[0xcb2] != 0) {
    FUN_00471210(param_1 + 0xcb2);
    param_1[0xcb2] = 0;
  }
  if (param_1[0xcb3] != 0) {
    FUN_00471210(param_1 + 0xcb3);
    param_1[0xcb3] = 0;
  }
  uVar2 = FUN_00474e00(0x5ba568);
  if (uVar2 == 0) {
    FUN_0044c430(DAT_005c0028,(param_1[0xcd6] & 1) + 3);
  }
  if (DAT_005c069c == param_1) {
    DAT_005c069c = (undefined4 *)0x0;
  }
  if (DAT_005c06a0 == param_1) {
    DAT_005c06a0 = (undefined4 *)0x0;
  }
  FUN_00471c40((int)(param_1 + 4));
  FUN_0041fdf0(param_1);
  *unaff_FS_OFFSET = local_10;
  return;
}

