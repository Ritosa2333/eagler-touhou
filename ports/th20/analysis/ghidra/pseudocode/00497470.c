/* Entry: 0x00497470; symbol: FUN_00497470; body bytes: 94 */

void __fastcall FUN_00497470(int param_1)

{
  undefined4 uVar1;
  uint uVar2;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar1 = FUN_0040c310(&DAT_005c0240,10);
  FUN_0040b730(&local_c,uVar1);
  uVar2 = FUN_004235f0((uint *)(param_1 + 4));
  *(uint *)(param_1 + 0x14) = uVar2;
  FUN_0040b900(&local_c);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

