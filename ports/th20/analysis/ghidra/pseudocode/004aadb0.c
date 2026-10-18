/* Entry: 0x004aadb0; symbol: FUN_004aadb0; body bytes: 61 */

undefined * __fastcall FUN_004aadb0(int *param_1)

{
  int iVar1;
  undefined *local_c;
  
  iVar1 = FUN_004aaac0(param_1);
  if (iVar1 == 0) {
    local_c = &DAT_0057038c;
  }
  else {
    iVar1 = FUN_004aaac0(param_1);
    local_c = (undefined *)FUN_0047a2c0(iVar1);
  }
  return local_c;
}

