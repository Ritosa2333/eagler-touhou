/* Entry: 0x00411d50; symbol: FUN_00411d50; body bytes: 216 */

void __fastcall FUN_00411d50(int param_1)

{
  if (*(int *)(param_1 + 0x10) != 0) {
    if (**(int **)(param_1 + 0x10) == param_1) {
      **(undefined4 **)(param_1 + 0x10) = 0;
    }
    if (((*(int *)(*(int *)(param_1 + 0x10) + 4) == param_1) &&
        (*(int *)(*(int *)(param_1 + 0x10) + 4) != 0)) &&
       (*(undefined4 *)(*(int *)(param_1 + 0x10) + 4) = *(undefined4 *)(param_1 + 4),
       *(int *)(*(int *)(param_1 + 0x10) + 4) != 0)) {
      FUN_00412da0(*(void **)(*(int *)(param_1 + 0x10) + 4),*(undefined4 *)(param_1 + 0x10));
    }
    *(undefined4 *)(param_1 + 0x10) = 0;
  }
  if (*(int *)(param_1 + 4) != 0) {
    *(undefined4 *)(*(int *)(param_1 + 4) + 8) = *(undefined4 *)(param_1 + 8);
  }
  if (*(int *)(param_1 + 8) != 0) {
    *(undefined4 *)(*(int *)(param_1 + 8) + 4) = *(undefined4 *)(param_1 + 4);
  }
  *(undefined4 *)(param_1 + 4) = 0;
  *(undefined4 *)(param_1 + 8) = 0;
  *(undefined4 *)(param_1 + 0xc) = 0;
  return;
}

