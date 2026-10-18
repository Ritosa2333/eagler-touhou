/* Entry: 0x004df670; symbol: FUN_004df670; body bytes: 103 */

undefined4 __fastcall FUN_004df670(int param_1)

{
  int *piVar1;
  
  piVar1 = FUN_00412310(0x10,FUN_004dfac0,param_1);
  *(int **)(param_1 + 8) = piVar1;
  piVar1 = FUN_004123b0(0x5c,FUN_0049dea0,param_1);
  *(int **)(param_1 + 0xc) = piVar1;
  FUN_004b99f0(FUN_004df6e0);
  FUN_00423520((void *)(param_1 + 0x14),0);
  *(undefined4 *)(param_1 + 0x10) = 0;
  return 0;
}

