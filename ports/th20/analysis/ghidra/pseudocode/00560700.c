/* Entry: 0x00560700; symbol: FUN_00560700; body bytes: 302 */

int __cdecl FUN_00560700(short *param_1,int param_2,short *param_3,int param_4)

{
  short sVar1;
  __acrt_ptd *p_Var2;
  short *psVar3;
  short *psVar4;
  int iVar5;
  int iVar6;
  
  if (param_4 == 0) {
    if (param_1 == (short *)0x0) {
      if (param_2 == 0) {
        return param_4;
      }
      goto LAB_00560816;
    }
  }
  else if (param_1 == (short *)0x0) goto LAB_00560816;
  if (param_2 != 0) {
    psVar4 = param_1;
    iVar5 = param_2;
    if ((param_4 != 0) && (param_3 == (short *)0x0)) {
      *param_1 = 0;
      p_Var2 = FUN_005516c1();
      *(undefined4 *)p_Var2 = 0x16;
      FUN_005480bc();
      return 0x16;
    }
    while (*psVar4 != 0) {
      iVar5 = iVar5 + -1;
      psVar4 = psVar4 + 1;
      if (iVar5 == 0) {
        *param_1 = 0;
        p_Var2 = FUN_005516c1();
        *(undefined4 *)p_Var2 = 0x16;
        FUN_005480bc();
        return 0x16;
      }
    }
    if (param_4 == -1) {
      psVar3 = psVar4;
      do {
        sVar1 = *(short *)(((int)param_3 - (int)psVar4) + (int)psVar3);
        *psVar3 = sVar1;
        if (sVar1 == 0) break;
        iVar5 = iVar5 + -1;
        psVar3 = psVar3 + 1;
      } while (iVar5 != 0);
    }
    else {
      iVar6 = param_4;
      if (param_4 != 0) {
        do {
          sVar1 = *param_3;
          param_3 = param_3 + 1;
          *psVar4 = sVar1;
          psVar4 = psVar4 + 1;
          if (sVar1 == 0) {
            return 0;
          }
          iVar5 = iVar5 + -1;
        } while ((iVar5 != 0) && (iVar6 = iVar6 + -1, iVar6 != 0));
      }
      if (iVar6 == 0) {
        *psVar4 = 0;
      }
    }
    if (iVar5 != 0) {
      return 0;
    }
    if (param_4 == -1) {
      param_1[param_2 + -1] = 0;
      return 0x50;
    }
    *param_1 = 0;
    p_Var2 = FUN_005516c1();
    *(undefined4 *)p_Var2 = 0x22;
    FUN_005480bc();
    return 0x22;
  }
LAB_00560816:
  p_Var2 = FUN_005516c1();
  *(undefined4 *)p_Var2 = 0x16;
  FUN_005480bc();
  return 0x16;
}

