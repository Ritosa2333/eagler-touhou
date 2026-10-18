/* Entry: 0x00537730; symbol: FUN_00537730; body bytes: 721 */

void __fastcall FUN_00537730(int param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  float *pfVar4;
  undefined4 *puVar5;
  undefined1 auStack_c8 [20];
  undefined4 uStack_b4;
  undefined4 local_98;
  void *local_94;
  void *local_90;
  int local_8c;
  int local_88;
  int local_84;
  void *local_80;
  void *local_7c;
  void *local_78;
  undefined4 local_74;
  undefined4 local_70;
  void *local_6c;
  void *local_68;
  void *local_64;
  int local_60;
  void *local_5c;
  int local_58;
  int local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  float local_18;
  float local_14;
  float local_10;
  undefined1 local_c [4];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_5c = (void *)(param_1 + 0x14);
  local_54 = param_1;
  bVar1 = FUN_00423590(local_5c,0);
  if (!bVar1) {
    local_64 = (void *)FUN_00464080(0);
    iVar2 = FUN_00464080(0);
    iVar2 = FUN_0051b920(iVar2);
    iVar3 = FUN_0040ff90(local_54 + 0x14);
    local_60 = FUN_00534260();
    local_60 = (iVar2 * iVar3) / local_60;
    FUN_004be940(local_64,local_60);
    local_58 = 0;
    while( true ) {
      iVar2 = FUN_00464080(0);
      iVar2 = FUN_004b81a0(iVar2);
      if (iVar2 <= local_58) break;
      local_68 = (void *)FUN_00460830(0);
      iVar2 = FUN_005357c0(local_68,local_58);
      pfVar4 = (float *)FUN_00421970(iVar2);
      local_18 = *pfVar4;
      local_14 = pfVar4[1];
      local_10 = pfVar4[2];
      local_6c = (void *)FUN_00414580(&DAT_005c69a8,local_58);
      bVar1 = FUN_00449430(local_6c,0);
      if (bVar1) {
        FUN_0047ba30(&local_50);
        iVar2 = FUN_00460830(0);
        puVar5 = (undefined4 *)FUN_00460850(iVar2);
        local_50 = *puVar5;
        local_4c = puVar5[1];
        local_48 = puVar5[2];
        local_70 = FUN_00437520(0);
        uStack_b4 = 0x53788b;
        puVar5 = FUN_0049db70(&local_98,8,&local_50,0);
        local_74 = *puVar5;
        puVar5 = (undefined4 *)FUN_00414580(&DAT_005c69a8,local_58);
        *puVar5 = local_74;
        local_78 = (void *)FUN_00414580(&DAT_005c69a8,local_58);
        FUN_004506b0(local_78,DAT_0056cd94,DAT_0056c8cc);
      }
      local_7c = (void *)FUN_00414580(&DAT_005c69a8,local_58);
      FUN_004645e0(local_7c,&local_18);
      FUN_0040c080(local_c,4);
      FUN_004142a0(local_c,local_54);
      if (*(int *)(local_54 + 0x58) != 0) {
        *(int *)(local_54 + 0x58) = *(int *)(local_54 + 0x58) + -1;
      }
      if (*(int *)(local_54 + 0x58) == 0) {
        local_80 = (void *)FUN_00478e80(0);
        FUN_00536e60(auStack_c8,local_c);
        local_8c = FUN_0047d240(local_80,&local_18,DAT_0056d7bc);
        local_88 = *(int *)(local_54 + 0x58);
        *(int *)(local_54 + 0x58) = local_88 + local_8c;
        *(int *)(local_54 + 0x58) = *(int *)(local_54 + 0x58) << 1;
        local_84 = local_8c;
        local_90 = (void *)FUN_00478ee0(0);
        uStack_b4 = 0x5379d6;
        FUN_004caad0(local_90,&local_18,DAT_0056d7bc,0,1);
      }
      local_58 = local_58 + 1;
    }
    local_94 = (void *)(local_54 + 0x14);
    FUN_00429420(local_94);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

