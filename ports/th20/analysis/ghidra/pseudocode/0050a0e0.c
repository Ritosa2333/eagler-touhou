/* Entry: 0x0050a0e0; symbol: FUN_0050a0e0; body bytes: 47 */

int __fastcall FUN_0050a0e0(int param_1)

{
  int iVar1;
  undefined4 local_8;
  
  local_8 = param_1;
  while( true ) {
    iVar1 = FUN_0040ff90(local_8);
    if (iVar1 == 0) break;
    local_8 = FUN_0040ff90(local_8);
  }
  return local_8;
}

