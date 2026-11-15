// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 4fbff0

undefined4 __fastcall FUN_004fbff0(int param_1)

{
  short sVar1;
  int *piVar2;
  int iVar3;
  float fVar4;
  float fVar5;
  undefined4 uVar6;
  
  iVar3 = FUN_0040e0d0();
  while (iVar3 != 0) {
    fVar4 = *(float *)(iVar3 + 0x654) - *(float *)(param_1 + 0x4c);
    fVar5 = *(float *)(iVar3 + 0x650) - *(float *)(param_1 + 0x48);
    if (((fVar5 * fVar5 + fVar4 * fVar4 < DAT_005aa47c) &&
        (*(int *)(iVar3 + 0x1070) != *(int *)(param_1 + 0x5628))) &&
       (0x14 < *(int *)(iVar3 + 0x104c))) {
      *(int *)(iVar3 + 0x1070) = *(int *)(param_1 + 0x5628);
      sVar1 = *(short *)(iVar3 + 0x1078);
      if ((sVar1 == 0) || (sVar1 == 1)) {
        iVar3 = (int)*(short *)(iVar3 + 0x107a);
        uVar6 = 4;
      }
      else if ((sVar1 == 4) || (sVar1 == 5)) {
        iVar3 = 1;
        uVar6 = 0x12;
      }
      else if (sVar1 == 8) {
        iVar3 = 1;
        uVar6 = 0x14;
      }
      else if (sVar1 == 9) {
        iVar3 = 1;
        uVar6 = 0x1f;
      }
      else {
        if (sVar1 != 0x12) goto LAB_004fc0b9;
        iVar3 = 1;
        uVar6 = 0x21;
      }
      FUN_004f11a0(uVar6,iVar3);
    }
LAB_004fc0b9:
    iVar3 = **(int **)(param_1 + 0x563c);
    piVar2 = *(int **)(iVar3 + 0x8156e8 + *(int *)(iVar3 + 0x8156f8) * 4);
    if (piVar2 == (int *)0x0) break;
    *(int *)(iVar3 + 0x8156e8 + *(int *)(iVar3 + 0x8156f8) * 4) = piVar2[1];
    iVar3 = *piVar2;
  }
  iVar3 = **(int **)(param_1 + 0x563c);
  *(undefined4 *)(iVar3 + 0x8156e8 + *(int *)(iVar3 + 0x8156f8) * 4) = 0;
  if (0 < *(int *)(iVar3 + 0x8156f8)) {
    *(int *)(iVar3 + 0x8156f8) = *(int *)(iVar3 + 0x8156f8) + -1;
  }
  return 0;
}


// Reference 00599500
