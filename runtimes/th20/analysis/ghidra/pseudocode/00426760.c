/* Entry: 0x00426760; symbol: FUN_00426760; body bytes: 214 */

undefined4 __fastcall FUN_00426760(int *param_1)

{
  int iVar1;
  code *pcVar2;
  bool bVar3;
  undefined3 extraout_var;
  int *piVar4;
  undefined4 *puVar5;
  
  bVar3 = FUN_00428fc0(0x5ba830);
  if (CONCAT31(extraout_var,bVar3) != 0) {
    if (*param_1 != 0) {
      (**(code **)(*(int *)*param_1 + 8))(*param_1);
      *param_1 = 0;
    }
    iVar1 = *(int *)(param_1[2] + 4);
    piVar4 = (int *)FUN_00414580(&DAT_005bcb54,iVar1);
    if (*piVar4 == 0) {
      piVar4 = (int *)FUN_00414580(&DAT_005bca34,iVar1);
      *param_1 = *piVar4;
    }
    else {
      piVar4 = (int *)FUN_0040c300((undefined4 *)&DAT_005ba830);
      pcVar2 = *(code **)(*piVar4 + 0x14);
      puVar5 = (undefined4 *)FUN_00414580(&DAT_005bca34,iVar1);
      (*pcVar2)(piVar4,*puVar5,param_1);
    }
    piVar4 = (int *)FUN_00414580(&DAT_005bcb54,iVar1);
    *piVar4 = *piVar4 + 1;
  }
  return 0;
}

