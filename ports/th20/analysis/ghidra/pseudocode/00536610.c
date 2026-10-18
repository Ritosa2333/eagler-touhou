/* Entry: 0x00536610; symbol: FUN_00536610; body bytes: 513 */

void __thiscall FUN_00536610(void *this,int param_1)

{
  bool bVar1;
  int iVar2;
  float *pfVar3;
  undefined4 *puVar4;
  undefined4 local_80;
  int local_7c;
  int local_78;
  void *local_74;
  void *local_70;
  void *local_6c;
  undefined4 local_68;
  undefined4 local_64;
  void *local_60;
  void *local_5c;
  int local_58;
  void *local_54;
  int local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_54 = this;
  for (local_50 = 0; local_50 < param_1; local_50 = local_50 + 1) {
    local_5c = (void *)FUN_00460830(0);
    iVar2 = FUN_005357c0(local_5c,local_50);
    pfVar3 = (float *)FUN_00421970(iVar2);
    local_14 = *pfVar3;
    local_10 = pfVar3[1];
    local_c = pfVar3[2];
    local_60 = (void *)FUN_00414580(&DAT_005c6984,local_50);
    bVar1 = FUN_00449430(local_60,0);
    if (bVar1) {
      FUN_0047ba30(&local_4c);
      iVar2 = FUN_00460830(0);
      puVar4 = (undefined4 *)FUN_00460850(iVar2);
      local_4c = *puVar4;
      local_48 = puVar4[1];
      local_44 = puVar4[2];
      local_64 = FUN_00437520(0);
      puVar4 = FUN_0049db70(&local_80,8,&local_4c,0);
      local_68 = *puVar4;
      puVar4 = (undefined4 *)FUN_00414580(&DAT_005c6984,local_50);
      *puVar4 = local_68;
      local_6c = (void *)FUN_00414580(&DAT_005c6984,local_50);
      FUN_004506b0(local_6c,DAT_0056cd94,DAT_0056c8cc);
    }
    local_70 = (void *)FUN_00414580(&DAT_005c6984,local_50);
    FUN_004645e0(local_70,&local_14);
    if (*(int *)((int)local_54 + 0x58) != 0) {
      *(int *)((int)local_54 + 0x58) = *(int *)((int)local_54 + 0x58) + -1;
    }
    if (*(int *)((int)local_54 + 0x58) == 0) {
      local_74 = (void *)FUN_00478e80(0);
      local_7c = FUN_0047cf60(local_74,&local_14,DAT_0056c8d0,0,99999,0);
      local_78 = *(int *)((int)local_54 + 0x58);
      *(int *)((int)local_54 + 0x58) = local_78 + local_7c;
    }
  }
  for (local_58 = param_1; local_58 < 4; local_58 = local_58 + 1) {
    puVar4 = (undefined4 *)FUN_00414580(&DAT_005c6984,local_58);
    FUN_0044fcd0(puVar4);
    puVar4 = (undefined4 *)FUN_00414580(&DAT_005c6998,local_58);
    FUN_004c1f00(puVar4);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

