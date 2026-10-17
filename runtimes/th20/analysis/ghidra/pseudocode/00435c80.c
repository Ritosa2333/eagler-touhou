/* Entry: 0x00435c80; symbol: FUN_00435c80; body bytes: 5388 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00435c80(void *param_1)

{
  float *pfVar1;
  int iVar2;
  undefined4 *puVar3;
  void *this;
  void *this_00;
  float *pfVar4;
  float10 fVar5;
  undefined1 local_150 [12];
  float local_144;
  undefined4 local_140;
  undefined4 local_13c;
  float local_138;
  undefined4 local_134;
  undefined4 local_130;
  float local_12c;
  float local_128;
  float local_124;
  float local_120;
  float local_11c;
  float local_118;
  float local_114;
  float local_110;
  float local_10c;
  float local_108;
  float local_104;
  float local_100;
  float local_fc;
  float local_f8;
  undefined4 local_f4;
  undefined4 local_f0;
  float local_ec;
  float local_e8;
  float local_e4;
  float local_e0;
  float local_dc;
  float local_d8;
  undefined4 local_d4;
  float local_d0;
  float local_cc;
  float local_c8;
  float local_c4;
  float local_c0;
  float local_bc;
  float local_b8;
  float local_b4;
  int local_b0;
  int local_ac;
  int local_a8;
  int local_a4;
  int local_a0;
  int local_9c;
  int local_98;
  int local_94;
  float local_90;
  int local_8c;
  float local_88;
  float local_84;
  float local_80;
  float local_7c;
  float local_78;
  float local_74;
  float local_70;
  float local_6c;
  float local_68;
  float local_64;
  float local_60;
  float local_5c;
  float local_58;
  float local_54;
  float *local_50;
  float *local_4c;
  float *local_48;
  undefined4 *local_44;
  float *local_40;
  void *local_3c;
  float local_38 [3];
  float local_2c;
  undefined4 local_28;
  undefined4 local_24;
  float local_20;
  undefined4 local_1c;
  undefined4 local_18;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_8c = *(byte *)((int)param_1 + 0x498) - 9;
  local_3c = param_1;
  switch(*(byte *)((int)param_1 + 0x498)) {
  case 9:
    local_94 = *(int *)((int)param_1 + 0x444);
    local_88 = *(float *)((int)param_1 + 0x40);
    local_e4 = (DAT_0056e0f0 * DAT_0056c8d0) / (float)(local_94 + -1);
    local_40 = *(float **)((int)param_1 + 0x564);
    local_70 = 0.0;
    local_e0 = (float)*(int *)((int)param_1 + 0x448) / (float)(local_94 + -1);
    FUN_00422e10(&local_20);
    if (*(char *)((int)local_3c + 0x498) == '\t') {
      FUN_004376e0(local_3c,&local_20);
    }
    else {
      FUN_00422dd0(&local_138,0,0,0);
      local_20 = local_138;
      local_1c = local_134;
      local_18 = local_130;
    }
    *(uint *)((int)local_3c + 0x49c) = *(uint *)((int)local_3c + 0x49c) & 0xffdfffff;
    local_128 = *(float *)((int)local_3c + 0x490);
    if ((*(byte *)((int)local_3c + 0x4a1) >> 2 & 7) == 0) {
      local_d0 = *(float *)((int)local_3c + 0x490);
    }
    else {
      local_d0 = *(float *)((int)local_3c + 0x494);
    }
    local_12c = local_d0;
    local_58 = *(float *)((int)local_3c + 0x50) * DAT_0056e0ec + *(float *)((int)local_3c + 0x54);
    local_5c = *(float *)((int)local_3c + 0x54) - *(float *)((int)local_3c + 0x50) * DAT_0056e0ec;
    if ((*(int *)((int)local_3c + 0x55c) != 0) &&
       ((*(uint *)((int)local_3c + 0x49c) >> 0xc & 1) == 0)) {
      fVar5 = FUN_004379c0(*(int *)((int)local_3c + 0x55c));
      local_120 = (float)fVar5;
      local_58 = local_120 * local_58;
      fVar5 = FUN_004379a0(*(int *)((int)local_3c + 0x55c));
      local_124 = (float)fVar5;
      local_5c = local_124 * local_5c;
    }
    if (*(char *)((int)local_3c + 0x4a4) == '\x01') {
      local_58 = local_58 * DAT_005b8818;
      local_5c = local_5c * DAT_005b8818;
    }
    else if (*(char *)((int)local_3c + 0x4a4) == '\x02') {
      local_58 = DAT_005b8818 * DAT_0056e0ec * local_58;
      local_5c = DAT_005b8818 * DAT_0056e0ec * local_5c;
    }
    for (local_98 = 0; local_98 < local_94 + -1; local_98 = local_98 + 1) {
      local_40[3] = DAT_0056c8cc;
      local_40[4] = local_128;
      local_40[5] = *(float *)((int)local_3c + 0x378) + *(float *)((int)local_3c + 0x78);
      local_40[6] = local_70 + *(float *)((int)local_3c + 0x7c);
      FUN_00439330(local_40,local_40,local_88,local_58);
      local_40[2] = 0.0;
      FUN_004296e0(local_40,&local_20);
      pfVar1 = local_40 + 7;
      local_40[10] = DAT_0056c8cc;
      local_40[0xb] = local_12c;
      local_40[0xc] = *(float *)((int)local_3c + 0x380) + *(float *)((int)local_3c + 0x78);
      local_40[0xd] = local_70 + *(float *)((int)local_3c + 0x7c);
      local_40 = pfVar1;
      FUN_00439330(local_3c,pfVar1,local_88,local_5c);
      local_40[2] = 0.0;
      FUN_004296e0(local_40,&local_20);
      local_40 = local_40 + 7;
      local_70 = local_70 + local_e0;
      fVar5 = (float10)FUN_00438600(local_88,local_e4);
      local_88 = (float)fVar5;
    }
    pfVar1 = *(float **)((int)local_3c + 0x564);
    pfVar4 = local_40;
    for (iVar2 = 7; iVar2 != 0; iVar2 = iVar2 + -1) {
      *pfVar4 = *pfVar1;
      pfVar1 = pfVar1 + 1;
      pfVar4 = pfVar4 + 1;
    }
    local_40[6] = local_70 + *(float *)((int)local_3c + 0x7c);
    pfVar1 = (float *)(*(int *)((int)local_3c + 0x564) + 0x1c);
    pfVar4 = local_40 + 7;
    for (iVar2 = 7; iVar2 != 0; iVar2 = iVar2 + -1) {
      *pfVar4 = *pfVar1;
      pfVar1 = pfVar1 + 1;
      pfVar4 = pfVar4 + 1;
    }
    local_40[0xd] = local_70 + *(float *)((int)local_3c + 0x7c);
    break;
  default:
    break;
  case 0xd:
  case 0xe:
    local_a8 = *(int *)((int)param_1 + 0x444);
    fVar5 = FUN_00438540(*(float *)((int)param_1 + 0x40) -
                         *(float *)((int)param_1 + 0x38) / DAT_0056c8d0);
    local_78 = (float)fVar5;
    local_114 = *(float *)((int)local_3c + 0x38) / (float)(local_a8 + -1);
    local_4c = *(float **)((int)local_3c + 0x564);
    local_7c = 0.0;
    local_110 = (float)*(int *)((int)local_3c + 0x448) / (float)(local_a8 + -1);
    FUN_00422e10(local_38);
    FUN_004376e0(local_3c,local_38);
    *(uint *)((int)local_3c + 0x49c) = *(uint *)((int)local_3c + 0x49c) & 0xffdfffff;
    if (*(char *)((int)local_3c + 0x498) == '\x0e') {
      fVar5 = FUN_00438540(*(float *)((int)local_3c + 0x40));
      local_78 = (float)fVar5;
    }
    if ((*(byte *)((int)local_3c + 0x4a1) >> 2 & 7) == 0) {
      local_c0 = *(float *)((int)local_3c + 0x490);
    }
    else {
      local_c0 = *(float *)((int)local_3c + 0x494);
    }
    local_c4 = local_c0;
    local_68 = *(float *)((int)local_3c + 0x50) * DAT_0056e0ec + *(float *)((int)local_3c + 0x54);
    local_6c = *(float *)((int)local_3c + 0x54) - *(float *)((int)local_3c + 0x50) * DAT_0056e0ec;
    if ((*(int *)((int)local_3c + 0x55c) != 0) &&
       ((*(uint *)((int)local_3c + 0x49c) >> 0xc & 1) == 0)) {
      fVar5 = FUN_004379c0(*(int *)((int)local_3c + 0x55c));
      local_108 = (float)fVar5;
      local_68 = local_108 * local_68;
      fVar5 = FUN_004379a0(*(int *)((int)local_3c + 0x55c));
      local_10c = (float)fVar5;
      local_6c = local_10c * local_6c;
    }
    if (*(char *)((int)local_3c + 0x4a4) == '\x01') {
      local_68 = local_68 * DAT_005b8818;
      local_6c = local_6c * DAT_005b8818;
    }
    else if (*(char *)((int)local_3c + 0x4a4) == '\x02') {
      local_68 = DAT_005b8818 * DAT_0056e0ec * local_68;
      local_6c = DAT_005b8818 * DAT_0056e0ec * local_6c;
    }
    for (local_a4 = 0; local_a4 < local_a8; local_a4 = local_a4 + 1) {
      local_4c[3] = DAT_0056c8cc;
      local_4c[4] = local_c4;
      local_4c[5] = *(float *)((int)local_3c + 0x378) + *(float *)((int)local_3c + 0x78);
      local_4c[6] = local_7c + *(float *)((int)local_3c + 0x7c);
      FUN_00439330(local_4c,local_4c,local_78,local_68);
      local_4c[2] = 0.0;
      FUN_004296e0(local_4c,local_38);
      pfVar1 = local_4c + 7;
      local_4c[10] = DAT_0056c8cc;
      local_4c[0xb] = local_c4;
      local_4c[0xc] = *(float *)((int)local_3c + 0x380) + *(float *)((int)local_3c + 0x78);
      local_4c[0xd] = local_7c + *(float *)((int)local_3c + 0x7c);
      local_4c = pfVar1;
      FUN_00439330(local_3c,pfVar1,local_78,local_6c);
      local_4c[2] = 0.0;
      FUN_004296e0(local_4c,local_38);
      local_4c = local_4c + 7;
      local_7c = local_7c + local_110;
      fVar5 = (float10)FUN_00438600(local_78,local_114);
      local_78 = (float)fVar5;
    }
    break;
  case 0x18:
  case 0x19:
    local_b0 = *(int *)((int)param_1 + 0x444);
    fVar5 = FUN_00438540(*(float *)((int)param_1 + 0x460) -
                         *(float *)((int)param_1 + 0x454) / DAT_0056c8d0);
    local_84 = (float)fVar5;
    local_11c = *(float *)((int)local_3c + 0x454) / (float)(local_b0 + -1);
    local_50 = *(float **)((int)local_3c + 0x564);
    local_80 = 0.0;
    local_118 = (float)*(int *)((int)local_3c + 0x448) / (float)(local_b0 + -1);
    *(uint *)((int)local_3c + 0x49c) = *(uint *)((int)local_3c + 0x49c) & 0xffdfffff;
    if ((*(byte *)((int)local_3c + 0x4a1) >> 2 & 7) == 0) {
      local_c8 = *(float *)((int)local_3c + 0x490);
    }
    else {
      local_c8 = *(float *)((int)local_3c + 0x494);
    }
    local_cc = local_c8;
    FUN_00422e10(&local_14);
    local_bc = *(float *)((int)local_3c + 0x458) / DAT_0056c8d0;
    local_b4 = *(float *)((int)local_3c + 0x45c);
    local_b8 = *(float *)((int)local_3c + 0x45c);
    if (*(char *)((int)local_3c + 0x498) == '\x19') {
      local_bc = 0.0;
      local_b4 = local_b4 - *(float *)((int)local_3c + 0x458) / DAT_0056c8d0;
      local_b8 = *(float *)((int)local_3c + 0x458) / DAT_0056c8d0 + local_b8;
    }
    for (local_ac = 0; local_ac < local_b0; local_ac = local_ac + 1) {
      FUN_00439330(&local_14,&local_14,local_84,local_b4);
      pfVar1 = local_50;
      local_50[3] = local_cc;
      local_50[4] = *(float *)((int)local_3c + 0x378) + *(float *)((int)local_3c + 0x78);
      local_50[5] = local_80 + *(float *)((int)local_3c + 0x7c);
      *local_50 = local_14;
      local_50[1] = local_bc;
      local_50[2] = local_10;
      local_50 = local_50 + 6;
      FUN_00439330(pfVar1,&local_14,local_84,local_b8);
      local_50[3] = local_cc;
      local_50[4] = *(float *)((int)local_3c + 0x380) + *(float *)((int)local_3c + 0x78);
      local_50[5] = local_80 + *(float *)((int)local_3c + 0x7c);
      *local_50 = local_14;
      local_50[1] = (float)((uint)local_bc ^ _DAT_0056d7d0);
      local_50[2] = local_10;
      local_50 = local_50 + 6;
      local_80 = local_80 + local_118;
      fVar5 = (float10)FUN_00438600(local_84,local_11c);
      local_84 = (float)fVar5;
    }
    break;
  case 0x2f:
    local_9c = *(int *)((int)param_1 + 0x444);
    local_90 = *(float *)((int)param_1 + 0x40);
    local_fc = (DAT_0056e0f0 * DAT_0056c8d0) / (float)(local_9c + -1);
    local_44 = *(undefined4 **)((int)param_1 + 0x564);
    local_74 = 0.0;
    local_f8 = (float)*(int *)((int)param_1 + 0x448) / (float)(local_9c + -1);
    FUN_00422e10(&local_2c);
    FUN_00422dd0(&local_144,0,0,0);
    local_2c = local_144;
    local_28 = local_140;
    local_24 = local_13c;
    *(uint *)((int)local_3c + 0x49c) = *(uint *)((int)local_3c + 0x49c) & 0xffdfffff;
    local_f0 = *(undefined4 *)((int)local_3c + 0x490);
    if ((*(byte *)((int)local_3c + 0x4a1) >> 2 & 7) == 0) {
      local_d4 = *(undefined4 *)((int)local_3c + 0x490);
    }
    else {
      local_d4 = *(undefined4 *)((int)local_3c + 0x494);
    }
    local_f4 = local_d4;
    local_60 = *(float *)((int)local_3c + 0x454) * DAT_0056e0ec + *(float *)((int)local_3c + 0x458);
    local_64 = *(float *)((int)local_3c + 0x458) - *(float *)((int)local_3c + 0x454) * DAT_0056e0ec;
    if ((*(int *)((int)local_3c + 0x55c) != 0) &&
       ((*(uint *)((int)local_3c + 0x49c) >> 0xc & 1) == 0)) {
      fVar5 = FUN_004379c0(*(int *)((int)local_3c + 0x55c));
      local_e8 = (float)fVar5;
      local_60 = local_e8 * local_60;
      fVar5 = FUN_004379a0(*(int *)((int)local_3c + 0x55c));
      local_ec = (float)fVar5;
      local_64 = local_ec * local_64;
    }
    if (*(char *)((int)local_3c + 0x4a4) == '\x01') {
      local_60 = local_60 * DAT_005b8818;
      local_64 = local_64 * DAT_005b8818;
    }
    else if (*(char *)((int)local_3c + 0x4a4) == '\x02') {
      local_60 = DAT_005b8818 * DAT_0056e0ec * local_60;
      local_64 = DAT_005b8818 * DAT_0056e0ec * local_64;
    }
    for (local_a0 = 0; local_a0 < local_9c + -1; local_a0 = local_a0 + 1) {
      local_44[3] = local_f0;
      local_44[4] = *(float *)((int)local_3c + 0x378) + *(float *)((int)local_3c + 0x78);
      local_44[5] = local_74 + *(float *)((int)local_3c + 0x7c);
      FUN_00439330(local_44,(float *)local_44,local_90,local_60);
      local_44[2] = 0;
      FUN_004296e0(local_44,&local_2c);
      puVar3 = local_44 + 6;
      local_44[9] = local_f4;
      local_44[10] = *(float *)((int)local_3c + 0x380) + *(float *)((int)local_3c + 0x78);
      local_44[0xb] = local_74 + *(float *)((int)local_3c + 0x7c);
      local_44 = puVar3;
      FUN_00439330(puVar3,(float *)puVar3,local_90,local_64);
      local_44[2] = 0;
      FUN_004296e0(local_44,&local_2c);
      local_44 = local_44 + 6;
      local_74 = local_74 + local_f8;
      fVar5 = (float10)FUN_00438600(local_90,local_fc);
      local_90 = (float)fVar5;
    }
    puVar3 = *(undefined4 **)((int)local_3c + 0x564);
    *local_44 = *puVar3;
    local_44[1] = puVar3[1];
    local_44[2] = puVar3[2];
    local_44[3] = puVar3[3];
    local_44[4] = puVar3[4];
    local_44[5] = puVar3[5];
    local_44[5] = local_74 + *(float *)((int)local_3c + 0x7c);
    iVar2 = *(int *)((int)local_3c + 0x564);
    local_44[6] = *(undefined4 *)(iVar2 + 0x18);
    local_44[7] = *(undefined4 *)(iVar2 + 0x1c);
    local_44[8] = *(undefined4 *)(iVar2 + 0x20);
    local_44[9] = *(undefined4 *)(iVar2 + 0x24);
    local_44[10] = *(undefined4 *)(iVar2 + 0x28);
    local_44[0xb] = *(undefined4 *)(iVar2 + 0x2c);
    local_44[0xb] = local_74 + *(float *)((int)local_3c + 0x7c);
    break;
  case 0x30:
    local_48 = *(float **)((int)param_1 + 0x564);
    *(uint *)((int)param_1 + 0x49c) = *(uint *)((int)param_1 + 0x49c) & 0xffdfffff;
    local_104 = *(float *)((int)param_1 + 0x490);
    if ((*(byte *)((int)param_1 + 0x4a1) >> 2 & 7) == 0) {
      local_d8 = *(float *)((int)param_1 + 0x490);
    }
    else {
      local_d8 = *(float *)((int)param_1 + 0x494);
    }
    local_dc = local_d8;
    local_54 = *(float *)((int)param_1 + 0x454);
    if ((*(int *)((int)param_1 + 0x55c) != 0) && ((*(uint *)((int)param_1 + 0x49c) >> 0xc & 1) == 0)
       ) {
      fVar5 = FUN_004379c0(*(int *)((int)param_1 + 0x55c));
      local_100 = (float)fVar5;
      local_54 = local_100 * local_54;
    }
    if (*(char *)((int)local_3c + 0x4a4) == '\x01') {
      local_54 = local_54 * DAT_005b8818;
    }
    else if (*(char *)((int)local_3c + 0x4a4) == '\x02') {
      local_54 = DAT_005b8818 * DAT_0056e0ec * local_54;
    }
    local_48[3] = local_104;
    local_48[9] = local_dc;
    local_48[0xf] = local_dc;
    local_48[4] = (*(float *)((int)local_3c + 0x380) - *(float *)((int)local_3c + 0x378)) /
                  DAT_0056c8d0 + *(float *)((int)local_3c + 0x378) +
                  *(float *)((int)local_3c + 0x78);
    local_48[5] = *(float *)((int)local_3c + 0x37c) + *(float *)((int)local_3c + 0x7c);
    local_48[10] = *(float *)((int)local_3c + 0x388) + *(float *)((int)local_3c + 0x78);
    local_48[0xb] = *(float *)((int)local_3c + 0x38c) + *(float *)((int)local_3c + 0x7c);
    local_48[0x10] = *(float *)((int)local_3c + 0x390) + *(float *)((int)local_3c + 0x78);
    local_48[0x11] = *(float *)((int)local_3c + 0x394) + *(float *)((int)local_3c + 0x7c);
    FUN_00439330(local_48,local_48,DAT_0056e0f8 / DAT_0056c8d0,local_54);
    FUN_00439330(this,local_48 + 6,
                 DAT_0056e0f8 / DAT_0056c8d0 +
                 ((DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056c8d8) * DAT_0056c8d0,local_54);
    FUN_00439330(this_00,local_48 + 0xc,
                 DAT_0056e0f8 / DAT_0056c8d0 + (DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056c8d8,local_54
                );
    local_48[2] = 0.0;
    local_48[8] = 0.0;
    local_48[0xe] = 0.0;
    FUN_00429740(local_48 + 6,local_48);
    FUN_00429740(local_48 + 0xc,local_48);
    pfVar1 = (float *)FUN_00422dd0(local_150,0,0,0);
    *local_48 = *pfVar1;
    local_48[1] = pfVar1[1];
    local_48[2] = pfVar1[2];
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

