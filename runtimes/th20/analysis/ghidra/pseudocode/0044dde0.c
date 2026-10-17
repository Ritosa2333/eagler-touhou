/* Entry: 0x0044dde0; symbol: FUN_0044dde0; body bytes: 441 */

void * __cdecl FUN_0044dde0(void *param_1)

{
  uint uVar1;
  uint uVar2;
  undefined4 *puVar3;
  uint extraout_EDX;
  undefined8 uVar4;
  longlong lVar5;
  undefined1 local_7c [8];
  undefined1 local_74 [8];
  undefined1 local_6c [8];
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  int local_4c;
  int local_48;
  int local_44;
  int local_40;
  longlong local_3c;
  undefined8 local_34;
  undefined8 local_2c;
  undefined8 local_24;
  undefined8 local_1c;
  uint local_14;
  uint local_10;
  undefined8 local_c;
  
  local_14 = FUN_0054187d();
  local_10 = extraout_EDX;
  uVar4 = FUN_00541866();
  local_54 = 10000000;
  local_50 = 0;
  local_5c = 24000000;
  local_58 = 0;
  local_c._0_4_ = (uint)uVar4;
  local_c._4_4_ = (uint)((ulonglong)uVar4 >> 0x20);
  uVar1 = local_c._4_4_;
  local_c = uVar4;
  if ((local_14 == 10000000) && (local_10 == 0)) {
    local_64 = 100;
    local_60 = 0;
    uVar2 = (uint)local_c;
    local_3c = __allmul(uVar2,uVar1,100,0);
    puVar3 = FUN_00446da0(local_6c,(undefined4 *)&local_3c);
    FUN_00447d20(param_1,puVar3);
  }
  else if ((local_14 == 24000000) && (local_10 == 0)) {
    uVar2 = (uint)local_c;
    uVar4 = __alldiv(uVar2,uVar1,24000000,0);
    local_1c = __allmul((uint)uVar4,(int)((ulonglong)uVar4 >> 0x20),1000000000,0);
    uVar4 = __allrem((uint)local_c,(uint)((ulonglong)local_c >> 0x20),24000000,0);
    lVar5 = __allmul((uint)uVar4,(int)((ulonglong)uVar4 >> 0x20),1000000000,0);
    uVar4 = __alldiv((uint)lVar5,(uint)((ulonglong)lVar5 >> 0x20),24000000,0);
    local_24._0_4_ = (uint)uVar4;
    local_44 = (uint)local_1c + (uint)local_24;
    local_24._4_4_ = (int)((ulonglong)uVar4 >> 0x20);
    local_40 = local_1c._4_4_ + local_24._4_4_ + (uint)CARRY4((uint)local_1c,(uint)local_24);
    local_24 = uVar4;
    puVar3 = FUN_00446da0(local_74,&local_44);
    FUN_00447d20(param_1,puVar3);
  }
  else {
    uVar2 = (uint)local_c;
    uVar4 = __alldiv(uVar2,uVar1,local_14,local_10);
    local_2c = __allmul((uint)uVar4,(int)((ulonglong)uVar4 >> 0x20),1000000000,0);
    uVar4 = __allrem((uint)local_c,(uint)((ulonglong)local_c >> 0x20),local_14,local_10);
    lVar5 = __allmul((uint)uVar4,(int)((ulonglong)uVar4 >> 0x20),1000000000,0);
    uVar4 = __alldiv((uint)lVar5,(uint)((ulonglong)lVar5 >> 0x20),local_14,local_10);
    local_34._0_4_ = (uint)uVar4;
    local_4c = (uint)local_2c + (uint)local_34;
    local_34._4_4_ = (int)((ulonglong)uVar4 >> 0x20);
    local_48 = local_2c._4_4_ + local_34._4_4_ + (uint)CARRY4((uint)local_2c,(uint)local_34);
    local_34 = uVar4;
    puVar3 = FUN_00446da0(local_7c,&local_4c);
    FUN_00447d20(param_1,puVar3);
  }
  return param_1;
}

