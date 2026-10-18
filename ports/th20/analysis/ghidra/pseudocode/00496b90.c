/* Entry: 0x00496b90; symbol: FUN_00496b90; body bytes: 967 */

void __fastcall FUN_00496b90(void *param_1)

{
  undefined1 uVar1;
  uint uVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  float10 fVar5;
  undefined4 local_88 [3];
  undefined4 local_7c;
  undefined4 local_78;
  undefined4 *local_74;
  float local_70;
  float local_6c;
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  undefined4 local_4c;
  float local_48;
  float local_44;
  float local_40;
  float local_3c;
  float local_38;
  float local_34;
  int local_30;
  void *local_2c;
  void *local_28;
  int local_24;
  int local_20;
  byte local_19;
  byte local_18;
  undefined1 local_17;
  undefined1 local_16;
  undefined1 local_15;
  void *local_14;
  void *local_10;
  byte local_c;
  undefined1 local_b;
  undefined1 local_a;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10 = param_1;
  local_30 = FUN_00499310(*(int *)((int)param_1 + 8));
  local_24 = FUN_004ab080(local_10,0.0);
  uVar2 = FUN_0049cb30((int)local_10 + 0xc);
  if (uVar2 < local_24 + 1U) {
    local_28 = (void *)((int)local_10 + 0xc);
    FUN_0049c2a0(local_28,local_24 + 1);
  }
  local_2c = (void *)((int)local_10 + 0xc);
  puVar3 = (undefined4 *)FUN_0048bce0(local_2c,local_24);
  local_14 = (void *)FUN_0044ced0(puVar3);
  if (local_14 != (void *)0x0) {
    local_20 = *(short *)(local_30 + 4) + -0x13f;
    switch(local_20) {
    case 0:
      fVar5 = (float10)FUN_004aafb0(local_10,1);
      FUN_00450590(local_14,(float)fVar5);
      break;
    case 6:
      local_15 = FUN_004ab080(local_10,4.2039e-45);
      local_16 = FUN_004ab080(local_10,2.8026e-45);
      local_17 = FUN_004ab080(local_10,1.4013e-45);
      FUN_0049c860(local_14,local_17,local_16,local_15);
      break;
    case 7:
      FUN_0048b720(&local_c);
      local_a = FUN_004ab080(local_10,4.2039e-45);
      local_b = FUN_004ab080(local_10,5.60519e-45);
      local_c = FUN_004ab080(local_10,7.00649e-45);
      local_54 = FUN_004ab080(local_10,2.8026e-45);
      local_58 = FUN_004ab080(local_10,1.4013e-45);
      FUN_0049c5e0(local_14,local_58,local_54,&local_c);
      break;
    case 8:
      uVar1 = FUN_004ab080(local_10,1.4013e-45);
      FUN_00470b60(local_14,uVar1);
      break;
    case 9:
      local_18 = FUN_004ab080(local_10,4.2039e-45);
      local_5c = FUN_004ab080(local_10,2.8026e-45);
      local_60 = FUN_004ab080(local_10,1.4013e-45);
      FUN_00464580(local_14,local_60,local_5c,local_18);
      break;
    case 10:
      fVar5 = (float10)FUN_004aafb0(local_10,2);
      local_34 = (float)fVar5;
      fVar5 = (float10)FUN_004aafb0(local_10,1);
      local_38 = (float)fVar5;
      FUN_00470e50(local_14,local_38,local_34);
      break;
    case 0xb:
      fVar5 = (float10)FUN_004aafb0(local_10,4);
      local_44 = (float)fVar5;
      fVar5 = (float10)FUN_004aafb0(local_10,3);
      local_48 = (float)fVar5;
      local_4c = FUN_004ab080(local_10,2.8026e-45);
      local_50 = FUN_004ab080(local_10,1.4013e-45);
      FUN_0049c680(local_14,local_50,local_4c,local_48,local_44);
      break;
    case 0xc:
      uVar1 = FUN_004ab080(local_10,1.4013e-45);
      FUN_0049c8a0(local_14,uVar1);
      break;
    case 0xd:
      local_19 = FUN_004ab080(local_10,4.2039e-45);
      local_64 = FUN_004ab080(local_10,2.8026e-45);
      local_68 = FUN_004ab080(local_10,1.4013e-45);
      FUN_0049c520(local_14,local_68,local_64,local_19);
      break;
    case 0xe:
      fVar5 = (float10)FUN_004aafb0(local_10,3);
      local_70 = (float)fVar5;
      fVar5 = (float10)FUN_004aafb0(local_10,4);
      local_6c = (float)fVar5;
      FUN_00422dd0(local_88,local_70,local_6c,0);
      local_74 = (undefined4 *)FUN_0044cb20((int)local_14);
      local_78 = FUN_004ab080(local_10,2.8026e-45);
      local_7c = FUN_004ab080(local_10,1.4013e-45);
      FUN_004614a0(local_14,local_7c,local_78,local_74,local_88);
      break;
    case 0x10:
      fVar5 = (float10)FUN_004aafb0(local_10,2);
      local_3c = (float)fVar5;
      fVar5 = (float10)FUN_004aafb0(local_10,1);
      local_40 = (float)fVar5;
      FUN_004864f0(local_14,local_40,local_3c);
      break;
    case 0x11:
      uVar4 = FUN_004ab080(local_10,1.4013e-45);
      FUN_00450350(local_14,uVar4);
      break;
    case 0x12:
      uVar1 = FUN_004ab080(local_10,1.4013e-45);
      FUN_0045dd40(local_14,uVar1);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

