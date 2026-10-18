/* Entry: 0x004bd8a0; symbol: FUN_004bd8a0; body bytes: 69 */

void __fastcall FUN_004bd8a0(int param_1)

{
  if (*(int *)(param_1 + 500) < 7) {
    *(int *)(param_1 + 500) = *(int *)(param_1 + 500) + 1;
  }
  DAT_005c6110 = &DAT_005b0038 + *(int *)(param_1 + 500) * 0x134;
  return;
}

