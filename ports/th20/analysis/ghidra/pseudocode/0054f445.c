/* Entry: 0x0054f445; symbol: FUN_0054f445; body bytes: 20 */

void __fastcall FUN_0054f445(int param_1)

{
  if (**(short **)(param_1 + 8) == 0x2a) {
    *(undefined1 *)(param_1 + 0x18) = 1;
    *(short **)(param_1 + 8) = *(short **)(param_1 + 8) + 1;
  }
  return;
}

