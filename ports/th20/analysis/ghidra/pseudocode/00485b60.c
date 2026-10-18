/* Entry: 0x00485b60; symbol: FUN_00485b60; body bytes: 1844 */

undefined4 __fastcall FUN_00485b60(void *param_1)

{
  bool bVar1;
  undefined4 uVar2;
  int iVar3;
  int *piVar4;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  float *pfVar5;
  float10 fVar6;
  undefined1 local_8c [12];
  undefined1 local_80 [12];
  undefined1 local_74 [12];
  undefined1 local_68 [12];
  float local_5c;
  float local_58;
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
  float local_24;
  float local_20;
  float local_1c;
  undefined4 *local_18;
  float local_14;
  int local_10;
  int local_c;
  void *local_8;
  
  *(uint *)((int)param_1 + 0x14) = *(uint *)((int)param_1 + 0x14) | 0x400;
  local_18 = (undefined4 *)((int)param_1 + 0x508);
  local_8 = param_1;
  FUN_00423540(local_18);
  if ((*(uint *)((int)local_8 + 0x14) >> 3 & 1) == 0) {
    fVar6 = FUN_004292e0((float *)&DAT_005aefe4);
    local_14 = (float)fVar6;
    if ((((*(int *)((int)local_8 + 0x50) != 2) && (*(int *)((int)local_8 + 0x50) != 3)) &&
        (*(int *)((int)local_8 + 0x50) != 4)) && ((*(uint *)((int)local_8 + 0x14) >> 9 & 1) == 0)) {
      fVar6 = FUN_00423bd0((int)local_8 + 0x4d8);
      local_20 = (float)fVar6;
      local_1c = local_20;
      fVar6 = FUN_00423bd0((int)local_8 + 0x4e8);
      local_24 = (float)fVar6;
      FUN_004292a0(&DAT_005aefe4,local_20 * local_24);
    }
    if ((*(uint *)((int)local_8 + 0x90) >> 0x16 & 1) != 0) {
      fVar6 = FUN_0042a110((float *)((int)local_8 + 0x474));
      *(float *)((int)local_8 + 0x48) = (float)fVar6;
      iVar3 = FUN_0041caf0((int)local_8 + 0x474);
      if ((iVar3 == 0) &&
         (*(uint *)((int)local_8 + 0x90) = *(uint *)((int)local_8 + 0x90) & 0xffbfffff,
         *(float *)((int)local_8 + 0x48) == DAT_0056c8cc)) {
        *(uint *)((int)local_8 + 0x14) = *(uint *)((int)local_8 + 0x14) & 0xffffffbf;
      }
    }
    local_10 = *(int *)((int)local_8 + 0x50) + -1;
    switch(local_10) {
    case 0:
switchD_00485cc0_caseD_0:
      do {
        while (((*(uint *)((int)local_8 + 0x90) >> 0x1a & 1) == 0 &&
               (FUN_0047dcf0(local_8), *(int *)((int)local_8 + 0x50) == 2))) {
LAB_00485cf7:
          FUN_0047c5f0((void *)((int)local_8 + 0x4d8),DAT_0056c8cc);
          FUN_0047c5f0((void *)((int)local_8 + 0x4e8),DAT_0056c8cc);
          FUN_004292a0(&DAT_005aefe4,DAT_0056c8cc);
          piVar4 = (int *)FUN_0045d0c0(*(int *)((int)local_8 + 0x1c));
          if (*piVar4 == 0) goto switchD_00485cc0_default;
          *(undefined4 *)((int)local_8 + 0x50) = 1;
        }
        local_c = 0;
        if ((*(uint *)((int)local_8 + 0x90) & 1) != 0) {
          bVar1 = FUN_004840e0((int)local_8);
          local_c = CONCAT31(extraout_var,bVar1) + local_c;
        }
        if ((*(uint *)((int)local_8 + 0x90) >> 2 & 1) != 0) {
          iVar3 = FUN_00483f00((int)local_8);
          local_c = iVar3 + local_c;
        }
        if ((*(uint *)((int)local_8 + 0x90) >> 0x15 & 1) != 0) {
          iVar3 = FUN_00483d20((int)local_8);
          local_c = iVar3 + local_c;
        }
        if ((*(uint *)((int)local_8 + 0x90) >> 3 & 1) != 0) {
          bVar1 = FUN_00483920((int)local_8);
          local_c = CONCAT31(extraout_var_00,bVar1) + local_c;
        }
        if ((*(uint *)((int)local_8 + 0x90) >> 4 & 1) != 0) {
          iVar3 = FUN_00483580((int)local_8);
          local_c = iVar3 + local_c;
        }
        if ((*(uint *)((int)local_8 + 0x90) >> 6 & 1) != 0) {
          iVar3 = FUN_00482a60((int)local_8);
          local_c = iVar3 + local_c;
        }
        if (*(int *)((int)local_8 + 0x90) < 0) {
          iVar3 = FUN_00480db0((int)local_8);
          local_c = iVar3 + local_c;
        }
        if ((*(uint *)((int)local_8 + 0x90) >> 0x11 & 1) != 0) {
          iVar3 = FUN_004827e0((int)local_8);
          local_c = iVar3 + local_c;
        }
        if ((*(uint *)((int)local_8 + 0x90) >> 0x13 & 1) != 0) {
          bVar1 = FUN_0047c800((int)local_8);
          local_c = CONCAT31(extraout_var_01,bVar1) + local_c;
        }
        if ((*(uint *)((int)local_8 + 0x90) >> 8 & 1) != 0) {
          iVar3 = FUN_00481200((int)local_8);
          local_c = iVar3 + local_c;
        }
        if ((*(uint *)((int)local_8 + 0x90) >> 0x1e & 1) != 0) {
          local_2c = (void *)((int)local_8 + 0xa0);
          local_30 = (void *)FUN_0047c680(local_2c,5);
          bVar1 = FUN_00423590(local_30,0);
          if (bVar1) {
            *(uint *)((int)local_8 + 0x90) = *(uint *)((int)local_8 + 0x90) & 0xbfffffff;
            local_c = local_c + 1;
          }
          else {
            local_34 = (void *)((int)local_8 + 0xa0);
            local_38 = (void *)FUN_0047c680(local_34,5);
            FUN_00429420(local_38);
          }
        }
        if ((*(uint *)((int)local_8 + 0x90) >> 0x1a & 1) != 0) {
          local_3c = (void *)((int)local_8 + 0xa0);
          local_40 = (void *)FUN_0047c680(local_3c,0xd);
          bVar1 = FUN_00423590(local_40,0);
          if (bVar1) {
            *(uint *)((int)local_8 + 0x90) = *(uint *)((int)local_8 + 0x90) & 0xfbffffff;
            *(uint *)((int)local_8 + 0x14) = *(uint *)((int)local_8 + 0x14) & 0xfffffdff;
            local_c = local_c + 1;
            goto switchD_00485cc0_caseD_0;
          }
          *(uint *)((int)local_8 + 0x14) = *(uint *)((int)local_8 + 0x14) | 0x200;
          local_44 = (void *)((int)local_8 + 0xa0);
          local_48 = (void *)FUN_0047c680(local_44,0xd);
          FUN_00429420(local_48);
        }
      } while (local_c != 0);
      if ((*(uint *)((int)local_8 + 0x14) >> 9 & 1) == 0) {
        local_4c = (void *)((int)local_8 + 0x70);
        fVar6 = FUN_004292e0((float *)&DAT_005aefe4);
        pfVar5 = (float *)FUN_004292f0(local_4c,local_74,(float)fVar6);
        FUN_004296e0((void *)((int)local_8 + 100),pfVar5);
      }
      break;
    case 1:
      local_28 = (void *)((int)local_8 + 0x70);
      pfVar5 = (float *)FUN_00452ff0(local_28,local_68,DAT_0056c8d0);
      FUN_004296e0((void *)((int)local_8 + 100),pfVar5);
      goto LAB_00485cf7;
    case 2:
      FUN_0047c5f0((void *)((int)local_8 + 0x4d8),DAT_0056c8cc);
      FUN_0047c5f0((void *)((int)local_8 + 0x4e8),DAT_0056c8cc);
      FUN_004292a0(&DAT_005aefe4,DAT_0056c8cc);
      local_50 = (void *)((int)local_8 + 0x70);
      fVar6 = FUN_004292e0((float *)&DAT_005aefe4);
      local_54 = FUN_004292f0(local_50,local_80,(float)fVar6);
      pfVar5 = (float *)FUN_00452ff0(local_54,local_8c,DAT_0056c8d0);
      FUN_004296e0((void *)((int)local_8 + 100),pfVar5);
      break;
    case 3:
      FUN_0047c5f0((void *)((int)local_8 + 0x4d8),DAT_0056c8cc);
      FUN_0047c5f0((void *)((int)local_8 + 0x4e8),DAT_0056c8cc);
      FUN_004292a0(&DAT_005aefe4,DAT_0056c8cc);
    }
switchD_00485cc0_default:
    iVar3 = FUN_00437cd0(*(int *)((int)local_8 + 0x1c));
    if (iVar3 != 0) {
      if ((*(uint *)((int)local_8 + 0x90) >> 0xc & 1) != 0) {
        FUN_00483270((int)local_8);
      }
      if (((*(uint *)((int)local_8 + 0x90) >> 8 & 1) == 0) && (*(int *)((int)local_8 + 0x30) < 1)) {
        iVar3 = FUN_00437cd0(*(int *)((int)local_8 + 0x1c));
        local_58 = *(float *)(iVar3 + 0x48) * *(float *)((int)local_8 + 0x48);
        iVar3 = FUN_00437cd0(*(int *)((int)local_8 + 0x1c));
        local_5c = *(float *)(iVar3 + 0x4c) * *(float *)((int)local_8 + 0x48);
        iVar3 = FUN_00485580((float *)((int)local_8 + 100),local_5c,local_58);
        if (iVar3 != 0) {
          FUN_0047da40(local_8);
          FUN_004292a0(&DAT_005aefe4,local_14);
          return 0xffffffff;
        }
      }
    }
    if (*(int *)((int)local_8 + 0x18) != 0) {
      *(int *)((int)local_8 + 0x18) = *(int *)((int)local_8 + 0x18) + -1;
    }
    if (0 < *(int *)((int)local_8 + 0x30)) {
      *(int *)((int)local_8 + 0x30) = *(int *)((int)local_8 + 0x30) + -1;
    }
    if (((*(uint *)((int)local_8 + 0x14) >> 9 & 1) == 0) &&
       (iVar3 = FUN_0042b5d0(*(void **)((int)local_8 + 0x1c)), iVar3 != 0)) {
      FUN_0047da40(local_8);
      FUN_004292a0(&DAT_005aefe4,local_14);
      uVar2 = 0xffffffff;
    }
    else {
      FUN_004292a0(&DAT_005aefe4,local_14);
      uVar2 = 0;
    }
  }
  else {
    FUN_0047da40(local_8);
    uVar2 = 0xffffffff;
  }
  return uVar2;
}

