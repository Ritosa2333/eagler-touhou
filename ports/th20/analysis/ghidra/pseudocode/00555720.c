/* Entry: 0x00555720; symbol: FUN_00555720; body bytes: 40 */

undefined4 __cdecl FUN_00555720(int param_1,int param_2)

{
  __acrt_ptd *p_Var1;
  
  if ((param_1 == 0) || (param_2 == 0)) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
  }
  return 0x7fffffff;
}

