/* Entry: 0x004bd300; symbol: FUN_004bd300; body bytes: 48 */

void __fastcall FUN_004bd300(int param_1)

{
  if (*(int *)(param_1 + 0x14) != 0) {
    FUN_0041f7c0(*(undefined4 **)(param_1 + 0x14));
    *(undefined4 *)(param_1 + 0x14) = 0;
  }
  return;
}

