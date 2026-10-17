/* Entry: 0x00534130; symbol: FUN_00534130; body bytes: 75 */

void __fastcall FUN_00534130(int param_1)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = FUN_00460830(0);
  bVar1 = FUN_004ff5e0(iVar2);
  if (bVar1) {
    (**(code **)(**(int **)(param_1 + 0x2c) + 0x4c))();
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x30) + 0x4c))();
  }
  return;
}

