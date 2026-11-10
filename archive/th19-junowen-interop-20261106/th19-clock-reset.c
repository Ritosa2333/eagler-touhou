// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 515d4d

undefined4 __fastcall FUN_00515a10(int param_1)

{
  int iVar1;
  int iVar2;
  char *pcVar3;
  int iVar4;
  int *piVar5;
  undefined4 extraout_ECX;
  uint uVar6;
  int *piVar7;
  int *piVar8;
  undefined4 *puVar9;
  undefined4 *local_6c;
  undefined1 auStack_60 [92];

  iVar1 = DAT_005d19bc;
  if ((*(uint *)(param_1 + 0x140) & 0x1000) != 0) {
    FUN_004cb550(*(undefined4 *)(DAT_005d19bc + 0x64c0868));
    FUN_004cb550(*(undefined4 *)(iVar1 + 0x64c086c));
    *(uint *)(param_1 + 0x140) = *(uint *)(param_1 + 0x140) & 0xffffefff;
    local_6c = &DAT_005d1a64;
    do {
      FUN_004f62b0();
      FUN_00549250();
      FUN_00413510();
      FUN_00411df0();
      iVar1 = local_6c[4];
      iVar4 = *(int *)(iVar1 + 0x34);
      if (*(int *)(iVar1 + 0x24 + iVar4 * 4) != 0) {
        iVar4 = iVar4 + 1;
        *(int *)(iVar1 + 0x34) = iVar4;
        if (3 < iVar4) {
          *(undefined4 *)(iVar1 + 0x34) = 3;
          iVar4 = 3;
        }
      }
      piVar8 = *(int **)(iVar1 + 0x14);
      if (piVar8 == (int *)0x0) {
        *(undefined4 *)(iVar1 + 0x24 + iVar4 * 4) = 0;
      }
      else {
        *(int *)(iVar1 + 0x24 + iVar4 * 4) = piVar8[1];
        while( true ) {
          iVar4 = *piVar8;
          iVar2 = *(int *)(iVar4 + 0x14);
          if ((iVar2 != 0) && (*(int *)(iVar2 + 0x10) == iVar4 + 8)) {
            *(undefined4 *)(iVar2 + 0x10) = *(undefined4 *)(iVar4 + 0x10);
          }
          piVar7 = (int *)(iVar4 + 0xc);
          piVar5 = (int *)(iVar4 + 0x10);
          if (*piVar7 != 0) {
            *(int *)(*piVar7 + 8) = *piVar5;
          }
          if (*piVar5 != 0) {
            *(int *)(*piVar5 + 4) = *piVar7;
          }
          *piVar7 = 0;
          *piVar5 = 0;
          *(undefined4 *)(iVar4 + 0x14) = 0;
          piVar8 = (int *)*piVar8;
          if (piVar8 != (int *)0x0) {
            (**(code **)(*piVar8 + 0x88))(0);
            iVar4 = FUN_0046230c(&DAT_0062ad48);
            if (iVar4 != 0) {
                    /* WARNING: Subroutine does not return */
              FUN_004627e2(5);
            }
            if (DAT_0062ad74 == 0x7fffffff) {
              DAT_0062ad74 = 0x7ffffffe;
                    /* WARNING: Subroutine does not return */
              FUN_004627e2(6);
            }
            thunk_FUN_0047a4db(piVar8);
            FUN_0046231d(&DAT_0062ad48);
          }
          piVar8 = *(int **)(iVar1 + 0x24 + *(int *)(iVar1 + 0x34) * 4);
          if (piVar8 == (int *)0x0) break;
          *(int *)(iVar1 + 0x24 + *(int *)(iVar1 + 0x34) * 4) = piVar8[1];
        }
      }
      *(undefined4 *)(iVar1 + 0x24 + *(int *)(iVar1 + 0x34) * 4) = 0;
      if (0 < *(int *)(iVar1 + 0x34)) {
        *(int *)(iVar1 + 0x34) = *(int *)(iVar1 + 0x34) + -1;
      }
      local_6c = local_6c + 0xf;
      *(undefined4 *)(iVar1 + 0x38) = 0;
    } while ((int)local_6c < 0x5d1adc);
    DAT_0062ba78 = 0;
    DAT_0062ba7c = 0;
    if (DAT_005d1bfc != (int *)0x0) {
      (**(code **)(*DAT_005d1bfc + 4))();
    }
    FUN_0040b300();
    FUN_0050d410(&DAT_00599dd0,extraout_ECX);
    FUN_0050d410(&DAT_00599dd8,auStack_60);
    (**(code **)(*DAT_005d1a50 + 4))();
    (**(code **)(*DAT_005d1bf8 + 4))();
    puVar9 = &DAT_005d1a60;
    do {
      (**(code **)(*(int *)puVar9[1] + 4))();
      (**(code **)(*(int *)*puVar9 + 4))();
      (**(code **)(*(int *)puVar9[2] + 4))();
      (**(code **)(*(int *)puVar9[3] + 4))();
      (**(code **)(*(int *)puVar9[5] + 4))();
      (**(code **)(*(int *)puVar9[10] + 4))();
      (**(code **)(*(int *)puVar9[7] + 4))();
      (**(code **)(*(int *)puVar9[9] + 4))();
      (**(code **)(*(int *)puVar9[4] + 4))();
      FUN_004de0e0();
      if ((int *)puVar9[0xd] != (int *)0x0) {
        (**(code **)(*(int *)puVar9[0xd] + 4))();
      }
      puVar9 = puVar9 + 0xf;
    } while ((int)puVar9 < 0x5d1ad8);
    pcVar3 = "dummy";
    uVar6 = DAT_0062bde4 >> 4 & 1;
    if (uVar6 != 0) {
      pcVar3 = "dummy";
    }
    FUN_004babe0(uVar6 + 3,0,pcVar3);
    iVar1 = *DAT_005d1c04;
    if ((DAT_0062bde4 & 0x10) != 0) {
      FUN_004babe0(4,0,"dummy");
    }
    FUN_004babe0(2,0,"dummy");
    *(undefined1 *)(DAT_005d1c00 + 0x2e89e + iVar1) = 1;
    iVar1 = DAT_005d1a34;
    FUN_004cb830(*(undefined4 *)(DAT_005d1a34 + 0x197c0),1);
    *(undefined4 *)(iVar1 + 0x197c0) = 0;
    if ((*(uint *)(param_1 + 0x20) & 1) == 0) {
      *(undefined4 *)(param_1 + 0x14) = 0;
      *(undefined4 *)(param_1 + 0x18) = 0;
      *(undefined4 *)(param_1 + 0x1c) = 0;
      *(uint *)(param_1 + 0x20) = *(uint *)(param_1 + 0x20) | 1;
    }
    *(undefined4 *)(param_1 + 0x14) = 0;
    *(undefined4 *)(param_1 + 0x18) = 0;
    *(undefined4 *)(param_1 + 0x10) = 0xffffffff;
  }
  return 0;
}


