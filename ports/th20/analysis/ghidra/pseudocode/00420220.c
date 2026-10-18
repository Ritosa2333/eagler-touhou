/* Entry: 0x00420220; symbol: FUN_00420220; body bytes: 158 */

undefined4 FUN_00420220(int param_1)

{
  code *pcVar1;
  int iVar2;
  undefined4 uVar3;
  int *piVar4;
  int local_8;
  
  iVar2 = FUN_00421180((uint *)(param_1 + 0x14));
  if (iVar2 == 0) {
    for (local_8 = 0; local_8 < 4; local_8 = local_8 + 1) {
      iVar2 = FUN_00421850(DAT_005b8898,local_8);
      if (iVar2 == 0) {
        piVar4 = (int *)FUN_00421810((int)DAT_005b8898);
        pcVar1 = *(code **)(*piVar4 + 0xc);
        iVar2 = FUN_00421870(DAT_005b8898,local_8);
        (*pcVar1)(piVar4,param_1 + 4,iVar2,0);
        return 1;
      }
    }
    uVar3 = 0;
  }
  else {
    uVar3 = 1;
  }
  return uVar3;
}

