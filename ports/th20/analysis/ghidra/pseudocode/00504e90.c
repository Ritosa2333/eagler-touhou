/* Entry: 0x00504e90; symbol: FUN_00504e90; body bytes: 2949 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall
FUN_00504e90(void *this,char *param_1,undefined4 param_2,undefined4 *param_3,char *param_4,
            undefined4 param_5)

{
  bool bVar1;
  int iVar2;
  float *pfVar3;
  char *pcVar4;
  undefined4 *puVar5;
  default_delete<class_std::_Facet_base> *pdVar6;
  float10 fVar7;
  undefined4 local_110;
  undefined4 local_10c;
  undefined1 local_108 [4];
  default_delete<class_std::_Facet_base> *local_104;
  int local_100;
  float local_fc;
  code *local_f8;
  void *local_f4;
  char *local_f0;
  char *local_ec;
  char *local_e8;
  float local_e4;
  char *local_e0;
  float local_dc;
  float local_d8;
  float local_d4;
  void *local_d0;
  float local_cc;
  float local_c8;
  float local_c4;
  float local_c0;
  float local_bc;
  float local_b8;
  float local_b4;
  float local_b0;
  char **local_ac;
  char **local_a8;
  void *local_a4;
  int local_a0;
  char **local_9c;
  char **local_98;
  float local_94;
  char **local_90;
  float local_8c;
  float local_88;
  float local_84;
  void *local_80;
  int local_7c;
  char **local_78;
  char **local_74;
  char *local_70;
  char *local_6c;
  char **local_68;
  void *local_64;
  void *local_60;
  int local_5c;
  char **local_58;
  char **local_54;
  int local_50;
  char *local_4c;
  int local_48;
  char *local_44;
  default_delete<class_std::_Facet_base> *local_40;
  default_delete<class_std::_Facet_base> *local_3c;
  default_delete<class_std::_Facet_base> *local_38;
  default_delete<class_std::_Facet_base> *local_34;
  void *local_30;
  int local_2c;
  undefined1 local_25;
  void *local_24;
  int local_20;
  char **local_1c;
  float local_18 [3];
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_1c = (char **)this;
  FUN_00506d80(this,param_5);
  local_d0 = (void *)FUN_0040ff90((int)local_1c[0x48]);
  local_20 = FUN_00506480(local_d0,(uint)param_1);
  local_1c[0x27] = (char *)0x1;
  local_1c[0x36] = param_1;
  FUN_00423520(local_1c + 7,0);
  local_1c[0x2b] = (char *)(int)*(short *)(local_20 + 2);
  local_1c[0x2c] = *(char **)(local_20 + 0xc);
  local_1c[0x2d] = *(char **)(local_20 + 0x10);
  local_1c[0x44] = param_4;
  *(undefined1 *)(local_1c + 0x45) = 0;
  iVar2 = FUN_0040ff90((int)local_1c[0x48]);
  local_25 = FUN_004ff5e0(iVar2);
  local_1c[0xf] = (char *)((uint)local_1c[0xf] & 0xfffffffd | (uint)(byte)local_25 << 1);
  if ((*(byte *)(local_20 + 0x28) & 0xf) != 0) {
    if (((uint)local_1c[0xf] >> 1 & 1) == 0) {
      iVar2 = FUN_00464080(0);
      iVar2 = FUN_005064f0(iVar2);
      local_50 = (iVar2 * (int)local_1c[0x2b]) / 100;
      local_4c = local_1c[0x2b];
      local_1c[0x2b] = local_4c + local_50;
    }
    else {
      iVar2 = FUN_00464080(0);
      iVar2 = FUN_005061c0(iVar2);
      local_48 = (iVar2 * (int)local_1c[0x2b]) / 100;
      local_44 = local_1c[0x2b];
      local_1c[0x2b] = local_44 + local_48;
    }
  }
  local_1c[0x10] = *(char **)(&DAT_00573178 + *(int *)(local_20 + 0x48) * 4);
  local_1c[0x11] = *(char **)(&DAT_00573200 + *(int *)(local_20 + 0x4c) * 4);
  local_1c[0x12] = *(char **)(&DAT_005731fc + *(int *)(local_20 + 0x50) * 4);
  local_1c[0x13] = *(char **)(&DAT_00573248 + *(int *)(local_20 + 0x54) * 4);
  FUN_004a73d0(local_1c + 0x14);
  if ((*(byte *)(local_20 + 0x28) & 0xf) == 0) {
    local_54 = local_1c + 0x14;
    FUN_004393f0(local_54,param_3);
  }
  else if ((*(byte *)(local_20 + 0x28) & 0xf) == 6) {
    local_58 = local_1c + 0x14;
    FUN_004393f0(local_58,(undefined4 *)&DAT_00573278);
  }
  else {
    FUN_00422e10(local_18);
    local_60 = (void *)FUN_0040ff90((int)local_1c[0x48]);
    local_5c = ((int)*(char *)(local_20 + 0x28) & 0xfU) - 1;
    local_64 = (void *)FUN_005062f0(local_60,local_5c);
    FUN_004fee00(local_64,local_18);
    local_68 = local_1c + 0x14;
    FUN_004393f0(local_68,local_18);
  }
  if (*(char *)(local_20 + 0x29) == '\x02') {
    FUN_00452f20(local_1c + 0x26,*(float *)(local_20 + 0x14));
    local_6c = local_1c[0x47];
    FUN_00506bf0(local_6c,*(int *)(local_20 + 0x38),1);
    local_70 = local_1c[0x47];
    FUN_00506bc0(local_70,*(int *)(local_20 + 0x38),local_1c);
  }
  local_74 = local_1c + 0x14;
  FUN_0047a5c0(local_74,*(undefined4 *)(local_20 + 0x1c));
  if (*(float *)(local_20 + 0x14) < DAT_0056cdb0) {
    if (*(float *)(local_20 + 0x14) < _DAT_005732cc) {
      local_ac = local_1c + 0x14;
      FUN_0047a560(local_ac,*(float *)(local_20 + 0x14));
    }
    else if ((*(byte *)(local_20 + 0x28) & 0xf) == 0) {
      local_9c = local_1c + 0x14;
      FUN_0047a560(local_9c,*(float *)(local_20 + 0x14));
    }
    else {
      local_a8 = local_1c + 0x14;
      local_a4 = (void *)FUN_0040ff90((int)local_1c[0x48]);
      local_a0 = ((int)*(char *)(local_20 + 0x28) & 0xfU) - 1;
      fVar7 = (float10)FUN_00506310(local_a4,local_a0);
      FUN_0047a560(local_a8,(float)fVar7);
    }
  }
  else if ((*(byte *)(local_20 + 0x28) & 0xf) == 0) {
    local_78 = local_1c + 0x14;
    FUN_0047a560(local_78,*(float *)(local_20 + 0x14));
  }
  else {
    local_90 = local_1c + 0x14;
    local_80 = (void *)FUN_0040ff90((int)local_1c[0x48]);
    local_7c = ((int)*(char *)(local_20 + 0x28) & 0xfU) - 1;
    fVar7 = (float10)FUN_00506310(local_80,local_7c);
    local_8c = (float)fVar7;
    local_84 = local_8c;
    fVar7 = FUN_004298e0(0x5ba4a8);
    local_88 = (float)fVar7;
    FUN_0047a560(local_90,local_8c + (local_88 * DAT_0056e0f0) / DAT_0056f7a8);
    local_98 = local_1c + 0x14;
    fVar7 = FUN_004298e0(0x5ba4a8);
    local_94 = (float)fVar7;
    FUN_0047a5c0(local_98,local_94 * DAT_0056c8d0 + *(float *)(local_20 + 0x1c));
  }
  FUN_00453e40(local_1c + 0x14);
  if (((*(char *)(local_20 + 0x29) == '\x04') || (*(char *)(local_20 + 0x29) == '\x05')) ||
     (*(char *)(local_20 + 0x29) == '\x06')) {
    local_c4 = *(float *)(local_20 + 4);
    local_3c = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(local_1c + 0x14));
    local_c0 = *(float *)local_3c;
    *(float *)local_3c = local_c0 + local_c4;
    local_cc = *(float *)(local_20 + 8);
    local_40 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(local_1c + 0x14));
    local_40 = local_40 + 4;
    local_c8 = *(float *)local_40;
    *(float *)local_40 = local_c8 + local_cc;
  }
  else {
    pfVar3 = (float *)FUN_004562c0((int)(local_1c + 0x14));
    local_b4 = *(float *)(local_20 + 4) - *pfVar3;
    local_34 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(local_1c + 0x14));
    local_b0 = *(float *)local_34;
    *(float *)local_34 = local_b0 + local_b4;
    iVar2 = FUN_004562c0((int)(local_1c + 0x14));
    local_bc = *(float *)(local_20 + 8) - *(float *)(iVar2 + 4);
    local_38 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(local_1c + 0x14));
    local_38 = local_38 + 4;
    local_b8 = *(float *)local_38;
    *(float *)local_38 = local_b8 + local_bc;
  }
  iVar2 = FUN_0040ff90((int)local_1c[0x48]);
  pcVar4 = (char *)FUN_0041caf0(iVar2);
  local_1c[0x43] = pcVar4;
  local_f0 = local_1c[0x43];
  puVar5 = FUN_00450c70(&local_110,(uint *)0x0,(int)*(short *)(local_20 + 0x2a),-1,(undefined4 *)0x0
                       );
  local_1c[6] = (char *)*puVar5;
  local_24 = (void *)FUN_0044ced0(local_1c + 6);
  local_2c = (*(byte *)((int)local_24 + 0x4a2) >> 5) - 1;
  switch(local_2c) {
  case 0:
    FUN_00450590(local_24,*(undefined4 *)(local_20 + 0x14));
    break;
  case 1:
    FUN_00429210(&local_c,*(float *)(local_20 + 0x14));
    fVar7 = FUN_004292e0(&local_c);
    local_d4 = (float)fVar7;
    if (DAT_0056e0f8 / DAT_0056c8d0 <= local_d4) {
      fVar7 = FUN_004292e0(&local_c);
      local_d8 = (float)fVar7;
      if (local_d8 <= DAT_0056e0f0 / DAT_0056c8d0) {
        fVar7 = FUN_004292e0(&local_c);
        FUN_00450590(local_24,(float)fVar7);
        fVar7 = FUN_004379c0((int)local_24);
        fVar7 = FUN_0047a630((float)fVar7);
        FUN_004777f0(local_24,(float)fVar7);
        break;
      }
    }
    pfVar3 = (float *)FUN_00452fc0(&local_c,local_108,DAT_0056e0f0);
    fVar7 = FUN_004292e0(pfVar3);
    FUN_00450590(local_24,(float)fVar7);
    fVar7 = FUN_004379c0((int)local_24);
    fVar7 = FUN_0047a630((float)fVar7);
    local_dc = (float)fVar7;
    FUN_004777f0(local_24,(uint)local_dc ^ _DAT_0056d7d0);
    break;
  case 2:
    fVar7 = FUN_00438540(*(float *)(local_20 + 0x14) + DAT_0056e0f0);
    FUN_00450590(local_24,(float)fVar7);
    break;
  case 3:
    fVar7 = FUN_00438540(DAT_0056e0f0 / DAT_0056c8d0 + *(float *)(local_20 + 0x14));
    FUN_00450590(local_24,(float)fVar7);
    break;
  case 4:
    fVar7 = FUN_00438540(*(float *)(local_20 + 0x14) - DAT_0056e0f0 / DAT_0056c8d0);
    FUN_00450590(local_24,(float)fVar7);
  }
  bVar1 = FUN_005069a0((int)local_1c[0x47]);
  if (bVar1) {
    local_1c[0x2c] = (char *)((float)local_1c[0x2c] * DAT_00573160);
    local_1c[0x2d] = (char *)((float)local_1c[0x2d] * DAT_00573160);
    if (*(char *)(local_20 + 0x29) == '\x02') {
      FUN_004864f0(local_24,DAT_0056c8cc,DAT_0056c8cc);
    }
    else {
      FUN_004864f0(local_24,DAT_00573160,DAT_00573160);
    }
  }
  else if (*(char *)(local_20 + 0x29) == '\x02') {
    FUN_004864f0(local_24,DAT_0056c8cc,DAT_0056c8cc);
  }
  else {
    FUN_004864f0(local_24,DAT_0056f798,DAT_0056f798);
  }
  local_f4 = (void *)FUN_00412710((int)local_1c[0x48]);
  local_e0 = local_1c[0x2b];
  pfVar3 = (float *)FUN_00421830((int)(local_1c + 0x14));
  fVar7 = FUN_004292e0(pfVar3);
  local_e4 = (float)fVar7;
  local_e8 = local_1c[0x2d];
  local_ec = local_1c[0x2c];
  local_104 = std::
              _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                            *)(local_1c + 0x14));
  puVar5 = FUN_004c1ae0(local_f4,&local_10c,(undefined4 *)local_104,local_ec,local_e8,local_e4,
                        9999999,local_e0);
  local_1c[0x37] = (char *)*puVar5;
  local_30 = (void *)FUN_004c0e10(local_1c + 0x37);
  if (local_30 != (void *)0x0) {
    FUN_00506ba0(local_30,1);
    FUN_00506d20(local_30,local_1c[5]);
    FUN_00506cf0(local_30,1);
  }
  local_1c[0xf] = (char *)((uint)local_1c[0xf] | 1);
  if (local_1c[0x10] != (char *)0x0) {
    local_f8 = (code *)local_1c[0x10];
    iVar2 = (*local_f8)(param_2);
    if (iVar2 != 0) {
      FUN_00506af0(local_1c);
      goto LAB_00505a05;
    }
  }
  if (-1 < *(short *)(local_20 + 0x2c)) {
    pdVar6 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(local_1c + 0x14));
    local_fc = *(float *)pdVar6;
    local_100 = (int)*(short *)(local_20 + 0x2c);
    FUN_00426eb0(&DAT_005ba830,local_100,local_fc);
  }
  pdVar6 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)(local_1c + 0x14));
  *(undefined4 *)((int)local_24 + 0x5bc) = *(undefined4 *)pdVar6;
  *(undefined4 *)((int)local_24 + 0x5c0) = *(undefined4 *)(pdVar6 + 4);
  *(undefined4 *)((int)local_24 + 0x5c4) = *(undefined4 *)(pdVar6 + 8);
LAB_00505a05:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

