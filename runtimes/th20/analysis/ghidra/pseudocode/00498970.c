/* Entry: 0x00498970; symbol: FUN_00498970; body bytes: 92 */

void __fastcall FUN_00498970(int param_1)

{
  int *piVar1;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0040c080(&local_c,4);
  FUN_004891b0(&local_c,(void *)(param_1 + 0x54));
  piVar1 = (int *)FUN_0045f420(&local_c);
  FUN_004aaac0(piVar1);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

