/* Entry: 0x00504480; symbol: FUN_00504480; body bytes: 1198 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00504480(char **param_1)

{
  bool bVar1;
  int iVar2;
  default_delete<class_std::_Facet_base> *pdVar3;
  float *pfVar4;
  float10 fVar5;
  undefined1 local_94 [4];
  undefined1 local_90 [4];
  undefined1 local_8c [4];
  undefined1 local_88 [4];
  char **local_84;
  void *local_80;
  void *local_7c;
  void *local_78;
  float local_74;
  float local_70;
  float local_6c;
  char **local_68;
  char **local_64;
  char **local_60;
  char **local_5c;
  void *local_58;
  char *local_54;
  int local_50;
  void *local_4c;
  int local_48;
  void *local_44;
  char **local_40;
  float local_3c;
  float local_38 [12];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_40 = param_1;
  local_58 = (void *)FUN_0040ff90((int)param_1[0x48]);
  local_54 = local_40[0x36];
  local_48 = FUN_00506480(local_58,(uint)local_54);
  if (0 < *(int *)(local_48 + 0x34)) {
    local_5c = local_40 + 7;
    bVar1 = FUN_004235c0(local_5c,*(int *)(local_48 + 0x34));
    if (bVar1) {
      FUN_00479040(local_40 + 6);
      FUN_00423520(local_40 + 7,-999);
    }
  }
  if ((local_40[0x11] != (char *)0x0) && (iVar2 = (*(code *)local_40[0x11])(), iVar2 != 0)) {
    FUN_00506af0(local_40);
    goto LAB_00504920;
  }
  if (local_40[0x27] != (char *)0x2) {
    local_60 = local_40 + 0x14;
    FUN_0047a240(local_60,*(float *)(local_48 + 0x18));
    local_64 = local_40 + 0x14;
    FUN_00506050(local_64,*(float *)(local_48 + 0x20));
  }
  FUN_0047a1f0((float *)(local_40 + 0x14));
  local_44 = (void *)FUN_0044cd00();
  if (local_44 == (void *)0x0) {
    FUN_00506af0(local_40);
    goto LAB_00504920;
  }
  if ((*(char *)(local_48 + 0x29) != '\x02') && (*(char *)(local_48 + 0x29) != '\b')) {
    FUN_0040bc20(local_38,0xc,4,FUN_00422e10);
    FUN_00445770(local_44,local_38);
    local_68 = local_40 + 7;
    bVar1 = FUN_004235c0(local_68,0xf);
    if ((bVar1) && (iVar2 = FUN_00506570(local_38), iVar2 != 0)) {
      FUN_00506af0(local_40);
      goto LAB_00504920;
    }
  }
  iVar2 = FUN_0040c300(local_40 + 0x37);
  if (((iVar2 != 0) && (((uint)local_40[0xf] & 1) != 0)) &&
     (local_4c = (void *)FUN_004c0e10(local_40 + 0x37), local_4c != (void *)0x0)) {
    pdVar3 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(local_40 + 0x14));
    FUN_004c2030(local_4c,(undefined4 *)pdVar3);
    pfVar4 = (float *)FUN_00421830((int)(local_40 + 0x14));
    fVar5 = FUN_004292e0(pfVar4);
    FUN_00506a20(local_4c,(float)fVar5);
    FUN_00506a50(local_4c,local_40 + 0x2c);
    FUN_00506a90(local_4c,local_40[0x2b]);
  }
  pdVar3 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)(local_40 + 0x14));
  *(undefined4 *)((int)local_44 + 0x5bc) = *(undefined4 *)pdVar3;
  *(undefined4 *)((int)local_44 + 0x5c0) = *(undefined4 *)(pdVar3 + 4);
  *(undefined4 *)((int)local_44 + 0x5c4) = *(undefined4 *)(pdVar3 + 8);
  local_50 = (*(byte *)((int)local_44 + 0x4a2) >> 5) - 1;
  switch(local_50) {
  case 0:
    pfVar4 = (float *)FUN_00421830((int)(local_40 + 0x14));
    fVar5 = FUN_004292e0(pfVar4);
    FUN_00450590(local_44,(float)fVar5);
    break;
  case 1:
    pfVar4 = (float *)FUN_00421830((int)(local_40 + 0x14));
    local_3c = *pfVar4;
    fVar5 = FUN_004292e0(&local_3c);
    local_6c = (float)fVar5;
    if (DAT_0056e0f8 / DAT_0056c8d0 <= local_6c) {
      fVar5 = FUN_004292e0(&local_3c);
      local_70 = (float)fVar5;
      if (local_70 <= DAT_0056e0f0 / DAT_0056c8d0) {
        fVar5 = FUN_004292e0(&local_3c);
        FUN_00450590(local_44,(float)fVar5);
        fVar5 = FUN_004379c0((int)local_44);
        fVar5 = FUN_0047a630((float)fVar5);
        FUN_004777f0(local_44,(float)fVar5);
        break;
      }
    }
    pfVar4 = (float *)FUN_00452fc0(&local_3c,local_88,DAT_0056e0f0);
    fVar5 = FUN_004292e0(pfVar4);
    FUN_00450590(local_44,(float)fVar5);
    fVar5 = FUN_004379c0((int)local_44);
    fVar5 = FUN_0047a630((float)fVar5);
    local_74 = (float)fVar5;
    FUN_004777f0(local_44,(uint)local_74 ^ _DAT_0056d7d0);
    break;
  case 2:
    local_78 = (void *)FUN_00421830((int)(local_40 + 0x14));
    pfVar4 = (float *)FUN_00452fc0(local_78,local_8c,DAT_0056e0f0);
    fVar5 = FUN_004292e0(pfVar4);
    fVar5 = FUN_00438540((float)fVar5);
    FUN_00450590(local_44,(float)fVar5);
    break;
  case 3:
    local_7c = (void *)FUN_00421830((int)(local_40 + 0x14));
    pfVar4 = (float *)FUN_00452fc0(local_7c,local_90,DAT_0056e0f0 / DAT_0056c8d0);
    fVar5 = FUN_004292e0(pfVar4);
    fVar5 = FUN_00438540((float)fVar5);
    FUN_00450590(local_44,(float)fVar5);
    break;
  case 4:
    local_80 = (void *)FUN_00421830((int)(local_40 + 0x14));
    pfVar4 = (float *)FUN_00452f80(local_80,local_94,DAT_0056e0f0 / DAT_0056c8d0);
    fVar5 = FUN_004292e0(pfVar4);
    fVar5 = FUN_00438540((float)fVar5);
    FUN_00450590(local_44,(float)fVar5);
  }
  local_84 = local_40 + 7;
  FUN_00423540(local_84);
LAB_00504920:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

