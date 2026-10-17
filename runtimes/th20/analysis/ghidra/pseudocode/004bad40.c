/* Entry: 0x004bad40; symbol: FUN_004bad40; body bytes: 4049 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004bad40(int *param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  void *pvVar2;
  int iVar3;
  undefined4 uVar4;
  void *this;
  int iVar5;
  uint uVar6;
  undefined4 *puVar7;
  int *piVar8;
  int iVar9;
  int *piVar10;
  undefined8 uVar11;
  undefined8 uVar12;
  uint local_168;
  uint local_164;
  uint local_160;
  uint local_15c;
  char *local_c;
  
  FUN_00451520(0);
  param_1[0xc] = 0;
  FUN_004bd920((int)param_1);
  param_1[0xc] = param_1[0xc] + 1;
  param_1[0x3a] = param_1[0x3a] | 4;
  while (-1 < *(int *)(DAT_005c0028 + 0x18)) {
    if ((DAT_005c5888 >> 5 & 3) != 0) goto LAB_004bbd18;
    Sleep(1);
  }
  bVar1 = FUN_004bd730();
  if (CONCAT31(extraout_var,bVar1) != 0) {
    Sleep(0x3c);
  }
  FUN_004388f0(DAT_005c4eec,2);
  FUN_0042b5d0(DAT_005c4eec);
  FUN_004292a0(&DAT_005aefe4,DAT_0056c8cc);
  *(uint *)(DAT_005ba828 + 0xe8) = *(uint *)(DAT_005ba828 + 0xe8) & 0xffffff7f;
  *(uint *)(DAT_005ba828 + 0xe8) = *(uint *)(DAT_005ba828 + 0xe8) & 0xffffffbf;
  FUN_004bd9b0(param_1,0);
  pvVar2 = (void *)FUN_00498f40();
  FUN_004bdb80(pvVar2,0);
  pvVar2 = (void *)FUN_00498f40();
  FUN_004b8830(pvVar2,0);
  pvVar2 = (void *)FUN_00464080(0);
  iVar3 = FUN_00464080(0);
  FUN_00412730(iVar3);
  uVar4 = FUN_00464100((int)DAT_005c6108);
  FUN_0041de70(pvVar2,uVar4);
  pvVar2 = (void *)FUN_00464080(0);
  iVar3 = FUN_00464080(0);
  FUN_00412730(iVar3);
  uVar4 = FUN_00464100((int)DAT_005c6108);
  FUN_0041df50(pvVar2,uVar4);
  pvVar2 = (void *)FUN_00464080(0);
  iVar3 = FUN_00464080(0);
  FUN_00412730(iVar3);
  uVar4 = FUN_00464100((int)DAT_005c6108);
  FUN_0041df30(pvVar2,uVar4);
  pvVar2 = (void *)FUN_00464080(0);
  iVar3 = FUN_00464080(0);
  FUN_00412730(iVar3);
  uVar4 = FUN_00464100((int)DAT_005c6108);
  FUN_00412d10(pvVar2,uVar4);
  pvVar2 = (void *)FUN_00464080(0);
  iVar3 = FUN_00464080(0);
  FUN_00412730(iVar3);
  uVar4 = FUN_00464100((int)DAT_005c6108);
  FUN_00412dc0(pvVar2,uVar4);
  pvVar2 = (void *)FUN_00464080(0);
  iVar3 = FUN_00464080(0);
  FUN_00412730(iVar3);
  uVar4 = FUN_00464100((int)DAT_005c6108);
  FUN_0041de50(pvVar2,uVar4);
  pvVar2 = (void *)FUN_00464080(0);
  iVar3 = FUN_00464080(0);
  FUN_00412730(iVar3);
  uVar4 = FUN_00464100((int)DAT_005c6108);
  FUN_00412da0(pvVar2,uVar4);
  pvVar2 = (void *)FUN_00464080(0);
  iVar3 = FUN_00464080(0);
  FUN_00412730(iVar3);
  uVar4 = FUN_00464100((int)DAT_005c6108);
  FUN_0041de90(pvVar2,uVar4);
  pvVar2 = (void *)FUN_00464230(0);
  iVar3 = FUN_00464080(0);
  iVar3 = FUN_00412730(iVar3);
  FUN_00534080(pvVar2,iVar3);
  pvVar2 = (void *)FUN_0040bbc0(&DAT_005ba568,0);
  this = (void *)FUN_00498f40();
  iVar3 = FUN_00488720(this,0);
  FUN_0041df50(pvVar2,iVar3);
  iVar3 = FUN_0049c000();
  if (iVar3 == 0) {
    uVar11 = FUN_004b8290(0x5ba568);
    uVar12 = FUN_004b8030(0x5ba568);
    local_15c = (uint)((ulonglong)uVar11 >> 0x20);
    local_164 = (uint)((ulonglong)uVar12 >> 0x20);
    if (local_164 <= local_15c) {
      if (local_15c <= local_164) {
        local_160 = (uint)uVar11;
        local_168 = (uint)uVar12;
        if (local_160 <= local_168) goto LAB_004bb871;
      }
      uVar11 = FUN_004b8290(0x5ba568);
      FUN_004b8960(&DAT_005ba568,(int)uVar11,(int)((ulonglong)uVar11 >> 0x20));
    }
  }
  else {
    iVar3 = FUN_004640a0(0x5ba568);
    _DAT_005ba5e0 = *(undefined4 *)(&DAT_005afd08 + iVar3 * 4);
    FUN_004be3c0(&DAT_005ba568,*DAT_005c6110);
    iVar3 = FUN_00498f40();
    FUN_004bbd80(iVar3);
    iVar3 = FUN_00474d60(0x5ba568);
    if ((iVar3 == 7) && (iVar3 = FUN_004640a0(0x5ba568), iVar3 < 4)) {
      FUN_004be1b0(&DAT_005ba568,4);
    }
    bVar1 = FUN_00488850(0x5ba568);
    if (bVar1) {
      FUN_004b7f70(0x5ba568);
      iVar3 = FUN_00464100((int)DAT_005c6108);
      iVar5 = FUN_004b7f70(0x5ba568);
      iVar3 = FUN_004bd460(DAT_005c6108,iVar5,iVar3);
      iVar5 = FUN_00499460(0x5ba568);
      iVar3 = FUN_00486e10((void *)(iVar3 + 0xb08),iVar5);
      FUN_004b8960(&DAT_005ba568,*(undefined4 *)(iVar3 + 0xd8),*(undefined4 *)(iVar3 + 0xdc));
      FUN_004b8940(&DAT_005ba568,0);
    }
    else {
      bVar1 = FUN_004bd820(0x5ba568);
      if (bVar1) {
        FUN_004b7f70(0x5ba568);
        iVar3 = FUN_00464100((int)DAT_005c6108);
        iVar5 = FUN_004b7f70(0x5ba568);
        iVar3 = FUN_004bd460(DAT_005c6108,iVar5,iVar3);
        iVar5 = FUN_004640a0(0x5ba568);
        iVar9 = FUN_00474d60(0x5ba568);
        puVar7 = (undefined4 *)(iVar3 + 0x76f8 + iVar5 * 0x90 + (iVar9 + -1) * 0x10);
        FUN_004b8960(&DAT_005ba568,*puVar7,puVar7[1]);
        FUN_004b8940(&DAT_005ba568,0);
      }
      else {
        FUN_004b7f70(0x5ba568);
        iVar3 = FUN_00464100((int)DAT_005c6108);
        iVar5 = FUN_004b7f70(0x5ba568);
        iVar3 = FUN_004bd460(DAT_005c6108,iVar5,iVar3);
        iVar5 = FUN_004640a0(0x5ba568);
        puVar7 = (undefined4 *)(iVar3 + 0x18 + iVar5 * 400);
        FUN_004b8960(&DAT_005ba568,*puVar7,puVar7[1]);
        iVar3 = FUN_0050fc50(DAT_005c6108);
        iVar5 = FUN_004640a0(0x5ba568);
        FUN_004b8940(&DAT_005ba568,(int)*(char *)(iVar3 + iVar5 * 400 + 0x21));
      }
    }
    uVar6 = FUN_004b8370(0x5ba568);
    if (uVar6 == 0) {
      FUN_004bdb00(&DAT_005ba568,0);
    }
    pvVar2 = (void *)FUN_00464080(0);
    FUN_004bdc60(pvVar2,0);
    FUN_004be270(&DAT_005ba568,0);
    FUN_004bbd80(0x5ba5f0);
    pvVar2 = (void *)FUN_00464080(0);
    FUN_004be940(pvVar2,0);
    iVar3 = FUN_004640a0(0x5ba568);
    FUN_004bdcf0(&DAT_005ba5f0,*(int *)(&DAT_005afd20 + iVar3 * 4) * 100);
    iVar3 = FUN_004640a0(0x5ba568);
    FUN_004bdcd0(&DAT_005ba5f0,*(int *)(&DAT_005afd38 + iVar3 * 4) * 100);
    iVar3 = FUN_004bd570(0x5ba5f0);
    FUN_004bdfd0(&DAT_005ba5f0,(float)iVar3 / DAT_0056e04c);
    bVar1 = FUN_00488850(0x5ba568);
    if (bVar1) {
      pvVar2 = (void *)FUN_00464080(0);
      FUN_004bdee0(pvVar2,7);
      pvVar2 = (void *)FUN_00464080(0);
      FUN_004bda30(pvVar2,7);
      pvVar2 = (void *)FUN_00464080(0);
      FUN_004bde90(pvVar2,0);
      pvVar2 = (void *)FUN_00464080(0);
      FUN_004e15e0(pvVar2,0);
    }
    else {
      bVar1 = FUN_004bd820(0x5ba568);
      if (bVar1) {
        if (DAT_005ba5dc == 0) {
          pvVar2 = (void *)FUN_00464080(0);
          FUN_004bdee0(pvVar2,7);
          pvVar2 = (void *)FUN_00464080(0);
          FUN_004bda30(pvVar2,7);
          pvVar2 = (void *)FUN_00464080(0);
          FUN_004bde90(pvVar2,7);
        }
        else {
          pvVar2 = (void *)FUN_00464080(0);
          FUN_004bde90(pvVar2,DAT_005ba5dc + -1);
          pvVar2 = (void *)FUN_00464080(0);
          FUN_004bdee0(pvVar2,7);
        }
      }
      else {
        pvVar2 = (void *)FUN_00464080(0);
        FUN_004bdee0(pvVar2,7);
        pvVar2 = (void *)FUN_00464080(0);
        FUN_004bda30(pvVar2,7);
        pvVar2 = (void *)FUN_00464080(0);
        FUN_004bde90(pvVar2,2);
        pvVar2 = (void *)FUN_00464080(0);
        FUN_004e15e0(pvVar2,2);
      }
    }
    puVar7 = FUN_004ffff0(0);
    if (puVar7 == (undefined4 *)0x0) goto LAB_004bbd18;
    bVar1 = FUN_00488850(0x5ba568);
    if (bVar1) {
      pvVar2 = (void *)FUN_00464080(0);
      iVar3 = FUN_00464080(0);
      iVar3 = FUN_004b81d0(iVar3);
      FUN_004be0a0(pvVar2,iVar3 << 2);
    }
    else {
      iVar3 = FUN_00474d60(0x5ba568);
      if (iVar3 < 2) {
        pvVar2 = (void *)FUN_00464080(0);
        iVar3 = FUN_00464080(0);
        uVar4 = FUN_004b81d0(iVar3);
        FUN_004be0a0(pvVar2,uVar4);
      }
      else {
        iVar3 = FUN_00474d60(0x5ba568);
        if (iVar3 == 7) {
          pvVar2 = (void *)FUN_00464080(0);
          iVar3 = FUN_00464080(0);
          uVar4 = FUN_004b81d0(iVar3);
          FUN_004be0a0(pvVar2,uVar4);
        }
        else {
          pvVar2 = (void *)FUN_00464080(0);
          iVar3 = FUN_00464080(0);
          iVar3 = FUN_004b81d0(iVar3);
          FUN_004be0a0(pvVar2,iVar3 << 2);
        }
      }
    }
    FUN_004bd940(0x5ba568);
    iVar3 = FUN_00488830((int)param_1);
    if (((iVar3 == 0) && (DAT_005afcfc < 0)) &&
       (iVar3 = FUN_0050fc50(DAT_005c6108), *(int *)(iVar3 + 0x76a8) < 9999999)) {
      iVar3 = FUN_0050fc50(DAT_005c6108);
      *(int *)(iVar3 + 0x76a8) = *(int *)(iVar3 + 0x76a8) + 1;
    }
  }
LAB_004bb871:
  piVar8 = FUN_00412310(0x13,FUN_004bd0b0,param_1);
  param_1[2] = (int)piVar8;
  piVar8 = FUN_004123b0(2,FUN_004bd3c0,param_1);
  param_1[3] = (int)piVar8;
  piVar8 = &DAT_005c4f08;
  piVar10 = param_1 + 0xe;
  for (iVar3 = 0x2c; iVar3 != 0; iVar3 = iVar3 + -1) {
    *piVar10 = *piVar8;
    piVar8 = piVar8 + 1;
    piVar10 = piVar10 + 1;
  }
  FUN_004be360(&DAT_005ba568,*DAT_005c6110);
  local_c = (char *)DAT_005c6110[1];
  if (*DAT_005c6110 == 4) {
    iVar3 = FUN_00464080(0);
    uVar4 = FUN_0041cab0(iVar3);
    switch(uVar4) {
    case 0:
    case 1:
      local_c = "st04b.std";
      break;
    case 2:
    case 3:
      local_c = "st04d.std";
      break;
    case 4:
    case 5:
      local_c = "st04c.std";
      break;
    case 6:
    case 7:
      local_c = "st04a.std";
    }
  }
  uVar6 = FUN_004bd760(0x5ba568);
  if (uVar6 == 0) {
    puVar7 = FUN_0050a930(param_1[0x42],(undefined1 (*) [16])&DAT_005c4b20);
    if (((((puVar7 == (undefined4 *)0x0) ||
          (puVar7 = FUN_00477880((int)local_c,0), puVar7 == (undefined4 *)0x0)) ||
         ((puVar7 = FUN_004b9090(), puVar7 == (undefined4 *)0x0 ||
          ((puVar7 = FUN_00486630(0), puVar7 == (undefined4 *)0x0 ||
           (puVar7 = FUN_004c5010(0), puVar7 == (undefined4 *)0x0)))))) ||
        (puVar7 = FUN_004d7e40(0), puVar7 == (undefined4 *)0x0)) ||
       ((puVar7 = FUN_004e6b80(), puVar7 == (undefined4 *)0x0 ||
        (puVar7 = FUN_005108d0(0), puVar7 == (undefined4 *)0x0)))) goto LAB_004bbd18;
    FUN_00514240();
  }
  else {
    FUN_0050a6b0(DAT_005c60fc);
    FUN_004b5960(DAT_005c06a4);
    puVar7 = FUN_00477880((int)local_c,0);
    if (puVar7 == (undefined4 *)0x0) goto LAB_004bbd18;
  }
  uVar6 = FUN_00474e00(0x5ba568);
  if ((uVar6 == 0) && (uVar6 = FUN_004b8370(0x5ba568), uVar6 == 0)) {
    puVar7 = FUN_004aba70(0,DAT_005c6110[2]);
    if (puVar7 == (undefined4 *)0x0) goto LAB_004bbd18;
  }
  else {
    iVar3 = FUN_00478060(0);
    FUN_004a7040(iVar3);
  }
  puVar7 = FUN_00478300(0);
  if ((puVar7 != (undefined4 *)0x0) && (puVar7 = FUN_00488b20(0), puVar7 != (undefined4 *)0x0)) {
    FUN_0050f660((int)DAT_005c6108);
    if ((DAT_005ba5d4 >> 5 & 1) == 0) {
      bVar1 = FUN_00488850(0x5ba568);
      if (!bVar1) {
        FUN_004d9bc0();
      }
      FUN_004d9a70(0,DAT_005c6110[3]);
      FUN_004d9a70(1,DAT_005c6110[4]);
    }
    FUN_004bdcb0(DAT_005c4a00,0.0);
    FUN_004be200(DAT_005c4a00,0.0);
    FUN_00423520(param_1 + 4,0);
    while (piVar8 = (int *)FUN_004260d0(&DAT_005bcd74,0), *piVar8 != 0) {
      Sleep(0x10);
    }
    iVar3 = FUN_00437520(0);
    FUN_004bed20(iVar3);
    pvVar2 = (void *)FUN_00460830(0);
    FUN_004faca0(pvVar2,-1);
    param_1[0x3d] = 0x3c;
    FUN_004d95b0(0x5c4d40);
    param_1[0x3a] = param_1[0x3a] & 0xfffffffb;
    FUN_004be220(&DAT_005ba568,0);
    FUN_004bdd90(&DAT_005ba568,0);
    FUN_004bdad0(&DAT_005ba568,0);
    FUN_004ba8b0((undefined4 *)&DAT_005c5ad0);
    DAT_005ba518 = 0;
    DAT_005c4d30 = 0;
    DAT_005c4d34 = 0;
    FUN_00486330(DAT_005c4eec);
    FUN_00486330(DAT_005c4ef0);
    FUN_00486330(DAT_005c4ef4);
    FUN_00486330(DAT_005c4ef8);
    iVar3 = FUN_00488830(DAT_005ba828);
    if ((iVar3 == 0) && (bVar1 = FUN_004bd820(0x5ba568), !bVar1)) {
      iVar3 = FUN_0050fc50(DAT_005c6108);
      iVar5 = FUN_004640a0(0x5ba568);
      iVar9 = FUN_00474d60(0x5ba568);
      *(undefined1 *)(iVar3 + iVar5 * 0x90 + (iVar9 + -1) * 0x10 + 0x7701) = 1;
    }
    FUN_004bd6e0(0x5ba568);
    (**(code **)(*param_1 + 4))();
    return 0;
  }
LAB_004bbd18:
  param_1[0x3a] = param_1[0x3a] | 8;
  FUN_004d94e0(0x5c4d40);
  FUN_004ba8b0((undefined4 *)&DAT_005c5ad0);
  (**(code **)(*param_1 + 4))();
  return 0xffffffff;
}

