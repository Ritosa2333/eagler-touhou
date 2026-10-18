/* Entry: 0x00527f00; symbol: FUN_00527f00; body bytes: 1497 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00527f00(void *param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  undefined4 *puVar3;
  undefined4 local_80;
  void *local_7c;
  int local_78;
  int local_74;
  int local_70;
  void *local_6c;
  void *local_68;
  undefined4 local_64;
  void *local_60;
  void *local_5c;
  void *local_58;
  void *local_54;
  void *local_50;
  int local_4c;
  void *local_48;
  int local_44;
  void *local_40;
  void *local_3c;
  void *local_38;
  void *local_34;
  void *local_30;
  void *local_2c;
  void *local_28;
  void *local_24;
  uint local_20;
  uint local_1c;
  int local_18;
  int local_14;
  undefined4 local_10;
  int local_c;
  void *local_8;
  
  local_10 = *(undefined4 *)((int)param_1 + 0x20);
  local_8 = param_1;
  switch(local_10) {
  case 0:
    local_24 = (void *)((int)param_1 + 0x24);
    FUN_0049c4e0(local_24,5);
    FUN_004bed50((void *)((int)local_8 + 0x24),0);
    if (-1 < DAT_005b0a58) {
      FUN_004bed50((void *)((int)local_8 + 0x24),DAT_005b0a58);
      DAT_005b0a58 = -1;
    }
    DAT_005c6128 = 1;
    iVar2 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
    FUN_0051fd80(local_8,iVar2);
    FUN_0052ce50(local_8,1);
  case 1:
    local_28 = (void *)((int)local_8 + 0x154);
    bVar1 = FUN_00461070(local_28,10);
    if (bVar1) {
      FUN_0052ce50(local_8,2);
    }
    break;
  case 2:
    FUN_004bfc00((undefined4 *)((int)param_1 + 0x24));
    if (DAT_005b889c == (void *)0x0) {
      local_14 = 0;
    }
    else {
      local_2c = DAT_005b889c;
      local_14 = FUN_0041a280(DAT_005b889c,0x10);
    }
    if (local_14 != 0) {
      local_30 = (void *)((int)local_8 + 0x24);
      FUN_004bf040(local_30);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_18 = 0;
    }
    else {
      local_34 = DAT_005b889c;
      local_18 = FUN_0041a280(DAT_005b889c,0x20);
    }
    if (local_18 != 0) {
      local_38 = (void *)((int)local_8 + 0x24);
      FUN_004bf020(local_38);
    }
    bVar1 = FUN_0045d030((int *)((int)local_8 + 0x24));
    if (CONCAT31(extraout_var,bVar1) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
      iVar2 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      FUN_0051fd80(local_8,iVar2);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_1c = 0;
    }
    else {
      local_3c = DAT_005b889c;
      local_1c = FUN_00419c00(DAT_005b889c,0x106);
    }
    if (local_1c == 0) {
      if (DAT_005b889c == (void *)0x0) {
        local_20 = 0;
      }
      else {
        local_40 = DAT_005b889c;
        local_20 = FUN_00419c00(DAT_005b889c,0x80001);
      }
      if (local_20 != 0) {
        iVar2 = FUN_0050fc50(DAT_005c6108);
        local_48 = (void *)(iVar2 + 0xb08);
        iVar2 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
        local_44 = *(int *)((int)local_8 + iVar2 * 4 + 0x58e4);
        iVar2 = FUN_00486e10(local_48,local_44);
        if (*(int *)(iVar2 + 200) == 0) {
          iVar2 = FUN_0050fc50(DAT_005c6108);
          local_50 = (void *)(iVar2 + 0xb08);
          iVar2 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
          local_4c = *(int *)((int)local_8 + iVar2 * 4 + 0x58e4);
          iVar2 = FUN_00486e10(local_50,local_4c);
          if (*(int *)(iVar2 + 0xcc) == 0) {
            FUN_00426d70(&DAT_005ba830,0x10,0);
            break;
          }
        }
        local_54 = (void *)((int)local_8 + 0x394);
        iVar2 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
        local_58 = (void *)FUN_00414580(local_54,iVar2);
        FUN_0044ef90(local_58,6);
        FUN_0052ce50(local_8,3);
        FUN_00426d70(&DAT_005ba830,7,0);
        FUN_004d99d0(DAT_0056fe5c);
        FUN_00426d70(&DAT_005ba830,0x32,0);
      }
    }
    else {
      FUN_0052ce50(local_8,4);
      FUN_00426d70(&DAT_005ba830,9,0);
    }
    break;
  case 3:
    iVar2 = FUN_00437520(0);
    iVar2 = FUN_00438520(iVar2);
    if (iVar2 == 0) {
      local_5c = (void *)((int)local_8 + 0x154);
      FUN_00429420(local_5c);
      return 1;
    }
    local_60 = (void *)((int)local_8 + 0x154);
    bVar1 = FUN_004785e0(local_60,10);
    if (bVar1) {
      FUN_004a0aa0(DAT_0056cda8,DAT_00570388);
      local_64 = FUN_00437520(0);
      puVar3 = FUN_0049dcf0(&local_80,0,0,0);
      _DAT_005c4f04 = *puVar3;
      FUN_0051c6b0(&DAT_005c4f04,0);
    }
    local_68 = (void *)((int)local_8 + 0x154);
    bVar1 = FUN_004235c0(local_68,0x28);
    if (bVar1) {
      FUN_004c60b0((int)local_8 + 0x24);
      FUN_0052ced0(local_8,2);
      FUN_004be360(&DAT_005ba568,*(int *)((int)local_8 + 0x58d8) + 1);
      iVar2 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      FUN_0050a7f0(&DAT_005ba568,*(undefined4 *)((int)local_8 + iVar2 * 4 + 0x58e4));
      iVar2 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      FUN_004be1b0(&DAT_005ba568,
                   *(undefined4 *)(&DAT_005af058 + *(int *)((int)local_8 + iVar2 * 4 + 0x58e4) * 4))
      ;
      FUN_004be250(&DAT_005c4d40,7);
      DAT_005c6128 = 5;
      DAT_005b0a54 = *(undefined4 *)((int)local_8 + 0x58d8);
      DAT_005b0a5c = *(undefined4 *)((int)local_8 + 0x58dc);
      DAT_005b0a58 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
    }
    break;
  case 4:
    local_6c = (void *)((int)param_1 + 0x154);
    bVar1 = FUN_004235c0(local_6c,6);
    if (bVar1) {
      FUN_0052cc30(local_8,0x56);
      FUN_0052ced0(local_8,0x13);
      FUN_004e68a0((int)local_8 + 0x24);
      FUN_004c6010((undefined4 *)((int)local_8 + 0x24));
      return 1;
    }
  }
  local_70 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
  local_74 = *(int *)((int)local_8 + 0x58dc);
  local_78 = *(int *)((int)local_8 + 0x58d8);
  FUN_0051feb0(local_8,local_78,local_74,(int)local_8 + 0x58e4,local_70);
  for (local_c = 0; local_c < 5; local_c = local_c + 1) {
    if (*(int *)((int)local_8 + local_c * 4 + 0x58e4) < 0) {
      local_7c = (void *)((int)local_8 + 0x24);
      FUN_0051d790(local_7c,local_c);
    }
  }
  return 1;
}

