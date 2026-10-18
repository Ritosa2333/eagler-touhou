/* Entry: 0x00414eb0; symbol: FUN_00414eb0; body bytes: 124 */

void __thiscall FUN_00414eb0(void *this,int param_1)

{
  undefined4 uVar1;
  undefined4 *local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_c = *(undefined4 **)(param_1 + 4);
  FUN_0040cd70(*(undefined4 *)((int)this + 4),&param_1);
  FUN_0040cd70(*(int *)((int)this + 4) + 4,&local_c);
  uVar1 = *(undefined4 *)((int)this + 4);
  *(undefined4 *)((int)this + 4) = 0;
  *(undefined4 *)(param_1 + 4) = uVar1;
  *local_c = uVar1;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

