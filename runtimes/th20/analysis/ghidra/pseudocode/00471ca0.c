/* Entry: 0x00471ca0; symbol: FUN_00471ca0; body bytes: 266 */

void __fastcall FUN_00471ca0(int *param_1)

{
  int *unaff_FS_OFFSET;
  int local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005679a0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0044fcd0(param_1 + 2);
  if (param_1[5] != 0) {
    FUN_0041f670((LPVOID)param_1[5]);
    param_1[5] = 0;
  }
  if (param_1[6] != 0) {
    FUN_0041f670((LPVOID)param_1[6]);
    param_1[6] = 0;
  }
  for (local_18 = 0; local_18 < *param_1 + -1; local_18 = local_18 + 1) {
    FUN_0044fcd0((undefined4 *)(param_1[3] + local_18 * 4));
  }
  if (param_1[3] != 0) {
    FUN_0041f670((LPVOID)param_1[3]);
    param_1[3] = 0;
  }
  if (param_1[4] != 0) {
    FUN_0041f670((LPVOID)param_1[4]);
    param_1[4] = 0;
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

