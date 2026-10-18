/* Entry: 0x00548610; symbol: FUN_00548610; body bytes: 143 */

undefined4 __cdecl FUN_00548610(char *param_1,int param_2,int param_3)

{
  char cVar1;
  __acrt_ptd *p_Var2;
  char *pcVar3;
  
  if ((param_1 != (char *)0x0) && (param_2 != 0)) {
    if (param_3 == 0) {
      *param_1 = '\0';
      p_Var2 = FUN_005516c1();
      *(undefined4 *)p_Var2 = 0x16;
      FUN_005480bc();
      return 0x16;
    }
    pcVar3 = param_1;
    do {
      cVar1 = pcVar3[param_3 - (int)param_1];
      *pcVar3 = cVar1;
      if (cVar1 == '\0') {
        return 0;
      }
      param_2 = param_2 + -1;
      pcVar3 = pcVar3 + 1;
    } while (param_2 != 0);
    *param_1 = '\0';
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0x22;
    FUN_005480bc();
    return 0x22;
  }
  p_Var2 = FUN_005516c1();
  *(undefined4 *)p_Var2 = 0x16;
  FUN_005480bc();
  return 0x16;
}

