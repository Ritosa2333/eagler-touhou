/* Entry: 0x005486a0; symbol: FUN_005486a0; body bytes: 155 */

undefined4 __cdecl FUN_005486a0(char *param_1,int param_2,int param_3)

{
  char cVar1;
  __acrt_ptd *p_Var2;
  char *pcVar3;
  char *pcVar4;
  
  if ((param_1 != (char *)0x0) && (param_2 != 0)) {
    pcVar3 = param_1;
    if (param_3 != 0) {
      do {
        if (*pcVar3 == '\0') {
          pcVar4 = pcVar3;
          do {
            cVar1 = pcVar4[param_3 - (int)pcVar3];
            *pcVar4 = cVar1;
            if (cVar1 == '\0') {
              return 0;
            }
            param_2 = param_2 + -1;
            pcVar4 = pcVar4 + 1;
          } while (param_2 != 0);
          *param_1 = '\0';
          p_Var2 = FUN_005516c1();
          *(undefined4 *)p_Var2 = 0x22;
          FUN_005480bc();
          return 0x22;
        }
        param_2 = param_2 + -1;
        pcVar3 = pcVar3 + 1;
      } while (param_2 != 0);
    }
    *param_1 = '\0';
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0x16;
    FUN_005480bc();
    return 0x16;
  }
  p_Var2 = FUN_005516c1();
  *(undefined4 *)p_Var2 = 0x16;
  FUN_005480bc();
  return 0x16;
}

