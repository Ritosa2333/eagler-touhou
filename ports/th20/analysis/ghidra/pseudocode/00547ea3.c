/* Entry: 0x00547ea3; symbol: FUN_00547ea3; body bytes: 28 */

int __fastcall FUN_00547ea3(int *param_1)

{
  __acrt_ptd *p_Var1;
  
  if (*param_1 == 0) {
    p_Var1 = FUN_00547e5d(param_1);
    if (p_Var1 == (__acrt_ptd *)0x0) {
                    /* WARNING: Subroutine does not return */
      _abort();
    }
  }
  return *param_1;
}

