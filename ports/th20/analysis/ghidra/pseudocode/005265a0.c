/* Entry: 0x005265a0; symbol: FUN_005265a0; body bytes: 1229 */

undefined4 __fastcall FUN_005265a0(void *param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined3 extraout_var;
  int iVar3;
  undefined4 local_84;
  undefined4 local_80;
  undefined4 local_7c;
  void *local_78;
  int local_74;
  void *local_70;
  undefined4 *local_6c;
  undefined4 *local_68;
  void *local_64;
  void *local_60;
  void *local_5c;
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
  undefined4 local_30;
  void *local_2c;
  uint local_28;
  uint local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  undefined4 local_10;
  int local_c;
  void *local_8;
  
  local_c = 0x4e;
  local_10 = *(undefined4 *)((int)param_1 + 0x20);
  local_8 = param_1;
  switch(local_10) {
  case 0:
    local_2c = (void *)((int)param_1 + 0x390);
    bVar1 = FUN_00449430(local_2c,0);
    if (bVar1) {
      local_30 = FUN_00437950(DAT_005c0698);
      puVar2 = FUN_00450c70(&local_7c,(uint *)0x0,0x13,-1,(undefined4 *)0x0);
      *(undefined4 *)((int)local_8 + 0x390) = *puVar2;
    }
    local_34 = (void *)((int)local_8 + 0x24);
    FUN_0049c4e0(local_34,3);
    local_38 = (void *)((int)local_8 + 0x24);
    FUN_004bfbe0(local_38,1);
    FUN_0052cc30(local_8,local_c);
    FUN_0052cf80(local_8,local_c);
    FUN_0052cc90(local_8,local_c);
    local_3c = (void *)((int)local_8 + 0x24);
    iVar3 = FUN_0051e080(local_3c,7);
    FUN_0052ccd0(local_8,local_c,(int)(short)iVar3);
    FUN_0052cf80(local_8,0x2e);
    FUN_0052ce50(local_8,1);
  case 1:
    local_40 = (void *)((int)local_8 + 0x154);
    bVar1 = FUN_00461070(local_40,6);
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
      local_44 = DAT_005b889c;
      local_14 = FUN_0041a280(DAT_005b889c,0x10);
    }
    if (local_14 == 0) {
      if (DAT_005b889c == (void *)0x0) {
        local_18 = 0;
      }
      else {
        local_48 = DAT_005b889c;
        local_18 = FUN_0041a280(DAT_005b889c,0x40);
      }
      if (local_18 != 0) goto LAB_00526758;
    }
    else {
LAB_00526758:
      local_4c = (void *)((int)local_8 + 0x24);
      FUN_004bf040(local_4c);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_1c = 0;
    }
    else {
      local_50 = DAT_005b889c;
      local_1c = FUN_0041a280(DAT_005b889c,0x20);
    }
    if (local_1c == 0) {
      if (DAT_005b889c == (void *)0x0) {
        local_20 = 0;
      }
      else {
        local_54 = DAT_005b889c;
        local_20 = FUN_0041a280(DAT_005b889c,0x80);
      }
      if (local_20 != 0) goto LAB_005267f3;
    }
    else {
LAB_005267f3:
      local_58 = (void *)((int)local_8 + 0x24);
      FUN_004bf020(local_58);
    }
    bVar1 = FUN_0045d030((int *)((int)local_8 + 0x24));
    if (CONCAT31(extraout_var,bVar1) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
      FUN_0052cc90(local_8,local_c);
      local_5c = (void *)((int)local_8 + 0x24);
      iVar3 = FUN_0051e080(local_5c,7);
      FUN_0052ccd0(local_8,local_c,(int)(short)iVar3);
    }
    if (DAT_005b889c == (void *)0x0) {
      local_24 = 0;
    }
    else {
      local_60 = DAT_005b889c;
      local_24 = FUN_00419c00(DAT_005b889c,0x106);
    }
    if (local_24 == 0) {
      if (DAT_005b889c == (void *)0x0) {
        local_28 = 0;
      }
      else {
        local_64 = DAT_005b889c;
        local_28 = FUN_00419c00(DAT_005b889c,0x80001);
      }
      if (local_28 != 0) {
        FUN_0052ccd0(local_8,local_c,6);
        iVar3 = FUN_004640a0(0x5ba568);
        if (iVar3 < 4) {
          iVar3 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
          local_68 = FUN_0052c240(local_8,&local_80,local_c,iVar3 + 0x30);
          FUN_0044ef90(local_68,2);
        }
        else {
          local_6c = FUN_0052c240(local_8,&local_84,local_c,0x34);
          FUN_0044ef90(local_6c,2);
        }
        FUN_0052ce50(local_8,3);
        FUN_00426d70(&DAT_005ba830,7,0);
      }
    }
    else {
      FUN_0052ce50(local_8,4);
      FUN_00426d70(&DAT_005ba830,9,0);
      FUN_0052cc30(local_8,local_c);
    }
    break;
  case 3:
    local_70 = (void *)((int)param_1 + 0x154);
    bVar1 = FUN_004235c0(local_70,0xe);
    if (bVar1) {
      FUN_0052cc30(local_8,0x2e);
      local_74 = FUN_0040c300((undefined4 *)((int)local_8 + 0x24));
      if (local_74 == 2) {
        FUN_0052ced0(local_8,0x17);
      }
      else {
        FUN_0052ced0(local_8,0xb);
      }
      FUN_004c60b0((int)local_8 + 0x24);
    }
    break;
  case 4:
    local_78 = (void *)((int)param_1 + 0x154);
    bVar1 = FUN_004235c0(local_78,6);
    if (bVar1) {
      FUN_0052cc30(local_8,0x2e);
      FUN_0052ced0(local_8,1);
      FUN_00479040((void *)((int)local_8 + 0x390));
      FUN_004117a0((void *)((int)local_8 + 0x390),0);
      FUN_0052ced0(local_8,1);
      FUN_004c6010((undefined4 *)((int)local_8 + 0x24));
    }
  }
  return 1;
}

