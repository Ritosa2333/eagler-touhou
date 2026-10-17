/* Entry: 0x0052b8e0; symbol: FUN_0052b8e0; body bytes: 1837 */

/* WARNING: Type propagation algorithm not settling */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0052b8e0(void)

{
  bool bVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  undefined4 local_a4;
  undefined4 local_a0;
  undefined4 local_9c;
  undefined4 local_98;
  undefined4 local_94;
  undefined4 local_90;
  undefined4 *local_8c;
  int local_88;
  int local_84;
  int local_80;
  void *local_7c;
  void *local_78;
  void *local_74;
  undefined4 *local_70;
  int local_6c;
  undefined4 local_68;
  void *local_64;
  void *local_60;
  undefined4 *local_5c;
  int local_58;
  void *local_54;
  void *local_50;
  int local_4c;
  int *local_48;
  int local_44;
  int local_40;
  int *local_3c;
  undefined4 local_38;
  int local_34;
  int local_30;
  int local_2c;
  int local_28;
  void *local_24;
  int local_20 [4];
  int local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar2 = FUN_004640a0(0x5ba568);
  if (iVar2 == 4) {
    local_34 = 0xe;
  }
  else {
    local_34 = 0xc;
  }
  local_30 = local_34;
  local_28 = 0xd;
  local_38 = *(undefined4 *)((int)local_24 + 0x20);
  switch(local_38) {
  case 0:
    local_7c = (void *)((int)local_24 + 0x24);
    FUN_0049c4e0(local_7c,3);
    FUN_0052cf80(local_24,local_28);
    local_20[2] = 0;
    local_3c = local_20 + 2;
    FUN_00461510(&local_c);
    FUN_0050ae80(local_20 + 3);
    while (bVar1 = FUN_00449460(&local_c,local_20 + 3), bVar1) {
      local_40 = FUN_0040c300(&local_c);
      local_80 = FUN_004b7f70(0x5ba568);
      local_4c = FUN_004640a0(0x5ba568);
      iVar2 = FUN_0052c440(DAT_005c6108,local_4c,local_80,local_40);
      if (iVar2 == 0) {
        FUN_0052c240(local_24,&local_90,local_28,local_40 + 0x16);
        FUN_00450120();
      }
      FUN_00461860(&local_c);
    }
    *(undefined4 *)((int)local_24 + 0x5960) = 0xffffffff;
    *(undefined4 *)((int)local_24 + 0x5964) = 0xffffffff;
    if ((DAT_005c6128 != 4) && (DAT_005c6128 != 5)) {
      local_50 = (void *)FUN_0051b960();
      FUN_0051a0b0(local_50,0);
      *(undefined4 *)((int)local_24 + 0x20) = 1;
      goto switchD_0052b938_caseD_1;
    }
LAB_0052bd85:
    iVar2 = FUN_00464080(0);
    iVar2 = FUN_00412730(iVar2);
    local_70 = FUN_0052c240(local_24,&local_9c,local_30,iVar2 + 6);
    iVar2 = FUN_004b8340(0x5ba568);
    local_6c = iVar2 / 2 + 0x1e;
    FUN_0051c6b0(local_70,local_6c);
    FUN_004c60b0((int)local_24 + 0x24);
    FUN_0052cc30(local_24,0x24);
    bVar1 = FUN_00488850(0x5ba568);
    if (bVar1) {
      FUN_0052ced0(local_24,0x12);
    }
    else {
      FUN_0052ced0(local_24,8);
    }
    goto LAB_0052be11;
  case 1:
switchD_0052b938_caseD_1:
    local_54 = (void *)((int)local_24 + 0x154);
    bVar1 = FUN_00461070(local_54,0x1e);
    if (bVar1) {
      FUN_0052cf80(local_24,0x24);
      FUN_0052ce50(local_24,2);
    }
    break;
  case 2:
    iVar2 = FUN_0051b960();
    iVar2 = FUN_0051bb90(iVar2);
    if (iVar2 != *(int *)((int)local_24 + 0x5964)) {
      iVar2 = FUN_0051b960();
      uVar4 = FUN_0051bb90(iVar2);
      *(undefined4 *)((int)local_24 + 0x5964) = uVar4;
      iVar2 = FUN_0051b960();
      local_2c = FUN_0051bb90(iVar2);
      if (local_2c == 0) {
        FUN_0052cc90(local_24,local_28);
      }
      else if (local_2c == 1) {
        FUN_0052cd10(local_24,local_28,
                     (int)(short)((short)*(undefined4 *)((int)local_24 + 0x5960) + 7));
      }
      else if (local_2c == 2) {
        FUN_0052cd10(local_24,local_28,
                     (int)(short)((short)*(undefined4 *)((int)local_24 + 0x5960) + 0x25));
      }
    }
    iVar2 = FUN_00464080(0);
    iVar2 = FUN_0041cab0(iVar2);
    if (iVar2 != *(int *)((int)local_24 + 0x5960)) {
      FUN_0052cc90(local_24,local_28);
      iVar2 = FUN_0051b960();
      iVar2 = FUN_0051bb90(iVar2);
      if ((iVar2 != 0) && (0 < *(int *)((int)local_24 + 0x5960))) {
        FUN_0052cd10(local_24,local_28,
                     (int)(short)((short)*(undefined4 *)((int)local_24 + 0x5960) + 0x11));
      }
      iVar2 = FUN_00464080(0);
      uVar4 = FUN_0041cab0(iVar2);
      *(undefined4 *)((int)local_24 + 0x5960) = uVar4;
      iVar2 = FUN_0051b960();
      local_44 = FUN_0051bb90(iVar2);
      if (local_44 == 1) {
        FUN_0052cd10(local_24,local_28,
                     (int)(short)((short)*(undefined4 *)((int)local_24 + 0x5960) + 7));
      }
      else if (local_44 == 2) {
        FUN_0052cd10(local_24,local_28,
                     (int)(short)((short)*(undefined4 *)((int)local_24 + 0x5960) + 0x25));
      }
      iVar2 = FUN_00464080(0);
      iVar2 = FUN_00412730(iVar2);
      local_5c = FUN_0052c240(local_24,&local_94,local_30,iVar2 + 6);
      local_58 = *(int *)((int)local_24 + 0x5960) / 2 + 0x1e;
      FUN_0051c6b0(local_5c,local_58);
    }
    iVar2 = FUN_0051b960();
    bVar1 = FUN_0052c9c0(iVar2);
    if (bVar1) {
      FUN_0052ce50(local_24,3);
    }
    else {
      iVar2 = FUN_0051b960();
      bVar1 = FUN_0052c700(iVar2);
      if (!bVar1) {
        FUN_0052ce50(local_24,4);
      }
    }
    break;
  case 3:
    iVar2 = FUN_00437520(0);
    iVar2 = FUN_00438520(iVar2);
    if (iVar2 == 0) {
      local_60 = (void *)((int)local_24 + 0x154);
      FUN_00429420(local_60);
      break;
    }
    local_64 = (void *)((int)local_24 + 0x154);
    bVar1 = FUN_004785e0(local_64,10);
    if (bVar1) {
      bVar1 = FUN_004bd820(0x5ba568);
      if (bVar1) goto LAB_0052bd85;
      FUN_004a0aa0(DAT_0056cda8,DAT_00570388);
      local_68 = FUN_00437520(0);
      puVar3 = FUN_0049dcf0(&local_98,0,0,0);
      _DAT_005c4f04 = *puVar3;
      FUN_0051c6b0(&DAT_005c4f04,0);
      FUN_004d99d0(DAT_0056fe5c);
    }
LAB_0052be11:
    local_74 = (void *)((int)local_24 + 0x154);
    bVar1 = FUN_004235c0(local_74,0x28);
    if (bVar1) {
      uVar4 = FUN_0040c300((undefined4 *)((int)local_24 + 0x24));
      FUN_0052cf40(&DAT_005ba568,uVar4);
      DAT_005c6144 = FUN_004b8340(0x5ba568);
      FUN_004c60b0((int)local_24 + 0x24);
      FUN_0050a7f0(&DAT_005ba568,0xffffffff);
      FUN_0052ced0(local_24,2);
      iVar2 = FUN_004640a0(0x5ba568);
      if (iVar2 < 4) {
        FUN_004be360(&DAT_005ba568,1);
      }
      else {
        FUN_004be360(&DAT_005ba568,7);
      }
      DAT_005afcfc = 0xffffffff;
      FUN_004be250(&DAT_005c4d40,7);
    }
    break;
  case 4:
    local_78 = (void *)((int)local_24 + 0x154);
    bVar1 = FUN_004235c0(local_78,6);
    if (bVar1) {
      local_20[1] = 0;
      local_48 = local_20 + 1;
      FUN_00461510(&local_10);
      FUN_0050ae80(local_20);
      while (bVar1 = FUN_00449460(&local_10,local_20), bVar1) {
        local_84 = FUN_0040c300(&local_10);
        FUN_0052c240(local_24,&local_a0,local_28,local_84 + 0x16);
        FUN_00450120();
        FUN_00461860(&local_10);
      }
      DAT_005c6144 = FUN_004b8340(0x5ba568);
      FUN_0052cc30(local_24,local_28);
      iVar2 = FUN_00464080(0);
      iVar2 = FUN_00412730(iVar2);
      local_8c = FUN_0052c240(local_24,&local_a4,local_30,iVar2 + 6);
      iVar2 = FUN_004b8340(0x5ba568);
      local_88 = iVar2 / 2 + 0x1e;
      FUN_0051c6b0(local_8c,local_88);
      FUN_0052cc30(local_24,0x24);
      FUN_0052ced0(local_24,6);
      FUN_004c6010((undefined4 *)((int)local_24 + 0x24));
      FUN_0050f660((int)DAT_005c6108);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

