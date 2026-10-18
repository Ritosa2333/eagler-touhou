/* Entry: 0x00506570; symbol: FUN_00506570; body bytes: 781 */

undefined4 __cdecl FUN_00506570(float *param_1)

{
  int iVar1;
  undefined4 local_8;
  
  iVar1 = FUN_00437660(0x5b6758);
  if (*param_1 <= (float)iVar1 - DAT_0056cda4 / DAT_0056c8d0) {
LAB_00506633:
    iVar1 = FUN_00437660(0x5b6758);
    if ((float)iVar1 - DAT_0056cda4 / DAT_0056c8d0 < param_1[3]) {
      iVar1 = FUN_00437660(0x5b6758);
      if (param_1[3] < (float)iVar1 + DAT_0056cda4 / DAT_0056c8d0) {
        iVar1 = FUN_004376a0(0x5b6758);
        if ((float)iVar1 < param_1[4]) {
          iVar1 = FUN_004376a0(0x5b6758);
          if (param_1[4] < (float)iVar1 + DAT_0056d7c0) goto LAB_0050686e;
        }
      }
    }
    iVar1 = FUN_00437660(0x5b6758);
    if ((float)iVar1 - DAT_0056cda4 / DAT_0056c8d0 < param_1[6]) {
      iVar1 = FUN_00437660(0x5b6758);
      if (param_1[6] < (float)iVar1 + DAT_0056cda4 / DAT_0056c8d0) {
        iVar1 = FUN_004376a0(0x5b6758);
        if ((float)iVar1 < param_1[7]) {
          iVar1 = FUN_004376a0(0x5b6758);
          if (param_1[7] < (float)iVar1 + DAT_0056d7c0) goto LAB_0050686e;
        }
      }
    }
    iVar1 = FUN_00437660(0x5b6758);
    if ((float)iVar1 - DAT_0056cda4 / DAT_0056c8d0 < param_1[9]) {
      iVar1 = FUN_00437660(0x5b6758);
      if (param_1[9] < (float)iVar1 + DAT_0056cda4 / DAT_0056c8d0) {
        iVar1 = FUN_004376a0(0x5b6758);
        if ((float)iVar1 < param_1[10]) {
          iVar1 = FUN_004376a0(0x5b6758);
          if (param_1[10] < (float)iVar1 + DAT_0056d7c0) goto LAB_0050686e;
        }
      }
    }
    local_8 = 1;
  }
  else {
    iVar1 = FUN_00437660(0x5b6758);
    if ((float)iVar1 + DAT_0056cda4 / DAT_0056c8d0 <= *param_1) goto LAB_00506633;
    iVar1 = FUN_004376a0(0x5b6758);
    if (param_1[1] <= (float)iVar1) goto LAB_00506633;
    iVar1 = FUN_004376a0(0x5b6758);
    if ((float)iVar1 + DAT_0056d7c0 <= param_1[1]) goto LAB_00506633;
LAB_0050686e:
    local_8 = 0;
  }
  return local_8;
}

