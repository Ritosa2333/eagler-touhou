/* Entry: 0x0051f3c0; symbol: FUN_0051f3c0; body bytes: 202 */

undefined4 __fastcall FUN_0051f3c0(int param_1)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  
  piVar1 = FUN_00412310(0xb,FUN_0052c1e0,param_1);
  *(int **)(param_1 + 8) = piVar1;
  piVar1 = FUN_004123b0(0x59,FUN_0052c210,param_1);
  *(int **)(param_1 + 0xc) = piVar1;
  iVar2 = FUN_0044ee50(DAT_005c0028,0xb,(char **)"title.anm");
  *(int *)(param_1 + 0x10) = iVar2;
  if (iVar2 == 0) {
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
    uVar3 = 0xffffffff;
  }
  else {
    iVar2 = FUN_0044ee50(DAT_005c0028,0xc,(char **)"title_v.anm");
    *(int *)(param_1 + 0x14) = iVar2;
    if (iVar2 == 0) {
      FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056f610);
      uVar3 = 0xffffffff;
    }
    else {
      FUN_004bfbe0((void *)(param_1 + 0x24),1);
      uVar3 = 0;
    }
  }
  return uVar3;
}

