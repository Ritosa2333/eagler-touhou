/* Entry: 0x00477cb0; symbol: FUN_00477cb0; body bytes: 45 */

undefined4 __fastcall FUN_00477cb0(int param_1)

{
  if (*(int *)(param_1 + 0x14) != 0) {
    (**(code **)(**(int **)(param_1 + 0x14) + 0xc))();
  }
  return 1;
}

