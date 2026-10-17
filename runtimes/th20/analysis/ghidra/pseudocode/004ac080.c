/* Entry: 0x004ac080; symbol: FUN_004ac080; body bytes: 38 */

undefined4 __fastcall FUN_004ac080(int param_1)

{
  int *piVar1;
  
  piVar1 = FUN_00412360(0x60,FUN_004ac0b0,param_1);
  *(int **)(param_1 + 0xc) = piVar1;
  return 0;
}

