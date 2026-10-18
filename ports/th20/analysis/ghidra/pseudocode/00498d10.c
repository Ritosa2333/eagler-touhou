/* Entry: 0x00498d10; symbol: FUN_00498d10; body bytes: 305 */

void __thiscall FUN_00498d10(void *this,void *param_1,int param_2)

{
  bool bVar1;
  undefined4 *puVar2;
  uint uVar3;
  _Ptr_base<struct__EXCEPTION_RECORD_const_> local_4c [8];
  undefined1 local_44 [4];
  undefined1 local_40 [4];
  undefined4 local_3c;
  undefined4 local_38;
  void *local_34;
  void *local_30;
  undefined4 local_2c;
  undefined4 local_28;
  void *local_24;
  void *local_20;
  void *local_1c;
  undefined4 local_18;
  undefined4 *local_14;
  int local_10;
  undefined1 local_9;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_20 = (void *)((int)this + 0x168);
  local_1c = this;
  puVar2 = (undefined4 *)FUN_004981c0(local_20,local_40);
  local_28 = *puVar2;
  local_24 = (void *)((int)local_1c + 0x168);
  puVar2 = (undefined4 *)FUN_00497e70(local_24,local_44);
  local_2c = *puVar2;
  uVar3 = FUN_0048b0c0();
  if (uVar3 < param_2 + 1U) {
    local_30 = (void *)((int)local_1c + 0x168);
    FUN_0049c280(local_30,param_2 + 1);
  }
  FUN_0040c080(&local_10,4);
  local_34 = (void *)((int)local_1c + 0x168);
  FUN_00497e70(local_34,&local_10);
  FUN_0048a820(&local_10,param_2);
  local_14 = (undefined4 *)FUN_0048bdc0(&local_10);
  bVar1 = FUN_004890a0(local_14);
  if (bVar1) {
    local_3c = FUN_0047b290(local_4c,&local_9);
    local_38 = local_3c;
    FUN_0047c530(local_3c);
    FUN_0047c300((int)local_4c);
  }
  local_18 = FUN_0040c300(local_14);
  FUN_00467cc0(param_1,&local_14,&local_18);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

