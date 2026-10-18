/* Entry: 0x005516c1; symbol: FUN_005516c1; body bytes: 21 */

__acrt_ptd * FUN_005516c1(void)

{
  __acrt_ptd *p_Var1;
  __acrt_ptd *p_Var2;
  
  p_Var1 = FUN_00557a13();
  p_Var2 = (__acrt_ptd *)&DAT_005b26bc;
  if (p_Var1 != (__acrt_ptd *)0x0) {
    p_Var2 = p_Var1 + 0x10;
  }
  return p_Var2;
}

