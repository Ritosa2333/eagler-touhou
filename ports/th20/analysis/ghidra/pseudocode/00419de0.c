/* Entry: 0x00419de0; symbol: FUN_00419de0; body bytes: 581 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00419de0(int param_1)

{
  int iVar1;
  int *piVar2;
  float10 fVar3;
  
  fVar3 = (float10)FUN_0041cb10();
  *(double *)(param_1 + 0x20c8) = (double)fVar3;
  if (*(double *)(param_1 + 0x20c8) <= *(double *)(param_1 + 0x20d0) &&
      *(double *)(param_1 + 0x20d0) != *(double *)(param_1 + 0x20c8)) {
    *(undefined8 *)(param_1 + 0x20d8) = *(undefined8 *)(param_1 + 0x20c8);
  }
  *(undefined8 *)(param_1 + 0x20d0) = *(undefined8 *)(param_1 + 0x20c8);
  if (_DAT_0056cd70 <=
      (*(double *)(param_1 + 0x20d8) - *(double *)(param_1 + 0x20c8)) * _DAT_0056cd88) {
    Sleep(1);
  }
  if (*(double *)(param_1 + 0x20d8) <= *(double *)(param_1 + 0x20c8) &&
      *(double *)(param_1 + 0x20c8) != *(double *)(param_1 + 0x20d8)) {
    while (*(double *)(param_1 + 0x20d8) <= *(double *)(param_1 + 0x20c8) &&
           *(double *)(param_1 + 0x20c8) != *(double *)(param_1 + 0x20d8)) {
      *(double *)(param_1 + 0x20d8) = DAT_0056c468 / _DAT_0056c470 + *(double *)(param_1 + 0x20d8);
    }
    FUN_004455c0(DAT_005c0028);
    FUN_0041dce0(&DAT_005c4d40,2);
    iVar1 = FUN_00412810(DAT_005b66d8);
    if (iVar1 == 0) {
      FUN_004d9e30((char **)&DAT_005c4d40);
      return 1;
    }
    if (iVar1 == -1) {
      FUN_004d9e30((char **)&DAT_005c4d40);
      return 2;
    }
    *(char *)(param_1 + 0x70) = *(char *)(param_1 + 0x70) + '\x01';
    if ((int)(DAT_005c4f84 + 1) <= (int)*(char *)(param_1 + 0x70)) {
      piVar2 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar2 + 0xa4))(piVar2);
      FUN_00445a40(DAT_005c0028);
      _DAT_005c5af8 = 0xff;
      FUN_004dda60(0x5c4d40);
      FUN_00412aa0(DAT_005b66d8);
      FUN_004455c0(DAT_005c0028);
      piVar2 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar2 + 0x104))(piVar2,0,0);
      piVar2 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar2 + 0xa8))(piVar2);
      *(undefined1 *)(param_1 + 0x70) = 0;
      FUN_004199a0();
    }
    fVar3 = (float10)FUN_0041cb10();
    _DAT_005c5b18 = (double)fVar3 - *(double *)(param_1 + 0x20c8);
    FUN_00412540();
  }
  return 0;
}

