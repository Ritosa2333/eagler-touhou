/* Entry: 0x004e5980; symbol: FUN_004e5980; body bytes: 89 */

undefined4 __fastcall FUN_004e5980(int param_1)

{
  int *piVar1;
  
  piVar1 = FUN_00412310(0xf,FUN_004e6520,param_1);
  *(int **)(param_1 + 8) = piVar1;
  piVar1 = FUN_004123b0(0x5f,FUN_004e66a0,param_1);
  *(int **)(param_1 + 0xc) = piVar1;
  FUN_00423520((void *)(param_1 + 0x10),0);
  FUN_00423520((void *)(param_1 + 0x20),0);
  return 0;
}

