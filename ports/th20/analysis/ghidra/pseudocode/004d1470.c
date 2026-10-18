/* Entry: 0x004d1470; symbol: FUN_004d1470; body bytes: 3704 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004d1470(int *param_1)

{
  float fVar1;
  float fVar2;
  uint uVar3;
  float *pfVar4;
  int iVar5;
  void *pvVar6;
  int iVar7;
  uint *puVar8;
  uint *puVar9;
  shared_ptr<struct__EXCEPTION_RECORD_const_> *psVar10;
  void *this;
  void *this_00;
  int *unaff_FS_OFFSET;
  float10 fVar11;
  float fVar12;
  float local_70;
  float local_6c;
  float local_68;
  float local_64;
  float local_48;
  float local_44;
  undefined1 local_40 [8];
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
  puStack_c = &LAB_00569c4d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
LAB_004d149f:
  uVar3 = FUN_00486600((int)(param_1 + 0x1cc));
  if (uVar3 <= (uint)param_1[0x1a4]) {
LAB_004d22d0:
    *unaff_FS_OFFSET = local_10;
    FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
    return;
  }
  pfVar4 = (float *)FUN_0047c6c0(param_1 + 0x1cc,param_1[0x1a4]);
  if ((pfVar4[8] == 0.0) || ((pfVar4[9] == 0.0 && (param_1[0x1a6] != 0 || param_1[0x1a7] != 0))))
  goto LAB_004d22d0;
  switch(pfVar4[8]) {
  case 0.0:
    param_1[0x1a6] = param_1[0x1a6] | 1;
    iVar5 = 0;
    pvVar6 = (void *)FUN_0047c680(param_1 + 0x24,0);
    FUN_00423520(pvVar6,iVar5);
    iVar5 = FUN_0047c680(param_1 + 0x24,0);
    *(undefined4 *)(iVar5 + 0x2c) = 0;
    break;
  case 2.8026e-45:
    param_1[0x1a6] = param_1[0x1a6] | 4;
    fVar12 = *pfVar4;
    iVar5 = FUN_0047c680(param_1 + 0x24,1);
    *(float *)(iVar5 + 0x10) = fVar12;
    fVar11 = FUN_00485740(param_1[0x1bb],(float)param_1[0x1c],(float *)(param_1 + 0x16),pfVar4[1],
                          pfVar4[2]);
    iVar5 = FUN_0047c680(param_1 + 0x24,1);
    *(float *)(iVar5 + 0x14) = (float)fVar11;
    iVar5 = 0;
    pvVar6 = (void *)FUN_0047c680(param_1 + 0x24,1);
    FUN_00423520(pvVar6,iVar5);
    fVar12 = pfVar4[4];
    iVar5 = FUN_0047c680(param_1 + 0x24,1);
    *(float *)(iVar5 + 0x30) = fVar12;
    iVar5 = FUN_0047c680(param_1 + 0x24,1);
    fVar12 = *(float *)(iVar5 + 0x10);
    iVar5 = FUN_0047c680(param_1 + 0x24,1);
    fVar1 = *(float *)(iVar5 + 0x14);
    iVar5 = FUN_0047c680(param_1 + 0x24,1);
    FUN_00439330(this,(float *)(iVar5 + 0x24),fVar1,fVar12);
    if ((param_1[0x1a4] != 0) && (-1 < param_1[0x1d1])) {
      FUN_00426d70(&DAT_005ba830,param_1[0x1d1],0);
    }
    break;
  case 4.2039e-45:
    param_1[0x1a6] = param_1[0x1a6] | 8;
    fVar12 = *pfVar4;
    iVar5 = FUN_0047c680(param_1 + 0x24,2);
    *(float *)(iVar5 + 0x10) = fVar12;
    fVar12 = pfVar4[1];
    iVar5 = FUN_0047c680(param_1 + 0x24,2);
    *(float *)(iVar5 + 0x14) = fVar12;
    iVar5 = 0;
    pvVar6 = (void *)FUN_0047c680(param_1 + 0x24,2);
    FUN_00423520(pvVar6,iVar5);
    fVar12 = pfVar4[4];
    iVar5 = FUN_0047c680(param_1 + 0x24,2);
    *(float *)(iVar5 + 0x30) = fVar12;
    if ((param_1[0x1a4] != 0) && (-1 < param_1[0x1d1])) {
      FUN_00426d70(&DAT_005ba830,param_1[0x1d1],0);
    }
    break;
  case 5.60519e-45:
    param_1[0x1a6] = param_1[0x1a6] | 0x10;
    fVar12 = *pfVar4;
    iVar5 = FUN_0047c680(param_1 + 0x24,3);
    *(float *)(iVar5 + 0x14) = fVar12;
    if (pfVar4[1] <= DAT_0056fd7c) {
      local_64 = (float)param_1[0x1f];
    }
    else {
      local_64 = pfVar4[1];
    }
    iVar5 = FUN_0047c680(param_1 + 0x24,3);
    *(float *)(iVar5 + 0x10) = local_64;
    iVar5 = 0;
    pvVar6 = (void *)FUN_0047c680(param_1 + 0x24,3);
    FUN_00423520(pvVar6,iVar5);
    fVar12 = pfVar4[4];
    iVar5 = FUN_0047c680(param_1 + 0x24,3);
    *(float *)(iVar5 + 0x30) = fVar12;
    fVar12 = pfVar4[5];
    iVar5 = FUN_0047c680(param_1 + 0x24,3);
    *(float *)(iVar5 + 0x34) = fVar12;
    iVar5 = FUN_0047c680(param_1 + 0x24,3);
    *(undefined4 *)(iVar5 + 0x38) = 0;
    fVar12 = pfVar4[6];
    iVar5 = FUN_0047c680(param_1 + 0x24,3);
    *(float *)(iVar5 + 0x3c) = fVar12;
    break;
  case 8.40779e-45:
    if (0 < (int)pfVar4[4]) {
      param_1[0x1a6] = param_1[0x1a6] | 0x40;
      if (*pfVar4 < _DAT_0056e0e8) {
        iVar5 = param_1[0x1f];
        iVar7 = FUN_0047c680(param_1 + 0x24,4);
        *(int *)(iVar7 + 0x10) = iVar5;
      }
      else {
        fVar12 = *pfVar4;
        iVar5 = FUN_0047c680(param_1 + 0x24,4);
        *(float *)(iVar5 + 0x10) = fVar12;
      }
      pfVar4[4] = (float)((int)pfVar4[4] + -1);
      fVar12 = pfVar4[4];
      iVar5 = FUN_0047c680(param_1 + 0x24,4);
      *(float *)(iVar5 + 0x34) = fVar12;
      iVar5 = FUN_0047c680(param_1 + 0x24,4);
      *(undefined4 *)(iVar5 + 0x30) = 0;
      fVar12 = pfVar4[5];
      iVar5 = FUN_0047c680(param_1 + 0x24,4);
      *(float *)(iVar5 + 0x38) = fVar12;
    }
    break;
  case 9.80909e-45:
    param_1[0x1b3] = (int)pfVar4[4];
    break;
  case 1.12104e-44:
    FUN_00423520(param_1 + 0x1af,(int)pfVar4[4]);
    param_1[0x1a4] = param_1[0x1a4] + 1;
    goto LAB_004d149f;
  case 1.26117e-44:
    iVar5 = FUN_0040c300((undefined4 *)param_1[0x1bc]);
    pvVar6 = *(void **)(iVar5 + 0x286d8c);
    iVar5 = FUN_004856c0((int)pfVar4[4]);
    FUN_00438380(pvVar6,param_1 + 0x1d3,iVar5 + (int)pfVar4[5]);
    param_1[0x1a4] = param_1[0x1a4] + 1;
    goto LAB_004d149f;
  case 1.4013e-44:
    param_1[8] = 3;
    break;
  case 1.54143e-44:
    FUN_00426eb0(&DAT_005ba830,(int)pfVar4[4],(float)param_1[0x16]);
    param_1[0x1a4] = param_1[0x1a4] + 1;
    goto LAB_004d149f;
  case 1.68156e-44:
    param_1[0x1a6] = param_1[0x1a6] | 0x1000;
    fVar12 = pfVar4[4];
    pvVar6 = (void *)FUN_0047c680(param_1 + 0x24,6);
    FUN_00423520(pvVar6,(int)fVar12);
    break;
  case 1.82169e-44:
    FUN_0040c080(local_20,8);
    FUN_0047b290(local_20,&local_15);
    local_8 = 0;
    iVar5 = FUN_0040c300((undefined4 *)local_20);
    FUN_0047bd20(&local_48);
    FUN_00439330(local_40,(float *)local_40,(float)param_1[0x1c],(float)param_1[0x1d]);
    FUN_004296e0(local_40,(float *)(param_1 + 0x16));
    local_38 = 0;
    *(undefined2 *)(iVar5 + 0x38) = *(undefined2 *)(pfVar4 + 4);
    *(float *)(iVar5 + 0x44) = pfVar4[5];
    local_24 = *(undefined2 *)(pfVar4 + 6);
    local_22 = *(undefined2 *)(pfVar4 + 7);
    if (DAT_0056fd80 < *pfVar4) {
      if (*pfVar4 < _DAT_0056fd54) {
        local_68 = *pfVar4;
      }
      else {
        iVar7 = FUN_0040ff90(param_1[0x1bc]);
        fVar11 = FUN_004ff350(iVar7,(float *)(param_1 + 0x16));
        local_68 = (float)fVar11;
      }
      local_6c = local_68;
    }
    else {
      local_6c = (float)param_1[0x1c];
    }
    local_34 = local_6c;
    local_30 = pfVar4[1];
    if (DAT_0056fd80 < pfVar4[2]) {
      local_70 = pfVar4[2];
    }
    else {
      local_70 = (float)param_1[0x1f];
    }
    local_2c = local_70;
    local_28 = pfVar4[3];
    param_1[0x1a4] = param_1[0x1a4] + 1;
    local_48 = pfVar4[0xf];
    local_44 = pfVar4[0x10];
    fVar12 = pfVar4[0x11];
    iVar7 = FUN_00486600(iVar5 + 4);
    puVar8 = (uint *)FUN_00414f30((int)(param_1 + 0x1cc));
    puVar9 = (uint *)FUN_00414f30(iVar5 + 4);
    FUN_00543e20(puVar9,puVar8,iVar7 * 0x2c);
    pvVar6 = (void *)FUN_0040c300((undefined4 *)param_1[0x1bc]);
    psVar10 = (shared_ptr<struct__EXCEPTION_RECORD_const_> *)move<>(local_20);
    std::shared_ptr<struct__EXCEPTION_RECORD_const_>::shared_ptr<struct__EXCEPTION_RECORD_const_>
              ((shared_ptr<struct__EXCEPTION_RECORD_const_> *)&stack0xfffffe68,psVar10);
    FUN_00481780(pvVar6,(int *)&local_48);
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
    goto LAB_004d149f;
  case 2.10195e-44:
    param_1[0x23] = (int)pfVar4[4];
    param_1[0x1a4] = param_1[0x1a4] + 1;
    goto LAB_004d149f;
  case 2.24208e-44:
    if ((int)pfVar4[5] < 1) {
      param_1[0x1a4] = (int)pfVar4[4];
      goto LAB_004d149f;
    }
    if (param_1[0x1aa] == 0) {
      param_1[0x1aa] = (int)pfVar4[5];
      param_1[0x1a4] = (int)pfVar4[4];
      goto LAB_004d149f;
    }
    if (param_1[0x1aa] != 1) {
      param_1[0x1aa] = param_1[0x1aa] + -1;
      param_1[0x1a4] = (int)pfVar4[4];
      goto LAB_004d149f;
    }
    param_1[0x1aa] = 0;
    break;
  case 2.8026e-44:
    if (pfVar4[4] == 0.0) {
      FUN_0045e590((int)(param_1 + 0x1d3));
    }
    else {
      FUN_0045e5b0((int)(param_1 + 0x1d3));
    }
    param_1[0x1a4] = param_1[0x1a4] + 1;
    goto LAB_004d149f;
  case 2.94273e-44:
    param_1[0x1a6] = param_1[0x1a6] | 0x200000;
    fVar12 = *pfVar4;
    fVar1 = (float)param_1[0x1f];
    fVar2 = pfVar4[4];
    iVar5 = FUN_0047c680(param_1 + 0x24,10);
    *(float *)(iVar5 + 0x10) = (fVar12 - fVar1) / (float)(int)fVar2;
    fVar11 = FUN_00485740(param_1[0x1bb],(float)param_1[0x1c],(float *)(param_1 + 0x16),pfVar4[1],
                          pfVar4[2]);
    iVar5 = FUN_0047c680(param_1 + 0x24,10);
    *(float *)(iVar5 + 0x14) = (float)fVar11;
    iVar5 = 0;
    pvVar6 = (void *)FUN_0047c680(param_1 + 0x24,10);
    FUN_00423520(pvVar6,iVar5);
    fVar12 = pfVar4[4];
    iVar5 = FUN_0047c680(param_1 + 0x24,10);
    *(float *)(iVar5 + 0x30) = fVar12;
    iVar5 = FUN_0047c680(param_1 + 0x24,10);
    fVar12 = *(float *)(iVar5 + 0x10);
    iVar5 = FUN_0047c680(param_1 + 0x24,10);
    fVar1 = *(float *)(iVar5 + 0x14);
    iVar5 = FUN_0047c680(param_1 + 0x24,10);
    FUN_00439330(this_00,(float *)(iVar5 + 0x24),fVar1,fVar12);
    if ((param_1[0x1a4] != 0) && (-1 < param_1[0x1d1])) {
      FUN_00426d70(&DAT_005ba830,param_1[0x1d1],0);
    }
    break;
  case 4.2039e-44:
    if (0 < (int)pfVar4[4]) {
      param_1[0x1a6] = param_1[0x1a6] | 0x40000000;
      fVar12 = pfVar4[4];
      pvVar6 = (void *)FUN_0047c680(param_1 + 0x24,5);
      FUN_00423520(pvVar6,(int)fVar12);
      break;
    }
    param_1[0x1a4] = param_1[0x1a4] + 1;
    goto LAB_004d149f;
  case 4.62428e-44:
    param_1[0x1a7] = param_1[0x1a7] & 0xfffffffdU | ((uint)pfVar4[4] & 1) << 1;
  }
  param_1[0x1a4] = param_1[0x1a4] + 1;
  goto LAB_004d149f;
}

