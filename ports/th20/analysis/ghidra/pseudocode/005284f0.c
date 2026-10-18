/* Entry: 0x005284f0; symbol: FUN_005284f0; body bytes: 1350 */

undefined4 __fastcall FUN_005284f0(void *param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int iVar2;
  undefined4 uVar3;
  void *pvVar4;
  int local_8c [8];
  int local_6c;
  void *local_68;
  void *local_64;
  void *local_60;
  int local_5c;
  int local_58;
  void *local_54;
  void *local_50;
  void *local_4c;
  void *local_48;
  void *local_44;
  void *local_40;
  void *local_3c;
  void *local_38;
  void *local_34;
  void *local_30;
  void *local_2c;
  uint local_28;
  uint local_24;
  int local_20;
  int local_1c;
  undefined4 local_18;
  int local_14;
  int local_10;
  int local_c;
  void *local_8;
  
  local_8 = param_1;
  local_8c[0] = 3;
  local_8c[1] = 3;
  local_8c[2] = 4;
  local_8c[3] = 4;
  local_8c[4] = 4;
  local_8c[5] = 7;
  local_8c[6] = 0xd;
  local_18 = *(undefined4 *)((int)param_1 + 0x20);
  switch(*(undefined4 *)((int)param_1 + 0x20)) {
  case 0:
    local_2c = (void *)((int)param_1 + 0x24);
    FUN_0049c4e0((void *)((int)param_1 + 0x24),local_8c[*(int *)((int)param_1 + 0x58d8)]);
    FUN_004e68a0((int)local_8 + 0x24);
    bVar1 = FUN_0052c750(local_8,0x25);
    if (CONCAT31(extraout_var,bVar1) == 0) {
      FUN_0052cf80(local_8,0x25);
    }
    bVar1 = FUN_0052c750(local_8,0x56);
    if (CONCAT31(extraout_var_00,bVar1) == 0) {
      FUN_0052cf80(local_8,0x56);
    }
    FUN_0052ce50(local_8,1);
    for (local_c = 0; local_c < 0xd - local_8c[*(int *)((int)local_8 + 0x58d8)];
        local_c = local_c + 1) {
      FUN_0052cd10(local_8,0x56,(int)(short)(0x27 - (short)local_c));
    }
    if (DAT_005c6128 != 5) goto switchD_00528549_caseD_1;
    FUN_004bed50((void *)((int)local_8 + 0x24),DAT_005b0a5c);
    DAT_005b0a5c = -1;
    FUN_0052cc90(local_8,0x56);
    local_30 = (void *)((int)local_8 + 0x24);
    iVar2 = FUN_0051e080(local_30,7);
    FUN_0052cd10(local_8,0x56,(int)(short)iVar2);
    FUN_0052cd10(local_8,0x56,6);
    goto LAB_005288f7;
  case 1:
switchD_00528549_caseD_1:
    local_34 = (void *)((int)local_8 + 0x154);
    bVar1 = FUN_00461070(local_34,10);
    if (bVar1) {
      FUN_0052ce50(local_8,2);
      FUN_0052cc90(local_8,0x56);
      local_38 = (void *)((int)local_8 + 0x24);
      iVar2 = FUN_0051e080(local_38,7);
      FUN_0052ccd0(local_8,0x56,(int)(short)iVar2);
    }
    break;
  case 2:
    FUN_004bfc00((undefined4 *)((int)param_1 + 0x24));
    if (DAT_005b889c == (void *)0x0) {
      local_1c = 0;
    }
    else {
      local_3c = DAT_005b889c;
      local_1c = FUN_0041a280(DAT_005b889c,0x10);
    }
    if (local_1c != 0) {
      local_40 = (void *)((int)local_8 + 0x24);
      FUN_004bf040(local_40);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_20 = 0;
    }
    else {
      local_44 = DAT_005b889c;
      local_20 = FUN_0041a280(DAT_005b889c,0x20);
    }
    if (local_20 != 0) {
      local_48 = (void *)((int)local_8 + 0x24);
      FUN_004bf020(local_48);
    }
    bVar1 = FUN_0045d030((int *)((int)local_8 + 0x24));
    if (CONCAT31(extraout_var_01,bVar1) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
      FUN_0052cc90(local_8,0x56);
      local_4c = (void *)((int)local_8 + 0x24);
      iVar2 = FUN_0051e080(local_4c,7);
      FUN_0052ccd0(local_8,0x56,(int)(short)iVar2);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_24 = 0;
    }
    else {
      local_50 = DAT_005b889c;
      local_24 = FUN_00419c00(DAT_005b889c,0x106);
    }
    if (local_24 == 0) {
      if (DAT_005b889c == (void *)0x0) {
        local_28 = 0;
      }
      else {
        local_54 = DAT_005b889c;
        local_28 = FUN_00419c00(DAT_005b889c,0x80001);
      }
      if (local_28 != 0) {
        local_58 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
        local_5c = *(int *)((int)local_8 + 0x58d8);
        iVar2 = FUN_0052cb30(local_5c,local_58);
        if (iVar2 == 0) {
          FUN_00426d70(&DAT_005ba830,0x10,0);
        }
        else {
          FUN_0052ccd0(local_8,0x56,6);
          FUN_0052ce50(local_8,3);
          FUN_00426d70(&DAT_005ba830,7,0);
        }
      }
    }
    else {
      FUN_0052ce50(local_8,4);
      FUN_00426d70(&DAT_005ba830,9,0);
    }
    break;
  case 3:
    local_60 = (void *)((int)param_1 + 0x154);
    bVar1 = FUN_004235c0(local_60,0xe);
    if (!bVar1) break;
LAB_005288f7:
    FUN_0052ced0(local_8,0x14);
    uVar3 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
    *(undefined4 *)((int)local_8 + 0x58dc) = uVar3;
    FUN_004c60b0((int)local_8 + 0x24);
    FUN_0052cc30(local_8,0x25);
    FUN_004bed50((void *)((int)local_8 + 0x24),0);
    break;
  case 4:
    local_64 = (void *)((int)param_1 + 0x154);
    bVar1 = FUN_004235c0(local_64,6);
    if (bVar1) {
      for (local_10 = 0; local_10 < 5; local_10 = local_10 + 1) {
        local_68 = (void *)((int)local_8 + 0x394);
        pvVar4 = (void *)FUN_00414580(local_68,local_10);
        FUN_00479040(pvVar4);
      }
      for (local_14 = 0; local_14 < 7; local_14 = local_14 + 1) {
        FUN_0052cdf0(local_8,local_14 + 0x7f);
      }
      FUN_0052cc30(local_8,0x25);
      FUN_0052cc30(local_8,0x56);
      FUN_0052cc30(local_8,0x55);
      FUN_0052ced0(local_8,0x12);
      FUN_004c6010((undefined4 *)((int)local_8 + 0x24));
      return 1;
    }
  }
  local_6c = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
  local_8c[7] = *(int *)((int)local_8 + 0x58d8);
  FUN_0051feb0(local_8,local_8c[7],local_6c,(int)local_8 + 0x58e4,-1);
  return 1;
}

