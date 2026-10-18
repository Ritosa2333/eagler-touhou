/* Entry: 0x004193e0; symbol: FUN_004193e0; body bytes: 884 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004193e0(int param_1)

{
  double dVar1;
  undefined4 uVar2;
  int iVar3;
  int *piVar4;
  float10 fVar5;
  int local_10 [3];
  
  local_10[2] = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  fVar5 = (float10)FUN_0041cb10();
  *(double *)(param_1 + 0x20c8) = (double)fVar5;
  *(int *)(param_1 + 0x20fc) =
       (int)(((DAT_0056c468 / _DAT_0056c470 + *(double *)(param_1 + 0x20e8)) -
             *(double *)(param_1 + 0x20c8)) * _DAT_0056cd88 - _DAT_0056cd78);
  iVar3 = FUN_0041ca50(param_1);
  if (*(int *)(iVar3 + 4) < *(int *)(param_1 + 0x20fc)) {
    iVar3 = FUN_0041ca50(param_1);
    *(int *)(iVar3 + 8) = *(int *)(iVar3 + 8) + 1;
    iVar3 = FUN_0041ca50(param_1);
    if (0xe < *(int *)(iVar3 + 8)) {
      iVar3 = FUN_0041ca50(param_1);
      piVar4 = (int *)FUN_0041ca50(param_1);
      if (*(int *)(iVar3 + 4) < *piVar4) {
        iVar3 = FUN_0041ca50(param_1);
        *(int *)(iVar3 + 4) = *(int *)(iVar3 + 4) + 1;
      }
      iVar3 = FUN_0041ca50(param_1);
      *(undefined4 *)(iVar3 + 8) = 0;
    }
  }
  else {
    if (*(int *)(param_1 + 0x20fc) < 0) {
      iVar3 = FUN_0041ca50(param_1);
      *(undefined4 *)(iVar3 + 4) = 0;
    }
    else {
      uVar2 = *(undefined4 *)(param_1 + 0x20fc);
      iVar3 = FUN_0041ca50(param_1);
      *(undefined4 *)(iVar3 + 4) = uVar2;
    }
    iVar3 = FUN_0041ca50(param_1);
    *(undefined4 *)(iVar3 + 8) = 0;
  }
  if (*(int *)(param_1 + 0x20fc) < 0) {
    *(undefined4 *)(param_1 + 0x20fc) = 0;
  }
  local_10[0] = 0;
  local_10[1] = 0;
  fVar5 = (float10)FUN_0041cb10();
  dVar1 = (double)fVar5;
  if (dVar1 < *(double *)(param_1 + 0x20f0)) {
    *(double *)(param_1 + 0x20f0) = dVar1;
  }
  if (DAT_0056cd60 <= dVar1 - *(double *)(param_1 + 0x20f0)) {
    iVar3 = FUN_0041ca50(param_1);
    if (0 < *(int *)(iVar3 + 4)) {
      iVar3 = FUN_0041ca50(param_1);
      *(int *)(iVar3 + 4) = *(int *)(iVar3 + 4) + -1;
    }
  }
  else {
    iVar3 = FUN_00415800(0x5c4d40);
    if (iVar3 == 0) {
      while (fVar5 = (float10)FUN_0041cb10(),
            (double)fVar5 - *(double *)(param_1 + 0x20f0) < DAT_0056cd58) {
        Sleep(1);
      }
      local_10[0] = 0;
      do {
        piVar4 = (int *)FUN_00412730(0x5c4d40);
        iVar3 = (**(code **)(*piVar4 + 0x4c))(piVar4,0,local_10);
        if (iVar3 != 0) break;
      } while (local_10[0] == 0);
    }
  }
  fVar5 = (float10)FUN_0041cb10();
  *(double *)(param_1 + 0x20f0) = (double)fVar5;
  FUN_00419760();
  piVar4 = (int *)FUN_00412730(0x5c4d40);
  iVar3 = (**(code **)(*piVar4 + 0x44))(piVar4,0,0,0,0);
  if (iVar3 < 0) {
    FUN_004dd840(&DAT_005c4d40);
    FUN_0041dbe0(DAT_005c0028);
    piVar4 = (int *)FUN_00412730(0x5c4d40);
    iVar3 = (**(code **)(*piVar4 + 0x40))(piVar4,&DAT_005c4e24);
    if (iVar3 == 0) {
      FUN_0041d9f0(DAT_005c0028);
      FUN_004dbd70(0x5c4d40);
      FUN_0041a2c0();
      _DAT_005c586c = 2;
    }
  }
  FUN_004193c0();
  FUN_0041cb10();
  iVar3 = FUN_0041ca50(param_1);
  if (0 < *(int *)(iVar3 + 4)) {
    timeBeginPeriod(1);
    iVar3 = FUN_0041ca50(param_1);
    Sleep(*(DWORD *)(iVar3 + 4));
    timeEndPeriod(1);
  }
  fVar5 = (float10)FUN_0041cb10();
  *(double *)(param_1 + 0x20e8) = (double)fVar5;
  FUN_005429ee(local_10[2] ^ (uint)&stack0xfffffffc);
  return;
}

