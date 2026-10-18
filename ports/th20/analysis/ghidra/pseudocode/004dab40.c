/* Entry: 0x004dab40; symbol: FUN_004dab40; body bytes: 62 */

void __fastcall FUN_004dab40(int param_1)

{
  if (*(int *)(param_1 + 0x6000e1c) != 0) {
    (**(code **)(**(int **)(param_1 + 0x6000e1c) + 8))(*(undefined4 *)(param_1 + 0x6000e1c));
    *(undefined4 *)(param_1 + 0x6000e1c) = 0;
  }
  return;
}

