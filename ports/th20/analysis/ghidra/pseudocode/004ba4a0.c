/* Entry: 0x004ba4a0; symbol: FUN_004ba4a0; body bytes: 972 */

undefined4 __fastcall FUN_004ba4a0(int param_1)

{
  bool bVar1;
  void *pvVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  uint local_c;
  
  *(int *)(param_1 + 0x30) = *(int *)(param_1 + 0x30) + 1;
  pvVar2 = (void *)FUN_00498f40();
  FUN_004bcd50(pvVar2,1);
  if ((DAT_005c5888 >> 5 & 3) == 0) {
    uVar4 = FUN_004ab270(param_1);
    if ((uVar4 & 0xff) != 0) {
      if ((*(uint *)(param_1 + 0xe8) >> 4 & 1) != 0) {
        return 3;
      }
      *(int *)(param_1 + 0x10c) = *(int *)(param_1 + 0x10c) + 1;
      iVar5 = FUN_00488830(param_1);
      if ((iVar5 != 0) && (*(int *)(param_1 + 0x10c) == 0x78)) {
        FUN_004e5f30();
      }
      iVar5 = FUN_00488830(param_1);
      if ((iVar5 == 0) && (*(int *)(param_1 + 0x10c) == 0xb4)) {
        FUN_004250c0(5,0x3c,0,0,0,0x67);
      }
      iVar5 = FUN_00488830(param_1);
      if ((iVar5 == 0) && (0xef < *(int *)(param_1 + 0x10c))) {
        iVar5 = FUN_004640a0(0x5ba568);
        if (iVar5 == 4) {
          FUN_004a0fb0(&DAT_005c4d40,0x10);
        }
        else {
          FUN_004a0fb0(&DAT_005c4d40,0xf);
        }
      }
    }
    bVar1 = FUN_004785e0((void *)(param_1 + 0x10),0);
    if (bVar1) {
      iVar5 = FUN_004ba940(param_1);
      if (iVar5 != 0) {
        return 1;
      }
    }
    else {
      bVar1 = FUN_004785e0((void *)(param_1 + 0x10),0x1e);
      if (bVar1) {
        FUN_004bbf90(param_1);
      }
    }
    if ((DAT_005c06a0 != (undefined4 *)0x0) && (uVar4 = FUN_004bd710((int)DAT_005c06a0), uVar4 != 0)
       ) {
      FUN_004217c0(DAT_005c06a0);
    }
    bVar1 = FUN_004785e0((void *)(param_1 + 0x10),5);
    if (bVar1) {
      DAT_005b8858 = 2;
    }
    if ((*(uint *)(param_1 + 0xe8) >> 2 & 1) == 0) {
      if ((DAT_005ba5d4 >> 5 & 1) != 0) {
        if (DAT_005b889c == (void *)0x0) {
          local_c = 0;
        }
        else {
          local_c = FUN_004495a0(DAT_005b889c,0x8010f);
        }
        if ((((local_c != 0) || ((*(uint *)(param_1 + 0xe8) >> 4 & 1) != 0)) ||
            ((*(uint *)(param_1 + 0xe8) >> 5 & 1) != 0)) ||
           ((*(uint *)(param_1 + 0xe8) >> 6 & 1) != 0)) {
          FUN_004a0fb0(&DAT_005c4d40,4);
        }
        bVar1 = FUN_004785e0((void *)(param_1 + 0x10),0xdd4);
        if (bVar1) {
          FUN_004250c0(5,0x3c,0,0,0,0x6d);
        }
        else {
          bVar1 = FUN_004785e0((void *)(param_1 + 0x10),0xe10);
          if (bVar1) {
            FUN_004a0fb0(&DAT_005c4d40,4);
          }
        }
      }
      FUN_004b4490(DAT_005c06a4);
      if ((*(uint *)(param_1 + 0xe8) >> 4 & 1) == 0) {
        if ((*(uint *)(param_1 + 0xe8) >> 5 & 1) == 0) {
          FUN_0042b5d0(DAT_005c4eec);
          FUN_0042b5d0(DAT_005c4ef0);
          FUN_0042b5d0(DAT_005c4ef4);
          FUN_0042b5d0(DAT_005c4ef8);
          FUN_00464080(0);
          FUN_0040e5e0();
          pvVar2 = (void *)FUN_00498f40();
          FUN_004bcdb0(pvVar2,0);
          pvVar2 = (void *)FUN_00498f40();
          FUN_004bcd50(pvVar2,0);
          if (DAT_005ba518 != 0) {
            DAT_005ba518 = DAT_005ba518 + -1;
          }
          FUN_00423540((undefined4 *)(param_1 + 0x10));
          pvVar2 = (void *)FUN_00498f40();
          FUN_004bcdb0(pvVar2,1);
          uVar3 = 1;
        }
        else {
          uVar3 = 3;
        }
      }
      else {
        uVar3 = 3;
      }
    }
    else {
      *(uint *)(param_1 + 0xe8) = *(uint *)(param_1 + 0xe8) | 0x100;
      uVar3 = 1;
    }
  }
  else {
    uVar3 = 0;
  }
  return uVar3;
}

