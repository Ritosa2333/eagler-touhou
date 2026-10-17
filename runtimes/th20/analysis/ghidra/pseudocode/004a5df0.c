/* Entry: 0x004a5df0; symbol: FUN_004a5df0; body bytes: 4673 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004a5df0(int param_1)

{
  bool bVar1;
  char cVar2;
  undefined4 *puVar3;
  int iVar4;
  int *piVar5;
  uint uVar6;
  undefined3 extraout_var;
  int iVar7;
  default_delete<class_std::_Facet_base> *pdVar8;
  undefined3 extraout_var_00;
  float *pfVar9;
  float10 fVar10;
  undefined4 local_17c;
  undefined4 local_178;
  undefined4 local_174;
  undefined4 local_170;
  void *local_16c;
  void *local_168;
  void *local_164;
  void *local_160;
  int local_15c;
  float local_158;
  void *local_154;
  default_delete<class_std::_Facet_base> *local_150;
  void *local_14c;
  float *local_148;
  void *local_144;
  void *local_140;
  float local_13c;
  float local_138;
  float local_134;
  float local_130;
  void *local_12c;
  void *local_128;
  void *local_124;
  float local_120;
  void *local_11c;
  void *local_118;
  float local_114;
  void *local_110;
  void *local_10c;
  void *local_108;
  void *local_104;
  int local_100;
  int local_fc;
  code *local_f8;
  void *local_f4;
  float local_f0;
  float local_ec;
  void *local_e8;
  default_delete<class_std::_Facet_base> *local_e4;
  float local_e0;
  int local_dc;
  int local_d8;
  int local_d4;
  void *local_d0;
  void *local_cc;
  default_delete<class_std::_Facet_base> *local_c8;
  float local_c4;
  int local_c0;
  int local_bc;
  int local_b8;
  void *local_b4;
  void *local_b0;
  float local_ac;
  void *local_a8;
  void *local_a4;
  default_delete<class_std::_Facet_base> *local_a0;
  float local_9c;
  int local_98;
  int local_94;
  int local_90;
  void *local_8c;
  void *local_88;
  default_delete<class_std::_Facet_base> *local_84;
  float local_80;
  int local_7c;
  int local_78;
  int local_74;
  void *local_70;
  int local_6c;
  int local_68;
  void *local_64;
  int local_60;
  int local_5c;
  void *local_58;
  int local_54;
  int local_50;
  void *local_4c;
  int local_48;
  int local_44;
  int *local_40;
  int *local_3c;
  int *local_38;
  int local_34;
  int *local_30;
  int local_2c;
  void *local_28;
  int local_24;
  int local_20;
  float local_1c [3];
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_124 = (void *)(param_1 + 0xc);
  local_20 = param_1;
  puVar3 = (undefined4 *)FUN_0048bce0(local_124,0);
  iVar4 = FUN_0044ced0(puVar3);
  if (iVar4 == 0) goto LAB_004a7022;
  *(uint *)(local_20 + 0x2d0) = *(uint *)(local_20 + 0x2d0) & 0xffffffef;
  if ((*(uint *)(local_20 + 0x2cc) >> 0xc & 1) == 0) {
    iVar4 = FUN_00424010(*(int *)(local_20 + 0x2ec));
    bVar1 = FUN_00478130(iVar4);
    if ((!bVar1) && ((*(uint *)(local_20 + 0x2cc) >> 0xd & 1) != 0)) {
      if (-1 < *(int *)(local_20 + 0x54)) {
        *(undefined4 *)(local_20 + 0x28) = *(undefined4 *)(local_20 + 0x58);
        local_64 = (void *)(local_20 + 0xc);
        local_6c = FUN_0048bce0(local_64,0);
        local_68 = *(int *)(local_20 + 0x28);
        FUN_0044bcd0(local_68);
      }
      *(uint *)(local_20 + 0x2cc) = *(uint *)(local_20 + 0x2cc) & 0xffffdfff;
      *(uint *)(local_20 + 0x2c8) = *(uint *)(local_20 + 0x2c8) & 0xfffffffe;
    }
  }
  else {
    iVar4 = FUN_00424010(*(int *)(local_20 + 0x2ec));
    bVar1 = FUN_00478130(iVar4);
    if ((bVar1) && ((*(uint *)(local_20 + 0x2cc) >> 0xd & 1) == 0)) {
      if (-1 < *(int *)(local_20 + 0x54)) {
        *(undefined4 *)(local_20 + 0x28) = *(undefined4 *)(local_20 + 0x54);
        local_4c = (void *)(local_20 + 0xc);
        local_54 = FUN_0048bce0(local_4c,0);
        local_50 = *(int *)(local_20 + 0x28);
        FUN_0044bcd0(local_50);
      }
      *(uint *)(local_20 + 0x2cc) = *(uint *)(local_20 + 0x2cc) | 0x2000;
      *(uint *)(local_20 + 0x2c8) = *(uint *)(local_20 + 0x2c8) | 1;
    }
    else {
      iVar4 = FUN_00424010(*(int *)(local_20 + 0x2ec));
      bVar1 = FUN_00478130(iVar4);
      if ((!bVar1) && ((*(uint *)(local_20 + 0x2cc) >> 0xd & 1) != 0)) {
        if (-1 < *(int *)(local_20 + 0x54)) {
          *(undefined4 *)(local_20 + 0x28) = *(undefined4 *)(local_20 + 0x58);
          local_58 = (void *)(local_20 + 0xc);
          local_60 = FUN_0048bce0(local_58,0);
          local_5c = *(int *)(local_20 + 0x28);
          FUN_0044bcd0(local_5c);
        }
        *(uint *)(local_20 + 0x2cc) = *(uint *)(local_20 + 0x2cc) & 0xffffdfff;
        *(uint *)(local_20 + 0x2c8) = *(uint *)(local_20 + 0x2c8) & 0xfffffffe;
      }
    }
  }
  if ((*(uint *)(local_20 + 0x2c8) >> 0xb & 1) != 0) {
    local_2c = 0;
    local_10 = 0;
    *(uint *)(local_20 + 0x2cc) = *(uint *)(local_20 + 0x2cc) & 0xfdffffff;
    if ((*(uint *)(local_20 + 0x2c8) >> 0xc & 1) == 0) {
      local_88 = (void *)FUN_00412710(*(int *)(local_20 + 0x2ec));
      local_70 = *(void **)(local_20 + 8);
      piVar5 = FUN_00498f90(local_70,&local_17c);
      local_7c = *piVar5;
      local_80 = *(float *)(local_20 + 0x5c) / DAT_0056c8d0;
      local_78 = local_7c;
      local_74 = local_7c;
      local_84 = std::
                 _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                               *)(local_20 + 0x110));
      local_2c = FUN_004c0480(local_88,(float *)local_84,(float *)0x0,0.0,local_80,&local_10,
                              (undefined4 *)(local_20 + 0x6c),1,local_7c);
    }
    else {
      local_a4 = (void *)FUN_00412710(*(int *)(local_20 + 0x2ec));
      local_8c = *(void **)(local_20 + 8);
      piVar5 = FUN_00498f90(local_8c,&local_170);
      local_98 = *piVar5;
      local_9c = *(float *)(local_20 + 0x38);
      local_94 = local_98;
      local_90 = local_98;
      local_a0 = std::
                 _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                               *)(local_20 + 0x110));
      local_2c = FUN_004c0480(local_a4,(float *)local_a0,(float *)(local_20 + 0x5c),local_9c,0.0,
                              &local_10,(undefined4 *)(local_20 + 0x6c),1,local_98);
    }
    if (((local_2c != 0) && (local_10 != 0)) &&
       (iVar4 = FUN_004a5640(*(void **)(local_20 + 8)), iVar4 != 0)) goto LAB_004a7022;
  }
  local_30 = FUN_004aa400(*(int *)(local_20 + 8));
  if (local_30 != (int *)0x0) {
    FUN_00423520((void *)(local_20 + 0xa8),0);
    FUN_004973c0(*(int *)(local_20 + 8));
    FUN_004972c0(*(int *)(local_20 + 8));
    local_a8 = *(void **)(local_20 + 8);
    FUN_00540200(local_a8,(char *)local_30);
    local_b0 = *(void **)(local_20 + 8);
    fVar10 = (float10)FUN_00456240(local_20 + 0xa8);
    local_ac = (float)fVar10;
    iVar4 = FUN_0053e2b0(local_b0,local_ac);
    if (iVar4 != 0) goto LAB_004a7022;
  }
  local_c = 0;
  *(uint *)(local_20 + 0x2cc) = *(uint *)(local_20 + 0x2cc) & 0xffffffdf;
  if (((*(uint *)(local_20 + 0x2c8) & 1) == 0) && ((*(uint *)(local_20 + 0x2c8) >> 5 & 1) == 0)) {
    local_24 = 0;
    if (_DAT_0056e0e8 < *(float *)(local_20 + 0x5c)) {
      if ((*(uint *)(local_20 + 0x2c8) >> 0xc & 1) == 0) {
        local_cc = (void *)FUN_00412710(*(int *)(local_20 + 0x2ec));
        local_b4 = *(void **)(local_20 + 8);
        piVar5 = FUN_00498f90(local_b4,&local_174);
        local_c0 = *piVar5;
        local_c4 = *(float *)(local_20 + 0x5c) / DAT_0056c8d0;
        local_bc = local_c0;
        local_b8 = local_c0;
        local_c8 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)(local_20 + 0x110));
        local_24 = FUN_004c0480(local_cc,(float *)local_c8,(float *)0x0,0.0,local_c4,&local_c,
                                (undefined4 *)(local_20 + 0x6c),0,local_c0);
      }
      else {
        local_e8 = (void *)FUN_00412710(*(int *)(local_20 + 0x2ec));
        local_d0 = *(void **)(local_20 + 8);
        piVar5 = FUN_00498f90(local_d0,&local_178);
        local_dc = *piVar5;
        local_e0 = *(float *)(local_20 + 0x38);
        local_d8 = local_dc;
        local_d4 = local_dc;
        local_e4 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)(local_20 + 0x110));
        local_24 = FUN_004c0480(local_e8,(float *)local_e4,(float *)(local_20 + 0x5c),local_e0,0.0,
                                &local_c,(undefined4 *)(local_20 + 0x6c),0,local_dc);
      }
      local_ec = (float)local_24;
      iVar4 = FUN_0040ff90(*(int *)(local_20 + 0x2ec));
      fVar10 = FUN_004aaa40(iVar4);
      local_f0 = (float)fVar10;
      local_24 = (int)(local_ec * local_f0);
      uVar6 = FUN_004ab380(local_20);
      if ((uVar6 & 0xff) != 0) {
        iVar4 = FUN_00464080(0);
        iVar4 = FUN_004aaf50(iVar4);
        if ((0 < iVar4) && (0 < local_24)) {
          iVar4 = FUN_00464080(0);
          iVar4 = FUN_004aaf50(iVar4);
          local_24 = (iVar4 * local_24) / 100 + local_24;
          iVar4 = FUN_00464230(0);
          local_f4 = (void *)FUN_004859d0(iVar4);
          FUN_004865b0(local_f4,1);
        }
      }
    }
    if (*(int *)(local_20 + 0x2dc) != 0) {
      local_f8 = *(code **)(local_20 + 0x2dc);
      iVar4 = (*local_f8)(local_24);
      local_24 = iVar4 + local_24;
    }
    if (0 < *(int *)(local_20 + 0x48)) {
      local_24 = local_24 + *(int *)(local_20 + 0x48);
      *(undefined4 *)(local_20 + 0x48) = 0;
    }
    iVar4 = FUN_0040ff90(*(int *)(local_20 + 0x2ec));
    iVar4 = FUN_00411700(iVar4);
    if (iVar4 == 2) {
LAB_004a65d3:
      local_24 = local_24 / 5;
    }
    else {
      iVar4 = FUN_0040ff90(*(int *)(local_20 + 0x2ec));
      iVar4 = FUN_00411700(iVar4);
      if (iVar4 == 0) goto LAB_004a65d3;
    }
    bVar1 = FUN_00478160(DAT_005c06a4);
    if (CONCAT31(extraout_var,bVar1) != 0) {
      local_24 = 0;
    }
    if (0 < local_24) {
      if (local_c == 0) {
        iVar4 = FUN_00412730(*(int *)(local_20 + 0x2ec));
        local_3c = (int *)(iVar4 + 0xb0);
        *local_3c = *local_3c + local_24;
      }
      else {
        puVar3 = (undefined4 *)FUN_00498f50(local_20);
        iVar4 = FUN_0040c300(puVar3);
        if (local_24 < iVar4) {
          local_34 = local_24;
        }
        else {
          puVar3 = (undefined4 *)FUN_00498f50(local_20);
          iVar4 = FUN_0040c300(puVar3);
          puVar3 = (undefined4 *)FUN_00498f50(local_20);
          iVar7 = FUN_0040c300(puVar3);
          local_34 = iVar4 + ((int)((local_24 - iVar7) + (local_24 - iVar7 >> 0x1f & 3U)) >> 2);
        }
        local_fc = local_34;
        iVar4 = FUN_00412730(*(int *)(local_20 + 0x2ec));
        local_38 = (int *)(iVar4 + 0xac);
        *local_38 = *local_38 + local_fc;
      }
    }
    iVar4 = FUN_00424010(*(int *)(local_20 + 0x2ec));
    bVar1 = FUN_00478130(iVar4);
    if ((bVar1) &&
       (*(float *)(local_20 + 0x4c) <= DAT_0056c8cc && DAT_0056c8cc != *(float *)(local_20 + 0x4c)))
    {
      if ((local_24 != 0) && (*(float *)(local_20 + 0x4c) <= 0.0)) {
        pdVar8 = std::
                 _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                               *)(local_20 + 0x110));
        FUN_00426eb0(&DAT_005ba830,0x24,*(float *)pdVar8);
      }
      local_24 = (int)((float)local_24 * *(float *)(local_20 + 0x4c));
    }
    if (local_24 != 0) {
      iVar4 = FUN_00460830(0);
      local_104 = (void *)FUN_004aaa60(iVar4);
      local_100 = *(int *)(local_20 + 0x268);
      FUN_004aa000(local_104,local_100,5);
      iVar4 = FUN_00411700(*(int *)(local_20 + 0x2ec));
      cVar2 = FUN_004ab340(iVar4);
      if (cVar2 != '\0') {
        local_24 = local_24 / 0x1e;
      }
      if ((*(uint *)(local_20 + 0x2c8) >> 4 & 1) == 0) {
        local_108 = (void *)(local_20 + 0x288);
        bVar1 = FUN_00423590(local_108,0);
        if (!bVar1) goto LAB_004a67e0;
        FUN_004a3f80((void *)(local_20 + 0x18c),local_24);
      }
      else {
LAB_004a67e0:
        local_10c = (void *)(local_20 + 0x18c);
        FUN_004aa050(local_10c,local_24);
      }
      FUN_00423520((void *)(local_20 + 0x2a8),0x1e);
      local_40 = FUN_004aa250(*(int *)(local_20 + 8));
      if (local_40 != (int *)0x0) {
        if (local_c != 0) {
          FUN_0040e5e0();
        }
        FUN_00423520((void *)(local_20 + 0xa8),0);
        FUN_004973c0(*(int *)(local_20 + 8));
        FUN_004972c0(*(int *)(local_20 + 8));
        local_110 = *(void **)(local_20 + 8);
        FUN_00540200(local_110,(char *)local_40);
        local_118 = *(void **)(local_20 + 8);
        fVar10 = (float10)FUN_00456240(local_20 + 0xa8);
        local_114 = (float)fVar10;
        iVar4 = FUN_0053e2b0(local_118,local_114);
        if (iVar4 != 0) goto LAB_004a7022;
      }
      if (((*(uint *)(local_20 + 0x2c8) >> 7 & 1) == 0) &&
         (bVar1 = FUN_004ab240((int *)(local_20 + 0x18c)), CONCAT31(extraout_var_00,bVar1) == 0)) {
        if (local_c != 0) {
          FUN_0040e5e0();
        }
        iVar4 = FUN_004a5640(*(void **)(local_20 + 8));
        if (iVar4 != 0) goto LAB_004a7022;
      }
      *(uint *)(local_20 + 0x2cc) = *(uint *)(local_20 + 0x2cc) | 0x20;
    }
  }
  uVar6 = FUN_004ab290(local_20 + 0x18c);
  if ((uVar6 != 0) && (iVar4 = FUN_004a5640(*(void **)(local_20 + 8)), iVar4 != 0))
  goto LAB_004a7022;
  if (((*(uint *)(local_20 + 0x2c8) >> 1 & 1) == 0) && ((*(uint *)(local_20 + 0x2c8) >> 5 & 1) == 0)
     ) {
    local_11c = (void *)(local_20 + 0x298);
    bVar1 = FUN_00423590(local_11c,0);
    if (((bVar1) && ((*(uint *)(local_20 + 0x2cc) >> 10 & 1) == 0)) &&
       ((iVar4 = FUN_00412730(*(int *)(local_20 + 0x2ec)), *(int *)(iVar4 + 200) == 0 ||
        ((*(uint *)(local_20 + 0x2cc) >> 7 & 1) != 0)))) {
      if (*(int *)(local_20 + 0x2e4) == 0) {
        if ((*(uint *)(local_20 + 0x2c8) >> 0xc & 1) == 0) {
          local_128 = (void *)FUN_0040ff90(*(int *)(local_20 + 0x2ec));
          local_120 = *(float *)(local_20 + 100) / DAT_0056c8d0;
          local_150 = std::
                      _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                      ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                    *)(local_20 + 0x110));
          local_48 = FUN_004f8ff0(local_128,(float *)local_150,local_120,0);
        }
        else {
          FUN_00422e10(local_1c);
          local_12c = (void *)(local_20 + 0xc);
          puVar3 = (undefined4 *)FUN_0048bce0(local_12c,0);
          local_44 = FUN_0044ced0(puVar3);
          local_1c[0] = 0.0;
          local_1c[1] = 0.0;
          local_1c[2] = 0.0;
          if (local_44 != 0) {
            fVar10 = FUN_00437930(local_44);
            local_130 = (float)fVar10;
            fVar10 = FUN_00438540(local_130 + DAT_0056e0f0 / DAT_0056c8d0);
            FUN_00458fa0(local_1c,local_1c,local_1c,(float)fVar10);
          }
          pdVar8 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)(local_20 + 0x110));
          FUN_004296e0(local_1c,(float *)pdVar8);
          local_140 = (void *)FUN_0040ff90(*(int *)(local_20 + 0x2ec));
          local_134 = *(float *)(local_20 + 100);
          local_138 = *(float *)(local_20 + 0x68);
          local_13c = *(float *)(local_20 + 0x38);
          local_48 = FUN_004f91d0(local_140,local_1c,local_13c,local_138,local_134,0);
        }
        if (((*(uint *)(local_20 + 0x2c8) >> 9 & 1) != 0) && (local_48 == 2)) {
          local_144 = (void *)(local_20 + 0xa8);
          iVar4 = FUN_00472040(local_144,6);
          if (iVar4 == 0) {
            local_14c = (void *)FUN_0040ff90(*(int *)(local_20 + 0x2ec));
            iVar4 = FUN_0040ff90(*(int *)(local_20 + 0x2ec));
            local_148 = (float *)FUN_00460850(iVar4);
            FUN_004f8b90(local_14c,local_148,0);
          }
        }
      }
      else {
        (**(code **)(local_20 + 0x2e4))();
      }
    }
  }
  local_16c = (void *)(local_20 + 0xc);
  puVar3 = (undefined4 *)FUN_0048bce0(local_16c,0);
  local_28 = (void *)FUN_0044ced0(puVar3);
  if (local_28 != (void *)0x0) {
    if (*(int *)(local_20 + 0x25c) == 0) {
      if ((*(uint *)(local_20 + 0x2cc) >> 0xf & 1) != 0) {
        local_154 = (void *)(local_20 + 0xa8);
        iVar4 = FUN_00472040(local_154,4);
        if (iVar4 == 0) {
          FUN_004ab610(local_28,0xffff00ff);
          *(byte *)((int)local_28 + 0x4a1) = *(byte *)((int)local_28 + 0x4a1) & 0xe3 | 4;
        }
        else {
          *(byte *)((int)local_28 + 0x4a1) = *(byte *)((int)local_28 + 0x4a1) & 0xe3;
        }
      }
      if (((*(uint *)(local_20 + 0x2cc) >> 5 & 1) == 0) ||
         ((*(uint *)(local_20 + 0x2c8) >> 0xd & 1) != 0)) {
        local_160 = (void *)(local_20 + 0xa8);
        iVar4 = FUN_00472040(local_160,4);
        if (iVar4 == 0) {
          if (((*(uint *)(local_20 + 0x2cc) >> 0xe & 1) != 0) ||
             ((*(uint *)(local_20 + 0x2cc) >> 7 & 1) != 0)) {
            iVar4 = FUN_00411700(*(int *)(local_20 + 0x2ec));
            bVar1 = FUN_004887a0(iVar4);
            if (bVar1) {
              iVar4 = FUN_00411700(*(int *)(local_20 + 0x2ec));
              uVar6 = FUN_004ab320(iVar4);
              if (uVar6 != 0) goto LAB_004a6fe1;
            }
            iVar4 = FUN_00411700(*(int *)(local_20 + 0x2ec));
            bVar1 = FUN_004887a0(iVar4);
            if (bVar1) {
              iVar4 = FUN_004aabd0(*(int *)(local_20 + 8));
              iVar4 = FUN_00412730(iVar4);
              if (99 < iVar4) goto LAB_004a6f44;
LAB_004a6f79:
              FUN_004ab610(local_28,0xff0000ff);
              *(byte *)((int)local_28 + 0x4a1) = *(byte *)((int)local_28 + 0x4a1) & 0xe3 | 4;
            }
            else {
LAB_004a6f44:
              iVar4 = FUN_00411700(*(int *)(local_20 + 0x2ec));
              bVar1 = FUN_004887a0(iVar4);
              if (!bVar1) {
                iVar4 = FUN_004aabd0(*(int *)(local_20 + 8));
                iVar4 = FUN_00412730(iVar4);
                if (iVar4 < 500) goto LAB_004a6f79;
              }
            }
          }
        }
        else {
          *(byte *)((int)local_28 + 0x4a1) = *(byte *)((int)local_28 + 0x4a1) & 0xe3;
        }
      }
      else {
        FUN_004ab610(local_28,0xff0000ff);
        *(byte *)((int)local_28 + 0x4a1) = *(byte *)((int)local_28 + 0x4a1) & 0xe3 | 4;
        *(undefined4 *)(local_20 + 0x25c) = 4;
        if (-1 < *(int *)(local_20 + 0x264)) {
          pfVar9 = (float *)FUN_00499350(local_20);
          local_158 = *pfVar9;
          local_15c = *(int *)(local_20 + 0x264);
          FUN_00426eb0(&DAT_005ba830,local_15c,local_158);
          goto LAB_004a6fe1;
        }
        if (((*(uint *)(local_20 + 0x2cc) >> 0xe & 1) != 0) ||
           ((*(uint *)(local_20 + 0x2cc) >> 7 & 1) != 0)) {
          iVar4 = FUN_00411700(*(int *)(local_20 + 0x2ec));
          bVar1 = FUN_004887a0(iVar4);
          if (bVar1) {
            iVar4 = FUN_00411700(*(int *)(local_20 + 0x2ec));
            uVar6 = FUN_004ab320(iVar4);
            if (uVar6 != 0) goto LAB_004a6e25;
          }
          iVar4 = FUN_00411700(*(int *)(local_20 + 0x2ec));
          bVar1 = FUN_004887a0(iVar4);
          if (bVar1) {
            iVar4 = FUN_004aabd0(*(int *)(local_20 + 8));
            iVar4 = FUN_00412730(iVar4);
            if (199 < iVar4) goto LAB_004a6dcf;
LAB_004a6e04:
            pfVar9 = (float *)FUN_00499350(local_20);
            FUN_00426eb0(&DAT_005ba830,0x23,*pfVar9);
            goto LAB_004a6fe1;
          }
LAB_004a6dcf:
          iVar4 = FUN_00411700(*(int *)(local_20 + 0x2ec));
          bVar1 = FUN_004887a0(iVar4);
          if (!bVar1) {
            iVar4 = FUN_004aabd0(*(int *)(local_20 + 8));
            iVar4 = FUN_00412730(iVar4);
            if (iVar4 < 900) goto LAB_004a6e04;
          }
        }
LAB_004a6e25:
        pfVar9 = (float *)FUN_00499350(local_20);
        FUN_00426eb0(&DAT_005ba830,0x22,*pfVar9);
      }
    }
    else {
      *(byte *)((int)local_28 + 0x4a1) = *(byte *)((int)local_28 + 0x4a1) & 0xe3;
      *(int *)(local_20 + 0x25c) = *(int *)(local_20 + 0x25c) + -1;
    }
  }
LAB_004a6fe1:
  local_164 = (void *)(local_20 + 0x2a8);
  bVar1 = FUN_00461070(local_164,0);
  if (bVar1) {
    local_168 = (void *)(local_20 + 0x2a8);
    FUN_00429420(local_168);
  }
LAB_004a7022:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

