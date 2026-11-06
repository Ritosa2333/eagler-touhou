// Program SHA256: 8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9
// Address 54dc40

undefined4 __fastcall FUN_0054dc40(int param_1)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int extraout_ECX;
  int iVar4;
  float fVar5;
  float fVar6;
  double dVar7;
  float local_50;
  undefined8 local_4c;
  undefined4 local_44;
  
  if (*(int *)(param_1 + 0x9c) == 2) {
    return 0;
  }
  if (*(int *)(*(int *)(param_1 + 0x124) + 8) != 0) {
    iVar3 = *(int *)(param_1 + 0xa0);
    if (iVar3 == 0) {
      local_44 = *(undefined4 *)(param_1 + 0x5c);
      local_4c = *(undefined8 *)(param_1 + 0x54);
      piVar2 = (int *)FUN_00508530(&local_50,&local_4c);
      iVar3 = *piVar2;
      *(int *)(param_1 + 0xa0) = iVar3;
      if (iVar3 == 0) goto LAB_0054dec5;
    }
    iVar1 = *(int *)(*(int *)(param_1 + 0x124) + 8);
    iVar4 = *(int *)(iVar1 + 0x4b70);
    if (*(int *)(iVar1 + 0x4b60 + iVar4 * 4) != 0) {
      iVar4 = iVar4 + 1;
      *(int *)(iVar1 + 0x4b70) = iVar4;
      if (3 < iVar4) {
        *(undefined4 *)(iVar1 + 0x4b70) = 3;
        iVar4 = 3;
      }
    }
    piVar2 = *(int **)(iVar1 + 0x4b50);
    if (piVar2 == (int *)0x0) {
      *(undefined4 *)(iVar1 + 0x4b60 + iVar4 * 4) = 0;
    }
    else {
      do {
        *(int *)(iVar1 + 0x4b60 + iVar4 * 4) = piVar2[1];
        if (*(int *)(*piVar2 + 0x6858) == iVar3) {
          *(undefined4 *)(iVar1 + 0x4b60 + *(int *)(iVar1 + 0x4b70) * 4) = 0;
          if (0 < *(int *)(iVar1 + 0x4b70)) {
            *(int *)(iVar1 + 0x4b70) = *(int *)(iVar1 + 0x4b70) + -1;
          }
          FUN_0050d3e0();
          iVar3 = FUN_0042fc50();
          if (iVar3 == 0) {
            fVar5 = *(float *)(extraout_ECX + 0x127c) - *(float *)(param_1 + 0x58);
            FUN_00407490();
            FUN_00403b70();
            fVar6 = *(float *)(param_1 + 0x6c);
            if (*(int *)(param_1 + 0x1c) < 0x3c) {
              dVar7 = _fabs((double)fVar5);
              local_50 = (float)dVar7;
              if (local_50 < DAT_005aa2bc / DAT_005aa2d4) {
                dVar7 = _fabs((double)fVar5);
                local_50 = (float)dVar7;
                fVar5 = fVar6;
                if ((local_50 < DAT_005aa2bc / DAT_005aa300) &&
                   (fVar5 = DAT_005aa30c, fVar6 + DAT_005aa1f4 <= DAT_005aa30c)) {
                  fVar5 = fVar6 + DAT_005aa1f4;
                }
              }
              else {
                fVar5 = DAT_005aa2d4;
                if (DAT_005aa2d4 <= fVar6 - DAT_005aa1f4) {
                  fVar5 = fVar6 - DAT_005aa1f4;
                }
              }
              FUN_004074f0();
              FUN_0040fdf0();
              *(float *)(param_1 + 0x6c) = fVar5;
              return 0;
            }
            *(float *)(param_1 + 0x6c) = fVar6 + DAT_005aa1f4;
            return 0;
          }
          goto LAB_0054debb;
        }
        iVar4 = *(int *)(iVar1 + 0x4b70);
        piVar2 = *(int **)(iVar1 + 0x4b60 + iVar4 * 4);
      } while (piVar2 != (int *)0x0);
    }
    *(undefined4 *)(iVar1 + 0x4b60 + *(int *)(iVar1 + 0x4b70) * 4) = 0;
    if (0 < *(int *)(iVar1 + 0x4b70)) {
      *(int *)(iVar1 + 0x4b70) = *(int *)(iVar1 + 0x4b70) + -1;
    }
  }
LAB_0054debb:
  *(undefined4 *)(param_1 + 0xa0) = 0;
LAB_0054dec5:
  fVar5 = *(float *)(param_1 + 0x6c) + DAT_005aa1ec;
  fVar6 = DAT_005aa30c;
  if (fVar5 <= DAT_005aa30c) {
    fVar6 = fVar5;
  }
  *(float *)(param_1 + 0x6c) = fVar6;
  return 0;
}


// Reference 005a0404
// Reference 005a0420
