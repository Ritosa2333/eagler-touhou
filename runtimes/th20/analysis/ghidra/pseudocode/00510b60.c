/* Entry: 0x00510b60; symbol: FUN_00510b60; body bytes: 763 */

undefined4 __fastcall FUN_00510b60(undefined4 *param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined4 local_3c;
  undefined4 *local_38;
  undefined4 *local_34;
  undefined4 *local_30;
  void *local_2c;
  undefined4 *local_28;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  undefined4 local_14;
  uint local_10;
  int local_c;
  undefined4 *local_8;
  
  local_c = param_1[9] + -1;
  local_8 = param_1;
  switch(local_c) {
  case 0:
    break;
  case 1:
    goto switchD_00510b8a_caseD_1;
  default:
    goto switchD_00510b8a_caseD_2;
  case 3:
    goto switchD_00510b8a_caseD_3;
  case 5:
    local_34 = param_1 + 0xb;
    bVar1 = FUN_004785e0(local_34,10);
    if (bVar1) {
      if (DAT_005ba828 != 0) {
        FUN_004bc570(DAT_005ba828);
      }
      FUN_004217c0(local_8);
      return 1;
    }
    goto switchD_00510b8a_caseD_2;
  }
  FUN_00511590(DAT_005ba828);
  local_14 = local_8[4];
  puVar2 = FUN_00450c70(&local_3c,(uint *)"front",0x6f,-1,(undefined4 *)0x0);
  local_8[5] = *puVar2;
  iVar3 = FUN_00474d60(0x5ba568);
  local_8[10] = iVar3 * 100000;
  iVar3 = FUN_00464080(0);
  iVar3 = FUN_004b82c0(iVar3);
  iVar4 = FUN_00464080(0);
  iVar4 = FUN_005113c0(iVar4);
  iVar5 = FUN_00464080(0);
  iVar5 = FUN_00511320(iVar5);
  iVar6 = FUN_00464080(0);
  iVar6 = FUN_00511370(iVar6);
  iVar7 = FUN_00464080(0);
  iVar7 = FUN_00511410(iVar7);
  local_1c = iVar3 * (iVar4 + iVar5 + iVar6 + iVar7);
  local_18 = local_8[10];
  local_8[10] = local_18 + local_1c;
  iVar3 = FUN_00464080(0);
  iVar3 = FUN_005114f0(iVar3);
  iVar4 = FUN_00464080(0);
  iVar4 = FUN_00511460(iVar4);
  iVar5 = FUN_00464080(0);
  iVar5 = FUN_005114a0(iVar5);
  iVar6 = FUN_00464080(0);
  iVar6 = FUN_00511530(iVar6);
  local_24 = (iVar3 + iVar4 + iVar5 + iVar6) * 100000;
  local_20 = local_8[10];
  local_8[10] = local_20 + local_24;
  local_8[10] = local_8[10] - (int)local_8[10] % 10;
  FUN_00488550(&DAT_005ba568,local_8[10]);
  local_8[9] = 2;
  local_8[7] = 0;
  DAT_005ba518 = 0;
switchD_00510b8a_caseD_1:
  local_8[9] = 4;
switchD_00510b8a_caseD_3:
  local_28 = local_8 + 0xb;
  bVar1 = FUN_004235c0(local_28,0x78);
  if (bVar1) {
    if (DAT_005b889c == (void *)0x0) {
      local_10 = 0;
    }
    else {
      local_2c = DAT_005b889c;
      local_10 = FUN_00419c00(DAT_005b889c,0x80001);
    }
    if (local_10 == 0) {
      local_30 = local_8 + 0xb;
      bVar1 = FUN_004235c0(local_30,300);
      if (!bVar1) goto switchD_00510b8a_caseD_2;
    }
    if (DAT_005ba828 != 0) {
      *(uint *)(DAT_005ba828 + 0xe8) = *(uint *)(DAT_005ba828 + 0xe8) & 0xfbffffff;
    }
    FUN_0044fcd0(local_8 + 5);
    local_8[9] = 6;
    FUN_00423520(local_8 + 0xb,0);
    FUN_004d99d0(DAT_0056c8d0);
  }
switchD_00510b8a_caseD_2:
  local_38 = local_8 + 0xb;
  FUN_00423540(local_38);
  return 1;
}

