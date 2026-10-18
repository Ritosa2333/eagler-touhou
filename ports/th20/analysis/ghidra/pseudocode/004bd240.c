/* Entry: 0x004bd240; symbol: FUN_004bd240; body bytes: 48 */

void __fastcall FUN_004bd240(int param_1)

{
  if (*(int *)(param_1 + 0x20) != 0) {
    FUN_0041f7c0(*(undefined4 **)(param_1 + 0x20));
    *(undefined4 *)(param_1 + 0x20) = 0;
  }
  return;
}