// Address 515a10

undefined4 __fastcall FUN_00515a10(int param_1)

{
  int iVar1;
  int iVar2;
  char *pcVar3;
  int iVar4;
  int *piVar5;
  undefined4 extraout_ECX;
  uint uVar6;
  int *piVar7;
  int *piVar8;
  undefined4 *puVar9;
  undefined4 *local_6c;
  undefined1 auStack_60 [92];

  iVar1 = DAT_005d19bc;
  if ((*(uint *)(param_1 + 0x140) & 0x1000) != 0) {
    FUN_004cb550(*(undefined4 *)(DAT_005d19bc + 0x64c0868));
    FUN_004cb550(*(undefined4 *)(iVar1 + 0x64c086c));
    *(uint *)(param_1 + 0x140) = *(uint *)(param_1 + 0x140) & 0xffffefff;
    local_6c = &DAT_005d1a64;
    do {
      FUN_004f62b0();
      FUN_00549250();
      FUN_00413510();
      FUN_00411df0();
      iVar1 = local_6c[4];
      iVar4 = *(int *)(iVar1 + 0x34);
      if (*(int *)(iVar1 + 0x24 + iVar4 * 4) != 0) {
        iVar4 = iVar4 + 1;
        *(int *)(iVar1 + 0x34) = iVar4;
        if (3 < iVar4) {
          *(undefined4 *)(iVar1 + 0x34) = 3;
          iVar4 = 3;
        }
      }
      piVar8 = *(int **)(iVar1 + 0x14);
      if (piVar8 == (int *)0x0) {
        *(undefined4 *)(iVar1 + 0x24 + iVar4 * 4) = 0;
      }
      else {
        *(int *)(iVar1 + 0x24 + iVar4 * 4) = piVar8[1];
        while( true ) {
          iVar4 = *piVar8;
          iVar2 = *(int *)(iVar4 + 0x14);
          if ((iVar2 != 0) && (*(int *)(iVar2 + 0x10) == iVar4 + 8)) {
            *(undefined4 *)(iVar2 + 0x10) = *(undefined4 *)(iVar4 + 0x10);
          }
          piVar7 = (int *)(iVar4 + 0xc);
          piVar5 = (int *)(iVar4 + 0x10);
          if (*piVar7 != 0) {
            *(int *)(*piVar7 + 8) = *piVar5;
          }
          if (*piVar5 != 0) {
            *(int *)(*piVar5 + 4) = *piVar7;
          }
          *piVar7 = 0;
          *piVar5 = 0;
          *(undefined4 *)(iVar4 + 0x14) = 0;
          piVar8 = (int *)*piVar8;
          if (piVar8 != (int *)0x0) {
            (**(code **)(*piVar8 + 0x88))(0);
            iVar4 = FUN_0046230c(&DAT_0062ad48);
            if (iVar4 != 0) {
                    /* WARNING: Subroutine does not return */
              FUN_004627e2(5);
            }
            if (DAT_0062ad74 == 0x7fffffff) {
              DAT_0062ad74 = 0x7ffffffe;
                    /* WARNING: Subroutine does not return */
              FUN_004627e2(6);
            }
            thunk_FUN_0047a4db(piVar8);
            FUN_0046231d(&DAT_0062ad48);
          }
          piVar8 = *(int **)(iVar1 + 0x24 + *(int *)(iVar1 + 0x34) * 4);
          if (piVar8 == (int *)0x0) break;
          *(int *)(iVar1 + 0x24 + *(int *)(iVar1 + 0x34) * 4) = piVar8[1];
        }
      }
      *(undefined4 *)(iVar1 + 0x24 + *(int *)(iVar1 + 0x34) * 4) = 0;
      if (0 < *(int *)(iVar1 + 0x34)) {
        *(int *)(iVar1 + 0x34) = *(int *)(iVar1 + 0x34) + -1;
      }
      local_6c = local_6c + 0xf;
      *(undefined4 *)(iVar1 + 0x38) = 0;
    } while ((int)local_6c < 0x5d1adc);
    DAT_0062ba78 = 0;
    DAT_0062ba7c = 0;
    if (DAT_005d1bfc != (int *)0x0) {
      (**(code **)(*DAT_005d1bfc + 4))();
    }
    FUN_0040b300();
    FUN_0050d410(&DAT_00599dd0,extraout_ECX);
    FUN_0050d410(&DAT_00599dd8,auStack_60);
    (**(code **)(*DAT_005d1a50 + 4))();
    (**(code **)(*DAT_005d1bf8 + 4))();
    puVar9 = &DAT_005d1a60;
    do {
      (**(code **)(*(int *)puVar9[1] + 4))();
      (**(code **)(*(int *)*puVar9 + 4))();
      (**(code **)(*(int *)puVar9[2] + 4))();
      (**(code **)(*(int *)puVar9[3] + 4))();
      (**(code **)(*(int *)puVar9[5] + 4))();
      (**(code **)(*(int *)puVar9[10] + 4))();
      (**(code **)(*(int *)puVar9[7] + 4))();
      (**(code **)(*(int *)puVar9[9] + 4))();
      (**(code **)(*(int *)puVar9[4] + 4))();
      FUN_004de0e0();
      if ((int *)puVar9[0xd] != (int *)0x0) {
        (**(code **)(*(int *)puVar9[0xd] + 4))();
      }
      puVar9 = puVar9 + 0xf;
    } while ((int)puVar9 < 0x5d1ad8);
    pcVar3 = "dummy";
    uVar6 = DAT_0062bde4 >> 4 & 1;
    if (uVar6 != 0) {
      pcVar3 = "dummy";
    }
    FUN_004babe0(uVar6 + 3,0,pcVar3);
    iVar1 = *DAT_005d1c04;
    if ((DAT_0062bde4 & 0x10) != 0) {
      FUN_004babe0(4,0,"dummy");
    }
    FUN_004babe0(2,0,"dummy");
    *(undefined1 *)(DAT_005d1c00 + 0x2e89e + iVar1) = 1;
    iVar1 = DAT_005d1a34;
    FUN_004cb830(*(undefined4 *)(DAT_005d1a34 + 0x197c0),1);
    *(undefined4 *)(iVar1 + 0x197c0) = 0;
    if ((*(uint *)(param_1 + 0x20) & 1) == 0) {
      *(undefined4 *)(param_1 + 0x14) = 0;
      *(undefined4 *)(param_1 + 0x18) = 0;
      *(undefined4 *)(param_1 + 0x1c) = 0;
      *(uint *)(param_1 + 0x20) = *(uint *)(param_1 + 0x20) | 1;
    }
    *(undefined4 *)(param_1 + 0x14) = 0;
    *(undefined4 *)(param_1 + 0x18) = 0;
    *(undefined4 *)(param_1 + 0x10) = 0xffffffff;
  }
  return 0;
}


// Reference 005168a7
