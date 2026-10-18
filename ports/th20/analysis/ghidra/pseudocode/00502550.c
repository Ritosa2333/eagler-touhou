/* Entry: 0x00502550; symbol: FUN_00502550; body bytes: 1220 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00502550(void *param_1)

{
  bool bVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  float *pfVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined3 extraout_var;
  float10 fVar6;
  float local_9c [3];
  undefined4 local_90;
  undefined4 local_8c;
  undefined4 local_88;
  undefined1 local_84 [4];
  undefined1 local_80 [4];
  undefined1 local_7c [4];
  undefined1 local_78 [4];
  void *local_74;
  float local_70;
  float local_6c;
  void *local_68;
  void *local_64;
  void *local_60;
  void *local_5c;
  float local_58;
  void *local_54;
  void *local_50;
  void *local_4c;
  void *local_48;
  uint local_44;
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
  local_48 = (void *)FUN_0040ff90(*(int *)((int)param_1 + 0x120));
  local_44 = *(uint *)((int)local_2c + 0xd8);
  local_30 = FUN_00506480(local_48,local_44);
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
    local_4c = (void *)FUN_00466fb0(*(int *)((int)local_2c + 0x110));
    FUN_004fee00(local_4c,&local_1c);
  }
  local_50 = (void *)((int)local_2c + 0x50);
  FUN_004393f0(local_50,&local_1c);
  local_54 = (void *)((int)local_2c + 0x50);
  FUN_00422dd0(local_9c,*(undefined4 *)(local_30 + 4),*(undefined4 *)(local_30 + 8),0);
  FUN_00506010(local_54,local_9c);
  pdVar2 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)((int)local_2c + 0x50));
  local_1c = *(float *)pdVar2;
  local_18 = *(float *)(pdVar2 + 4);
  local_14 = *(float *)(pdVar2 + 8);
  pfVar3 = (float *)FUN_00421830((int)local_2c + 0x50);
  local_c = *pfVar3;
  FUN_00429210(&local_10,*(float *)(local_30 + 0x14));
  pfVar3 = (float *)FUN_004294e0(&local_c,local_78,&local_10);
  fVar6 = FUN_004292e0(pfVar3);
  fVar6 = FUN_0047a630((float)fVar6);
  local_58 = (float)fVar6;
  if (local_58 <= DAT_0056e714) {
    local_68 = (void *)((int)local_2c + 0x50);
    fVar6 = FUN_004292e0(&local_10);
    FUN_0047a560(local_68,(float)fVar6);
  }
  else {
    local_64 = (void *)((int)local_2c + 0x50);
    local_5c = FUN_004294e0(&local_10,local_7c,&local_c);
    local_60 = FUN_00429390(local_5c,local_80,DAT_0056ef28);
    pfVar3 = (float *)FUN_00429610(local_60,local_84,&local_c);
    fVar6 = FUN_004292e0(pfVar3);
    FUN_0047a560(local_64,(float)fVar6);
  }
  if (*(int *)((int)local_2c + 0x9c) == 2) goto LAB_00502a10;
  iVar4 = FUN_0040ff90(*(int *)((int)local_2c + 0x120));
  pfVar3 = (float *)FUN_00460850(iVar4);
  FUN_00429090(&DAT_005ba830,0x14,*pfVar3);
  if (*(float *)((int)local_2c + 0xb0) <= DAT_0056f114 &&
      DAT_0056f114 != *(float *)((int)local_2c + 0xb0)) {
    *(float *)((int)local_2c + 0xb0) = *(float *)((int)local_2c + 0xb0) + _DAT_00572644;
    local_38 = (void *)FUN_004c0e10((undefined4 *)((int)local_2c + 0xdc));
    if (local_38 != (void *)0x0) {
      FUN_00470b20(local_38,*(undefined4 *)((int)local_2c + 0xb0));
    }
  }
  local_6c = *(float *)((int)local_2c + 0xb0) / DAT_0056c8d0;
  pfVar3 = (float *)FUN_00421830((int)local_2c + 0x50);
  fVar6 = FUN_004292e0(pfVar3);
  local_70 = (float)fVar6;
  FUN_00439330(local_28,local_28,local_70,local_6c);
  FUN_004296e0(&local_1c,local_28);
  local_14 = 0.0;
  puVar5 = FUN_005064b0(local_2c,&local_88);
  local_34 = (void *)FUN_0044ced0(puVar5);
  if (local_34 != (void *)0x0) {
    FUN_00506de0(local_34,*(undefined4 *)((int)local_2c + 0xb0));
    FUN_00506e40(local_34,*(float *)((int)local_2c + 0xb0) / DAT_0056f114);
  }
  local_3c = (void *)FUN_004c0e10((undefined4 *)((int)local_2c + 0xdc));
  if (local_3c != (void *)0x0) {
    FUN_004c2030(local_3c,&local_1c);
  }
  if (((*(int *)((int)local_2c + 0xa4) == 0) && (*(int *)((int)local_2c + 0x9c) == 1)) &&
     (*(int *)((int)local_2c + 0xa8) == 1)) {
    puVar5 = FUN_005064b0(local_2c,&local_8c);
    FUN_00486350(puVar5);
    *(undefined4 *)((int)local_2c + 0xa8) = 0;
  }
  if (*(int *)((int)local_2c + 0x9c) == 1) {
    if ((((*(int *)(local_30 + 0x3c) != 0) ||
         (iVar4 = FUN_00506460(*(int *)((int)local_2c + 0x11c)), -1 < iVar4)) &&
        ((DAT_005c06a4 == 0 ||
         (bVar1 = FUN_00478160(DAT_005c06a4), CONCAT31(extraout_var,bVar1) == 0)))) &&
       (iVar4 = FUN_00412730(*(int *)((int)local_2c + 0x120)), iVar4 != 0)) {
      iVar4 = FUN_00464230(0);
      iVar4 = FUN_004ff6d0(iVar4);
      if (iVar4 != 0) goto LAB_00502a01;
    }
    local_40 = (char **)FUN_004c0e10((undefined4 *)((int)local_2c + 0xdc));
    if (local_40 != (char **)0x0) {
      FUN_004c1f30(local_40);
    }
    *(undefined4 *)((int)local_2c + 0x9c) = 2;
    puVar5 = FUN_005064b0(local_2c,&local_90);
    FUN_00479040(puVar5);
    local_74 = *(void **)((int)local_2c + 0x11c);
    FUN_00506bf0(local_74,*(int *)(local_30 + 0x38),0);
    FUN_00428890(&DAT_005ba830,0x14);
  }
LAB_00502a01:
  *(undefined4 *)((int)local_2c + 0xa4) = 0;
LAB_00502a10:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

