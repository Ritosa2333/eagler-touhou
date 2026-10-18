/* Entry: 0x0051bb90; symbol: FUN_0051bb90; body bytes: 117 */

undefined4 __fastcall FUN_0051bb90(int param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined4 local_c;
  
  if ((*(int *)(param_1 + 0x210f4) == 4) &&
     (bVar1 = FUN_0046b1b0((void *)(param_1 + 0x28),1), CONCAT31(extraout_var,bVar1) != 0)) {
    return 2;
  }
  if ((*(int *)(param_1 + 0x210f4) == 3) &&
     (bVar1 = FUN_0046b1b0((void *)(param_1 + 0x28),1), CONCAT31(extraout_var_00,bVar1) != 0)) {
    local_c = 1;
  }
  else {
    local_c = 0;
  }
  return local_c;
}

