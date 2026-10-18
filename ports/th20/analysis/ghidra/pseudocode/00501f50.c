/* Entry: 0x00501f50; symbol: FUN_00501f50; body bytes: 1493 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00501f50(void *param_1)

{
  bool bVar1;
  char cVar2;
  float *pfVar3;
  int iVar4;
  undefined4 *puVar5;
  uint uVar6;
  undefined3 extraout_var;
  void *this;
  float10 fVar7;
  float local_ac [3];
  undefined1 local_a0 [4];
  undefined1 local_9c [4];
  undefined4 local_98;
  undefined4 local_94;
  undefined4 local_90;
  undefined1 local_8c [4];
  undefined1 local_88 [4];
  void *local_84;
  uint local_80;
  void *local_7c;
  int local_78;
  uint local_74;
  int local_70;
  void *local_6c;
  float local_68;
  float local_64;
  void *local_60;
  void *local_5c;
  void *local_58;
  void *local_54;
  float local_50;
  void *local_4c;
  void *local_48;
  void *local_44;
  char **local_40;
  void *local_3c;
  void *local_38;
  void *local_34;
  int local_30;
  void *local_2c;
  float local_28 [3];
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_2c = param_1;
  local_84 = (void *)FUN_0040ff90(*(int *)((int)param_1 + 0x120));
  local_80 = *(uint *)((int)local_2c + 0xd8);
  local_30 = FUN_00506480(local_84,local_80);
  FUN_00422e10(&local_1c);
  FUN_00422e10(local_28);
  if (*(int *)((int)local_2c + 0x110) == 0) {
    iVar4 = FUN_0040ff90(*(int *)((int)local_2c + 0x120));
    pfVar3 = (float *)FUN_00460850(iVar4);
    local_1c = *pfVar3;
    local_18 = pfVar3[1];
    local_14 = pfVar3[2];
  }
  else {
    local_44 = (void *)FUN_00466fb0(*(int *)((int)local_2c + 0x110));
    FUN_004fee00(local_44,&local_1c);
  }
  local_48 = (void *)((int)local_2c + 0x50);
  FUN_004393f0(local_48,&local_1c);
  local_4c = (void *)((int)local_2c + 0x50);
  FUN_00422dd0(local_ac,*(undefined4 *)(local_30 + 4),*(undefined4 *)(local_30 + 8),0);
  FUN_00506010(local_4c,local_ac);
  pfVar3 = (float *)FUN_00421830((int)local_2c + 0x50);
  local_c = *pfVar3;
  FUN_00429210(&local_10,*(float *)(local_30 + 0x14));
  pfVar3 = (float *)FUN_004294e0(&local_c,local_9c,&local_10);
  fVar7 = FUN_004292e0(pfVar3);
  fVar7 = FUN_0047a630((float)fVar7);
  local_50 = (float)fVar7;
  if (local_50 <= DAT_0056e714) {
    local_60 = (void *)((int)local_2c + 0x50);
    fVar7 = FUN_004292e0(&local_10);
    FUN_0047a560(local_60,(float)fVar7);
  }
  else {
    local_5c = (void *)((int)local_2c + 0x50);
    local_54 = FUN_004294e0(&local_10,local_a0,&local_c);
    local_58 = FUN_00429390(local_54,local_88,DAT_0056e728);
    pfVar3 = (float *)FUN_00429610(local_58,local_8c,&local_c);
    fVar7 = FUN_004292e0(pfVar3);
    FUN_0047a560(local_5c,(float)fVar7);
  }
  if (*(int *)((int)local_2c + 0x9c) == 2) goto LAB_00502540;
  iVar4 = FUN_0040ff90(*(int *)((int)local_2c + 0x120));
  pfVar3 = (float *)FUN_00460850(iVar4);
  FUN_00429090(&DAT_005ba830,0x14,*pfVar3);
  if (*(float *)((int)local_2c + 0xb0) <= DAT_0056f114 &&
      DAT_0056f114 != *(float *)((int)local_2c + 0xb0)) {
    *(float *)((int)local_2c + 0xb0) = *(float *)((int)local_2c + 0xb0) + _DAT_00572644;
    local_3c = (void *)FUN_004c0e10((undefined4 *)((int)local_2c + 0xdc));
    if (local_3c != (void *)0x0) {
      FUN_00470b20(local_3c,*(undefined4 *)((int)local_2c + 0xb0));
    }
  }
  local_64 = *(float *)((int)local_2c + 0xb0) / DAT_0056c8d0;
  pfVar3 = (float *)FUN_00421830((int)local_2c + 0x50);
  fVar7 = FUN_004292e0(pfVar3);
  local_68 = (float)fVar7;
  FUN_00439330(this,local_28,local_68,local_64);
  FUN_004296e0(&local_1c,local_28);
  local_14 = 0.0;
  puVar5 = FUN_005064b0(local_2c,&local_90);
  local_38 = (void *)FUN_0044ced0(puVar5);
  if (local_38 != (void *)0x0) {
    FUN_00506de0(local_38,*(undefined4 *)((int)local_2c + 0xb0));
    FUN_00506e40(local_38,*(float *)((int)local_2c + 0xb0) / DAT_0056f114);
  }
  local_34 = (void *)FUN_004c0e10((undefined4 *)((int)local_2c + 0xdc));
  if (local_34 != (void *)0x0) {
    FUN_004c2030(local_34,&local_1c);
    FUN_00506a90(local_34,(int)*(short *)(local_30 + 2));
    pfVar3 = (float *)FUN_00421830((int)local_2c + 0x50);
    fVar7 = FUN_004292e0(pfVar3);
    FUN_00506c80(local_34,(float)fVar7);
  }
  if (((*(int *)((int)local_2c + 0xa4) == 0) && (*(int *)((int)local_2c + 0x9c) == 1)) &&
     (*(int *)((int)local_2c + 0xa8) == 1)) {
    puVar5 = FUN_005064b0(local_2c,&local_94);
    FUN_00486350(puVar5);
    *(undefined4 *)((int)local_2c + 0xa8) = 0;
  }
  if (*(int *)((int)local_2c + 0x9c) == 1) {
    if ((*(int *)(local_30 + 0x3c) != 0) ||
       (iVar4 = FUN_00506460(*(int *)((int)local_2c + 0x11c)), -1 < iVar4)) {
      iVar4 = FUN_0040ff90(*(int *)((int)local_2c + 0x120));
      iVar4 = FUN_00411700(iVar4);
      if (iVar4 != 2) {
        iVar4 = FUN_0040ff90(*(int *)((int)local_2c + 0x120));
        iVar4 = FUN_00411700(iVar4);
        if ((((iVar4 != 4) && (uVar6 = FUN_00506880(*(int *)((int)local_2c + 0x11c)), uVar6 == 0))
            && ((DAT_005c06a4 == 0 ||
                (bVar1 = FUN_00478160(DAT_005c06a4), CONCAT31(extraout_var,bVar1) == 0)))) &&
           (iVar4 = FUN_00412730(*(int *)((int)local_2c + 0x120)), iVar4 != 0)) {
          if (0 < *(int *)(local_30 + 0x3c)) {
            local_6c = (void *)((int)local_2c + 0x1c);
            bVar1 = FUN_004235c0(local_6c,*(int *)(local_30 + 0x3c));
            if (bVar1) goto LAB_005024c1;
          }
          if (*(int *)((int)local_2c + 0x110) != 0) {
            iVar4 = FUN_00464230(0);
            local_70 = FUN_00534210(iVar4);
            iVar4 = FUN_00506140(*(int *)((int)local_2c + 0x110));
            local_74 = iVar4 + 1;
            iVar4 = FUN_00460830(0);
            local_78 = FUN_00506340(iVar4);
            uVar6 = FUN_005068a0(local_78,local_74,local_70);
            if ((uVar6 & 0xff) == 0) goto LAB_005024c1;
          }
          iVar4 = FUN_00464230(0);
          cVar2 = FUN_00506950(iVar4);
          if (cVar2 == '\0') goto LAB_00502531;
        }
      }
    }
LAB_005024c1:
    local_40 = (char **)FUN_004c0e10((undefined4 *)((int)local_2c + 0xdc));
    if (local_40 != (char **)0x0) {
      FUN_004c1f30(local_40);
    }
    *(undefined4 *)((int)local_2c + 0x9c) = 2;
    puVar5 = FUN_005064b0(local_2c,&local_98);
    FUN_00479040(puVar5);
    local_7c = *(void **)((int)local_2c + 0x11c);
    FUN_00506bf0(local_7c,*(int *)(local_30 + 0x38),0);
    FUN_00428890(&DAT_005ba830,0x14);
  }
LAB_00502531:
  *(undefined4 *)((int)local_2c + 0xa4) = 0;
LAB_00502540:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

