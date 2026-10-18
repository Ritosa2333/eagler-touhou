/* Entry: 0x004fc5f0; symbol: FUN_004fc5f0; body bytes: 513 */

void __thiscall FUN_004fc5f0(void *this,uint param_1,byte *param_2,byte *param_3)

{
  undefined4 *puVar1;
  byte *pbVar2;
  undefined1 uVar3;
  undefined1 local_b8 [12];
  undefined4 local_ac;
  undefined1 local_a8 [4];
  undefined4 local_a4;
  undefined1 local_a0 [4];
  undefined1 local_9c [4];
  undefined4 local_98;
  undefined4 local_94;
  undefined4 local_90;
  undefined4 local_8c;
  undefined4 local_88;
  undefined4 local_84;
  undefined4 local_80;
  undefined4 local_7c;
  undefined4 local_78;
  undefined4 local_74;
  undefined4 *local_6c;
  void *local_68;
  undefined4 *local_64;
  void *local_60;
  int local_5c;
  void *local_58;
  void *local_54;
  void *local_50;
  int local_4c [4];
  undefined4 local_3c [5];
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  uint local_14;
  
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  local_64 = (undefined4 *)this;
  local_54 = this;
  local_50 = this;
  local_60 = FUN_004fecd0(this,local_9c);
  local_58 = local_50;
  puVar1 = (undefined4 *)FUN_004fecd0(local_50,local_a0);
  local_5c = FUN_0040c300(puVar1);
  local_5c = (int)param_2 - local_5c;
  FUN_004f58c0(local_60,&local_a4,local_5c);
  FUN_004feb80(local_64);
  FUN_0040c080(&local_28,0x10);
  FUN_004ec3e0((char *)&local_28,(void *)((int)local_50 + 0x10),param_1);
  if ((char)local_28 == '\r') {
    FUN_004ff060(&local_20,local_50,(int)local_50 + 0x10);
    local_68 = local_50;
    puVar1 = (undefined4 *)FUN_004fecd0(local_50,local_a8);
    FUN_0040c300(puVar1);
  }
  else {
    FUN_004f3d70(local_3c);
    uVar3 = (undefined1)local_28;
    puVar1 = (undefined4 *)FUN_004f4050(local_b8,local_3c,local_50,(int)local_50 + 0x10);
    FUN_004f4010(local_4c,puVar1,uVar3);
    pbVar2 = FUN_004edf70(param_2,param_3,local_4c);
    if ((pbVar2 == param_3) || (*pbVar2 != 0x7d)) {
      func_0x004fcd20("Missing \'}\' in format string.");
    }
    local_6c = (undefined4 *)((int)local_50 + 0x10);
    local_80 = local_28;
    local_7c = local_24;
    local_78 = local_20;
    local_74 = local_1c;
    local_90 = local_28;
    local_8c = local_24;
    local_88 = local_20;
    local_84 = local_1c;
    local_98 = move<>((int)local_50 + 0x10);
    local_94 = move<>(local_3c);
    FUN_004f3af0(&local_ac,&local_98);
    FUN_004feb60(local_6c);
  }
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffff0);
  return;
}

