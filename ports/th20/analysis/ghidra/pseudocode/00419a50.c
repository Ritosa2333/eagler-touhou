/* Entry: 0x00419a50; symbol: FUN_00419a50; body bytes: 385 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00419a50(int param_1)

{
  int *piVar1;
  int iVar2;
  DWORD dwMilliseconds;
  float10 fVar3;
  
  fVar3 = (float10)FUN_0041cb10();
  *(double *)(param_1 + 0x20c8) = (double)fVar3;
  if ((((DAT_005c4f89 == '\x01') &&
       (*(double *)(param_1 + 0x20c8) - *(double *)(param_1 + 0x20e8) < DAT_0056c468 / _DAT_0056c470
       )) && (_DAT_0056cd50 < *(double *)(param_1 + 0x20c8) - *(double *)(param_1 + 0x20e8))) &&
     (dwMilliseconds =
           (DWORD)(((DAT_0056c468 / _DAT_0056c470 + *(double *)(param_1 + 0x20e8)) -
                   *(double *)(param_1 + 0x20c8)) * _DAT_0056cd88 - _DAT_0056cd80),
     0 < (int)dwMilliseconds)) {
    Sleep(dwMilliseconds);
  }
  fVar3 = (float10)FUN_0041cb10();
  *(double *)(param_1 + 0x20f0) = (double)fVar3;
  FUN_00419760();
  piVar1 = (int *)FUN_00412730(0x5c4d40);
  iVar2 = (**(code **)(*piVar1 + 0x44))(piVar1,0,0,0,0);
  if (iVar2 < 0) {
    FUN_004dd840(&DAT_005c4d40);
    FUN_0041dbe0(DAT_005c0028);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0x40))(piVar1,&DAT_005c4e24);
    FUN_0041d9f0(DAT_005c0028);
    FUN_004dbd70(0x5c4d40);
    FUN_0041a2c0();
    _DAT_005c586c = 2;
  }
  fVar3 = (float10)FUN_0041cb10();
  *(double *)(param_1 + 0x20e8) = (double)fVar3;
  FUN_004193c0();
  return;
}

