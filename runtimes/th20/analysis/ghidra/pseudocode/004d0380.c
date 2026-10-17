/* Entry: 0x004d0380; symbol: FUN_004d0380; body bytes: 3817 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004d0380(int *param_1)

{
  uint uVar1;
  float *pfVar2;
  default_delete<class_std::_Facet_base> *pdVar3;
  int iVar4;
  uint *puVar5;
  uint *puVar6;
  shared_ptr<struct__EXCEPTION_RECORD_const_> *psVar7;
  int iVar8;
  void *pvVar9;
  void *this;
  void *this_00;
  int *unaff_FS_OFFSET;
  float10 fVar10;
  longlong lVar11;
  float fVar12;
  float local_78;
  float local_74;
  float local_70;
  float local_6c;
  float local_48;
  float local_44;
  float local_40 [2];
  undefined4 local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  undefined2 local_24;
  undefined2 local_22;
  _Ptr_base<struct__EXCEPTION_RECORD_const_> local_20 [11];
  undefined1 local_15;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00569c0d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
LAB_004d03af:
  uVar1 = FUN_00486600((int)(param_1 + 0x1c9));
  if ((uVar1 <= (uint)param_1[0x1a4]) || (0x17 < param_1[0x1a4])) {
LAB_004d124f:
    *unaff_FS_OFFSET = local_10;
    FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
    return;
  }
  pfVar2 = (float *)FUN_0047c6c0(param_1 + 0x1c9,param_1[0x1a4]);
  fVar12 = pfVar2[8];
  if ((fVar12 == 0.0) ||
     (((pfVar2[9] == 0.0 && (param_1[0x1a6] != 0 || param_1[0x1a7] != 0)) ||
      (lVar11 = __allshl(SUB41(pfVar2[8],0),0),
      ((uint)lVar11 & param_1[0x1a6]) != 0 ||
      ((uint)((ulonglong)lVar11 >> 0x20) & param_1[0x1a7]) != 0)))) goto LAB_004d124f;
  switch(fVar12) {
  case 0.0:
    param_1[0x1a6] = param_1[0x1a6] | 1;
    iVar8 = 0;
    pvVar9 = (void *)FUN_0047c680(param_1 + 0x24,0);
    FUN_00423520(pvVar9,iVar8);
    iVar8 = FUN_0047c680(param_1 + 0x24,0);
    *(undefined4 *)(iVar8 + 0x2c) = 0;
    break;
  case 2.8026e-45:
    iVar8 = FUN_004d5a90(param_1,(int)(float)(int)pfVar2[5]);
    *(undefined4 *)(iVar8 + 0x10) = 1;
    pvVar9 = *(void **)(iVar8 + 4);
    fVar12 = *(float *)(*(int *)(iVar8 + 4) + 0xc);
    pdVar3 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(iVar8 + 0x2c));
    FUN_004d6390(pvVar9,(undefined4 *)(iVar8 + 0x20),(float *)(iVar8 + 0x30),(float *)pdVar3,fVar12)
    ;
    *(undefined4 *)(iVar8 + 0x28) = 0;
    fVar12 = DAT_0056c8cc;
    fVar10 = FUN_004292e0((float *)(iVar8 + 0x2c));
    FUN_00439330(this,(float *)(iVar8 + 0x14),(float)fVar10,fVar12);
    *(undefined4 *)(iVar8 + 0x1c) = 0;
    *(float *)(iVar8 + 0x34) = *pfVar2;
    *(float *)(iVar8 + 0x38) = pfVar2[1];
    if ((int)pfVar2[4] < 0) {
      *(undefined4 *)(iVar8 + 0xc) = DAT_00571790;
    }
    else {
      *(float *)(iVar8 + 0xc) = (float)(int)pfVar2[5] + (float)(int)pfVar2[4];
      iVar8 = FUN_004d5a90(param_1,*(int *)(iVar8 + 0xc));
      *(undefined4 *)(iVar8 + 0x10) = 0;
      pvVar9 = *(void **)(iVar8 + 4);
      fVar12 = *(float *)(*(int *)(iVar8 + 4) + 0xc);
      pdVar3 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(iVar8 + 0x2c));
      FUN_004d6390(pvVar9,(undefined4 *)(iVar8 + 0x20),(float *)(iVar8 + 0x30),(float *)pdVar3,
                   fVar12);
      *(undefined4 *)(iVar8 + 0x28) = 0;
      fVar12 = DAT_0056c8cc;
      fVar10 = FUN_004292e0((float *)(iVar8 + 0x2c));
      FUN_00439330((void *)(iVar8 + 0x14),(float *)(iVar8 + 0x14),(float)fVar10,fVar12);
      *(undefined4 *)(iVar8 + 0x1c) = 0;
      *(undefined4 *)(iVar8 + 0xc) = DAT_00571790;
    }
    param_1[0x1a4] = param_1[0x1a4] + 1;
    goto LAB_004d03af;
  case 4.2039e-45:
    iVar8 = FUN_004d5a90(param_1,(int)(float)(int)pfVar2[5]);
    *(undefined4 *)(iVar8 + 0x10) = 2;
    pvVar9 = *(void **)(iVar8 + 4);
    fVar12 = *(float *)(*(int *)(iVar8 + 4) + 0xc);
    pdVar3 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(iVar8 + 0x2c));
    FUN_004d6390(pvVar9,(undefined4 *)(iVar8 + 0x20),(float *)(iVar8 + 0x30),(float *)pdVar3,fVar12)
    ;
    *(undefined4 *)(iVar8 + 0x28) = 0;
    fVar12 = DAT_0056c8cc;
    fVar10 = FUN_004292e0((float *)(iVar8 + 0x2c));
    FUN_00439330(this_00,(float *)(iVar8 + 0x14),(float)fVar10,fVar12);
    *(undefined4 *)(iVar8 + 0x1c) = 0;
    *(float *)(iVar8 + 0x34) = *pfVar2;
    *(float *)(iVar8 + 0x38) = pfVar2[1];
    if ((int)pfVar2[4] < 0) {
      *(undefined4 *)(iVar8 + 0xc) = DAT_00571790;
    }
    else {
      *(float *)(iVar8 + 0xc) = (float)(int)pfVar2[5] + (float)(int)pfVar2[4];
      iVar8 = FUN_004d5a90(param_1,*(int *)(iVar8 + 0xc));
      *(undefined4 *)(iVar8 + 0x10) = 0;
      pvVar9 = *(void **)(iVar8 + 4);
      fVar12 = *(float *)(*(int *)(iVar8 + 4) + 0xc);
      pdVar3 = std::
               _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
               ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                             *)(iVar8 + 0x2c));
      FUN_004d6390(pvVar9,(undefined4 *)(iVar8 + 0x20),(float *)(iVar8 + 0x30),(float *)pdVar3,
                   fVar12);
      *(undefined4 *)(iVar8 + 0x28) = 0;
      fVar12 = DAT_0056c8cc;
      fVar10 = FUN_004292e0((float *)(iVar8 + 0x2c));
      FUN_00439330((void *)(iVar8 + 0x14),(float *)(iVar8 + 0x14),(float)fVar10,fVar12);
      *(undefined4 *)(iVar8 + 0x1c) = 0;
      *(undefined4 *)(iVar8 + 0xc) = DAT_00571790;
    }
    break;
  case 5.60519e-45:
    param_1[0x1a6] = param_1[0x1a6] | 0x10;
    fVar12 = *pfVar2;
    iVar8 = FUN_0047c680(param_1 + 0x24,3);
    *(float *)(iVar8 + 0x14) = fVar12;
    if (pfVar2[1] <= DAT_0056fd7c) {
      local_6c = (float)param_1[0x1f];
    }
    else {
      local_6c = pfVar2[1];
    }
    iVar8 = FUN_0047c680(param_1 + 0x24,3);
    *(float *)(iVar8 + 0x10) = local_6c;
    iVar8 = 0;
    pvVar9 = (void *)FUN_0047c680(param_1 + 0x24,3);
    FUN_00423520(pvVar9,iVar8);
    fVar12 = pfVar2[4];
    iVar8 = FUN_0047c680(param_1 + 0x24,3);
    *(float *)(iVar8 + 0x30) = fVar12;
    fVar12 = pfVar2[5];
    iVar8 = FUN_0047c680(param_1 + 0x24,3);
    *(float *)(iVar8 + 0x34) = fVar12;
    iVar8 = FUN_0047c680(param_1 + 0x24,3);
    *(undefined4 *)(iVar8 + 0x38) = 0;
    fVar12 = pfVar2[6];
    iVar8 = FUN_0047c680(param_1 + 0x24,3);
    *(float *)(iVar8 + 0x3c) = fVar12;
    break;
  case 8.40779e-45:
    if (0 < (int)pfVar2[4]) {
      param_1[0x1a6] = param_1[0x1a6] | 0x40;
      if (*pfVar2 < _DAT_0056e0e8) {
        iVar8 = param_1[0x1f];
        iVar4 = FUN_0047c680(param_1 + 0x24,4);
        *(int *)(iVar4 + 0x10) = iVar8;
      }
      else {
        fVar12 = *pfVar2;
        iVar8 = FUN_0047c680(param_1 + 0x24,4);
        *(float *)(iVar8 + 0x10) = fVar12;
      }
      pfVar2[4] = (float)((int)pfVar2[4] + -1);
      fVar12 = pfVar2[4];
      iVar8 = FUN_0047c680(param_1 + 0x24,4);
      *(float *)(iVar8 + 0x34) = fVar12;
      iVar8 = FUN_0047c680(param_1 + 0x24,4);
      *(undefined4 *)(iVar8 + 0x30) = 0;
      fVar12 = pfVar2[5];
      iVar8 = FUN_0047c680(param_1 + 0x24,4);
      *(float *)(iVar8 + 0x38) = fVar12;
    }
    break;
  case 9.80909e-45:
    param_1[0x1b3] = (int)pfVar2[4];
    break;
  case 1.12104e-44:
    param_1[0x1a6] = param_1[0x1a6] | 0x100;
    fVar12 = pfVar2[4];
    pvVar9 = (void *)FUN_0047c680(param_1 + 0x24,0xb);
    FUN_00423520(pvVar9,(int)fVar12);
    fVar12 = pfVar2[5];
    iVar8 = FUN_0047c680(param_1 + 0x24,0xb);
    *(float *)(iVar8 + 0x30) = fVar12;
    param_1[0x1a4] = param_1[0x1a4] + 1;
    goto LAB_004d03af;
  case 1.26117e-44:
    iVar8 = FUN_0040c300((undefined4 *)param_1[0x1bc]);
    pvVar9 = *(void **)(iVar8 + 0x286d8c);
    iVar8 = FUN_004856c0((int)pfVar2[4]);
    FUN_00438380(pvVar9,param_1 + 0x1d5,iVar8 + (int)pfVar2[5]);
    param_1[0x1a4] = param_1[0x1a4] + 1;
    goto LAB_004d03af;
  case 1.4013e-44:
    param_1[8] = 3;
    break;
  case 1.54143e-44:
    FUN_00426eb0(&DAT_005ba830,(int)pfVar2[4],(float)param_1[0x16]);
    param_1[0x1a4] = param_1[0x1a4] + 1;
    goto LAB_004d03af;
  case 1.68156e-44:
    param_1[0x1a6] = param_1[0x1a6] | 0x1000;
    fVar12 = pfVar2[4];
    pvVar9 = (void *)FUN_0047c680(param_1 + 0x24,6);
    FUN_00423520(pvVar9,(int)fVar12);
    break;
  case 1.82169e-44:
    FUN_0040c080(local_20,8);
    FUN_0047b290(local_20,&local_15);
    local_8 = 0;
    iVar8 = FUN_0040c300((undefined4 *)local_20);
    FUN_0047bd20(&local_48);
    FUN_00439330(param_1,local_40,(float)param_1[0x1c],(float)param_1[0x1d]);
    FUN_004296e0(local_40,(float *)(param_1 + 0x16));
    local_38 = 0;
    *(undefined2 *)(iVar8 + 0x38) = *(undefined2 *)(pfVar2 + 4);
    *(float *)(iVar8 + 0x44) = pfVar2[5];
    local_24 = *(undefined2 *)(pfVar2 + 6);
    local_22 = *(undefined2 *)(pfVar2 + 7);
    if (DAT_0056fd80 < *pfVar2) {
      if (*pfVar2 < _DAT_0056fd54) {
        local_70 = *pfVar2;
      }
      else {
        iVar4 = FUN_0040ff90(param_1[0x1bc]);
        fVar10 = FUN_004ff350(iVar4,(float *)(param_1 + 0x16));
        local_70 = (float)fVar10;
      }
      local_74 = local_70;
    }
    else {
      local_74 = (float)param_1[0x1c];
    }
    local_34 = local_74;
    local_30 = pfVar2[1];
    if (DAT_0056fd80 < pfVar2[2]) {
      local_78 = pfVar2[2];
    }
    else {
      local_78 = (float)param_1[0x1f];
    }
    local_2c = local_78;
    local_28 = pfVar2[3];
    param_1[0x1a4] = param_1[0x1a4] + 1;
    local_48 = pfVar2[0xf];
    local_44 = pfVar2[0x10];
    fVar12 = pfVar2[0x11];
    puVar5 = (uint *)FUN_00414f30((int)(param_1 + 0x1c9));
    puVar6 = (uint *)FUN_00414f30(iVar8 + 4);
    FUN_00543e20(puVar6,puVar5,0x420);
    pvVar9 = (void *)FUN_0040c300((undefined4 *)param_1[0x1bc]);
    psVar7 = (shared_ptr<struct__EXCEPTION_RECORD_const_> *)move<>(local_20);
    std::shared_ptr<struct__EXCEPTION_RECORD_const_>::shared_ptr<struct__EXCEPTION_RECORD_const_>
              ((shared_ptr<struct__EXCEPTION_RECORD_const_> *)&stack0xfffffe7c,psVar7);
    FUN_00481780(pvVar9,(int *)&local_48);
    param_1[0x1a4] = param_1[0x1a4] + 1;
    if (fVar12 == 0.0) {
      local_8 = 0xffffffff;
      FUN_0047c300((int)local_20);
    }
    else {
      (**(code **)(*param_1 + 0x44))();
      local_8 = 0xffffffff;
      FUN_0047c300((int)local_20);
    }
    goto LAB_004d03af;
  case 2.10195e-44:
    param_1[0x23] = (int)pfVar2[4];
    param_1[0x1a4] = param_1[0x1a4] + 1;
    goto LAB_004d03af;
  case 2.24208e-44:
    param_1[0x1a4] = (int)pfVar2[4];
    goto LAB_004d03af;
  case 2.8026e-44:
    if (pfVar2[4] == 0.0) {
      FUN_0045e590((int)(param_1 + 0x1d5));
    }
    else {
      FUN_0045e5b0((int)(param_1 + 0x1d5));
    }
    param_1[0x1a4] = param_1[0x1a4] + 1;
    goto LAB_004d03af;
  case 3.92364e-44:
    param_1[7] = param_1[7] & 0xffffffefU | ((uint)pfVar2[4] & 1) << 4;
    break;
  case 4.2039e-44:
    if (0 < (int)pfVar2[4]) {
      param_1[0x1a6] = param_1[0x1a6] | 0x40000000;
      fVar12 = pfVar2[4];
      pvVar9 = (void *)FUN_0047c680(param_1 + 0x24,5);
      FUN_00423520(pvVar9,(int)fVar12);
      break;
    }
    param_1[0x1a4] = param_1[0x1a4] + 1;
    goto LAB_004d03af;
  case 4.34403e-44:
    param_1[0x4c9] = 1;
    param_1[0x1a6] = param_1[0x1a6] | 0x80000000;
    fVar12 = *pfVar2;
    iVar8 = FUN_0047c680(param_1 + 0x24,7);
    *(float *)(iVar8 + 0x10) = fVar12;
    fVar12 = pfVar2[1];
    iVar8 = FUN_0047c680(param_1 + 0x24,7);
    *(float *)(iVar8 + 0x14) = fVar12;
    fVar12 = pfVar2[2];
    iVar8 = FUN_0047c680(param_1 + 0x24,7);
    *(float *)(iVar8 + 0x24) = fVar12;
    iVar8 = 0;
    pvVar9 = (void *)FUN_0047c680(param_1 + 0x24,7);
    FUN_00423520(pvVar9,iVar8);
    fVar12 = pfVar2[4];
    iVar8 = FUN_0047c680(param_1 + 0x24,7);
    *(float *)(iVar8 + 0x30) = fVar12;
    break;
  case 4.62428e-44:
    param_1[0x1a7] = param_1[0x1a7] & 0xfffffffdU | ((uint)pfVar2[4] & 1) << 1;
  }
  param_1[0x1a4] = param_1[0x1a4] + 1;
  goto LAB_004d03af;
}

