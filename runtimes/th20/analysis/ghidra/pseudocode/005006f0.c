/* Entry: 0x005006f0; symbol: FUN_005006f0; body bytes: 150 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_005006f0(int param_1)

{
  void *this;
  float10 fVar1;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  this = (void *)FUN_00421830(param_1 + 0x50);
  fVar1 = FUN_004298e0(0x5ba4a8);
  FUN_00452fc0(this,&local_c,((float)fVar1 * DAT_0056e0f0 * DAT_0056edd4) / _DAT_00570db0);
  fVar1 = FUN_004292e0(&local_c);
  FUN_0047a560((void *)(param_1 + 0x50),(float)fVar1);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

