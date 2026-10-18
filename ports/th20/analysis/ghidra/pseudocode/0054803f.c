/* Entry: 0x0054803f; symbol: FUN_0054803f; body bytes: 124 */

void __cdecl
FUN_0054803f(wchar_t *param_1,wchar_t *param_2,wchar_t *param_3,uint param_4,uintptr_t param_5,
            int *param_6)

{
  __acrt_ptd *p_Var1;
  int iVar2;
  byte bVar3;
  code *pcVar4;
  
  p_Var1 = (__acrt_ptd *)*param_6;
  if (((p_Var1 == (__acrt_ptd *)0x0) &&
      (p_Var1 = FUN_00547e5d(param_6), p_Var1 == (__acrt_ptd *)0x0)) ||
     (pcVar4 = *(code **)(p_Var1 + 0x35c), pcVar4 == (code *)0x0)) {
    iVar2 = FUN_00547e34((int)param_6);
    bVar3 = (byte)DAT_005b25c0 & 0x1f;
    pcVar4 = (code *)((*(uint *)(&DAT_005e530c + iVar2 * 4) ^ DAT_005b25c0) >> bVar3 |
                     (*(uint *)(&DAT_005e530c + iVar2 * 4) ^ DAT_005b25c0) << 0x20 - bVar3);
    if (pcVar4 == (code *)0x0) {
                    /* WARNING: Subroutine does not return */
      __invoke_watson(param_1,param_2,param_3,param_4,param_5);
    }
  }
  (*(code *)PTR_guard_check_icall_0056c37c)(param_1,param_2,param_3,param_4,param_5);
  (*pcVar4)();
  return;
}

