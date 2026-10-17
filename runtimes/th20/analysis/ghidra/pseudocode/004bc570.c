/* Entry: 0x004bc570; symbol: FUN_004bc570; body bytes: 1836 */

undefined4 __fastcall FUN_004bc570(int param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  void *this;
  int iVar4;
  uint uVar5;
  int *piVar6;
  undefined8 uVar7;
  uint local_b0;
  int local_ac;
  
  iVar2 = FUN_00488830(DAT_005ba828);
  if (iVar2 == 0) {
LAB_004bc5d0:
    iVar2 = FUN_00460830(0);
    FUN_004bd0c0(iVar2);
    bVar1 = FUN_004bd820(0x5ba568);
    if (bVar1) {
      iVar2 = FUN_00488830(DAT_005ba828);
      if (iVar2 == 0) {
        bVar1 = FUN_00488850(0x5ba568);
        if (bVar1) {
          iVar2 = FUN_0050fc50(DAT_005c6108);
          iVar3 = FUN_00499460(0x5ba568);
          this = (void *)FUN_00486e10((void *)(iVar2 + 0xb08),iVar3);
          uVar7 = FUN_004b8290(0x5ba568);
          uVar7 = __aulldiv((uint)uVar7,(uint)((ulonglong)uVar7 >> 0x20),10,0);
          local_ac = (int)((ulonglong)uVar7 >> 0x20);
          local_b0 = (uint)uVar7;
          FUN_004be2d0(this,local_b0,local_ac);
        }
        iVar2 = FUN_00488830(DAT_005ba828);
        if ((iVar2 == 0) && (bVar1 = FUN_00488850(0x5ba568), !bVar1)) {
          iVar2 = FUN_0050fc50(DAT_005c6108);
          iVar3 = FUN_004640a0(0x5ba568);
          iVar4 = FUN_00474d60(0x5ba568);
          *(undefined1 *)(iVar2 + iVar3 * 0x90 + (iVar4 + -1) * 0x10 + 0x7700) = 1;
        }
        FUN_004e59e0();
      }
      else {
        FUN_004e5f30();
      }
    }
    else {
      iVar2 = FUN_00474d60(0x5ba568);
      if (iVar2 == 6) {
        FUN_004bd980(param_1);
        FUN_004bdbd0(DAT_005c06a4);
        FUN_004bce10(0x5ba568);
        iVar2 = FUN_004b7fa0(0x5ba568);
        if (iVar2 == 0) {
          iVar2 = FUN_004640a0(0x5ba568);
          FUN_0052f900(iVar2 + 0x22);
        }
        iVar2 = FUN_00498f40();
        iVar2 = FUN_00499580(iVar2);
        if (iVar2 == 0) {
          FUN_0052f900(0x26);
          iVar2 = FUN_004640a0(0x5ba568);
          if (iVar2 == 3) {
            FUN_0052f900(0x27);
          }
        }
        iVar2 = FUN_00464080(0);
        iVar2 = FUN_0041cab0(iVar2);
        iVar2 = FUN_004bd610(DAT_005c6108,iVar2);
        if (iVar2 < 9) {
          iVar2 = FUN_00464080(0);
          iVar2 = FUN_0041cab0(iVar2);
          FUN_004bddc0(DAT_005c6108,iVar2 + 1);
          iVar2 = FUN_00464080(0);
          uVar5 = FUN_0041cab0(iVar2);
          FUN_004bcf60(DAT_005c6108,uVar5);
        }
        iVar2 = FUN_00488830(DAT_005ba828);
        if (iVar2 == 0) {
          iVar2 = FUN_0050fc50(DAT_005c6108);
          iVar3 = FUN_004640a0(0x5ba568);
          piVar6 = (int *)FUN_00414580((void *)(iVar2 + 0x76b8),iVar3);
          if (*piVar6 < 99999) {
            iVar2 = FUN_0050fc50(DAT_005c6108);
            iVar3 = FUN_004640a0(0x5ba568);
            piVar6 = (int *)FUN_00414580((void *)(iVar2 + 0x76b8),iVar3);
            *piVar6 = *piVar6 + 1;
          }
          iVar2 = FUN_004b7fa0(0x5ba568);
          if (iVar2 == 0) {
            iVar2 = FUN_0050fc50(DAT_005c6108);
            iVar3 = FUN_004640a0(0x5ba568);
            piVar6 = (int *)FUN_00414580((void *)(iVar2 + 0x76d4),iVar3);
            if (*piVar6 < 99999) {
              iVar2 = FUN_0050fc50(DAT_005c6108);
              iVar3 = FUN_004640a0(0x5ba568);
              piVar6 = (int *)FUN_00414580((void *)(iVar2 + 0x76d4),iVar3);
              *piVar6 = *piVar6 + 1;
            }
            iVar2 = FUN_004640a0(0x5ba568);
            if (0 < iVar2) {
              iVar2 = FUN_0050fc50(DAT_005c6108);
              *(undefined4 *)(iVar2 + 0x76f0) = 1;
              iVar2 = FUN_00464080(0);
              iVar2 = FUN_00412730(iVar2);
              iVar3 = FUN_00464080(0);
              iVar3 = FUN_0041cab0(iVar3);
              iVar2 = FUN_004bd780(DAT_005c6108,iVar3 + 9 + iVar2 * 8);
              if (iVar2 == 0) {
                iVar2 = FUN_00464080(0);
                iVar2 = FUN_00412730(iVar2);
                iVar3 = FUN_00464080(0);
                iVar3 = FUN_0041cab0(iVar3);
                FUN_004bddc0(DAT_005c6108,iVar3 + 9 + iVar2 * 8);
              }
            }
          }
        }
      }
      else {
        iVar2 = FUN_00474d60(0x5ba568);
        if (iVar2 == 7) {
          FUN_004bdbd0(DAT_005c06a4);
          if (DAT_005afcfc < 0) {
            iVar2 = FUN_004b7f70(0x5ba568);
            iVar3 = FUN_00464080(0);
            iVar3 = FUN_0041cab0(iVar3);
            FUN_0052f900(iVar3 + 0x12 + iVar2 * 8);
            iVar2 = FUN_00464080(0);
            iVar2 = FUN_0041cab0(iVar2);
            iVar2 = FUN_004bd610(DAT_005c6108,iVar2);
            if (iVar2 < 9) {
              iVar2 = FUN_00464080(0);
              iVar2 = FUN_0041cab0(iVar2);
              FUN_004bddc0(DAT_005c6108,iVar2 + 1);
              iVar2 = FUN_00464080(0);
              uVar5 = FUN_0041cab0(iVar2);
              FUN_004bcf60(DAT_005c6108,uVar5);
            }
            iVar2 = FUN_00498f40();
            FUN_00499580(iVar2);
            iVar2 = FUN_00488830(DAT_005ba828);
            if (iVar2 == 0) {
              iVar2 = FUN_0050fc50(DAT_005c6108);
              iVar3 = FUN_004640a0(0x5ba568);
              piVar6 = (int *)FUN_00414580((void *)(iVar2 + 0x76b8),iVar3);
              if (*piVar6 < 99999) {
                iVar2 = FUN_0050fc50(DAT_005c6108);
                iVar3 = FUN_004640a0(0x5ba568);
                piVar6 = (int *)FUN_00414580((void *)(iVar2 + 0x76b8),iVar3);
                *piVar6 = *piVar6 + 1;
              }
              iVar2 = FUN_004b7fa0(0x5ba568);
              if (iVar2 == 0) {
                iVar2 = FUN_0050fc50(DAT_005c6108);
                iVar3 = FUN_004640a0(0x5ba568);
                piVar6 = (int *)FUN_00414580((void *)(iVar2 + 0x76d4),iVar3);
                if (*piVar6 < 99999) {
                  iVar2 = FUN_0050fc50(DAT_005c6108);
                  iVar3 = FUN_004640a0(0x5ba568);
                  piVar6 = (int *)FUN_00414580((void *)(iVar2 + 0x76d4),iVar3);
                  *piVar6 = *piVar6 + 1;
                }
                iVar2 = FUN_0050fc50(DAT_005c6108);
                *(undefined4 *)(iVar2 + 0x76f4) = 1;
              }
            }
            FUN_004bce10(0x5ba568);
            FUN_004bd980(param_1);
          }
          else {
            FUN_004a0fb0(&DAT_005c4d40,4);
            DAT_005afcfc = -1;
          }
        }
        else {
          iVar2 = FUN_00488830(DAT_005ba828);
          if ((iVar2 == 0) && (iVar2 = FUN_00488830(DAT_005ba828), iVar2 == 0)) {
            iVar2 = FUN_0050fc50(DAT_005c6108);
            iVar3 = FUN_004640a0(0x5ba568);
            iVar4 = FUN_00474d60(0x5ba568);
            *(undefined1 *)(iVar2 + iVar3 * 0x90 + (iVar4 + -1) * 0x10 + 0x7700) = 1;
          }
          FUN_004be250(&DAT_005c4d40,0xc);
          iVar2 = FUN_00498f40();
          FUN_004bd8a0(iVar2);
          FUN_004bdd90(&DAT_005ba568,1);
        }
      }
    }
  }
  else {
    iVar2 = FUN_00474d60(0x5ba568);
    if (iVar2 != 7) {
      iVar2 = FUN_00474d60(0x5ba568);
      iVar2 = FUN_00488770(DAT_005c60fc,iVar2 + 1);
      if (iVar2 != 0) goto LAB_004bc5d0;
    }
    FUN_004e5f30();
  }
  return 0;
}

