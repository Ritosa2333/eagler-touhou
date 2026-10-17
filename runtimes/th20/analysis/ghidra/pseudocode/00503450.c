/* Entry: 0x00503450; symbol: FUN_00503450; body bytes: 1652 */

void __thiscall FUN_00503450(void *this,float *param_1,float *param_2,float param_3,float param_4)

{
  undefined4 *puVar1;
  float *pfVar2;
  default_delete<class_std::_Facet_base> *pdVar3;
  int iVar4;
  void *this_00;
  float10 fVar5;
  float fVar6;
  char **in_stack_ffffff10;
  undefined4 local_e4;
  undefined4 local_e0;
  default_delete<class_std::_Facet_base> *local_dc;
  float local_d8;
  float local_d4;
  float local_d0;
  void *local_cc;
  void *local_c8;
  void *local_c4;
  uint local_c0;
  void *local_bc;
  undefined4 local_b8;
  undefined4 local_b4;
  int local_b0;
  void *local_ac;
  float local_a8;
  float local_a4;
  void *local_a0;
  float local_9c;
  float local_98;
  float local_94;
  float local_90;
  float local_8c;
  float local_88;
  default_delete<class_std::_Facet_base> *local_84;
  float local_80;
  float local_7c;
  float local_78;
  float local_74;
  float local_70;
  float local_6c;
  float local_68;
  undefined4 local_64;
  int local_60;
  void *local_5c;
  void *local_58;
  void *local_54;
  float local_50 [3];
  float local_44;
  float local_40 [2];
  undefined4 local_38;
  float local_34 [2];
  undefined4 local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c [3];
  float local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_54 = this;
  local_c8 = (void *)FUN_0040ff90(*(int *)((int)this + 0x120));
  local_c0 = *(uint *)((int)local_54 + 0xd8);
  local_b0 = FUN_00506480(local_c8,local_c0);
  *(undefined4 *)((int)local_54 + 0xa4) = 1;
  if (*(int *)((int)local_54 + 0xa8) == 0) {
    puVar1 = FUN_005064b0(local_54,&local_e4);
    FUN_00486310(puVar1);
    *(undefined4 *)((int)local_54 + 0xa8) = 1;
  }
  if (param_2 == (float *)0x0) {
    local_5c = (void *)FUN_004c0e10((undefined4 *)((int)local_54 + 0xdc));
    if (local_5c == (void *)0x0) goto LAB_00503ab4;
    fVar5 = FUN_004c4b70((int)local_5c);
    local_d0 = (float)fVar5;
    local_d4 = local_d0 / DAT_0056c8d0 + param_4;
    pfVar2 = (float *)FUN_00421830((int)local_54 + 0x50);
    fVar5 = FUN_004292e0(pfVar2);
    local_d8 = (float)fVar5;
    local_dc = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)((int)local_54 + 0x50));
    FUN_004aaa80(&local_10,&local_44,(float *)local_dc,local_d8,param_1,local_d4);
    *(float *)((int)local_54 + 0xb0) = local_10 + DAT_0056d7bc;
    if (*(float *)((int)local_54 + 0xb0) <= 0.0 && *(float *)((int)local_54 + 0xb0) != 0.0) {
      *(undefined4 *)((int)local_54 + 0xb0) = 0;
    }
    FUN_00470b20(local_5c,*(undefined4 *)((int)local_54 + 0xb0));
  }
  else {
    FUN_00422e10(local_1c);
    FUN_00422e10(local_50);
    local_58 = (void *)FUN_004c0e10((undefined4 *)((int)local_54 + 0xdc));
    if (local_58 == (void *)0x0) goto LAB_00503ab4;
    fVar5 = FUN_004c4b70((int)local_58);
    local_68 = (float)fVar5;
    local_70 = local_68 + param_2[1];
    fVar5 = FUN_004c4b70((int)local_58);
    local_6c = (float)fVar5;
    local_74 = local_6c + *param_2;
    local_78 = param_1[1];
    local_7c = *param_1;
    pfVar2 = (float *)FUN_00421830((int)local_54 + 0x50);
    fVar5 = FUN_004292e0(pfVar2);
    local_80 = (float)fVar5;
    local_84 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)((int)local_54 + 0x50));
    iVar4 = FUN_004547e0(local_1c,local_50,(float *)local_84,local_80,local_7c,local_78,local_74,
                         local_70,param_3);
    if (iVar4 == 0) {
      local_28 = *param_1;
      local_24 = param_1[1];
      local_20 = param_1[2];
      pdVar3 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)((int)local_54 + 0x50));
      FUN_00429740(&local_28,(float *)pdVar3);
      fVar5 = (float10)FUN_0047a2a0(&local_28);
      local_9c = (float)fVar5;
      *(float *)((int)local_54 + 0xb0) = local_9c - DAT_0056f7b4;
      if (*(float *)((int)local_54 + 0xb0) <= 0.0 && *(float *)((int)local_54 + 0xb0) != 0.0) {
        *(undefined4 *)((int)local_54 + 0xb0) = 0;
      }
      FUN_00470b20(local_58,*(float *)((int)local_54 + 0xb0) + DAT_0056cd90);
    }
    else {
      pfVar2 = (float *)FUN_00421830((int)local_54 + 0x50);
      fVar5 = FUN_004292e0(pfVar2);
      local_88 = (float)fVar5;
      pfVar2 = local_1c;
      pdVar3 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)((int)local_54 + 0x50));
      fVar5 = (float10)FUN_0047a2e0((float *)pdVar3,pfVar2);
      local_8c = (float)fVar5;
      local_90 = DAT_0056e0f0 / DAT_0056c8d0;
      fVar5 = FUN_004396c0(local_8c,local_88);
      fVar5 = FUN_0047a630((float)fVar5);
      local_94 = (float)fVar5;
      if (local_90 <= local_94) {
        *(float *)((int)local_54 + 0xb0) = DAT_0056d7bc + 0.0;
      }
      else {
        pdVar3 = std::
                 _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                 ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                               *)((int)local_54 + 0x50));
        FUN_00429740(local_1c,(float *)pdVar3);
        fVar5 = (float10)FUN_0047a2a0(local_1c);
        local_98 = (float)fVar5;
        *(float *)((int)local_54 + 0xb0) = local_98 + DAT_0056d7bc;
      }
      if (*(float *)((int)local_54 + 0xb0) <= 0.0 && *(float *)((int)local_54 + 0xb0) != 0.0) {
        *(undefined4 *)((int)local_54 + 0xb0) = 0;
      }
      FUN_00470b20(local_58,*(undefined4 *)((int)local_54 + 0xb0));
    }
  }
  local_a0 = (void *)FUN_00421830((int)local_54);
  iVar4 = FUN_00478e00(local_a0,2);
  if (iVar4 != 0) {
    FUN_00422e10(local_34);
    FUN_00422e10(local_40);
    local_a4 = *(float *)((int)local_54 + 0xb0);
    pfVar2 = (float *)FUN_00421830((int)local_54 + 0x50);
    fVar5 = FUN_004292e0(pfVar2);
    local_a8 = (float)fVar5;
    FUN_00439330(local_34,local_34,local_a8,local_a4);
    local_ac = (void *)((int)local_54 + 0x50);
    FUN_0048bd70(local_ac,(undefined4 *)&stack0xffffff10);
    FUN_004296e0(local_34,(float *)&stack0xffffff10);
    fVar6 = DAT_0056fa28;
    pfVar2 = (float *)FUN_00421830((int)local_54 + 0x50);
    fVar5 = FUN_004292e0(pfVar2);
    FUN_00439330(this_00,local_40,(float)fVar5,fVar6);
    local_2c = 0;
    local_38 = 0;
    FUN_00425cc0(&local_c);
    local_b4 = *(undefined4 *)((int)local_54 + 0x10c);
    puVar1 = FUN_004790e0(&local_e0,(uint *)0x0,(int)*(short *)(local_b0 + 0x2e),local_34,0,-1,
                          (undefined4 *)0x0);
    local_c = *puVar1;
    local_bc = (void *)FUN_0041cad0(*(int *)((int)local_54 + 0x120));
    local_b8 = local_c;
    FUN_00497d20(local_bc,local_c);
    pfVar2 = (float *)FUN_00421830((int)local_54 + 0x50);
    fVar5 = FUN_004292e0(pfVar2);
    FUN_00450560(&local_c,(float)fVar5);
    local_cc = (void *)FUN_0044ced0(&local_c);
    FUN_004614a0(local_cc,0x14,4,(undefined4 *)&DAT_00573278,local_40);
  }
  local_c4 = (void *)FUN_00421830((int)local_54);
  iVar4 = FUN_00478e00(local_c4,4);
  if (iVar4 != 0) {
    std::_Adl_verify_range<char*,char_const*>((char **)0x64,in_stack_ffffff10);
    local_60 = FUN_004c0e10((undefined4 *)((int)local_54 + 0xdc));
    if (local_60 == 0) {
      local_64 = 0;
    }
    else {
      local_64 = FUN_00506120(local_60);
    }
  }
LAB_00503ab4:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

