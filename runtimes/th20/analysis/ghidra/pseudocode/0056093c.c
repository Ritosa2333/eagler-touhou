/* Entry: 0x0056093c; symbol: FUN_0056093c; body bytes: 328 */

void __cdecl FUN_0056093c(LPVOID param_1)

{
  int *piVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  int local_8;
  
  if ((((*(undefined ***)((int)param_1 + 0x88) != (undefined **)0x0) &&
       (*(undefined ***)((int)param_1 + 0x88) != &PTR_DAT_005b2660)) &&
      (*(int **)((int)param_1 + 0x7c) != (int *)0x0)) && (**(int **)((int)param_1 + 0x7c) == 0)) {
    piVar1 = *(int **)((int)param_1 + 0x84);
    if ((piVar1 != (int *)0x0) && (*piVar1 == 0)) {
      FUN_00557ba0(piVar1);
      FID_conflict____acrt_locale_free_monetary(*(int *)((int)param_1 + 0x88));
    }
    piVar1 = *(int **)((int)param_1 + 0x80);
    if ((piVar1 != (int *)0x0) && (*piVar1 == 0)) {
      FUN_00557ba0(piVar1);
      FID_conflict____free_lconv_num(*(undefined4 **)((int)param_1 + 0x88));
    }
    FUN_00557ba0(*(LPVOID *)((int)param_1 + 0x7c));
    FUN_00557ba0(*(LPVOID *)((int)param_1 + 0x88));
  }
  if ((*(int **)((int)param_1 + 0x8c) != (int *)0x0) && (**(int **)((int)param_1 + 0x8c) == 0)) {
    FUN_00557ba0((LPVOID)(*(int *)((int)param_1 + 0x90) + -0xfe));
    FUN_00557ba0((LPVOID)(*(int *)((int)param_1 + 0x94) + -0x80));
    FUN_00557ba0((LPVOID)(*(int *)((int)param_1 + 0x98) + -0x80));
    FUN_00557ba0(*(LPVOID *)((int)param_1 + 0x8c));
  }
  ___acrt_locale_free_lc_time_if_unreferenced(*(undefined ***)((int)param_1 + 0x9c));
  puVar2 = (undefined4 *)((int)param_1 + 0xa0);
  local_8 = 6;
  puVar3 = (undefined4 *)((int)param_1 + 0x28);
  do {
    if ((((undefined2 *)puVar3[-2] != &DAT_005b2848) &&
        (piVar1 = (int *)*puVar3, piVar1 != (int *)0x0)) && (*piVar1 == 0)) {
      FUN_00557ba0(piVar1);
      FUN_00557ba0((LPVOID)*puVar2);
    }
    if (((puVar3[-3] != 0) && (piVar1 = (int *)puVar3[-1], piVar1 != (int *)0x0)) && (*piVar1 == 0))
    {
      FUN_00557ba0(piVar1);
    }
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 4;
    local_8 = local_8 + -1;
  } while (local_8 != 0);
  FUN_00557ba0(param_1);
  return;
}

