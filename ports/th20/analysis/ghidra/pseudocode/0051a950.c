/* Entry: 0x0051a950; symbol: FUN_0051a950; body bytes: 60 */

int __fastcall FUN_0051a950(int param_1)

{
  int iVar1;
  
  **(int **)(param_1 + 0x2c) = **(int **)(param_1 + 0x2c) + -1;
  iVar1 = **(int **)(param_1 + 0x1c) + 1;
  **(int **)(param_1 + 0x1c) = iVar1;
  return iVar1;
}

