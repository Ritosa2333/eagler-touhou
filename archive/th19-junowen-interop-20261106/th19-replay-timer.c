// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 4026c0

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004026c0(int *param_1)

{
  float fVar1;
  int iVar2;
  undefined *puVar3;
  float fVar4;
  
  if (param_1[3] != 0) {
    param_1[3] = 0;
  }
  puVar3 = PTR_DAT_00596da0;
  iVar2 = param_1[1];
  *param_1 = iVar2;
  if (puVar3 != (undefined *)0x0) {
    if ((_DAT_005aa22c < *(float *)puVar3) &&
       (*(float *)puVar3 <= DAT_005aa234 && DAT_005aa234 != *(float *)puVar3)) {
      param_1[2] = (int)((float)param_1[2] + DAT_005aa230);
      param_1[1] = iVar2 + 1;
      return;
    }
    fVar4 = *(float *)puVar3;
    fVar1 = (float)param_1[2];
    param_1[2] = (int)(fVar4 + fVar1);
    param_1[1] = (int)(fVar4 + fVar1);
    return;
  }
  fVar4 = (float)param_1[2] + DAT_005aa230;
  param_1[1] = iVar2 + 1;
  param_1[2] = (int)fVar4;
  return;
}


// Address 515d80

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00515d80(int param_1)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  undefined4 **ppuVar4;
  int *piVar5;
  int *piVar6;
  undefined4 extraout_ECX;
  int iVar7;
  int *piVar8;
  undefined4 **ppuVar9;
  undefined4 *puVar10;
  undefined4 *puStack_70;
  undefined4 *puStack_6c;
  undefined4 *puStack_68;
  undefined4 *puStack_64;
  undefined1 auStack_60 [92];
  
  uVar1 = *(uint *)(param_1 + 0x140);
  *(uint *)(param_1 + 0x140) = uVar1 | 0x4000000;
  if ((uVar1 & 8) != 0) {
    FUN_00413a10();
    FUN_004067e0();
    DAT_0062c450 = ~(DAT_0062c48c >> 9) & 1 | 2;
    return 1;
  }
  (**(code **)(*DAT_005d1a3c + 4))();
  if (DAT_005d1a38 != 0) {
    FUN_004ea560();
    piVar8 = DAT_005d1a3c;
    uVar1 = DAT_005d1a3c[0xd88];
    piVar5 = DAT_005d1a3c + 0xd86;
    piVar6 = DAT_005d1a3c + 0xd85;
    if ((uVar1 & 1) == 0) {
      *piVar6 = 0;
      *piVar5 = 0;
      piVar8[0xd87] = 0;
      piVar8[0xd88] = uVar1 | 1;
    }
    piVar8[0xd84] = 0x3b;
    *piVar6 = 0x3c;
    *piVar5 = 0x42700000;
    piVar8[0xd83] = piVar8[0xd83] | 4;
    piVar5 = DAT_005d1a50;
    *(uint *)(param_1 + 0x140) = *(uint *)(param_1 + 0x140) | 0x1000;
    FUN_004cb830(piVar5[0x49],1);
    return 0;
  }
  *(uint *)(param_1 + 0x140) = *(uint *)(param_1 + 0x140) & 0xffffefff;
  iVar2 = DAT_005d19bc;
  FUN_004cb550(*(undefined4 *)(DAT_005d19bc + 0x64c0868));
  FUN_004cb550(*(undefined4 *)(iVar2 + 0x64c086c));
  puStack_70 = &DAT_005d1a64;
  do {
    puVar10 = puStack_70;
    FUN_004f62b0();
    FUN_00549250();
    FUN_00413510();
    FUN_00411df0();
    iVar2 = puVar10[4];
    iVar7 = *(int *)(iVar2 + 0x34);
    if (*(int *)(iVar2 + 0x24 + iVar7 * 4) != 0) {
      iVar7 = iVar7 + 1;
      *(int *)(iVar2 + 0x34) = iVar7;
      if (3 < iVar7) {
        *(undefined4 *)(iVar2 + 0x34) = 3;
        iVar7 = 3;
      }
    }
    piVar5 = *(int **)(iVar2 + 0x14);
    if (piVar5 == (int *)0x0) {
      *(undefined4 *)(iVar2 + 0x24 + iVar7 * 4) = 0;
      puStack_70 = puVar10;
    }
    else {
      do {
        *(int *)(iVar2 + 0x24 + iVar7 * 4) = piVar5[1];
        iVar7 = *piVar5;
        iVar3 = *(int *)(iVar7 + 0x14);
        if ((iVar3 != 0) && (*(int *)(iVar3 + 0x10) == iVar7 + 8)) {
          *(undefined4 *)(iVar3 + 0x10) = *(undefined4 *)(iVar7 + 0x10);
        }
        piVar8 = (int *)(iVar7 + 0xc);
        piVar6 = (int *)(iVar7 + 0x10);
        if (*piVar8 != 0) {
          *(int *)(*piVar8 + 8) = *piVar6;
        }
        if (*piVar6 != 0) {
          *(int *)(*piVar6 + 4) = *piVar8;
        }
        *piVar8 = 0;
        *piVar6 = 0;
        *(undefined4 *)(iVar7 + 0x14) = 0;
        piVar5 = (int *)*piVar5;
        if (piVar5 != (int *)0x0) {
          (**(code **)(*piVar5 + 0x88))(0);
          iVar7 = FUN_0046230c(&DAT_0062ad48);
          if (iVar7 != 0) {
                    /* WARNING: Subroutine does not return */
            FUN_004627e2(5);
          }
          if (DAT_0062ad74 == 0x7fffffff) {
            DAT_0062ad74 = 0x7ffffffe;
                    /* WARNING: Subroutine does not return */
            FUN_004627e2(6);
          }
          thunk_FUN_0047a4db(piVar5);
          FUN_0046231d(&DAT_0062ad48);
        }
        iVar7 = *(int *)(iVar2 + 0x34);
        piVar5 = *(int **)(iVar2 + 0x24 + iVar7 * 4);
      } while (piVar5 != (int *)0x0);
    }
    *(undefined4 *)(iVar2 + 0x24 + *(int *)(iVar2 + 0x34) * 4) = 0;
    if (0 < *(int *)(iVar2 + 0x34)) {
      *(int *)(iVar2 + 0x34) = *(int *)(iVar2 + 0x34) + -1;
    }
    puStack_70 = puStack_70 + 0xf;
    *(undefined4 *)(iVar2 + 0x38) = 0;
  } while ((int)puStack_70 < 0x5d1adc);
  DAT_0062ba78 = 0;
  DAT_0062ba7c = 0;
  if (DAT_005d1bfc != (int *)0x0) {
    (**(code **)(*DAT_005d1bfc + 4))();
  }
  FUN_0040b300();
  FUN_0050d410(&DAT_00599dc8,extraout_ECX);
  FUN_0050d410(&DAT_00599dc8,auStack_60);
  (**(code **)(*DAT_005d1a50 + 4))();
  (**(code **)(*DAT_005d1bf8 + 4))();
  puVar10 = &DAT_005d1a64;
  do {
    (**(code **)(*(int *)puVar10[0xb] + 4))();
    (**(code **)(*(int *)*puVar10 + 4))();
    (**(code **)(*(int *)puVar10[5] + 4))();
    (**(code **)(*(int *)puVar10[-1] + 4))();
    (**(code **)(*(int *)puVar10[1] + 4))();
    (**(code **)(*(int *)puVar10[2] + 4))();
    (**(code **)(*(int *)puVar10[4] + 4))();
    (**(code **)(*(int *)puVar10[6] + 4))();
    (**(code **)(*(int *)puVar10[8] + 4))();
    (**(code **)(*(int *)puVar10[3] + 4))();
    (**(code **)(*(int *)puVar10[9] + 4))();
    (**(code **)(*(int *)puVar10[0xd] + 4))();
    FUN_004de0e0();
    if ((int *)puVar10[0xc] != (int *)0x0) {
      (**(code **)(*(int *)puVar10[0xc] + 4))();
    }
    puVar10 = puVar10 + 0xf;
  } while ((int)puVar10 < 0x5d1adc);
  (**(code **)(*DAT_005d1bd8 + 4))();
  if ((((byte)_DAT_0062b0ac & 0x60) != 0x40) && (-1 < (char)(byte)_DAT_0062b0ac)) {
    ppuVar9 = &puStack_68;
    puStack_64 = (undefined4 *)0x3;
    puStack_68 = (undefined4 *)0x0;
    if (-1 < (int)DAT_0062b254) {
      ppuVar9 = &DAT_0062b254;
    }
    ppuVar4 = &puStack_64;
    if ((int)DAT_0062b254 < 4) {
      ppuVar4 = ppuVar9;
    }
    puVar10 = *ppuVar4;
    if ((puVar10 != (undefined4 *)0x2) ||
       (DAT_0062b254 = (undefined4 *)0x2, (_DAT_0062b0ac & 2) != 0)) {
      puStack_70 = (undefined4 *)0x3;
      puStack_6c = (undefined4 *)0x0;
      ppuVar9 = &puStack_6c;
      if (-1 < (int)puVar10) {
        ppuVar9 = &DAT_0062b254;
      }
      ppuVar4 = &puStack_70;
      if ((int)puVar10 < 4) {
        ppuVar4 = ppuVar9;
      }
      DAT_0062b254 = *ppuVar4;
      if ((DAT_0062b254 != (undefined4 *)0x0) || ((_DAT_0062b0ac & 1) == 0)) goto LAB_005161da;
    }
    iVar2 = *DAT_005d1c04;
    if (((byte)DAT_0062bde4 & 0x10) != 0) {
      FUN_004babe0(4,0,"dummy");
    }
    FUN_004babe0(2,0,"dummy");
    *(undefined1 *)(iVar2 + 0x2e89e + DAT_005d1c00) = 1;
  }
LAB_005161da:
  _DAT_0062b0ac = _DAT_0062b0ac & 0xfffffffc;
  FUN_004cb830(*(undefined4 *)(DAT_005d1a34 + 0x197bc),1);
  iVar2 = DAT_005d1a34;
  FUN_004cb830(*(undefined4 *)(DAT_005d1a34 + 0x197c0),1);
  *(undefined4 *)(iVar2 + 0x197c0) = 0;
  FUN_004cb830(DAT_0062bd10,1);
  return 0;
}


