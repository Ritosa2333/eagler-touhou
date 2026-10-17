/* Entry: 0x005463e0; symbol: FUN_005463e0; body bytes: 149 */

/* WARNING: Function: __SEH_prolog4 replaced with injection: SEH_prolog4 */

void __cdecl FUN_005463e0(int *param_1)

{
  byte *pbVar1;
  int *piVar2;
  code *pcVar3;
  undefined4 *unaff_FS_OFFSET;
  undefined4 local_14;
  
  if ((((param_1 != (int *)0x0) && (*param_1 == -0x1f928c9d)) && (param_1[4] == 3)) &&
     ((((param_1[5] == 0x19930520 || (param_1[5] == 0x19930521)) || (param_1[5] == 0x19930522)) &&
      (pbVar1 = (byte *)param_1[7], pbVar1 != (byte *)0x0)))) {
    if (*(undefined **)(pbVar1 + 4) == (undefined *)0x0) {
      if (((*pbVar1 & 0x10) != 0) && (piVar2 = *(int **)param_1[6], piVar2 != (int *)0x0)) {
        pcVar3 = *(code **)(*piVar2 + 8);
        (*(code *)PTR_guard_check_icall_0056c37c)(piVar2);
        (*pcVar3)();
      }
    }
    else {
      FUN_00546488(param_1[6],*(undefined **)(pbVar1 + 4));
    }
  }
  *unaff_FS_OFFSET = local_14;
  return;
}

