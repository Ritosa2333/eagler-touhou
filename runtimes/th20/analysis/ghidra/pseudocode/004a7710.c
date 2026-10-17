/* Entry: 0x004a7710; symbol: FUN_004a7710; body bytes: 1675 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004a7710(int param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  void *this;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  void *extraout_EAX;
  default_delete<class_std::_Facet_base> *this_00;
  int iVar3;
  undefined4 *puVar4;
  float10 fVar5;
  float *pfVar6;
  undefined4 local_c8 [3];
  undefined4 local_bc;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_b8;
  void *local_b4;
  undefined4 *local_b0;
  float local_ac;
  float local_a8;
  float local_a4;
  float local_a0;
  void *local_9c;
  undefined4 local_98;
  void *local_94;
  undefined4 local_90;
  void *local_8c;
  void *local_88;
  uint local_84;
  void *local_80;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_7c;
  void *local_78;
  default_delete<class_std::_Facet_base> *local_74;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_70;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_6c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_68;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_64;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_60;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_5c;
  int local_58;
  int local_54;
  int local_50;
  int local_4c;
  int local_48;
  int local_44;
  int local_40;
  int local_3c;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_38;
  int local_34;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *local_30;
  int local_2c;
  undefined4 local_28 [3];
  float local_1c;
  undefined4 local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  puVar2 = (undefined4 *)(param_1 + 0x110);
  puVar4 = (undefined4 *)(param_1 + 200);
  for (iVar3 = 0x12; iVar3 != 0; iVar3 = iVar3 + -1) {
    *puVar4 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar4 = puVar4 + 1;
  }
  local_2c = param_1;
  if ((*(uint *)(param_1 + 0x2d0) >> 3 & 1) != 0) {
    iVar3 = FUN_004aad90(*(int *)(param_1 + 8));
    puVar2 = (undefined4 *)FUN_0047a2c0(iVar3);
    local_b4 = (void *)(local_2c + 0x158);
    local_b0 = puVar2;
    this = (void *)FUN_0048bd10(local_b4,0);
    FUN_00477b80(this,puVar2);
  }
  local_48 = local_2c + 0x158;
  local_38 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_00414f30(local_48);
  local_b8 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_00414f70(local_48);
  for (; local_38 != local_b8; local_38 = local_38 + 0x184) {
    local_30 = local_38;
    iVar3 = FUN_0041caf0((int)(local_38 + 0xac));
    if (((iVar3 != 0) && (bVar1 = FUN_0049bea0((int)local_30), CONCAT31(extraout_var,bVar1) == 0))
       && (bVar1 = FUN_004ab2b0((int)local_30), CONCAT31(extraout_var_00,bVar1) == 0)) {
      local_5c = local_30;
      fVar5 = FUN_0042a110((float *)(local_30 + 0xac));
      fVar5 = FUN_00438540((float)fVar5);
      FUN_0047a560(local_5c,(float)fVar5);
    }
    iVar3 = FUN_0041caf0((int)(local_30 + 0xd8));
    if (iVar3 != 0) {
      local_60 = local_30;
      fVar5 = FUN_0042a110((float *)(local_30 + 0xd8));
      FUN_0047a5c0(local_60,(float)fVar5);
    }
    iVar3 = FUN_00438520((int)(local_30 + 0x104));
    if (iVar3 != 0) {
      local_64 = local_30 + 0x104;
      FUN_0042a980(local_64,&local_1c);
      local_68 = local_30;
      FUN_0047a540(local_30,local_1c);
      local_6c = local_30;
      FUN_0047a520(local_30,local_18);
    }
    iVar3 = FUN_00438500((int)(local_30 + 0x48));
    if (iVar3 == 0) {
      FUN_00453e40(local_30);
    }
    else {
      local_70 = local_30 + 0x48;
      FUN_004a8d70((undefined4 *)local_70,local_c8);
      local_78 = extraout_EAX;
      local_74 = std::
                 _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 ::_Get_first(local_30);
      FUN_00429440(local_78,local_28,(float *)local_74);
      local_7c = local_30;
      FUN_0047a5e0(local_30,local_28);
    }
    FUN_00453ac0((float *)local_30);
    if ((*(uint *)(local_2c + 0x2cc) >> 10 & 1) != 0) {
      pfVar6 = (float *)&DAT_005c50fc;
      this_00 = std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first(local_30);
      FUN_004296e0(this_00,pfVar6);
    }
  }
  FUN_004a7da0(local_2c);
  if ((*(uint *)(local_2c + 0x2cc) >> 4 & 1) != 0) {
    local_34 = 0;
    pfVar6 = (float *)FUN_004562c0(local_2c + 0x110);
    if (DAT_00570564 < *pfVar6 || DAT_00570564 == *pfVar6) {
      pfVar6 = (float *)FUN_004562c0(local_2c + 0x110);
      if (*pfVar6 <= _DAT_00570558) {
        local_34 = 0;
      }
      else {
        local_34 = 1;
      }
    }
    else {
      local_34 = -1;
    }
    if (*(int *)(local_2c + 0x2c) != local_34) {
      local_3c = 0;
      local_40 = *(int *)(local_2c + 0x2c);
      if (local_40 == -1) {
        if (local_34 == 0) {
          local_4c = 3;
        }
        else {
          local_4c = 2;
        }
        local_3c = local_4c;
      }
      else if (local_40 == 0) {
        if (local_34 == -1) {
          local_50 = 1;
        }
        else {
          local_50 = 2;
        }
        local_3c = local_50;
      }
      else if (local_40 == 1) {
        if (local_34 == 0) {
          local_54 = 4;
        }
        else {
          local_54 = 1;
        }
        local_3c = local_54;
      }
      local_80 = (void *)(local_2c + 0xc);
      puVar2 = (undefined4 *)FUN_0048bce0(local_80,0);
      local_58 = FUN_0044ced0(puVar2);
      FUN_00422dd0(&local_14,0,0,0);
      local_88 = (void *)FUN_00412730(*(int *)(local_2c + 0x2ec));
      local_84 = *(uint *)(local_2c + 0x20);
      local_90 = FUN_004aae10(local_88,local_84);
      if (local_58 != 0) {
        pfVar6 = (float *)FUN_0044cb60(local_58);
        local_14 = *pfVar6;
        local_10 = pfVar6[1];
        local_c = pfVar6[2];
        local_8c = (void *)(local_2c + 0xc);
        puVar2 = (undefined4 *)FUN_0048bce0(local_8c,0);
        FUN_0044fcd0(puVar2);
      }
      puVar2 = FUN_004aba30(&local_bc,(uint *)0x0,*(int *)(local_2c + 0x28) + local_3c,&local_14,0,
                            *(int *)(local_2c + 0x34) + 7,(undefined4 *)0x0);
      local_98 = *puVar2;
      local_94 = (void *)(local_2c + 0xc);
      puVar2 = (undefined4 *)FUN_0048bce0(local_94,0);
      *puVar2 = local_98;
      *(int *)(local_2c + 0x2c) = local_34;
    }
  }
  local_9c = (void *)(local_2c + 0xc);
  puVar2 = (undefined4 *)FUN_0048bce0(local_9c,0);
  local_44 = FUN_0044ced0(puVar2);
  if (local_44 != 0) {
    fVar5 = FUN_004459a0(local_44);
    fVar5 = FUN_0047a630((float)fVar5);
    *(float *)(local_2c + 0x170) = (float)fVar5;
    fVar5 = FUN_004459d0(local_44);
    fVar5 = FUN_0047a630((float)fVar5);
    *(float *)(local_2c + 0x174) = (float)fVar5;
  }
  fVar5 = FUN_004292e0((float *)(local_2c + 0x110));
  local_a0 = (float)fVar5;
  if (DAT_0056fd6c / DAT_0056c8d0 <= local_a0 + *(float *)(local_2c + 0x170) / DAT_0056c8d0) {
    fVar5 = FUN_004292e0((float *)(local_2c + 0x110));
    local_a4 = (float)fVar5;
    if (local_a4 - *(float *)(local_2c + 0x170) / DAT_0056c8d0 <= DAT_0056cda4 / DAT_0056c8d0) {
      fVar5 = FUN_00499390(local_2c + 0x110);
      local_a8 = (float)fVar5;
      if (0.0 <= local_a8 + *(float *)(local_2c + 0x174) / DAT_0056c8d0) {
        fVar5 = FUN_00499390(local_2c + 0x110);
        local_ac = (float)fVar5;
        if (local_ac - *(float *)(local_2c + 0x174) / DAT_0056c8d0 <= DAT_0056d7c0) {
          *(uint *)(local_2c + 0x2cc) = *(uint *)(local_2c + 0x2cc) | 1;
          *(uint *)(local_2c + 0x2d0) = *(uint *)(local_2c + 0x2d0) | 0x40;
          goto LAB_004a7d8b;
        }
      }
      *(uint *)(local_2c + 0x2d0) = *(uint *)(local_2c + 0x2d0) & 0xffffffbf;
      goto LAB_004a7d8b;
    }
  }
  *(uint *)(local_2c + 0x2d0) = *(uint *)(local_2c + 0x2d0) & 0xffffffbf;
LAB_004a7d8b:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

