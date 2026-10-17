/* Entry: 0x0045bbe0; symbol: FUN_0045bbe0; body bytes: 211 */

undefined4 __fastcall FUN_0045bbe0(void *param_1)

{
  int *piVar1;
  undefined4 uVar2;
  uint uVar3;
  char **ppcVar4;
  float10 fVar5;
  
  if (*(int *)((int)param_1 + 8) == 0) {
    uVar2 = 0x800401f0;
  }
  else if (*(int *)((int)param_1 + 0x5c) == 0) {
    uVar2 = 0x800401f0;
  }
  else {
    *(undefined4 *)((int)param_1 + 0x5c) = 0;
    uVar3 = FUN_0045bcf0(*(int *)((int)param_1 + 0x10));
    ppcVar4 = (char **)FUN_00426370(*(int *)((int)param_1 + 0x10));
    FUN_0045af10(param_1,ppcVar4,uVar3);
    piVar1 = (int *)**(undefined4 **)((int)param_1 + 8);
    *(undefined4 *)((int)param_1 + 0x58) = 1;
    fVar5 = (float10)FUN_0041cb10();
    *(double *)((int)param_1 + 0x48) =
         ((double)fVar5 - *(double *)((int)param_1 + 0x40)) + *(double *)((int)param_1 + 0x48);
    uVar2 = (**(code **)(*piVar1 + 0x30))
                      (piVar1,0,*(undefined4 *)((int)param_1 + 0x24),
                       *(undefined4 *)((int)param_1 + 0x28));
  }
  return uVar2;
}

