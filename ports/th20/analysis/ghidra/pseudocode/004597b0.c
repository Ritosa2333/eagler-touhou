/* Entry: 0x004597b0; symbol: FUN_004597b0; body bytes: 232 */

void __fastcall FUN_004597b0(undefined4 *param_1)

{
  uint uVar1;
  int *unaff_FS_OFFSET;
  uint local_18;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005679a0;
  local_10 = *unaff_FS_OFFSET;
  uVar1 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  *param_1 = &PTR__scalar_deleting_destructor__0056e73c;
  for (local_18 = 0; local_18 < (uint)param_1[5]; local_18 = local_18 + 1) {
    if (*(int *)(param_1[2] + local_18 * 4) != 0) {
      (**(code **)(**(int **)(param_1[2] + local_18 * 4) + 8))
                (*(undefined4 *)(param_1[2] + local_18 * 4),uVar1);
      *(undefined4 *)(param_1[2] + local_18 * 4) = 0;
    }
  }
  if (param_1[2] != 0) {
    FUN_0040d840((LPVOID)param_1[2]);
    param_1[2] = 0;
  }
  if (param_1[4] != 0) {
    FUN_004592e0((void *)param_1[4]);
    param_1[4] = 0;
  }
  *unaff_FS_OFFSET = local_10;
  return;
}

