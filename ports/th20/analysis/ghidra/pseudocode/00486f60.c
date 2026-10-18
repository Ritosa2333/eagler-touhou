/* Entry: 0x00486f60; symbol: FUN_00486f60; body bytes: 875 */

undefined4 __fastcall FUN_00486f60(int param_1)

{
  bool bVar1;
  uint uVar2;
  int iVar3;
  float *pfVar4;
  undefined1 local_54 [12];
  undefined1 local_48 [12];
  void *local_3c;
  void *local_38;
  void *local_34;
  void *local_30;
  void *local_2c;
  void *local_28;
  void *local_24;
  void *local_20;
  undefined4 *local_1c;
  void *local_18;
  void *local_14;
  int local_10;
  int local_c;
  int local_8;
  
  if ((*(uint *)(param_1 + 0x78) & 1) == 0) {
    return 1;
  }
  *(int *)(param_1 + 0x8c) = *(int *)(param_1 + 0x8c) + 1;
  local_14 = (void *)(param_1 + 0x24);
  local_8 = param_1;
  bVar1 = FUN_004235c0(local_14,0x3c);
  if ((bVar1) && ((*(uint *)(local_8 + 0x78) >> 9 & 1) == 0)) {
    FUN_00487b90(DAT_005c069c);
  }
  local_18 = (void *)(local_8 + 0x24);
  bVar1 = FUN_004235c0(local_18,300);
  if ((bVar1) && ((*(uint *)(local_8 + 0x78) >> 3 & 1) == 0)) {
    *(int *)(local_8 + 0x7c) =
         *(int *)(local_8 + 0x7c) -
         (*(int *)(local_8 + 0x80) - *(int *)(local_8 + 0x80) / 3) /
         (*(int *)(local_8 + 0x84) + -300);
    *(int *)(local_8 + 0x7c) = *(int *)(local_8 + 0x7c) - *(int *)(local_8 + 0x7c) % 10;
  }
  local_1c = (undefined4 *)(local_8 + 0x24);
  FUN_00423540(local_1c);
  local_20 = (void *)(local_8 + 0x24);
  bVar1 = FUN_004235c0(local_20,0x78);
  if (!bVar1) goto LAB_004871e6;
  if ((*(uint *)(local_8 + 0x78) >> 2 & 1) != 0) {
    if ((*(uint *)(local_8 + 0x78) >> 8 & 1) == 0) {
      iVar3 = FUN_0040ff90(*(int *)(local_8 + 0xc0));
      iVar3 = FUN_00460850(iVar3);
      if (*(float *)(iVar3 + 4) <= DAT_0056cd98) goto LAB_0048716c;
    }
    else {
LAB_0048716c:
      if ((*(uint *)(local_8 + 0x78) >> 8 & 1) == 0) goto LAB_004871e6;
      iVar3 = FUN_0040ff90(*(int *)(local_8 + 0xc0));
      iVar3 = FUN_00460850(iVar3);
      if (DAT_0056d7c0 - DAT_0056cd98 < *(float *)(iVar3 + 4) ||
          DAT_0056d7c0 - DAT_0056cd98 == *(float *)(iVar3 + 4)) goto LAB_004871e6;
    }
    for (local_10 = 0; local_10 < 3; local_10 = local_10 + 1) {
      local_28 = (void *)(local_8 + 0x14 + local_10 * 4);
      FUN_0044ef90(local_28,2);
    }
    *(uint *)(local_8 + 0x78) = *(uint *)(local_8 + 0x78) & 0xfffffffb;
    goto LAB_004871e6;
  }
  uVar2 = FUN_004888a0(local_8);
  if (uVar2 == 0) {
    iVar3 = FUN_0040ff90(*(int *)(local_8 + 0xc0));
    iVar3 = FUN_00460850(iVar3);
    if (DAT_0056fe7c < *(float *)(iVar3 + 4) || DAT_0056fe7c == *(float *)(iVar3 + 4))
    goto LAB_004870ba;
  }
  else {
LAB_004870ba:
    uVar2 = FUN_004888a0(local_8);
    if (uVar2 == 0) goto LAB_004871e6;
    iVar3 = FUN_0040ff90(*(int *)(local_8 + 0xc0));
    iVar3 = FUN_00460850(iVar3);
    if (*(float *)(iVar3 + 4) <= DAT_0056d7c0 - DAT_0056fe7c) goto LAB_004871e6;
  }
  for (local_c = 0; local_c < 3; local_c = local_c + 1) {
    local_24 = (void *)(local_8 + 0x14 + local_c * 4);
    FUN_0044ef90(local_24,3);
  }
  *(uint *)(local_8 + 0x78) = *(uint *)(local_8 + 0x78) | 4;
LAB_004871e6:
  local_2c = (void *)FUN_00412730(*(int *)(local_8 + 0xc0));
  iVar3 = FUN_00485660(local_2c,0);
  if (iVar3 != 0) {
    local_30 = (void *)FUN_00412730(*(int *)(local_8 + 0xc0));
    iVar3 = FUN_00485660(local_30,0);
    local_34 = (void *)FUN_0047a2c0(iVar3);
    local_38 = FUN_00429440(local_34,local_48,(float *)(local_8 + 0xac));
    pfVar4 = (float *)FUN_004292f0(local_38,local_54,DAT_0056fe5c);
    FUN_004296e0((void *)(local_8 + 0xac),pfVar4);
  }
  local_3c = (void *)(local_8 + 0x20);
  FUN_004502c0(local_3c,(undefined4 *)(local_8 + 0xac));
  if ((*(uint *)(local_8 + 0x78) >> 5 & 1) != 0) {
    iVar3 = FUN_00424010(*(int *)(local_8 + 0xc0));
    bVar1 = FUN_00478130(iVar3);
    if (!bVar1) {
      *(uint *)(local_8 + 0x78) = *(uint *)(local_8 + 0x78) & 0xffffffdf;
    }
  }
  return 1;
}

