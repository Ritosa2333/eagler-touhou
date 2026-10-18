/* Entry: 0x00534210; symbol: FUN_00534210; body bytes: 75 */

void __fastcall FUN_00534210(int param_1)

{
  bool bVar1;
  int iVar2;
  
  iVar2 = FUN_00460830(0);
  bVar1 = FUN_004ff5e0(iVar2);
  if (bVar1) {
    (**(code **)(**(int **)(param_1 + 0x2c) + 100))();
  }
  else {
    (**(code **)(**(int **)(param_1 + 0x30) + 100))();
  }
  return;
}

