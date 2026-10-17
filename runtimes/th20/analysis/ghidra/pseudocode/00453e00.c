/* Entry: 0x00453e00; symbol: FUN_00453e00; body bytes: 57 */

void __fastcall FUN_00453e00(float *param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  
  bVar1 = FUN_00458f70((int)param_1);
  if (CONCAT31(extraout_var,bVar1) != 0) {
    param_1[3] = *param_1;
    param_1[4] = param_1[1];
    param_1[5] = param_1[2];
  }
  FUN_00453ac0(param_1);
  return;
}

