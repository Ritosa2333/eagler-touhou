/* Entry: 0x00538620; symbol: FUN_00538620; body bytes: 659 */

void __thiscall FUN_00538620(void *this,int param_1)

{
  bool bVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_90;
  undefined4 local_8c;
  void *local_88;
  void *local_84;
  void *local_80;
  void *local_7c;
  void *local_78;
  void *local_74;
  undefined4 local_70;
  void *local_6c;
  void *local_68;
  undefined4 local_64;
  undefined4 local_60;
  void *local_5c;
  void *local_58;
  int local_54;
  int local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_88 = this;
  for (local_50 = 0; local_50 < param_1; local_50 = local_50 + 1) {
    local_58 = (void *)FUN_00460830(0);
    iVar2 = FUN_005357c0(local_58,local_50);
    puVar3 = (undefined4 *)FUN_00421970(iVar2);
    local_14 = *puVar3;
    local_10 = puVar3[1];
    local_c = puVar3[2];
    local_5c = (void *)FUN_00414580(&DAT_005c69cc,local_50);
    bVar1 = FUN_00449430(local_5c,0);
    if (bVar1) {
      FUN_0047ba30(&local_4c);
      iVar2 = FUN_00460830(0);
      puVar3 = (undefined4 *)FUN_00460850(iVar2);
      local_4c = *puVar3;
      local_48 = puVar3[1];
      local_44 = puVar3[2];
      local_60 = FUN_00437520(0);
      puVar3 = FUN_0049db70(&local_8c,7,&local_4c,0);
      local_64 = *puVar3;
      puVar3 = (undefined4 *)FUN_00414580(&DAT_005c69cc,local_50);
      *puVar3 = local_64;
      local_68 = (void *)FUN_00414580(&DAT_005c69cc,local_50);
      FUN_004506b0(local_68,DAT_0056cd94,DAT_0056c8cc);
      puVar3 = (undefined4 *)FUN_00414580(&DAT_005c69e0,local_50);
      FUN_004c1f00(puVar3);
      local_6c = (void *)FUN_00478ec0(0);
      puVar3 = FUN_004c1b80(local_6c,&local_90,&local_14,DAT_0056cd94,0,9999999,2);
      local_70 = *puVar3;
      puVar3 = (undefined4 *)FUN_00414580(&DAT_005c69e0,local_50);
      *puVar3 = local_70;
      puVar3 = (undefined4 *)FUN_00414580(&DAT_005c69e0,local_50);
      local_74 = (void *)FUN_004c0e10(puVar3);
      FUN_0047a4e0(local_74,3);
      puVar3 = (undefined4 *)FUN_00414580(&DAT_005c69e0,local_50);
      local_78 = (void *)FUN_004c0e10(puVar3);
      FUN_00477750(local_78,9999999);
      puVar3 = (undefined4 *)FUN_00414580(&DAT_005c69e0,local_50);
      local_7c = (void *)FUN_004c0e10(puVar3);
      FUN_00506cf0(local_7c,1);
    }
    local_80 = (void *)FUN_00414580(&DAT_005c69cc,local_50);
    FUN_004645e0(local_80,&local_14);
    local_84 = (void *)FUN_00414580(&DAT_005c69e0,local_50);
    FUN_004c1ff0(local_84,&local_14);
  }
  for (local_54 = param_1; local_54 < 4; local_54 = local_54 + 1) {
    puVar3 = (undefined4 *)FUN_00414580(&DAT_005c69cc,local_54);
    FUN_0044fcd0(puVar3);
    puVar3 = (undefined4 *)FUN_00414580(&DAT_005c69e0,local_54);
    FUN_004c1f00(puVar3);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

