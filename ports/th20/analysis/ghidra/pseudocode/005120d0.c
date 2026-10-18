/* Entry: 0x005120d0; symbol: FUN_005120d0; body bytes: 2183 */

void FUN_005120d0(void)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  undefined3 extraout_var;
  int iVar4;
  undefined3 extraout_var_00;
  undefined4 *puVar5;
  char **ppcVar6;
  int *unaff_FS_OFFSET;
  float10 fVar7;
  undefined1 local_12c [12];
  int *local_120;
  void *local_11c;
  void *local_118;
  undefined4 *local_114;
  float local_110;
  void *local_10c;
  undefined4 *local_108;
  void *local_104;
  int local_100;
  void *local_fc;
  float local_f8;
  void *local_f4;
  int *local_f0;
  void *local_ec;
  undefined4 local_e8;
  undefined4 local_e4;
  void *local_e0;
  void *local_dc;
  undefined4 local_d8;
  undefined4 local_d4;
  void *local_d0;
  int local_cc;
  void *local_c8;
  void *local_c4;
  void *local_c0;
  void *local_bc;
  void *local_b8;
  char *local_b4;
  float local_b0;
  void *local_ac;
  undefined4 *local_a8;
  void *local_a4;
  undefined4 *local_a0;
  void *local_9c;
  int local_98;
  int local_94;
  int local_90;
  undefined1 local_8a;
  undefined1 local_89;
  int local_88;
  int local_84;
  int local_80;
  void *local_7c;
  undefined4 local_78;
  undefined4 local_74;
  undefined4 local_70;
  undefined4 local_64;
  int local_24 [2];
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056aa65;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  bVar1 = FUN_00488850(0x5ba568);
  if (bVar1) {
    local_fc = (void *)FUN_00464080(0);
    FUN_004be620(local_fc,0);
  }
  iVar3 = FUN_00478060(0);
  if (iVar3 != 0) {
    iVar3 = FUN_00478060(0);
    bVar1 = FUN_0049be20(iVar3);
    if (((bVar1) && (cVar2 = FUN_00485a40((int)local_7c), cVar2 == '\0')) &&
       (bVar1 = FUN_00478160(DAT_005c06a4), CONCAT31(extraout_var,bVar1) == 0)) {
      local_118 = (void *)((int)local_7c + 0x28);
      iVar3 = FUN_00472040(local_118,3);
      if (iVar3 < 2) {
        local_11c = (void *)FUN_00464080(0);
        FUN_00477f60(local_11c,1);
      }
      local_a8 = (undefined4 *)((int)local_7c + 0x28);
      FUN_00423540(local_a8);
    }
  }
  cVar2 = FUN_00485a40((int)local_7c);
  if (cVar2 == '\0') {
    iVar3 = FUN_00464080(0);
    iVar3 = FUN_00499520(iVar3);
    iVar4 = FUN_00464080(0);
    iVar4 = FUN_00513e60(iVar4);
    if (((iVar4 <= iVar3) &&
        (bVar1 = FUN_00478160(DAT_005c06a4), CONCAT31(extraout_var_00,bVar1) == 0)) &&
       (DAT_005c6114 == 0)) {
      FUN_0047bb30(&local_78);
      local_84 = 0;
      local_88 = -1;
      local_80 = 0;
      iVar3 = FUN_00464080(0);
      iVar3 = FUN_00513f20(iVar3);
      if (iVar3 < 0) {
        iVar3 = FUN_00464080(0);
        iVar3 = FUN_00513ea0(iVar3);
        if (local_88 < iVar3) {
          iVar3 = FUN_00464080(0);
          local_88 = FUN_00513ea0(iVar3);
          iVar3 = FUN_00464080(0);
          local_80 = FUN_005114f0(iVar3);
          local_84 = 0;
        }
        iVar3 = FUN_00464080(0);
        iVar3 = FUN_00513de0(iVar3);
        if (local_88 < iVar3) {
          iVar3 = FUN_00464080(0);
          local_88 = FUN_00513de0(iVar3);
          iVar3 = FUN_00464080(0);
          local_80 = FUN_00511460(iVar3);
          local_84 = 1;
        }
        iVar3 = FUN_00464080(0);
        iVar3 = FUN_00513ee0(iVar3);
        if (local_88 < iVar3) {
          iVar3 = FUN_00464080(0);
          local_88 = FUN_00513ee0(iVar3);
          iVar3 = FUN_00464080(0);
          local_80 = FUN_00511530(iVar3);
          local_84 = 2;
        }
        iVar3 = FUN_00464080(0);
        iVar3 = FUN_00513e20(iVar3);
        if (local_88 < iVar3) {
          iVar3 = FUN_00464080(0);
          local_88 = FUN_00513e20(iVar3);
          iVar3 = FUN_00464080(0);
          local_80 = FUN_005114a0(iVar3);
          local_84 = 3;
        }
      }
      else {
        iVar3 = FUN_00464080(0);
        local_94 = FUN_00513f20(iVar3);
        local_84 = local_94;
        switch(local_94) {
        default:
          iVar3 = FUN_00464080(0);
          local_80 = FUN_005114f0(iVar3);
          break;
        case 1:
          iVar3 = FUN_00464080(0);
          local_80 = FUN_00511460(iVar3);
          break;
        case 2:
          iVar3 = FUN_00464080(0);
          local_80 = FUN_00511530(iVar3);
          break;
        case 3:
          iVar3 = FUN_00464080(0);
          local_80 = FUN_005114a0(iVar3);
        }
        local_ac = (void *)FUN_00464080(0);
        FUN_00514170(local_ac,0xffffffff);
      }
      fVar7 = FUN_004298e0(0x5ba4a8);
      local_b0 = (float)fVar7;
      puVar5 = FUN_00422dd0(local_12c,local_b0 * DAT_00570ad4,DAT_00570ad0,0);
      local_78 = *puVar5;
      local_74 = puVar5[1];
      local_70 = puVar5[2];
      local_64 = *(undefined4 *)(&DAT_00573ff0 + local_80 * 4 + local_84 * 0x14);
      *(undefined1 *)((int)local_7c + 0xb0) = 1;
      local_b8 = (void *)FUN_00478060(0);
      local_b4 = (&PTR_s_StoneR_005b09d8)[local_84];
      FUN_004a8920(local_b8,local_b4,&local_78,0);
      local_98 = local_84;
      switch(local_84) {
      case 0:
        local_bc = (void *)FUN_00464080(0);
        FUN_004be760(local_bc,0);
        break;
      case 1:
        local_c0 = (void *)FUN_00464080(0);
        FUN_004be670(local_c0,0);
        break;
      case 2:
        local_c4 = (void *)FUN_00464080(0);
        FUN_004be7b0(local_c4,0);
        break;
      case 3:
        local_c8 = (void *)FUN_00464080(0);
        FUN_004be6c0(local_c8,0);
      }
      local_d0 = (void *)FUN_00464080(0);
      iVar3 = FUN_00464080(0);
      local_cc = FUN_00513e60(iVar3);
      local_cc = local_cc / 3;
      FUN_00514260(local_d0,local_cc);
      FUN_00423520((void *)((int)local_7c + 0x18),0);
      local_dc = (void *)((int)local_7c + 0x38);
      local_d4 = 0;
      local_d8 = DAT_0056c8cc;
      FUN_00439550(local_dc,0x3c,4,&local_d8,&local_d4);
      local_e0 = (void *)((int)local_7c + 100);
      local_8a = 0;
      local_89 = 0xff;
      FUN_005141e0(local_e0,0x78,4,&local_89,&local_8a);
      local_ec = (void *)((int)local_7c + 0x84);
      local_e4 = 0;
      local_e8 = DAT_0056c8cc;
      FUN_00439550(local_ec,0x528,5,&local_e8,&local_e4);
    }
  }
  local_90 = 0;
  local_9c = local_7c;
  FUN_0040c080(local_1c,8);
  FUN_00412280(local_9c,local_1c);
  local_8 = 0;
  local_f0 = (int *)FUN_00412540();
  while (bVar1 = FUN_00411bb0(local_1c,local_f0), bVar1) {
    local_a0 = (undefined4 *)FUN_0040c300(local_1c);
    local_90 = local_90 + 1;
    iVar3 = FUN_0040c300(local_a0);
    iVar3 = FUN_00512980(iVar3);
    if (iVar3 != 0) {
      ppcVar6 = (char **)FUN_0040c300(local_a0);
      FUN_00512f30(ppcVar6);
      *(undefined1 *)((int)local_7c + 0xb0) = 0;
    }
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  cVar2 = FUN_00485a40((int)local_7c);
  if (cVar2 != '\0') {
    local_f4 = (void *)((int)local_7c + 0x18);
    bVar1 = FUN_00461070(local_f4,10);
    if ((bVar1) && (local_90 == 0)) {
      *(undefined1 *)((int)local_7c + 0xb0) = 0;
    }
    local_104 = (void *)FUN_00464080(0);
    iVar3 = FUN_00464080(0);
    iVar3 = FUN_00513e60(iVar3);
    local_f8 = (float)iVar3;
    fVar7 = FUN_00511cf0((float *)((int)local_7c + 0x84));
    local_110 = (float)fVar7;
    local_100 = (int)(local_f8 * local_110);
    FUN_004be620(local_104,local_100);
    local_108 = (undefined4 *)((int)local_7c + 0x18);
    FUN_00423540(local_108);
    FUN_0042a110((float *)((int)local_7c + 0x38));
    FUN_00513710((byte *)((int)local_7c + 100));
    FUN_0042a110((float *)((int)local_7c + 0x84));
    cVar2 = FUN_00514080((int)local_7c + 0x84);
    if (cVar2 != '\0') {
      local_10c = (void *)((int)local_7c + 0x18);
      bVar1 = FUN_004235c0(local_10c,0x708);
      if (!bVar1) goto LAB_0051293d;
    }
    *(undefined1 *)((int)local_7c + 0xb0) = 0;
    FUN_00423520((void *)((int)local_7c + 0x18),0);
    local_a4 = local_7c;
    FUN_0040c080(local_24,8);
    FUN_00412280(local_a4,local_24);
    local_8 = 1;
    local_120 = (int *)FUN_00412540();
    while (bVar1 = FUN_00411bb0(local_24,local_120), bVar1) {
      local_114 = (undefined4 *)FUN_0040c300(local_24);
      ppcVar6 = (char **)FUN_0040c300(local_114);
      FUN_00512f30(ppcVar6);
      FUN_00411c30(local_24);
    }
    local_8 = 0xffffffff;
    FUN_00411b00(local_24);
  }
LAB_0051293d:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

