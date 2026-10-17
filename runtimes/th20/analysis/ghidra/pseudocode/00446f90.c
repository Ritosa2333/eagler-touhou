/* Entry: 0x00446f90; symbol: FUN_00446f90; body bytes: 157 */

short * __cdecl FUN_00446f90(short *param_1,short *param_2,short *param_3)

{
  char cVar1;
  short *psVar2;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    cVar1 = FUN_00446f50();
    psVar2 = param_2;
    if (cVar1 != '\0') {
      FUN_00425620(&param_1);
      FUN_00425620(&param_2);
      psVar2 = (short *)FUN_00447030();
    }
  }
  else {
    for (; (psVar2 = param_1, param_1 != param_2 && (*param_1 != *param_3)); param_1 = param_1 + 1)
    {
    }
  }
  return psVar2;
}

