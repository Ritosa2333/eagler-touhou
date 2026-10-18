/* Entry: 0x00483270; symbol: FUN_00483270; body bytes: 764 */

undefined4 __fastcall FUN_00483270(int param_1)

{
  float fVar1;
  int iVar2;
  int iVar3;
  
  iVar2 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
  fVar1 = *(float *)(iVar2 + 0x48);
  iVar2 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
  iVar2 = FUN_004854d0((float *)(param_1 + 100),*(float *)(iVar2 + 0x4c),fVar1);
  if (iVar2 != 0) {
    iVar2 = FUN_0047c680((void *)(param_1 + 0xa0),6);
    if (((*(uint *)(iVar2 + 0x38) & 1) == 0) || (0.0 <= *(float *)(param_1 + 0x68))) {
      iVar2 = FUN_0047c680((void *)(param_1 + 0xa0),6);
      if (((*(uint *)(iVar2 + 0x38) & 2) == 0) || (*(float *)(param_1 + 0x68) <= DAT_0056d7c0)) {
        iVar2 = FUN_0047c680((void *)(param_1 + 0xa0),6);
        if (((*(uint *)(iVar2 + 0x38) & 4) == 0) ||
           (DAT_0056fd6c / DAT_0056c8d0 < *(float *)(param_1 + 100) ||
            DAT_0056fd6c / DAT_0056c8d0 == *(float *)(param_1 + 100))) {
          iVar2 = FUN_0047c680((void *)(param_1 + 0xa0),6);
          if ((*(uint *)(iVar2 + 0x38) & 8) == 0) {
            return 0;
          }
          if (*(float *)(param_1 + 100) <= DAT_0056cda4 / DAT_0056c8d0) {
            return 0;
          }
          iVar2 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
          *(float *)(param_1 + 100) =
               *(float *)(param_1 + 100) - (DAT_0056cda4 + *(float *)(iVar2 + 0x4c));
        }
        else {
          iVar2 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
          *(float *)(param_1 + 100) =
               *(float *)(param_1 + 100) + DAT_0056cda4 + *(float *)(iVar2 + 0x4c);
        }
      }
      else {
        iVar2 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
        *(float *)(param_1 + 0x68) =
             *(float *)(param_1 + 0x68) - (DAT_0056d7c0 + *(float *)(iVar2 + 0x48));
      }
    }
    else {
      iVar2 = FUN_00437cd0(*(int *)(param_1 + 0x1c));
      *(float *)(param_1 + 0x68) =
           *(float *)(param_1 + 0x68) + DAT_0056d7c0 + *(float *)(iVar2 + 0x48);
    }
    iVar2 = FUN_0047c680((void *)(param_1 + 0xa0),6);
    *(int *)(iVar2 + 0x30) = *(int *)(iVar2 + 0x30) + 1;
    if (-1 < *(int *)(param_1 + 0x40)) {
      FUN_00426d70(&DAT_005ba830,*(int *)(param_1 + 0x40),0);
    }
    iVar2 = FUN_0047c680((void *)(param_1 + 0xa0),6);
    iVar3 = FUN_0047c680((void *)(param_1 + 0xa0),6);
    if (*(int *)(iVar2 + 0x34) <= *(int *)(iVar3 + 0x30)) {
      *(uint *)(param_1 + 0x90) = *(uint *)(param_1 + 0x90) & 0xffffefff;
      return 1;
    }
  }
  return 0;
}

