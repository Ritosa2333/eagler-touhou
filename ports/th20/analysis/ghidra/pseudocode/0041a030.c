/* Entry: 0x0041a030; symbol: FUN_0041a030; body bytes: 368 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_0041a030(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int *piVar3;
  float10 fVar4;
  
  FUN_004455c0(DAT_005c0028);
  FUN_0041dce0(&DAT_005c4d40,2);
  iVar1 = FUN_00412810(DAT_005b66d8);
  if (iVar1 == 0) {
    FUN_004d9e30((char **)&DAT_005c4d40);
    uVar2 = 1;
  }
  else if (iVar1 == -1) {
    FUN_004d9e30((char **)&DAT_005c4d40);
    uVar2 = 2;
  }
  else {
    *(char *)(param_1 + 0x70) = *(char *)(param_1 + 0x70) + '\x01';
    if ((int)(DAT_005c4f84 + 1) <= (int)*(char *)(param_1 + 0x70)) {
      piVar3 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar3 + 0xa4))(piVar3);
      FUN_00445a40(DAT_005c0028);
      _DAT_005c5af8 = 0xff;
      FUN_004dda60(0x5c4d40);
      FUN_00412aa0(DAT_005b66d8);
      FUN_004455c0(DAT_005c0028);
      piVar3 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar3 + 0x104))(piVar3,0,0);
      piVar3 = (int *)FUN_00412730(0x5c4d40);
      (**(code **)(*piVar3 + 0xa8))(piVar3);
      *(undefined1 *)(param_1 + 0x70) = 0;
      FUN_00419a50(param_1);
    }
    fVar4 = (float10)FUN_0041cb10();
    _DAT_005c5b18 = (double)fVar4 - *(double *)(param_1 + 0x20c8);
    FUN_00412540();
    uVar2 = 0;
  }
  return uVar2;
}

