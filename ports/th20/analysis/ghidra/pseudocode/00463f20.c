/* Entry: 0x00463f20; symbol: FUN_00463f20; body bytes: 134 */

void __fastcall FUN_00463f20(int param_1)

{
  undefined4 uVar1;
  int local_14;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar1 = FUN_0040c310(&DAT_005c0240,0x14);
  FUN_0040b730(&local_c,uVar1);
  for (local_14 = param_1 + 0x58; local_14 < param_1 + 0x1d0; local_14 = local_14 + 1) {
  }
  FUN_0040b900(&local_c);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

