/* Entry: 0x004ab340; symbol: FUN_004ab340; body bytes: 62 */

undefined1 __fastcall FUN_004ab340(int param_1)

{
  bool bVar1;
  undefined1 local_c;
  
  bVar1 = FUN_004887a0(param_1);
  if ((bVar1) && ((*(uint *)(param_1 + 0x78) >> 5 & 1) != 0)) {
    local_c = 1;
  }
  else {
    local_c = 0;
  }
  return local_c;
}

