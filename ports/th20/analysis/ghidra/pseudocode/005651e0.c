/* Entry: 0x005651e0; symbol: FUN_005651e0; body bytes: 242 */

int __cdecl FUN_005651e0(char *param_1,int param_2,int param_3,int param_4)

{
  char *pcVar1;
  char cVar2;
  __acrt_ptd *p_Var3;
  int iVar4;
  char *pcVar5;
  int iVar6;
  int iVar7;
  
  if (param_4 == 0) {
    if (param_1 == (char *)0x0) {
      if (param_2 == 0) {
        return param_4;
      }
      goto LAB_00565222;
    }
  }
  else if (param_1 == (char *)0x0) goto LAB_00565222;
  if (param_2 != 0) {
    if (param_4 == 0) {
      *param_1 = '\0';
      return param_4;
    }
    if (param_3 != 0) {
      pcVar5 = param_1;
      iVar6 = param_2;
      iVar4 = param_4;
      if (param_4 == -1) {
        do {
          cVar2 = pcVar5[param_3 - (int)param_1];
          *pcVar5 = cVar2;
          if (cVar2 == '\0') {
            return 0;
          }
          iVar6 = iVar6 + -1;
          pcVar5 = pcVar5 + 1;
        } while (iVar6 != 0);
        iVar6 = 0;
      }
      else {
        do {
          iVar7 = iVar4;
          cVar2 = pcVar5[param_3 - (int)param_1];
          pcVar1 = pcVar5 + 1;
          *pcVar5 = cVar2;
          if (cVar2 == '\0') {
            return 0;
          }
          iVar6 = iVar6 + -1;
        } while ((iVar6 != 0) && (pcVar5 = pcVar1, iVar4 = iVar7 + -1, iVar7 + -1 != 0));
        iVar4 = iVar7 + -1;
        if (iVar6 == 0) {
          iVar4 = iVar7;
        }
        if (iVar4 == 0) {
          *pcVar1 = '\0';
        }
      }
      if (iVar6 != 0) {
        return 0;
      }
      if (param_4 != -1) {
        *param_1 = '\0';
        p_Var3 = FUN_005516c1();
        *(undefined4 *)p_Var3 = 0x22;
        FUN_005480bc();
        return 0x22;
      }
      param_1[param_2 + -1] = '\0';
      return 0x50;
    }
    *param_1 = '\0';
  }
LAB_00565222:
  p_Var3 = FUN_005516c1();
  *(undefined4 *)p_Var3 = 0x16;
  FUN_005480bc();
  return 0x16;
}

