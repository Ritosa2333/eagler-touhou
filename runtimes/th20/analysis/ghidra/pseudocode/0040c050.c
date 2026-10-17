/* Entry: 0x0040c050; symbol: FUN_0040c050; body bytes: 44 */

bool __fastcall FUN_0040c050(int param_1)

{
  bool bVar1;
  
  bVar1 = *(int *)(param_1 + 0x2c) != 0x7fffffff;
  if (!bVar1) {
    *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + -1;
  }
  return bVar1;
}

