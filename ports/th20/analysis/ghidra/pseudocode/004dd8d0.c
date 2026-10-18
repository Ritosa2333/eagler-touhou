/* Entry: 0x004dd8d0; symbol: FUN_004dd8d0; body bytes: 54 */

void __fastcall FUN_004dd8d0(int param_1)

{
  *(undefined4 *)(param_1 + 500) = *(undefined4 *)(param_1 + 0x1f8);
  DAT_005c6110 = &DAT_005b0038 + *(int *)(param_1 + 500) * 0x134;
  return;
}

