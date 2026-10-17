/* Entry: 0x004818e0; symbol: FUN_004818e0; body bytes: 3735 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall
FUN_004818e0(void *this,int *param_1,undefined4 param_2,undefined4 param_3,uint param_4,uint param_5
            ,float param_6)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int iVar3;
  uint uVar4;
  float *pfVar5;
  void *extraout_ECX;
  void *this_00;
  void *this_01;
  int *unaff_FS_OFFSET;
  float10 fVar6;
  undefined1 local_dc [12];
  undefined4 local_d0;
  undefined4 local_cc;
  undefined4 local_c8;
  undefined4 local_c4;
  float local_c0;
  void *local_bc;
  void *local_b8;
  void *local_b4;
  int local_b0;
  float *local_ac;
  float *local_a8;
  float *local_a4;
  undefined4 local_a0;
  int local_9c;
  void *local_98;
  void *local_94;
  int local_90;
  void *local_8c;
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
  void *local_5c;
  float local_58;
  undefined4 local_54;
  float local_50;
  float local_4c;
  float local_48;
  float local_44;
  uint local_40;
  float local_3c;
  undefined4 *local_38;
  void *local_34;
  float local_30;
  float *local_2c;
  float local_28;
  void *local_24;
  float local_20;
  float local_1c;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  puStack_c = &LAB_0056918d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_8 = 0;
  local_34 = this;
  local_38 = (undefined4 *)FUN_004859f0((int)this + 0x286d4c);
  if (local_38 == (undefined4 *)0x0) {
    local_cc = 1;
    local_8 = 0xffffffff;
    FUN_0047c300((int)&param_2);
    goto LAB_0048275c;
  }
  local_24 = (void *)FUN_0040c300(local_38);
  FUN_00411ce0((int)local_24);
  local_5c = (void *)((int)local_34 + 0x286d64);
  FUN_0044a6b0(local_5c,local_24);
  local_2c = (float *)FUN_0040c300(&param_2);
  uVar1 = move<>(&param_2);
  FUN_0047c530(uVar1);
  FUN_00486570(local_24,*(undefined4 *)((int)local_34 + 0x286d9c));
  local_28 = 0.0;
  FUN_0047c5f0((void *)((int)local_24 + 0x4d8),DAT_0056c8cc);
  FUN_0047c5f0((void *)((int)local_24 + 0x4e8),DAT_0056c8cc);
  if (*(short *)((int)param_1 + 0x26) < 2) {
    local_3c = (float)param_1[7];
  }
  else {
    local_3c = (float)param_1[7] -
               (((float)param_1[7] - (float)param_1[8]) * (float)(int)param_5) /
               ((float)(int)*(short *)((int)param_1 + 0x26) - DAT_0056c8cc);
  }
  local_30 = local_3c;
  *(float *)((int)local_24 + 0x24) = local_3c;
  local_40 = (uint)*(ushort *)(local_2c + 0xe);
  switch(local_40) {
  case 0:
  case 1:
    if ((*(ushort *)(param_1 + 9) & 1) == 0) {
      local_44 = (float)((int)param_4 / 2) * (float)param_1[6] + (float)param_1[6] * DAT_0056e0ec;
    }
    else {
      local_44 = (float)((int)(param_4 + 1) / 2) * (float)param_1[6];
    }
    local_28 = local_28 + local_44;
    if ((param_4 & 1) != 0) {
      local_28 = local_28 * DAT_0056e0f4;
    }
    if (*(short *)(local_2c + 0xe) == 0) {
      local_28 = local_28 + param_6;
    }
    local_28 = local_28 + (float)param_1[5];
    break;
  case 2:
    local_28 = local_28 + param_6;
  case 3:
    local_28 = (float)(int)param_5 * (float)param_1[6] + (float)param_1[5] +
               ((float)(int)param_4 * DAT_0056e0f0 * DAT_0056c8d0) / (float)(int)(short)param_1[9] +
               local_28;
    break;
  case 4:
    local_28 = local_28 + param_6;
  case 5:
    local_28 = (float)(int)param_5 * (float)param_1[6] + (float)param_1[5] +
               ((float)(int)param_4 * DAT_0056e0f0 * DAT_0056c8d0) / (float)(int)(short)param_1[9] +
               DAT_0056e0f0 / (float)(int)(short)param_1[9] + local_28;
    break;
  case 6:
    fVar6 = FUN_004298b0(0x5ba4a8,(float)param_1[6]);
    local_60 = (float)fVar6;
    local_28 = local_60 + (float)param_1[5];
    break;
  case 7:
    fVar6 = FUN_00429800(0x5ba4a8,(float)param_1[8]);
    local_64 = (float)fVar6;
    local_30 = local_64 + (float)param_1[7];
    local_28 = (float)(int)param_5 * (float)param_1[6] + (float)param_1[5] +
               ((float)(int)param_4 * DAT_0056e0f0 * DAT_0056c8d0) / (float)(int)(short)param_1[9] +
               local_28;
    break;
  case 8:
    fVar6 = FUN_004298b0(0x5ba4a8,(float)param_1[6]);
    local_68 = (float)fVar6;
    local_28 = local_68 + (float)param_1[5];
    fVar6 = FUN_00429800(0x5ba4a8,(float)param_1[8]);
    local_6c = (float)fVar6;
    local_30 = local_6c + (float)param_1[7];
    break;
  case 9:
  case 10:
    local_28 = ((float)(int)param_4 * DAT_0056e0f0 * DAT_0056c8d0) / (float)(int)(short)param_1[9];
    if ((*(ushort *)((int)param_1 + 0x26) & 1) == 0) {
      local_28 = (float)((int)param_5 / 2) * (float)param_1[6] + (float)param_1[6] * DAT_0056e0ec +
                 local_28;
      if (1 < *(short *)((int)param_1 + 0x26)) {
        local_30 = ((float)(param_5 & 0xfffe) * ((float)param_1[8] - (float)param_1[7])) /
                   (float)(*(short *)((int)param_1 + 0x26) + -1) + (float)param_1[7];
      }
    }
    else {
      local_28 = (float)((int)(param_5 + 1) / 2) * (float)param_1[6] + local_28;
      if (1 < *(short *)((int)param_1 + 0x26)) {
        local_30 = ((float)(param_5 + 1 & 0xfffe) * ((float)param_1[8] - (float)param_1[7])) /
                   (float)(*(short *)((int)param_1 + 0x26) + -1) + (float)param_1[7];
      }
    }
    if ((param_5 & 1) != 0) {
      local_28 = local_28 * DAT_0056e0f4;
    }
    if (*(short *)(local_2c + 0xe) == 9) {
      local_28 = local_28 + param_6;
    }
    local_28 = local_28 + (float)param_1[5];
    break;
  case 0xb:
    local_70 = ((float)(int)param_4 * DAT_0056e0f0 * DAT_0056c8d0) / (float)(int)(short)param_1[9];
    local_28 = local_70;
    fVar6 = FUN_004397c0((void *)(int)(short)param_1[9],local_70);
    local_74 = (float)fVar6;
    local_48 = local_74 * (float)param_1[7];
    fVar6 = FUN_00439820(extraout_ECX,local_28);
    local_78 = (float)fVar6;
    local_4c = local_78 * (float)param_1[8];
    fVar6 = (float10)FUN_00455fb0(local_48,local_4c);
    local_30 = (float)fVar6;
    fVar6 = FUN_00459280(local_4c,local_48);
    local_7c = (float)fVar6;
    local_28 = local_7c + (float)param_1[5];
    break;
  case 0xc:
    local_50 = ((float)(int)param_4 * DAT_0056e0f0 * DAT_0056c8d0) / (float)(int)(short)param_1[9] +
               DAT_0056e0f0 / (float)(int)(short)param_1[9];
    local_28 = local_50 + (float)param_1[5] + local_28;
    fVar6 = FUN_00439820(param_1,local_50);
    fVar6 = FUN_0047a630((float)fVar6);
    local_80 = (float)fVar6;
    local_30 = (DAT_0056c8cc - local_80 * (float)param_1[8]) * local_30;
  }
  *(float *)((int)local_24 + 0x20) = local_30;
  fVar6 = (float10)FUN_00438600(local_28,0.0);
  FUN_00452f20((void *)((int)local_24 + 0x84),(float)fVar6);
  *(int *)((int)local_24 + 100) = param_1[2];
  *(int *)((int)local_24 + 0x68) = param_1[3];
  *(int *)((int)local_24 + 0x6c) = param_1[4];
  if (*local_2c != _DAT_0056e0e8) {
    FUN_00422e10(&local_20);
    local_84 = *local_2c;
    fVar6 = FUN_004292e0((float *)((int)local_24 + 0x84));
    local_88 = (float)fVar6;
    FUN_00439330(this_00,&local_20,local_88,local_84);
    *(float *)((int)local_24 + 100) = *(float *)((int)local_24 + 100) + local_20;
    *(float *)((int)local_24 + 0x68) = *(float *)((int)local_24 + 0x68) + local_1c;
  }
  *(undefined4 *)((int)local_24 + 0x6c) = DAT_0056ef28;
  *(undefined4 *)((int)local_24 + 0x50) = 1;
  *(uint *)((int)local_24 + 0x14) = *(uint *)((int)local_24 + 0x14) | 1;
  FUN_00423520((void *)((int)local_24 + 0x4f8),0);
  FUN_00423520((void *)((int)local_24 + 0x508),0);
  *(undefined4 *)((int)local_24 + 0x18) = 0;
  *(float *)((int)local_24 + 0x48) = DAT_0056c8cc;
  FUN_00429970((int)local_24 + 0x474);
  FUN_00439330(this_01,(float *)((int)local_24 + 0x70),local_28,local_30);
  *(short *)((int)local_24 + 0x4e) = (short)param_1[1];
  *(short *)((int)local_24 + 0x4c) = (short)*param_1;
  *(undefined4 *)((int)local_24 + 0x54) = 0;
  *(uint *)((int)local_24 + 0x14) = *(uint *)((int)local_24 + 0x14) & 0xfffffff7;
  *(uint *)((int)local_24 + 0x14) = *(uint *)((int)local_24 + 0x14) & 0xfffffffb;
  *(uint *)((int)local_24 + 0x14) = *(uint *)((int)local_24 + 0x14) | 2;
  *(undefined4 *)((int)local_24 + 0x4d0) = 0x3c;
  FUN_00423520((void *)((int)local_24 + 0x4a0),0);
  FUN_00423520((void *)((int)local_24 + 0x4b0),0);
  FUN_00423520((void *)((int)local_24 + 0x4c0),0);
  *(undefined4 *)((int)local_24 + 0x4d4) = 3;
  puVar2 = FUN_0044c9b0(DAT_005c0028);
  *(undefined4 **)((int)local_24 + 0x1c) = puVar2;
  FUN_004299d0(*(int *)((int)local_24 + 0x1c));
  *(void **)(*(int *)((int)local_24 + 0x1c) + 0x5c8) = local_24;
  local_8c = *(void **)((int)local_24 + 0x1c);
  FUN_00438a30(local_8c,FUN_0047d580);
  local_98 = *(void **)((int)local_34 + 0x286d8c);
  local_90 = FUN_004856c0(*param_1);
  local_94 = *(void **)((int)local_24 + 0x1c);
  FUN_004383a0(local_98,local_94,local_90);
  *(uint *)((int)local_24 + 0x14) = *(uint *)((int)local_24 + 0x14) | 0x10;
  *(byte *)(*(int *)((int)local_24 + 0x1c) + 0x4a3) =
       *(byte *)(*(int *)((int)local_24 + 0x1c) + 0x4a3) & 0xfc | 1;
  if (*(int *)(&DAT_005c07fc + *param_1 * 0x158) != 0) {
    iVar3 = FUN_0040c300(*(undefined4 **)((int)local_34 + 0x286da0));
    local_a0 = *(undefined4 *)(iVar3 + 0x286d8c);
    local_9c = *(int *)(&DAT_005c07fc + *param_1 * 0x158);
    puVar2 = FUN_004790e0(&local_d0,(uint *)0x0,local_9c,(float *)((int)local_24 + 100),0,-1,
                          (undefined4 *)0x0);
    *(undefined4 *)((int)local_24 + 0x5c) = *puVar2;
  }
  *(undefined **)((int)local_24 + 0x58) = &DAT_005c06a8 + *param_1 * 0x158;
  FUN_0045d650((void *)((int)local_24 + 0x88),0xffd08080);
  local_54 = *(undefined4 *)(&DAT_005c07f4 + *param_1 * 0x158);
  switch(local_54) {
  case 0:
    *(int *)((int)local_24 + 0x34) = param_1[1] * 2 + 6;
    break;
  case 1:
    *(undefined4 *)((int)local_24 + 0x34) = *(undefined4 *)(&DAT_005aeff8 + param_1[1] * 4);
    break;
  case 2:
    *(undefined4 *)((int)local_24 + 0x34) = 0xffffffff;
    *(uint *)((int)local_24 + 0x14) = *(uint *)((int)local_24 + 0x14) | 0x10;
    break;
  case 3:
    *(undefined4 *)((int)local_24 + 0x34) = 0x12;
    break;
  case 4:
    *(undefined4 *)((int)local_24 + 0x34) = 8;
    break;
  case 6:
    *(undefined4 *)((int)local_24 + 0x34) =
         *(undefined4 *)
          (&DAT_005c06b8 + *(short *)((int)local_24 + 0x4c) * 0x158 + param_1[1] * 0x14);
    FUN_0045d650((void *)((int)local_24 + 0x88),
                 *(undefined4 *)
                  (&DAT_005c06bc + *(short *)((int)local_24 + 0x4c) * 0x158 + param_1[1] * 0x14));
    *(uint *)((int)local_24 + 0x14) = *(uint *)((int)local_24 + 0x14) | 0x10;
    break;
  case 7:
    *(undefined4 *)((int)local_24 + 0x34) = 0x107;
    *(uint *)((int)local_24 + 0x14) = *(uint *)((int)local_24 + 0x14) | 0x10;
    break;
  case 8:
    *(undefined4 *)((int)local_24 + 0x34) = 0x10a;
    *(uint *)((int)local_24 + 0x14) = *(uint *)((int)local_24 + 0x14) | 0x10;
    break;
  case 9:
    *(undefined4 *)((int)local_24 + 0x34) = 0x10d;
    *(uint *)((int)local_24 + 0x14) = *(uint *)((int)local_24 + 0x14) | 0x10;
    break;
  case 10:
    *(undefined4 *)((int)local_24 + 0x34) = 0x116;
    *(uint *)((int)local_24 + 0x14) = *(uint *)((int)local_24 + 0x14) | 0x10;
  }
  *(uint *)((int)local_24 + 0x14) = *(uint *)((int)local_24 + 0x14) & 0xffffe7ff;
  *(undefined4 *)((int)local_24 + 0x44) = *(undefined4 *)(&DAT_005c07f0 + *param_1 * 0x158);
  *(float *)((int)local_24 + 0x40) = local_2c[0x10];
  *(undefined4 *)((int)local_24 + 0x30) = 5;
  fVar6 = FUN_00485700(*param_1);
  local_58 = (float)fVar6;
  *(float *)((int)local_24 + 0x80) = local_58;
  *(float *)((int)local_24 + 0x7c) = local_58;
  *(undefined4 *)((int)local_24 + 0x90) = 0;
  *(undefined4 *)((int)local_24 + 0x94) = 0;
  *(undefined4 *)((int)local_24 + 0x3c) = 0;
  *(float *)((int)local_24 + 0x38) = local_2c[0x11];
  uVar4 = FUN_00486600((int)(local_2c + 1));
  if (*(uint *)((int)local_24 + 0x38) < uVar4) {
    local_a4 = local_2c + 1;
    iVar3 = FUN_0047c6c0(local_a4,*(int *)((int)local_24 + 0x38));
    if (*(int *)(iVar3 + 0x20) != 1) goto LAB_00482698;
    local_a8 = local_2c + 1;
    iVar3 = FUN_0047c6c0(local_a8,*(int *)((int)local_24 + 0x38));
    if (*(short *)(iVar3 + 0x10) != 1) {
      local_b4 = *(void **)((int)local_24 + 0x1c);
      local_ac = local_2c + 1;
      iVar3 = FUN_0047c6c0(local_ac,*(int *)((int)local_24 + 0x38));
      local_b0 = *(short *)(iVar3 + 0x10) + 7;
      FUN_004388f0(local_b4,local_b0);
    }
    *(undefined4 *)((int)local_24 + 0x50) = 2;
    local_b8 = (void *)((int)local_24 + 0x70);
    pfVar5 = (float *)FUN_004292f0(local_b8,local_dc,DAT_0056c8e0);
    FUN_00429740((void *)((int)local_24 + 100),pfVar5);
    *(int *)((int)local_24 + 0x38) = *(int *)((int)local_24 + 0x38) + 1;
  }
  else {
LAB_00482698:
    local_bc = *(void **)((int)local_24 + 0x1c);
    FUN_004388f0(local_bc,2);
  }
  FUN_0047dcf0(local_24);
  FUN_0042b5d0(*(void **)((int)local_24 + 0x1c));
  if (_DAT_0056e0e8 < *(float *)((int)local_34 + 0x48)) {
    iVar3 = FUN_0040ff90(*(int *)((int)local_34 + 0x286da0));
    pfVar5 = (float *)FUN_00460850(iVar3);
    fVar6 = FUN_004560d0((float *)((int)local_24 + 100),pfVar5);
    local_c0 = (float)fVar6;
    if (local_c0 < *(float *)((int)local_34 + 0x48)) {
      FUN_0047da40(local_24);
      local_c4 = 0xffffffff;
      local_8 = 0xffffffff;
      FUN_0047c300((int)&param_2);
      goto LAB_0048275c;
    }
  }
  local_c8 = 0;
  local_8 = 0xffffffff;
  FUN_0047c300((int)&param_2);
LAB_0048275c:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

