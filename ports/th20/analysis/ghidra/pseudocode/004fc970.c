/* Entry: 0x004fc970; symbol: FUN_004fc970; body bytes: 373 */

void FUN_004fc970(uint param_1,int param_2)

{
  undefined4 *puVar1;
  undefined4 local_94;
  undefined4 local_90;
  undefined1 local_8c [4];
  undefined1 local_88 [4];
  undefined4 local_84;
  undefined4 local_80;
  undefined4 local_7c;
  undefined4 local_78;
  undefined4 *local_74;
  undefined4 local_70;
  undefined4 local_6c;
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 *local_4c;
  undefined4 *local_48;
  undefined4 *local_44;
  undefined4 *local_40;
  void *local_3c;
  int local_38;
  undefined4 *local_34;
  undefined4 *local_30;
  undefined4 *local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  uint local_14;
  
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  FUN_0040c080(&local_28,0x10);
  FUN_004ec3e0((char *)&local_28,local_2c + 4,param_1);
  if ((char)local_28 == '\r') {
    local_40 = local_2c;
    local_30 = local_2c;
    local_3c = FUN_004fecd0(local_2c,local_88);
    local_34 = local_2c;
    puVar1 = (undefined4 *)FUN_004fecd0(local_2c,local_8c);
    local_38 = FUN_0040c300(puVar1);
    local_38 = param_2 - local_38;
    FUN_004f58c0(local_3c,&local_90,local_38);
    FUN_004feb80(local_40);
  }
  local_4c = local_2c + 4;
  local_60 = local_28;
  local_5c = local_24;
  local_58 = local_20;
  local_54 = local_1c;
  local_70 = local_28;
  local_6c = local_24;
  local_68 = local_20;
  local_64 = local_1c;
  local_44 = local_2c + 4;
  FUN_004ff9e0(local_44,&local_84);
  puVar1 = (undefined4 *)FUN_004fbff0((int)(local_2c + 4));
  local_80 = *puVar1;
  local_7c = puVar1[1];
  local_48 = local_2c + 4;
  FUN_004fc010(local_48,&local_78);
  local_74 = local_2c;
  FUN_004f3b20(&local_94,&local_84);
  FUN_004feb60(local_4c);
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffff0);
  return;
}

