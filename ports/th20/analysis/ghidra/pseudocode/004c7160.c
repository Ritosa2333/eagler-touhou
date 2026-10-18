/* Entry: 0x004c7160; symbol: FUN_004c7160; body bytes: 1519 */

undefined4 __fastcall FUN_004c7160(void *param_1)

{
  int *piVar1;
  bool bVar2;
  undefined3 extraout_var;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  uint local_34;
  uint local_30;
  uint local_2c;
  uint local_24;
  uint local_1c;
  int local_18;
  int local_14;
  
  switch(*(undefined4 *)((int)param_1 + 0x100)) {
  case 0:
    FUN_0049c4e0((void *)((int)param_1 + 0x14),3);
    FUN_004bed50((void *)((int)param_1 + 0x14),*(int *)((int)param_1 + 0x10c));
    uVar5 = FUN_00421990(DAT_005b8898,0);
    *(undefined4 *)((int)param_1 + 0x104) = uVar5;
    uVar5 = FUN_00421990(DAT_005b8898,1);
    *(undefined4 *)((int)param_1 + 0x108) = uVar5;
    FUN_00423520((void *)((int)param_1 + 0xd0),0);
    FUN_004c61a0(param_1,1);
  case 1:
    bVar2 = FUN_00461070((void *)((int)param_1 + 0xc0),6);
    if ((bVar2) && (FUN_004c61a0(param_1,2), *(int *)((int)param_1 + 0x110) != 0)) {
      FUN_00420d80(DAT_005b8898);
      *(undefined4 *)((int)param_1 + 0x110) = 0;
    }
    break;
  case 2:
    FUN_004bfc00((undefined4 *)((int)param_1 + 0x14));
    if (DAT_005b88a4 == (void *)0x0) {
      local_14 = 0;
    }
    else {
      local_14 = FUN_0041a280(DAT_005b88a4,0x10);
    }
    if (local_14 != 0) {
      FUN_004bf040((void *)((int)param_1 + 0x14));
    }
    if (DAT_005b88a4 == (void *)0x0) {
      local_18 = 0;
    }
    else {
      local_18 = FUN_0041a280(DAT_005b88a4,0x20);
    }
    if (local_18 != 0) {
      FUN_004bf020((void *)((int)param_1 + 0x14));
    }
    bVar2 = FUN_0045d030((int *)((int)param_1 + 0x14));
    if (CONCAT31(extraout_var,bVar2) != 0) {
      FUN_00426d70(&DAT_005ba830,10,0);
      FUN_00423520((void *)((int)param_1 + 0xec),8);
    }
    iVar3 = FUN_0040c300((undefined4 *)((int)param_1 + 0x14));
    if (iVar3 == 0) {
      if (DAT_005b88a4 == (void *)0x0) {
        local_1c = 0;
      }
      else {
        local_1c = FUN_00419c00(DAT_005b88a4,0x40);
      }
      if (local_1c != 0) {
        iVar3 = FUN_0040c300((undefined4 *)((int)param_1 + 0x14));
        piVar1 = (int *)((int)param_1 + iVar3 * 4 + 0x104);
        *piVar1 = *piVar1 + -1;
        iVar3 = FUN_0040c300((undefined4 *)((int)param_1 + 0x14));
        if (*(int *)((int)param_1 + iVar3 * 4 + 0x104) < 0) {
          iVar3 = FUN_0041ca90((int)DAT_005b8898);
          iVar4 = FUN_0040c300((undefined4 *)((int)param_1 + 0x14));
          *(int *)((int)param_1 + iVar4 * 4 + 0x104) = iVar3 + -1;
        }
        FUN_00426d70(&DAT_005ba830,10,0);
        FUN_00423520((void *)((int)param_1 + 0xec),8);
      }
      if (DAT_005b88a4 == (void *)0x0) {
        local_24 = 0;
      }
      else {
        local_24 = FUN_00419c00(DAT_005b88a4,0x80);
      }
      if (local_24 != 0) {
        iVar3 = FUN_0040c300((undefined4 *)((int)param_1 + 0x14));
        piVar1 = (int *)((int)param_1 + iVar3 * 4 + 0x104);
        *piVar1 = *piVar1 + 1;
        iVar3 = FUN_0040c300((undefined4 *)((int)param_1 + 0x14));
        iVar4 = FUN_0041ca90((int)DAT_005b8898);
        if (iVar4 <= *(int *)((int)param_1 + iVar3 * 4 + 0x104)) {
          iVar3 = FUN_0040c300((undefined4 *)((int)param_1 + 0x14));
          *(undefined4 *)((int)param_1 + iVar3 * 4 + 0x104) = 0;
        }
        FUN_00426d70(&DAT_005ba830,10,0);
        FUN_00423520((void *)((int)param_1 + 0xec),8);
      }
      FUN_004c6210(DAT_005b8898,0,*(undefined4 *)((int)param_1 + 0x104));
      if (DAT_005b88a4 == (void *)0x0) {
        local_2c = 0;
      }
      else {
        local_2c = FUN_00419c00(DAT_005b88a4,0x8010f);
      }
      if (local_2c == 0) {
        return 1;
      }
      FUN_004c61a0(param_1,3);
      FUN_00423520((void *)((int)param_1 + 0xd0),0);
      FUN_00426d70(&DAT_005ba830,7,0);
      goto switchD_004c7184_caseD_3;
    }
    if (iVar3 == 1) {
      if (DAT_005b88a4 == (void *)0x0) {
        local_30 = 0;
      }
      else {
        local_30 = FUN_00419c00(DAT_005b88a4,0x8010f);
      }
      if (local_30 == 0) {
        return 1;
      }
      FUN_00426d70(&DAT_005ba830,7,0);
      FUN_004c6130(DAT_005b8898,0);
      FUN_004c6130(DAT_005b8898,1);
      puVar6 = &DAT_005b9eb0;
      puVar7 = &DAT_005c4f10;
      for (iVar3 = 0xc; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar7 = *puVar6;
        puVar6 = puVar6 + 1;
        puVar7 = puVar7 + 1;
      }
      puVar6 = &DAT_005b9eb0;
      puVar7 = &DAT_005c4f40;
      for (iVar3 = 0xc; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar7 = *puVar6;
        puVar6 = puVar6 + 1;
        puVar7 = puVar7 + 1;
      }
      FUN_00420d80(DAT_005b8898);
      return 1;
    }
    if (iVar3 != 2) {
      return 1;
    }
    if (DAT_005b88a4 == (void *)0x0) {
      local_34 = 0;
    }
    else {
      local_34 = FUN_00419c00(DAT_005b88a4,0x8010f);
    }
    if (local_34 == 0) {
      return 1;
    }
    FUN_004c61a0(param_1,4);
    FUN_00426d70(&DAT_005ba830,7,0);
  case 4:
    DAT_005c4f70 = FUN_00421990(DAT_005b8898,0);
    DAT_005c4f74 = FUN_00421990(DAT_005b8898,1);
    FUN_004c61d0(param_1,3);
    break;
  case 3:
switchD_004c7184_caseD_3:
    FUN_00423540((undefined4 *)((int)param_1 + 0xd0));
    bVar2 = FUN_004235c0((void *)((int)param_1 + 0xc0),10);
    if (bVar2) {
      uVar5 = FUN_0040c300((undefined4 *)((int)param_1 + 0x14));
      *(undefined4 *)((int)param_1 + 0x10c) = uVar5;
      FUN_004c60b0((int)param_1 + 0x14);
      FUN_004c61d0(param_1,2);
    }
    break;
  default:
    break;
  }
  return 1;
}

