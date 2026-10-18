/* Entry: 0x004502f0; symbol: FUN_004502f0; body bytes: 40 */

void FUN_004502f0(int param_1,undefined4 *param_2)

{
  *(undefined4 *)(param_1 + 0x5bc) = *param_2;
  *(undefined4 *)(param_1 + 0x5c0) = param_2[1];
  *(undefined4 *)(param_1 + 0x5c4) = param_2[2];
  return;
}

