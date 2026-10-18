/* Entry: 0x0040d840; symbol: FUN_0040d840; body bytes: 120 */

void FUN_0040d840(LPVOID param_1)

{
  undefined4 uVar1;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (param_1 != (LPVOID)0x0) {
    uVar1 = FUN_0040c310(&DAT_005c0240,1);
    FUN_0040b730(&local_c,uVar1);
    thunk_FUN_00557ba0(param_1);
    FUN_0040b900(&local_c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

