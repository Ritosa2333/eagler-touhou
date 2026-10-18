/* Entry: 0x00505a30; symbol: FUN_00505a30; body bytes: 851 */

undefined4 __fastcall FUN_00505a30(void *param_1)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined3 extraout_var;
  void *this;
  undefined4 uVar5;
  undefined4 uVar6;
  uint local_1c;
  uint local_18;
  uint local_14;
  uint local_10;
  
  uVar2 = FUN_004ab2e0(DAT_005ba828);
  if (uVar2 != 0) {
    return 0;
  }
  iVar3 = FUN_00498fb0(DAT_005b8898,*(int *)((int)param_1 + 0x12590));
  if ((iVar3 < 0) || ((&DAT_005b889c)[iVar3] == 0)) {
    local_10 = 0;
  }
  else {
    local_10 = FUN_004973a0((void *)(&DAT_005b889c)[iVar3],1);
  }
  if (local_10 != 0) {
    *(uint *)((int)param_1 + 0x1255c) = *(uint *)((int)param_1 + 0x1255c) | 2;
  }
  iVar4 = FUN_0040ff90(*(int *)((int)param_1 + 0x12594));
  iVar4 = FUN_00411700(iVar4);
  if (iVar4 != 1) {
    *(undefined4 *)((int)param_1 + 0x12578) = 0;
    *(undefined1 *)((int)param_1 + 0x1257c) = 0;
    FUN_00423520((void *)((int)param_1 + 0x12400),-1);
    return 0;
  }
  iVar4 = FUN_00506460((int)param_1);
  if (iVar4 < 0) {
    if ((iVar3 < 0) || ((&DAT_005b889c)[iVar3] == 0)) {
      local_14 = 0;
    }
    else {
      local_14 = FUN_004973a0((void *)(&DAT_005b889c)[iVar3],1);
    }
    if ((local_14 == 0) || ((*(uint *)((int)param_1 + 0x1255c) >> 2 & 1) != 0)) goto LAB_00505c8d;
    bVar1 = FUN_00423560((void *)((int)param_1 + 0x12410),0);
    if (bVar1) {
      FUN_00423520((void *)((int)param_1 + 0x12410),0);
    }
    FUN_00506cd0(param_1,0);
  }
  bVar1 = FUN_0045d030((int *)((int)param_1 + 0x12400));
  if (CONCAT31(extraout_var,bVar1) != 0) {
    this = (void *)FUN_00464250(*(int *)((int)param_1 + 0x12594));
    iVar4 = FUN_0041caf0(*(int *)((int)param_1 + 0x12594));
    iVar4 = FUN_004b81a0(iVar4);
    uVar5 = FUN_0040ff90((int)param_1 + 0x12410);
    uVar6 = FUN_0040ff90((int)param_1 + 0x12400);
    FUN_005331e0(this,uVar6,uVar5,iVar4);
  }
  bVar1 = FUN_004235c0((void *)((int)param_1 + 0x12400),0xe);
  if (bVar1) {
    if ((iVar3 < 0) || ((&DAT_005b889c)[iVar3] == 0)) {
      local_18 = 0;
    }
    else {
      local_18 = FUN_004973a0((void *)(&DAT_005b889c)[iVar3],1);
    }
    if ((local_18 == 0) || ((*(uint *)((int)param_1 + 0x1255c) >> 2 & 1) != 0)) {
      FUN_00423520((void *)((int)param_1 + 0x12400),-1);
    }
    else {
      FUN_004c9170((void *)((int)param_1 + 0x12400),0xe);
    }
  }
  else {
    FUN_00423540((undefined4 *)((int)param_1 + 0x12400));
  }
LAB_00505c8d:
  bVar1 = FUN_004235c0((void *)((int)param_1 + 0x12410),0);
  if (bVar1) {
    bVar1 = FUN_004235c0((void *)((int)param_1 + 0x12410),0x77);
    if (bVar1) {
      if ((iVar3 < 0) || ((&DAT_005b889c)[iVar3] == 0)) {
        local_1c = 0;
      }
      else {
        local_1c = FUN_004973a0((void *)(&DAT_005b889c)[iVar3],1);
      }
      if ((local_1c == 0) || ((*(uint *)((int)param_1 + 0x1255c) >> 2 & 1) != 0)) {
        FUN_00423520((void *)((int)param_1 + 0x12410),-1);
      }
      else {
        FUN_004c9170((void *)((int)param_1 + 0x12410),0x77);
      }
    }
    else {
      FUN_00423540((undefined4 *)((int)param_1 + 0x12410));
    }
  }
  return 0;
}

