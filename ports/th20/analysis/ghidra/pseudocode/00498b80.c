/* Entry: 0x00498b80; symbol: FUN_00498b80; body bytes: 396 */

void __thiscall FUN_00498b80(void *this,void *param_1,int param_2)

{
  bool bVar1;
  undefined4 *puVar2;
  uint uVar3;
  int iVar4;
  undefined4 uVar5;
  _Ptr_base<struct__EXCEPTION_RECORD_const_> local_5c [8];
  _Ptr_base<struct__EXCEPTION_RECORD_const_> local_54 [8];
  undefined1 local_4c [4];
  undefined1 local_48 [4];
  undefined4 local_44;
  undefined4 local_40;
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
  pDNameNode *local_14;
  int local_10;
  undefined1 local_9;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_20 = (void *)((int)this + 0x168);
  local_1c = this;
  puVar2 = (undefined4 *)FUN_004981c0(local_20,local_48);
  local_28 = *puVar2;
  local_24 = (void *)((int)local_1c + 0x168);
  puVar2 = (undefined4 *)FUN_00497e70(local_24,local_4c);
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
  local_14 = (pDNameNode *)FUN_0048bdc0(&local_10);
  bVar1 = FUN_004890a0((undefined4 *)local_14);
  if (!bVar1) {
    iVar4 = pDNameNode::length(local_14);
    if (iVar4 < 2) goto LAB_00498cdd;
  }
  bVar1 = FUN_004890a0((undefined4 *)local_14);
  if (bVar1) {
    local_44 = FUN_0047b290(local_5c,&local_9);
    local_40 = local_44;
    FUN_0047c530(local_44);
    FUN_0047c300((int)local_5c);
  }
  else {
    uVar5 = FUN_0040c300((undefined4 *)local_14);
    local_3c = FUN_0048a8a0(local_54,&local_9,uVar5);
    local_38 = local_3c;
    FUN_0047c530(local_3c);
    FUN_0047c300((int)local_54);
  }
LAB_00498cdd:
  local_18 = FUN_0040c300((undefined4 *)local_14);
  FUN_00467cc0(param_1,&local_14,&local_18);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

