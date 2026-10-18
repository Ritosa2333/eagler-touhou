/* Entry: 0x004bf0a0; symbol: FUN_004bf0a0; body bytes: 2057 */

void __fastcall FUN_004bf0a0(void *param_1)

{
  undefined4 uVar1;
  bool bVar2;
  undefined3 extraout_var;
  undefined4 *puVar3;
  int *piVar4;
  int iVar5;
  uint local_58;
  uint local_54;
  uint local_50;
  uint local_4c;
  uint local_44;
  uint local_40;
  int local_3c;
  int local_38;
  int local_2c;
  int local_28;
  int local_24;
  int local_20;
  int local_1c;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422dd0(local_14,*(undefined4 *)((int)param_1 + 0xac),0,0);
  iVar5 = *(int *)((int)param_1 + 0x10);
  if (iVar5 == 0) {
    *(undefined4 *)((int)param_1 + 0x10) = 1;
    goto LAB_004bf88c;
  }
  if (iVar5 != 1) {
    if ((iVar5 == 2) && (bVar2 = FUN_004235c0((void *)((int)param_1 + 0x14),0x1e), bVar2)) {
      *(undefined4 *)((int)param_1 + 0xa8) = 1;
    }
    goto LAB_004bf88c;
  }
  switch(*(undefined4 *)((int)param_1 + 0xb8)) {
  case 0:
    FUN_0049c4e0((void *)((int)param_1 + 0x24),9);
    FUN_004bed50((void *)((int)param_1 + 0x24),0);
    FUN_004bfbe0((void *)((int)param_1 + 0x24),1);
    for (local_1c = 0; local_1c < 9; local_1c = local_1c + 1) {
      FUN_004bfc20(param_1,local_1c,local_14);
      iVar5 = FUN_0040c300((undefined4 *)((int)param_1 + 0x24));
      if (iVar5 == local_1c) {
        FUN_004bfb20(param_1,local_1c);
      }
      else {
        FUN_004bfb80(param_1,local_1c);
      }
    }
    puVar3 = (undefined4 *)FUN_004bfae0(*(void **)((int)param_1 + 0xb0),1);
    FUN_00449d80(puVar3);
    *(undefined4 *)((int)param_1 + 0xb8) = 1;
  case 1:
    bVar2 = FUN_00423560((void *)((int)param_1 + 0x14),0x14);
    if (!bVar2) {
      FUN_004bfc00((undefined4 *)((int)param_1 + 0x24));
      if (DAT_005b88a4 == (void *)0x0) {
        local_38 = 0;
      }
      else {
        local_38 = FUN_0041a280(DAT_005b88a4,0x10);
      }
      if (local_38 != 0) {
        FUN_004bf040((void *)((int)param_1 + 0x24));
      }
      if (DAT_005b88a4 == (void *)0x0) {
        local_3c = 0;
      }
      else {
        local_3c = FUN_0041a280(DAT_005b88a4,0x20);
      }
      if (local_3c != 0) {
        FUN_004bf020((void *)((int)param_1 + 0x24));
      }
      bVar2 = FUN_0045d030((int *)((int)param_1 + 0x24));
      if (CONCAT31(extraout_var,bVar2) != 0) {
        FUN_00426d70(&DAT_005ba830,10,0);
        for (local_24 = 0; local_24 < 9; local_24 = local_24 + 1) {
          iVar5 = FUN_0040c300((undefined4 *)((int)param_1 + 0x24));
          if (iVar5 == local_24) {
            FUN_004bfb20(param_1,local_24);
          }
          else {
            FUN_004bfb80(param_1,local_24);
          }
        }
      }
      if (DAT_005b88a4 == (void *)0x0) {
        local_40 = 0;
      }
      else {
        local_40 = FUN_00419c00(DAT_005b88a4,0x80001);
      }
      if (local_40 == 0) {
        if (DAT_005b88a4 == (void *)0x0) {
          local_44 = 0;
        }
        else {
          local_44 = FUN_00419c00(DAT_005b88a4,0x106);
        }
        if (local_44 != 0) {
          FUN_00426d70(&DAT_005ba830,9,0);
          for (local_2c = 0; local_2c < 9; local_2c = local_2c + 1) {
            FUN_004bfb50(param_1,local_2c);
          }
          *(undefined4 *)((int)param_1 + 0x10) = 2;
          *(undefined4 *)((int)param_1 + 0xb8) = 0;
          FUN_00423520((void *)((int)param_1 + 0x14),0);
        }
      }
      else {
        FUN_00426d70(&DAT_005ba830,7,0);
LAB_004bf37d:
        *(undefined4 *)((int)param_1 + 0xb8) = 2;
        FUN_00423520((void *)((int)param_1 + 0x14),0);
        FUN_0040c300((undefined4 *)((int)param_1 + 0x24));
        FUN_004bee40((wchar_t *)((int)param_1 + 0xbc),(wchar_t *)"help_%.2d.png");
        FUN_004b99f0(FUN_004bf8d0);
        for (local_28 = 0; local_28 < 9; local_28 = local_28 + 1) {
          FUN_004bfb50(param_1,local_28);
        }
      }
    }
    break;
  default:
    break;
  case 3:
    uVar1 = *(undefined4 *)((int)param_1 + 0x13c);
    iVar5 = *(int *)((int)param_1 + 0xb4);
    puVar3 = (undefined4 *)FUN_004bfae0(*(void **)((int)param_1 + 0xb0),1);
    FUN_0044dc20(puVar3,iVar5,uVar1,1,0,0);
    if (*(int *)((int)param_1 + 0xb4) != 0) {
      FUN_0041f670(*(LPVOID *)((int)param_1 + 0xb4));
      *(undefined4 *)((int)param_1 + 0xb4) = 0;
    }
    *(undefined4 *)((int)param_1 + 0xb4) = 0;
    piVar4 = (int *)FUN_004bfb00(*(void **)((int)param_1 + 0xb0),1);
    (**(code **)(*piVar4 + 0x24))(piVar4);
    FUN_004bfc20(param_1,0xd,local_14);
    *(undefined4 *)((int)param_1 + 0xb8) = 4;
    FUN_00423520((void *)((int)param_1 + 0x14),0);
  case 4:
    bVar2 = FUN_00423560((void *)((int)param_1 + 0x14),0x14);
    if (!bVar2) {
      if (DAT_005b88a4 == (void *)0x0) {
        local_4c = 0;
      }
      else {
        local_4c = FUN_00419c00(DAT_005b88a4,0x20);
      }
      if ((local_4c == 0) || (iVar5 = FUN_0040c300((undefined4 *)((int)param_1 + 0x24)), 7 < iVar5))
      {
        if (DAT_005b88a4 == (void *)0x0) {
          local_50 = 0;
        }
        else {
          local_50 = FUN_00419c00(DAT_005b88a4,0x10);
        }
        if ((local_50 == 0) ||
           (iVar5 = FUN_0040c300((undefined4 *)((int)param_1 + 0x24)), iVar5 < 1)) {
          if (DAT_005b88a4 == (void *)0x0) {
            local_54 = 0;
          }
          else {
            local_54 = FUN_00419c00(DAT_005b88a4,0x80001);
          }
          if (local_54 == 0) {
            if (DAT_005b88a4 == (void *)0x0) {
              local_58 = 0;
            }
            else {
              local_58 = FUN_00419c00(DAT_005b88a4,0x106);
            }
            if (local_58 == 0) break;
          }
          FUN_00426d70(&DAT_005ba830,9,0);
          *(undefined4 *)((int)param_1 + 0xb8) = 1;
          FUN_00423520((void *)((int)param_1 + 0x14),0);
          FUN_004bfb50(param_1,0xd);
          for (local_20 = 0; local_20 < 9; local_20 = local_20 + 1) {
            FUN_004bfc20(param_1,local_20,local_14);
            iVar5 = FUN_0040c300((undefined4 *)((int)param_1 + 0x24));
            if (iVar5 == local_20) {
              FUN_004bfb20(param_1,local_20);
            }
            else {
              FUN_004bfb80(param_1,local_20);
            }
          }
          break;
        }
        *(undefined4 *)((int)param_1 + 0xb8) = 5;
        FUN_00423520((void *)((int)param_1 + 0x14),0);
        FUN_00426d70(&DAT_005ba830,7,0);
        FUN_004bf040((void *)((int)param_1 + 0x24));
        FUN_004bfbb0(param_1,0xd,8);
      }
      else {
        *(undefined4 *)((int)param_1 + 0xb8) = 5;
        FUN_00423520((void *)((int)param_1 + 0x14),0);
        FUN_00426d70(&DAT_005ba830,7,0);
        FUN_004bf020((void *)((int)param_1 + 0x24));
        FUN_004bfbb0(param_1,0xd,7);
      }
switchD_004bf12e_caseD_5:
      bVar2 = FUN_004235c0((void *)((int)param_1 + 0x14),0x14);
      if (!bVar2) break;
      goto LAB_004bf37d;
    }
    break;
  case 5:
    goto switchD_004bf12e_caseD_5;
  }
LAB_004bf88c:
  FUN_00423540((undefined4 *)((int)param_1 + 0x14));
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

