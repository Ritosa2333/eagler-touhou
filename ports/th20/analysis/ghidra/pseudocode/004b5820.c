/* Entry: 0x004b5820; symbol: FUN_004b5820; body bytes: 192 */

undefined4 __fastcall FUN_004b5820(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  
  while (DAT_005c4a18 == 0) {
    Sleep(10);
  }
  iVar1 = FUN_0044eb30(DAT_005c0028,5,(char **)"fronttr.anm");
  *(int *)(param_1 + 0x2cc) = iVar1;
  if (iVar1 == 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
    uVar2 = 0xffffffff;
  }
  else {
    iVar1 = FUN_004b5960(param_1);
    if (iVar1 == 0) {
      piVar3 = FUN_00412310(0x2a,FUN_004b7c00,param_1);
      *(int **)(param_1 + 8) = piVar3;
      piVar3 = FUN_004123b0(0x3a,FUN_004b7d70,param_1);
      *(int **)(param_1 + 0xc) = piVar3;
      piVar3 = FUN_004123b0(0x34,FUN_004b7d60,param_1);
      *(int **)(param_1 + 0x150) = piVar3;
      uVar2 = 0;
    }
    else {
      uVar2 = 0xffffffff;
    }
  }
  return uVar2;
}

