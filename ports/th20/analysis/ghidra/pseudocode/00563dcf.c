/* Entry: 0x00563dcf; symbol: FUN_00563dcf; body bytes: 86 */

byte __cdecl FUN_00563dcf(uint param_1)

{
  __acrt_ptd *p_Var1;
  
  if (param_1 == 0xfffffffe) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 9;
  }
  else {
    if ((-1 < (int)param_1) && (param_1 < DAT_005e5890)) {
      return *(byte *)((&DAT_005e5690)[param_1 >> 6] + 0x28 + (param_1 & 0x3f) * 0x38) & 0x40;
    }
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 9;
    FUN_005480bc();
  }
  return 0;
}

