/* Entry: 0x0056433a; symbol: FUN_0056433a; body bytes: 173 */

undefined4 __cdecl FUN_0056433a(uint *param_1,short *param_2,uint param_3,wchar_t *param_4)

{
  wchar_t wVar1;
  __acrt_ptd *p_Var2;
  wchar_t *pwVar3;
  int iVar4;
  wchar_t *pwVar5;
  uint uVar6;
  
  if (param_1 == (uint *)0x0) {
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0x16;
    FUN_005480bc();
    return 0x16;
  }
  *param_1 = 0;
  if (param_2 == (short *)0x0) {
    if (param_3 == 0) goto LAB_00564376;
  }
  else if (param_3 != 0) {
    *param_2 = 0;
LAB_00564376:
    pwVar3 = common_getenv_nolock<wchar_t>(param_4);
    if (pwVar3 != (wchar_t *)0x0) {
      pwVar5 = pwVar3;
      do {
        wVar1 = *pwVar5;
        pwVar5 = pwVar5 + 1;
      } while (wVar1 != L'\0');
      uVar6 = ((int)pwVar5 - (int)(pwVar3 + 1) >> 1) + 1;
      *param_1 = uVar6;
      if (param_3 != 0) {
        if (param_3 < uVar6) {
          return 0x22;
        }
        iVar4 = FUN_00548100(param_2,param_3,(int)pwVar3);
        if (iVar4 != 0) {
                    /* WARNING: Subroutine does not return */
          __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
      }
    }
    return 0;
  }
  p_Var2 = FUN_005516c1();
  *(undefined4 *)p_Var2 = 0x16;
  FUN_005480bc();
  return 0x16;
}

