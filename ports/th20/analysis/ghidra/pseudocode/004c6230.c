/* Entry: 0x004c6230; symbol: FUN_004c6230; body bytes: 2267 */

undefined4 __fastcall FUN_004c6230(void *param_1)

{
  byte bVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  bool bVar5;
  bool bVar6;
  int *piVar7;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  int iVar8;
  undefined2 *puVar9;
  DName *pDVar10;
  int iVar11;
  undefined3 extraout_var_01;
  undefined3 extraout_var_02;
  undefined4 *puVar12;
  int local_50;
  int local_4c;
  int local_48;
  int local_44;
  int local_40;
  undefined2 *local_38;
  undefined4 *local_34;
  undefined2 *local_30;
  int local_28;
  int local_24;
  int local_1c;
  int local_18;
  
  piVar7 = (int *)FUN_004c5e20(DAT_005b8898,*(int *)((int)param_1 + 0x10c));
  bVar5 = FUN_004c5e50(piVar7);
  if (CONCAT31(extraout_var,bVar5) == 0) {
    piVar7 = (int *)FUN_004c5e20(DAT_005b8898,*(int *)((int)param_1 + 0x10c));
    bVar5 = FUN_004c5eb0(piVar7);
    if (CONCAT31(extraout_var_00,bVar5) == 0) {
      local_30 = (undefined2 *)((int)param_1 + 0xa0);
      iVar8 = FUN_004217f0(DAT_005b8898,*(int *)((int)param_1 + 0x10c));
      local_38 = (undefined2 *)(iVar8 + 0x20);
      local_34 = (undefined4 *)&DAT_005b9ed0;
    }
    else {
      local_30 = (undefined2 *)((int)param_1 + 0x80);
      iVar8 = FUN_004217f0(DAT_005b8898,*(int *)((int)param_1 + 0x10c));
      local_38 = (undefined2 *)(iVar8 + 0x10);
      local_34 = (undefined4 *)&DAT_005b9ec0;
    }
  }
  else {
    local_30 = (undefined2 *)((int)param_1 + 0x60);
    local_38 = (undefined2 *)FUN_004217f0(DAT_005b8898,*(int *)((int)param_1 + 0x10c));
    local_34 = &DAT_005b9eb0;
  }
  switch(*(undefined4 *)((int)param_1 + 0x100)) {
  case 0:
    pDVar10 = (DName *)FUN_004c5e20(DAT_005b8898,*(int *)((int)param_1 + 0x10c));
    iVar8 = DName::isEmpty(pDVar10);
    if (iVar8 == 0) {
      FUN_0049c4e0((void *)((int)param_1 + 0x14),6);
    }
    else {
      FUN_0049c4e0((void *)((int)param_1 + 0x14),9);
    }
    FUN_004bed50((void *)((int)param_1 + 0x14),0);
    FUN_00423520((void *)((int)param_1 + 0xd0),0);
    FUN_004c61a0(param_1,1);
    puVar9 = (undefined2 *)FUN_004217f0(DAT_005b8898,*(int *)((int)param_1 + 0x10c));
    FUN_004c5ee0((undefined2 *)((int)param_1 + 0x60),puVar9);
    iVar8 = FUN_004217f0(DAT_005b8898,*(int *)((int)param_1 + 0x10c));
    FUN_004c5ee0((undefined2 *)((int)param_1 + 0x80),(undefined2 *)(iVar8 + 0x10));
    iVar8 = FUN_004217f0(DAT_005b8898,*(int *)((int)param_1 + 0x10c));
    FUN_004c5ee0((undefined2 *)((int)param_1 + 0xa0),(undefined2 *)(iVar8 + 0x20));
    FUN_004c61a0(param_1,1);
  case 1:
    bVar5 = FUN_00461070((void *)((int)param_1 + 0xc0),6);
    if (bVar5) {
      FUN_004c61a0(param_1,2);
    }
    break;
  case 2:
    FUN_004bfc00((undefined4 *)((int)param_1 + 0x14));
    iVar8 = FUN_004c5e20(DAT_005b8898,*(int *)((int)param_1 + 0x10c));
    iVar8 = FUN_004c5e00(iVar8);
    bVar5 = false;
    pDVar10 = (DName *)FUN_004c5e20(DAT_005b8898,*(int *)((int)param_1 + 0x10c));
    iVar11 = DName::isEmpty(pDVar10);
    if (iVar11 == 0) {
      if ((*(int *)((int)param_1 + 0x10c) < 0) ||
         ((&DAT_005b889c)[*(int *)((int)param_1 + 0x10c)] == 0)) {
        local_4c = 0;
      }
      else {
        local_4c = FUN_0041a280((void *)(&DAT_005b889c)[*(int *)((int)param_1 + 0x10c)],0x10);
      }
      if (local_4c != 0) {
        FUN_004bf040((void *)((int)param_1 + 0x14));
      }
      if ((*(int *)((int)param_1 + 0x10c) < 0) ||
         ((&DAT_005b889c)[*(int *)((int)param_1 + 0x10c)] == 0)) {
        local_50 = 0;
      }
      else {
        local_50 = FUN_0041a280((void *)(&DAT_005b889c)[*(int *)((int)param_1 + 0x10c)],0x20);
      }
      if (local_50 != 0) {
        FUN_004bf020((void *)((int)param_1 + 0x14));
      }
      bVar6 = FUN_0045d030((int *)((int)param_1 + 0x14));
      if (CONCAT31(extraout_var_02,bVar6) != 0) {
        FUN_00426d70(&DAT_005ba830,10,0);
        FUN_00423520((void *)((int)param_1 + 0xec),8);
      }
      for (local_28 = 0; local_28 < 0x1f; local_28 = local_28 + 1) {
        if ((*(byte *)(iVar8 + local_28) & 0x80) != 0) {
          bVar5 = true;
        }
      }
      iVar11 = FUN_0040c300((undefined4 *)((int)param_1 + 0x14));
      if (iVar11 == 4) {
        if (!bVar5) {
          return 1;
        }
        bVar5 = FUN_004235c0((void *)((int)param_1 + 0xc0),0x14);
        if (!bVar5) {
          return 1;
        }
        FUN_004c5ee0(local_30,(undefined2 *)local_34);
        FUN_00426d70(&DAT_005ba830,7,0);
        FUN_00423520((void *)((int)param_1 + 0xc0),0);
        return 1;
      }
      if (iVar11 != 5) {
        local_1c = 0;
        while( true ) {
          if (0x1e < local_1c) {
            return 1;
          }
          if ((*(byte *)(iVar8 + local_1c) & 0x80) != 0) break;
          local_1c = local_1c + 1;
        }
        iVar8 = FUN_0040c300((undefined4 *)((int)param_1 + 0x14));
        FUN_004c7dd0(param_1,iVar8,local_1c);
        return 1;
      }
    }
    else {
      if (DAT_005b88a8 == (void *)0x0) {
        local_40 = 0;
      }
      else {
        local_40 = FUN_0041a280(DAT_005b88a8,0x10);
      }
      if (local_40 != 0) {
        FUN_004bf040((void *)((int)param_1 + 0x14));
      }
      if (DAT_005b88a8 == (void *)0x0) {
        local_44 = 0;
      }
      else {
        local_44 = FUN_0041a280(DAT_005b88a8,0x20);
      }
      if (local_44 != 0) {
        FUN_004bf020((void *)((int)param_1 + 0x14));
      }
      bVar6 = FUN_0045d030((int *)((int)param_1 + 0x14));
      if (CONCAT31(extraout_var_01,bVar6) != 0) {
        FUN_00426d70(&DAT_005ba830,10,0);
        FUN_00423520((void *)((int)param_1 + 0xec),8);
      }
      for (local_24 = 0; local_24 < 0x48; local_24 = local_24 + 1) {
        if ((*(byte *)(iVar8 + (uint)(byte)(&DAT_005ae270)[local_24]) & 0x80) != 0) {
          bVar5 = true;
        }
      }
      iVar11 = FUN_0040c300((undefined4 *)((int)param_1 + 0x14));
      if (iVar11 != 8) {
        bVar5 = FUN_004235c0((void *)((int)param_1 + 0xc0),0xf);
        if (!bVar5) {
          return 1;
        }
        local_18 = 0;
        while( true ) {
          if (0x47 < local_18) {
            return 1;
          }
          if ((*(byte *)(iVar8 + (uint)(byte)(&DAT_005ae270)[local_18]) & 0x80) != 0) break;
          local_18 = local_18 + 1;
        }
        bVar1 = (&DAT_005ae270)[local_18];
        iVar8 = FUN_0040c300((undefined4 *)((int)param_1 + 0x14));
        FUN_004c7dd0(param_1,iVar8,(uint)bVar1);
        FUN_00423520((void *)((int)param_1 + 0xc0),0);
        FUN_004bf020((void *)((int)param_1 + 0x14));
        FUN_00426d70(&DAT_005ba830,7,0);
        FUN_00423520((void *)((int)param_1 + 0xec),8);
        return 1;
      }
      if (DAT_005b88a8 == (void *)0x0) {
        local_48 = 0;
      }
      else {
        local_48 = FUN_0041a280(DAT_005b88a8,0x80001);
      }
      if (local_48 != 0) {
        bVar5 = true;
      }
      bVar6 = FUN_004235c0((void *)((int)param_1 + 0xc0),0xf);
      if (!bVar6) {
        return 1;
      }
    }
    if (bVar5) {
      FUN_004c7d10(local_30,local_38);
      puVar12 = (undefined4 *)FUN_004217f0(DAT_005b8898,*(int *)((int)param_1 + 0x10c));
      uVar2 = puVar12[1];
      uVar3 = puVar12[2];
      uVar4 = puVar12[3];
      iVar8 = *(int *)((int)param_1 + 0x10c);
      (&DAT_005c4f10)[iVar8 * 0xc] = *puVar12;
      (&DAT_005c4f14)[iVar8 * 0xc] = uVar2;
      *(undefined4 *)(&DAT_005c4f18 + iVar8 * 0x30) = uVar3;
      *(undefined4 *)(&DAT_005c4f1c + iVar8 * 0x30) = uVar4;
      iVar8 = FUN_004217f0(DAT_005b8898,*(int *)((int)param_1 + 0x10c));
      uVar2 = *(undefined4 *)(iVar8 + 0x14);
      uVar3 = *(undefined4 *)(iVar8 + 0x18);
      uVar4 = *(undefined4 *)(iVar8 + 0x1c);
      iVar11 = *(int *)((int)param_1 + 0x10c) * 0x30;
      *(undefined4 *)(&DAT_005c4f20 + iVar11) = *(undefined4 *)(iVar8 + 0x10);
      *(undefined4 *)(&DAT_005c4f24 + iVar11) = uVar2;
      *(undefined4 *)(&DAT_005c4f28 + iVar11) = uVar3;
      *(undefined4 *)(&DAT_005c4f2c + iVar11) = uVar4;
      iVar8 = FUN_004217f0(DAT_005b8898,*(int *)((int)param_1 + 0x10c));
      uVar2 = *(undefined4 *)(iVar8 + 0x24);
      uVar3 = *(undefined4 *)(iVar8 + 0x28);
      uVar4 = *(undefined4 *)(iVar8 + 0x2c);
      iVar11 = *(int *)((int)param_1 + 0x10c) * 0x30;
      *(undefined4 *)(&DAT_005c4f30 + iVar11) = *(undefined4 *)(iVar8 + 0x20);
      *(undefined4 *)(&DAT_005c4f34 + iVar11) = uVar2;
      *(undefined4 *)(&DAT_005c4f38 + iVar11) = uVar3;
      *(undefined4 *)(&DAT_005c4f3c + iVar11) = uVar4;
      FUN_00426d70(&DAT_005ba830,9,0);
      FUN_004c61a0(param_1,3);
    }
    break;
  case 3:
    FUN_00423540((undefined4 *)((int)param_1 + 0xd0));
    bVar5 = FUN_004235c0((void *)((int)param_1 + 0xc0),10);
    if (bVar5) {
      FUN_004c6010((undefined4 *)((int)param_1 + 0x14));
      FUN_004c61d0(param_1,1);
    }
    break;
  case 4:
    FUN_004c6010((undefined4 *)((int)param_1 + 0x14));
    FUN_004c61d0(param_1,1);
  }
  return 1;
}

