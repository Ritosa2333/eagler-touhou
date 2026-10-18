/* Entry: 0x004bd210; symbol: FUN_004bd210; body bytes: 48 */

void __fastcall FUN_004bd210(int param_1)

{
  if (*(int *)(param_1 + 0x10) != 0) {
    FUN_0041f7c0(*(undefined4 **)(param_1 + 0x10));
    *(undefined4 *)(param_1 + 0x10) = 0;
  }
  return;
}

