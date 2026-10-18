/* Entry: 0x00528fc0; symbol: FUN_00528fc0; body bytes: 1099 */

undefined4 __fastcall FUN_00528fc0(void *param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int iVar3;
  undefined4 uVar4;
  undefined4 local_5c;
  void *local_58;
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
  void *local_28;
  undefined4 local_24;
  void *local_20;
  uint local_1c;
  uint local_18;
  int local_14;
  int local_10;
  undefined4 local_c;
  void *local_8;
  
  local_c = *(undefined4 *)((int)param_1 + 0x20);
  local_8 = param_1;
  switch(local_c) {
  case 0:
    local_20 = (void *)((int)param_1 + 0x390);
    bVar1 = FUN_00449430(local_20,0);
    if (bVar1) {
      local_24 = FUN_00437950(DAT_005c0698);
      puVar2 = FUN_00450c70(&local_5c,(uint *)0x0,0x13,-1,(undefined4 *)0x0);
      *(undefined4 *)((int)local_8 + 0x390) = *puVar2;
    }
    local_28 = (void *)((int)local_8 + 0x24);
    FUN_0049c4e0(local_28,7);
    FUN_004e68a0((int)local_8 + 0x24);
    FUN_0052cd10(local_8,0xc,0x2f);
    bVar1 = FUN_0052c750(local_8,0x55);
    if (CONCAT31(extraout_var,bVar1) == 0) {
      FUN_0052cf80(local_8,0x55);
    }
    FUN_0052ce50(local_8,1);
    if (DAT_005c6128 != 5) {
      FUN_0052cf80(local_8,0x2c);
      goto switchD_00528fdf_caseD_1;
    }
    FUN_004bed50((void *)((int)local_8 + 0x24),DAT_005b0a54);
    DAT_005b0a54 = -1;
    FUN_0052cc90(local_8,0x55);
    local_2c = (void *)((int)local_8 + 0x24);
    iVar3 = FUN_0051e080(local_2c,7);
    FUN_0052cd10(local_8,0x55,(int)(short)iVar3);
    FUN_0052cd10(local_8,0x55,6);
    FUN_0052cc90(local_8,0x88);
    goto LAB_00529371;
  case 1:
switchD_00528fdf_caseD_1:
    local_30 = (void *)((int)local_8 + 0x154);
    bVar1 = FUN_00461070(local_30,10);
    if (bVar1) {
      FUN_0052ce50(local_8,2);
      FUN_0052cc90(local_8,0x55);
      local_34 = (void *)((int)local_8 + 0x24);
      iVar3 = FUN_0051e080(local_34,7);
      FUN_0052ccd0(local_8,0x55,(int)(short)iVar3);
      FUN_0052cc90(local_8,0x88);
    }
    break;
  case 2:
    FUN_004bfc00((undefined4 *)((int)param_1 + 0x24));
    if (DAT_005b889c == (void *)0x0) {
      local_10 = 0;
    }
    else {
      local_38 = DAT_005b889c;
      local_10 = FUN_0041a280(DAT_005b889c,0x10);
    }
    if (local_10 != 0) {
      local_3c = (void *)((int)local_8 + 0x24);
      FUN_004bf040(local_3c);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_14 = 0;
    }
    else {
      local_40 = DAT_005b889c;
      local_14 = FUN_0041a280(DAT_005b889c,0x20);
    }
    if (local_14 != 0) {
      local_44 = (void *)((int)local_8 + 0x24);
      FUN_004bf020(local_44);
    }
    bVar1 = FUN_0045d030((int *)((int)local_8 + 0x24));
    if (CONCAT31(extraout_var_00,bVar1) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
      FUN_0052cc90(local_8,0x55);
      local_48 = (void *)((int)local_8 + 0x24);
      iVar3 = FUN_0051e080(local_48,7);
      FUN_0052ccd0(local_8,0x55,(int)(short)iVar3);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_18 = 0;
    }
    else {
      local_4c = DAT_005b889c;
      local_18 = FUN_00419c00(DAT_005b889c,0x106);
    }
    if (local_18 == 0) {
      if (DAT_005b889c == (void *)0x0) {
        local_1c = 0;
      }
      else {
        local_50 = DAT_005b889c;
        local_1c = FUN_00419c00(DAT_005b889c,0x80001);
      }
      if (local_1c != 0) {
        FUN_0052ccd0(local_8,0x55,6);
        FUN_0052ce50(local_8,3);
        FUN_00426d70(&DAT_005ba830,7,0);
        DAT_005b0a50 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      }
    }
    else {
      FUN_0052ce50(local_8,4);
      FUN_00426d70(&DAT_005ba830,9,0);
      DAT_005b0a50 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
    }
    break;
  case 3:
    local_54 = (void *)((int)param_1 + 0x154);
    bVar1 = FUN_004235c0(local_54,0x14);
    if (!bVar1) {
      return 1;
    }
LAB_00529371:
    FUN_0052cc30(local_8,0x2c);
    FUN_0052ced0(local_8,0x13);
    uVar4 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
    *(undefined4 *)((int)local_8 + 0x58d8) = uVar4;
    FUN_004c60b0((int)local_8 + 0x24);
    FUN_004bed50((void *)((int)local_8 + 0x24),0);
    break;
  case 4:
    local_58 = (void *)((int)param_1 + 0x154);
    bVar1 = FUN_004235c0(local_58,6);
    if (bVar1) {
      FUN_0052cc30(local_8,0x2c);
      FUN_0052cc30(local_8,0x55);
      FUN_0052cd10(local_8,0xc,0x30);
      FUN_0052ced0(local_8,7);
      FUN_004c6010((undefined4 *)((int)local_8 + 0x24));
    }
  }
  return 1;
}

