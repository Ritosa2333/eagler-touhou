/* Entry: 0x0044c830; symbol: FUN_0044c830; body bytes: 108 */

void __thiscall FUN_0044c830(void *this,undefined4 *param_1,int param_2,int param_3)

{
  int iVar1;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar1 = FUN_0044c6b0(this,param_2,param_3);
  if (iVar1 == 0) {
    FUN_00425cc0(&local_c);
    FUN_004117a0(&local_c,0);
    *param_1 = local_c;
  }
  else {
    *param_1 = *(undefined4 *)(iVar1 + 0x4c0);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

