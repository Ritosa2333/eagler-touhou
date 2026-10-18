/* Entry: 0x004b5bb0; symbol: FUN_004b5bb0; body bytes: 1647 */

void __fastcall FUN_004b5bb0(void *param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  int *piVar3;
  int iVar4;
  uint uVar5;
  float10 fVar6;
  undefined4 local_f0;
  undefined4 local_ec;
  undefined4 local_e8;
  undefined4 local_e4;
  undefined4 local_e0;
  undefined4 local_dc;
  undefined4 local_d8;
  undefined4 local_d4;
  undefined4 local_d0;
  undefined4 local_cc;
  int local_c8;
  undefined4 local_c4;
  int local_c0;
  undefined4 local_bc;
  undefined4 local_b8;
  void *local_b4;
  undefined4 local_b0;
  int local_ac;
  void *local_a8;
  float local_a4;
  undefined4 local_a0;
  undefined4 local_9c;
  void *local_98;
  undefined4 local_94;
  undefined4 local_90;
  int local_8c;
  int local_88;
  int local_84;
  int local_80;
  int local_7c;
  int local_78;
  void *local_74;
  void *local_70;
  undefined4 local_6c;
  void *local_68;
  undefined4 local_64;
  undefined4 *local_60;
  void *local_5c;
  undefined4 local_58;
  void *local_54;
  undefined4 local_50;
  undefined4 *local_4c;
  void *local_48;
  undefined4 local_44;
  void *local_40;
  undefined4 local_3c;
  undefined4 *local_38;
  void *local_34;
  void *local_30;
  byte *local_2c;
  int local_28;
  int local_24;
  int local_20;
  void *local_1c;
  undefined4 local_18;
  float local_14;
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_1c = param_1;
  FUN_00421680((int)param_1);
  if (*(int *)((int)local_1c + 0x150) != 0) {
    FUN_00412d80(*(int *)((int)local_1c + 0x150));
  }
  local_a8 = (void *)((int)local_1c + 0x13c);
  bVar1 = FUN_00449430(local_a8,0);
  if (bVar1) {
    local_b8 = *(undefined4 *)((int)local_1c + 0x2cc);
    puVar2 = FUN_0049e080(&local_d4,(uint *)"front",0,(undefined4 *)0x0);
    *(undefined4 *)((int)local_1c + 0x13c) = *puVar2;
  }
  local_30 = (void *)((int)local_1c + 0x50);
  piVar3 = (int *)FUN_00414580(local_30,0);
  if (*piVar3 == 0) {
    for (local_24 = 0; local_24 < 7; local_24 = local_24 + 1) {
      local_3c = *(undefined4 *)((int)local_1c + 0x2cc);
      local_34 = (void *)((int)local_1c + 0x50);
      local_38 = (undefined4 *)FUN_00414580(local_34,local_24);
      puVar2 = FUN_004b8e30(&local_cc,(uint *)"front",local_24 + 0x20,-1,local_38);
      local_44 = *puVar2;
      local_40 = (void *)((int)local_1c + 0x10);
      puVar2 = (undefined4 *)FUN_00414580(local_40,local_24);
      *puVar2 = local_44;
    }
    for (local_28 = 0; local_28 < 7; local_28 = local_28 + 1) {
      local_50 = *(undefined4 *)((int)local_1c + 0x2cc);
      local_48 = (void *)((int)local_1c + 0x6c);
      local_4c = (undefined4 *)FUN_00414580(local_48,local_28);
      puVar2 = FUN_004b8e30(&local_d0,(uint *)"front",local_28 + 0x28,-1,local_4c);
      local_58 = *puVar2;
      local_54 = (void *)((int)local_1c + 0x2c);
      puVar2 = (undefined4 *)FUN_00414580(local_54,local_28);
      *puVar2 = local_58;
    }
    for (local_20 = 0; local_20 < 2; local_20 = local_20 + 1) {
      local_64 = FUN_00437950(DAT_005c0698);
      local_5c = (void *)((int)local_1c + 0x88);
      local_60 = (undefined4 *)FUN_00414580(local_5c,local_20);
      puVar2 = FUN_00450c70(&local_f0,(uint *)0x0,local_20 + 2,-1,local_60);
      local_6c = *puVar2;
      local_68 = (void *)((int)local_1c + 0x48);
      puVar2 = (undefined4 *)FUN_00414580(local_68,local_20);
      *puVar2 = local_6c;
      local_70 = (void *)((int)local_1c + 0x88);
      piVar3 = (int *)FUN_00414580(local_70,local_20);
      FUN_00450160(*piVar3);
      local_74 = (void *)((int)local_1c + 0x88);
      piVar3 = (int *)FUN_00414580(local_74,local_20);
      local_2c = (byte *)(*piVar3 + 0x4a3);
      *local_2c = *local_2c & 0xfc;
    }
  }
  iVar4 = FUN_00464080(0);
  local_78 = FUN_004b8100(iVar4);
  iVar4 = FUN_00464080(0);
  local_7c = FUN_004b8150(iVar4);
  iVar4 = FUN_00464080(0);
  local_80 = FUN_004b80b0(iVar4);
  FUN_004b8bf0(local_1c,local_80,local_7c,local_78);
  iVar4 = FUN_00464080(0);
  local_84 = FUN_004b7e20(iVar4);
  iVar4 = FUN_00464080(0);
  local_88 = FUN_004b7e70(iVar4);
  iVar4 = FUN_00464080(0);
  local_8c = FUN_00477ff0(iVar4);
  FUN_004b8650(local_1c,local_8c,local_88,local_84);
  iVar4 = FUN_004ac0c0(0x5c4d40);
  if (iVar4 != 8) {
    uVar5 = FUN_004b8390(0x5ba568);
    if (uVar5 == 0) {
      bVar1 = FUN_00488850(0x5ba568);
      if (!bVar1) {
        local_90 = *(undefined4 *)((int)local_1c + 0x170);
        FUN_00450c70(&local_d8,(uint *)0x0,1,-1,(undefined4 *)0x0);
      }
    }
  }
  uVar5 = FUN_004b8390(0x5ba568);
  if (uVar5 != 0) {
    local_94 = *(undefined4 *)((int)local_1c + 0x2cc);
    FUN_00450c70(&local_dc,(uint *)"front",0x6e,-1,(undefined4 *)0x0);
  }
  local_98 = (void *)((int)local_1c + 0x90);
  bVar1 = FUN_00449430(local_98,0);
  if (bVar1) {
    local_9c = *(undefined4 *)((int)local_1c + 0x2cc);
    puVar2 = FUN_00450c70(&local_e0,(uint *)"front",100,-1,(undefined4 *)0x0);
    *(undefined4 *)((int)local_1c + 0x90) = *puVar2;
  }
  iVar4 = FUN_00474d60(0x5ba568);
  if (iVar4 == 1) {
    iVar4 = FUN_00488830(DAT_005ba828);
    if (iVar4 != 0) goto LAB_004b5ff2;
    iVar4 = FUN_004b7fa0(0x5ba568);
    if (iVar4 != 0) goto LAB_004b5ff2;
  }
  else {
LAB_004b5ff2:
    if ((*(uint *)((int)local_1c + 0x1a4) >> 0xd & 1) == 0) goto LAB_004b60a0;
  }
  local_a0 = *(undefined4 *)((int)local_1c + 0x2cc);
  FUN_00450c70(&local_c,(uint *)"front",0x39,-1,(undefined4 *)0x0);
  FUN_00422e10(&local_18);
  local_18 = 0;
  iVar4 = FUN_00460830(0);
  fVar6 = FUN_004b8050(iVar4);
  local_a4 = (float)fVar6;
  local_14 = local_a4 * DAT_0056c8d0 - DAT_0056fa30;
  local_10 = 0;
  FUN_004502c0(&local_c,&local_18);
  *(uint *)((int)local_1c + 0x1a4) = *(uint *)((int)local_1c + 0x1a4) & 0xffffdfff;
LAB_004b60a0:
  iVar4 = FUN_0049c000();
  if (iVar4 != 0) {
    local_b0 = *(undefined4 *)((int)local_1c + 0x2cc);
    local_ac = FUN_004640a0(0x5ba568);
    local_ac = local_ac + 0x45;
    puVar2 = FUN_00450c70(&local_e4,(uint *)"front",local_ac,-1,(undefined4 *)0x0);
    *(undefined4 *)((int)local_1c + 0xf8) = *puVar2;
    FUN_00486350((void *)((int)local_1c + 0xf8));
    local_b4 = (void *)FUN_0051b960();
    FUN_0051a0b0(local_b4,1);
  }
  local_bc = *(undefined4 *)((int)local_1c + 0x2cc);
  local_c8 = FUN_004640a0(0x5ba568);
  local_c8 = local_c8 + 0x4b;
  puVar2 = FUN_00450c70(&local_e8,(uint *)"front",local_c8,-1,(undefined4 *)0x0);
  *(undefined4 *)((int)local_1c + 0xfc) = *puVar2;
  local_c4 = *(undefined4 *)((int)local_1c + 0x2cc);
  local_c0 = FUN_004b7f40(0x5ba568);
  local_c0 = local_c0 + 0x65;
  puVar2 = FUN_00450c70(&local_ec,(uint *)"front",local_c0,-1,(undefined4 *)0x0);
  *(undefined4 *)((int)local_1c + 0x100) = *puVar2;
  FUN_0049c720(local_1c,0);
  *(undefined4 *)((int)local_1c + 0x21c) = 0;
  *(undefined4 *)((int)local_1c + 0x270) = 0;
  *(undefined4 *)((int)local_1c + 0x2c4) = 0;
  FUN_004b7dd0((int)local_1c);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

