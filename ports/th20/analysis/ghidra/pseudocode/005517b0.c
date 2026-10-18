/* Entry: 0x005517b0; symbol: FUN_005517b0; body bytes: 161 */

undefined4 __cdecl FUN_005517b0(short *param_1,int param_2,int param_3)

{
  short sVar1;
  __acrt_ptd *p_Var2;
  short *psVar3;
  short *psVar4;
  
  if ((param_1 != (short *)0x0) && (param_2 != 0)) {
    psVar3 = param_1;
    if (param_3 != 0) {
      do {
        if (*psVar3 == 0) {
          psVar4 = psVar3;
          do {
            sVar1 = *(short *)((param_3 - (int)psVar3) + (int)psVar4);
            *psVar4 = sVar1;
            if (sVar1 == 0) {
              return 0;
            }
            param_2 = param_2 + -1;
            psVar4 = psVar4 + 1;
          } while (param_2 != 0);
          *param_1 = 0;
          p_Var2 = FUN_005516c1();
          *(undefined4 *)p_Var2 = 0x22;
          FUN_005480bc();
          return 0x22;
        }
        param_2 = param_2 + -1;
        psVar3 = psVar3 + 1;
      } while (param_2 != 0);
    }
    *param_1 = 0;
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

