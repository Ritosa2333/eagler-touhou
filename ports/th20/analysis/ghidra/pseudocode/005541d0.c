/* Entry: 0x005541d0; symbol: FUN_005541d0; body bytes: 49 */

void __cdecl FUN_005541d0(int param_1)

{
  __acrt_ptd *p_Var1;
  
  if (param_1 == 1) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x21;
  }
  else if ((param_1 == 2) || (param_1 == 3)) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x22;
    return;
  }
  return;
}

