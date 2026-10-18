/* Entry: 0x00424030; symbol: FUN_00424030; body bytes: 40 */

uint __fastcall FUN_00424030(int param_1)

{
  return *(uint *)(param_1 + 0xe8) & 1 | *(uint *)(param_1 + 0xe8) >> 2 & 1;
}

