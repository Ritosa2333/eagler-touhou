/* Entry: 0x00463310; symbol: FUN_00463310; body bytes: 2932 */

/* WARNING: Type propagation algorithm not settling */

void __thiscall FUN_00463310(void *this,int param_1)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined1 local_1e4 [12];
  undefined1 local_1d8 [12];
  undefined4 local_1cc;
  undefined4 local_1c8;
  void *local_1c4;
  int local_1c0;
  void *local_1bc;
  void *local_1b8;
  void *local_1b4;
  int local_1b0;
  void *local_1ac;
  void *local_1a8;
  void *local_1a4;
  void *local_1a0;
  void *local_19c;
  undefined4 local_198;
  float *local_194;
  default_delete<class_std::_Facet_base> *local_190;
  undefined4 local_18c;
  float *local_188;
  default_delete<class_std::_Facet_base> *local_184;
  default_delete<class_std::_Facet_base> *local_180;
  void *local_17c;
  void *local_178;
  void *local_174;
  void *local_170;
  void *local_16c;
  void *local_168;
  void *local_164;
  void *local_160;
  void *local_15c;
  void *local_158;
  void *local_154;
  int local_150;
  undefined4 local_14c;
  void *local_148;
  void *local_144;
  void *local_140;
  void *local_13c;
  void *local_138;
  void *local_134;
  void *local_130;
  void *local_12c;
  void *local_128;
  void *local_124;
  void *local_120;
  int local_11c;
  undefined4 local_118;
  void *local_114;
  void *local_110;
  void *local_10c;
  void *local_108;
  void *local_104;
  void *local_100;
  void *local_fc;
  void *local_f8;
  void *local_f4;
  void *local_f0;
  void *local_ec;
  int local_e8;
  undefined4 local_e4;
  void *local_e0;
  void *local_dc;
  void *local_d8;
  void *local_d4;
  void *local_d0;
  void *local_cc;
  void *local_c8;
  void *local_c4;
  void *local_c0;
  void *local_bc;
  void *local_b8;
  int local_b4;
  undefined4 local_b0;
  void *local_ac;
  void *local_a8;
  default_delete<class_std::_Facet_base> *local_a4;
  default_delete<class_std::_Facet_base> *local_a0;
  default_delete<class_std::_Facet_base> *local_9c;
  default_delete<class_std::_Facet_base> *local_98;
  void *local_94;
  void *local_90;
  int local_8c;
  undefined4 local_88;
  void *local_84;
  default_delete<class_std::_Facet_base> *local_80;
  default_delete<class_std::_Facet_base> *local_7c;
  void *local_78;
  default_delete<class_std::_Facet_base> *local_74;
  default_delete<class_std::_Facet_base> *local_70;
  int local_6c;
  int *local_68;
  default_delete<class_std::_Facet_base> *local_64;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_60;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_5c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_58;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_54;
  undefined4 *local_50;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_4c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_48;
  default_delete<class_std::_Facet_base> *local_44;
  default_delete<class_std::_Facet_base> *local_40;
  default_delete<class_std::_Facet_base> *local_3c;
  default_delete<class_std::_Facet_base> *local_38;
  default_delete<class_std::_Facet_base> *local_34;
  default_delete<class_std::_Facet_base> *local_30;
  int local_2c;
  int local_28;
  void *local_24;
  int local_20 [3];
  undefined4 local_14;
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_164 = (void *)((int)this + 0x13c);
  local_24 = this;
  FUN_0044ef90(local_164,param_1);
  local_2c = param_1 + -1;
  switch(param_1) {
  case 1:
    local_1a0 = *(void **)((int)local_24 + 4);
    FUN_00464580(local_1a0,0x14,0,0);
    FUN_00423520((void *)((int)local_24 + 0x15c),0x14);
    local_48 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)((int)local_24 + 0x128);
    local_30 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first(local_48);
    local_70 = (default_delete<class_std::_Facet_base> *)FUN_00421970((int)local_48);
    for (; local_30 != local_70; local_30 = local_30 + 4) {
      local_74 = local_30;
      FUN_0044ef90(local_30,param_1);
    }
    local_78 = (void *)((int)local_24 + 0x138);
    FUN_0044ef90(local_78,param_1);
    break;
  case 2:
    local_14 = 0;
    local_50 = &local_14;
    FUN_00461510(&local_c);
    FUN_004615d0(local_20 + 2);
    while (bVar1 = FUN_00449460(&local_c,local_20 + 2), bVar1) {
      local_6c = FUN_0040c300(&local_c);
      local_84 = (void *)((int)local_24 + 0x128);
      local_90 = (void *)FUN_00414580(local_84,local_6c);
      iVar3 = FUN_00464080(0);
      local_88 = FUN_00412730(iVar3);
      local_8c = FUN_00464100(DAT_005c6108);
      local_8c = local_8c + 7;
      FUN_0044f0d0(local_90,local_8c);
      FUN_00461860(&local_c);
    }
    local_94 = *(void **)((int)local_24 + 4);
    FUN_00464580(local_94,0x14,0,0xff);
    local_54 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)((int)local_24 + 0x128);
    local_38 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first(local_54);
    local_98 = (default_delete<class_std::_Facet_base> *)FUN_00421970((int)local_54);
    for (; local_38 != local_98; local_38 = local_38 + 4) {
      local_9c = local_38;
      FUN_0044ef90(local_38,param_1);
    }
    break;
  case 3:
    local_17c = *(void **)((int)local_24 + 4);
    FUN_00464580(local_17c,0x14,0,0x60);
    local_5c = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)((int)local_24 + 0x128);
    local_40 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first(local_5c);
    local_180 = (default_delete<class_std::_Facet_base> *)FUN_00421970((int)local_5c);
    for (; local_40 != local_180; local_40 = local_40 + 4) {
      local_184 = local_40;
      FUN_0044ef90(local_40,param_1);
    }
    break;
  case 6:
    local_4c = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)((int)local_24 + 0x128);
    local_34 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first(local_4c);
    local_7c = (default_delete<class_std::_Facet_base> *)FUN_00421970((int)local_4c);
    for (; local_34 != local_7c; local_34 = local_34 + 4) {
      local_80 = local_34;
      FUN_0044ef90(local_34,param_1);
    }
    break;
  case 7:
  case 0xc:
    local_58 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)((int)local_24 + 0x128);
    local_3c = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first(local_58);
    local_a0 = (default_delete<class_std::_Facet_base> *)FUN_00421970((int)local_58);
    for (; local_3c != local_a0; local_3c = local_3c + 4) {
      local_a4 = local_3c;
      FUN_0044ef90(local_3c,3);
    }
    local_a8 = (void *)((int)local_24 + 0x138);
    FUN_0044ef90(local_a8,2);
    *(undefined4 *)((int)local_24 + 0x16c) = 0;
    break;
  case 0xd:
    local_ac = (void *)((int)local_24 + 0x128);
    local_b8 = (void *)FUN_00414580(local_ac,0);
    iVar3 = FUN_00464080(0);
    local_b0 = FUN_00412730(iVar3);
    local_b4 = FUN_00464100(DAT_005c6108);
    local_b4 = local_b4 + 7;
    FUN_0044f0d0(local_b8,local_b4);
  case 8:
    local_bc = (void *)((int)local_24 + 0x128);
    local_c0 = (void *)FUN_00414580(local_bc,0);
    FUN_0044ef90(local_c0,2);
    local_c4 = (void *)((int)local_24 + 0x128);
    local_c8 = (void *)FUN_00414580(local_c4,1);
    FUN_0044ef90(local_c8,3);
    local_cc = (void *)((int)local_24 + 0x128);
    local_d0 = (void *)FUN_00414580(local_cc,2);
    FUN_0044ef90(local_d0,3);
    local_d4 = (void *)((int)local_24 + 0x128);
    local_d8 = (void *)FUN_00414580(local_d4,3);
    FUN_0044ef90(local_d8,3);
    local_dc = (void *)((int)local_24 + 0x138);
    FUN_0044ef90(local_dc,3);
    *(undefined4 *)((int)local_24 + 0x16c) = 1;
    break;
  case 0xe:
    local_e0 = (void *)((int)local_24 + 0x128);
    local_ec = (void *)FUN_00414580(local_e0,1);
    iVar3 = FUN_00464080(0);
    local_e4 = FUN_00412730(iVar3);
    local_e8 = FUN_00464100(DAT_005c6108);
    local_e8 = local_e8 + 7;
    FUN_0044f0d0(local_ec,local_e8);
  case 9:
    local_f0 = (void *)((int)local_24 + 0x128);
    local_f4 = (void *)FUN_00414580(local_f0,0);
    FUN_0044ef90(local_f4,3);
    local_f8 = (void *)((int)local_24 + 0x128);
    local_fc = (void *)FUN_00414580(local_f8,1);
    FUN_0044ef90(local_fc,2);
    local_100 = (void *)((int)local_24 + 0x128);
    local_104 = (void *)FUN_00414580(local_100,2);
    FUN_0044ef90(local_104,3);
    local_108 = (void *)((int)local_24 + 0x128);
    local_10c = (void *)FUN_00414580(local_108,3);
    FUN_0044ef90(local_10c,3);
    local_110 = (void *)((int)local_24 + 0x138);
    FUN_0044ef90(local_110,3);
    *(undefined4 *)((int)local_24 + 0x16c) = 2;
    break;
  case 0xf:
    local_114 = (void *)((int)local_24 + 0x128);
    local_120 = (void *)FUN_00414580(local_114,2);
    iVar3 = FUN_00464080(0);
    local_118 = FUN_00412730(iVar3);
    local_11c = FUN_00464100(DAT_005c6108);
    local_11c = local_11c + 7;
    FUN_0044f0d0(local_120,local_11c);
  case 10:
    local_124 = (void *)((int)local_24 + 0x128);
    local_128 = (void *)FUN_00414580(local_124,0);
    FUN_0044ef90(local_128,3);
    local_12c = (void *)((int)local_24 + 0x128);
    local_130 = (void *)FUN_00414580(local_12c,1);
    FUN_0044ef90(local_130,3);
    local_134 = (void *)((int)local_24 + 0x128);
    local_138 = (void *)FUN_00414580(local_134,2);
    FUN_0044ef90(local_138,2);
    local_13c = (void *)((int)local_24 + 0x128);
    local_140 = (void *)FUN_00414580(local_13c,3);
    FUN_0044ef90(local_140,3);
    local_144 = (void *)((int)local_24 + 0x138);
    FUN_0044ef90(local_144,3);
    *(undefined4 *)((int)local_24 + 0x16c) = 3;
    break;
  case 0x10:
    local_148 = (void *)((int)local_24 + 0x128);
    local_154 = (void *)FUN_00414580(local_148,3);
    iVar3 = FUN_00464080(0);
    local_14c = FUN_00412730(iVar3);
    local_150 = FUN_00464100(DAT_005c6108);
    local_150 = local_150 + 7;
    FUN_0044f0d0(local_154,local_150);
  case 0xb:
    local_158 = (void *)((int)local_24 + 0x128);
    local_15c = (void *)FUN_00414580(local_158,0);
    FUN_0044ef90(local_15c,3);
    local_160 = (void *)((int)local_24 + 0x128);
    local_19c = (void *)FUN_00414580(local_160,1);
    FUN_0044ef90(local_19c,3);
    local_168 = (void *)((int)local_24 + 0x128);
    local_16c = (void *)FUN_00414580(local_168,2);
    FUN_0044ef90(local_16c,3);
    local_170 = (void *)((int)local_24 + 0x128);
    local_174 = (void *)FUN_00414580(local_170,3);
    FUN_0044ef90(local_174,2);
    local_178 = (void *)((int)local_24 + 0x138);
    FUN_0044ef90(local_178,3);
    *(undefined4 *)((int)local_24 + 0x16c) = 4;
    break;
  case 0x11:
    *(undefined1 *)((int)local_24 + 0x170) = 1;
    FUN_0044fcd0((undefined4 *)((int)local_24 + 0x138));
    FUN_0044fcd0((undefined4 *)((int)local_24 + 0x13c));
    iVar3 = FUN_0051b960();
    local_18c = FUN_00411700(iVar3);
    local_188 = (float *)FUN_00422dd0(local_1d8,0,0,0);
    puVar4 = FUN_00464600(&local_1c8,(uint *)"stone",4,local_188,(undefined4 *)0x0);
    *(undefined4 *)((int)local_24 + 0x13c) = *puVar4;
    local_60 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)((int)local_24 + 0x128);
    local_44 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first(local_60);
    local_190 = (default_delete<class_std::_Facet_base> *)FUN_00421970((int)local_60);
    for (; local_44 != local_190; local_44 = local_44 + 4) {
      local_64 = local_44;
      FUN_0044fcd0((undefined4 *)local_44);
      iVar3 = FUN_0051b960();
      local_198 = FUN_00411700(iVar3);
      local_194 = (float *)FUN_00422dd0(local_1e4,0,0,0);
      puVar4 = FUN_00464600(&local_1cc,(uint *)"stone",5,local_194,(undefined4 *)0x0);
      *(undefined4 *)local_64 = *puVar4;
    }
    local_20[1] = 0;
    local_68 = local_20 + 1;
    FUN_00461510(&local_10);
    FUN_004615d0(local_20);
    while (bVar1 = FUN_00449460(&local_10,local_20), bVar1) {
      local_28 = FUN_0040c300(&local_10);
      local_1a4 = (void *)FUN_00464080(0);
      cVar2 = FUN_00464420(local_1a4,local_28);
      if (cVar2 == '\0') {
        local_1a8 = (void *)((int)local_24 + 0x128);
        local_1b4 = (void *)FUN_00414580(local_1a8,local_28);
        local_1ac = (void *)FUN_00464080(0);
        local_1b0 = FUN_004641d0(local_1ac,local_28);
        FUN_00464510(local_1b4,local_1b0);
      }
      else {
        local_1b8 = (void *)((int)local_24 + 0x128);
        local_1c4 = (void *)FUN_00414580(local_1b8,local_28);
        local_1bc = (void *)FUN_00464080(0);
        local_1c0 = FUN_004641d0(local_1bc,local_28);
        local_1c0 = local_1c0 + 10;
        FUN_00464510(local_1c4,local_1c0);
      }
      FUN_00461860(&local_10);
    }
    break;
  case 0x1b:
    FUN_004644f0((void *)((int)local_24 + 0x138));
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

