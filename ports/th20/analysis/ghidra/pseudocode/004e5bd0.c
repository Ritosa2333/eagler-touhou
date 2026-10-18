/* Entry: 0x004e5bd0; symbol: FUN_004e5bd0; body bytes: 400 */

void __fastcall FUN_004e5bd0(void *param_1)

{
  bool bVar1;
  int iVar2;
  undefined4 uVar3;
  float10 fVar4;
  
  FUN_004bce10(0x5ba568);
  if (*(int *)(DAT_005ba828 + 0x108) == 1) {
    FUN_004a0fb0(&DAT_005c4d40,4);
  }
  else {
    FUN_004e6a90(param_1,2);
    FUN_004e69a0(param_1,2);
    *(uint *)(DAT_005ba828 + 0xe8) = *(uint *)(DAT_005ba828 + 0xe8) | 0x10;
    FUN_004ba440(0x5ba830);
    FUN_00426d70(&DAT_005ba830,0xe,0);
    bVar1 = FUN_00488850(0x5ba568);
    if (!bVar1) {
      FUN_004e60a0(&DAT_005ba830);
    }
    do {
      iVar2 = FUN_004277f0(&DAT_005ba830);
    } while (iVar2 != 0);
    FUN_004e60c0((int)param_1);
    uVar3 = FUN_004b8250(DAT_005c06a4);
    *(undefined4 *)((int)param_1 + 0x2d4) = uVar3;
    bVar1 = FUN_00488850(0x5ba568);
    if (!bVar1) {
      iVar2 = FUN_004b8090(0x5ba830);
      FUN_00548610((char *)((int)param_1 + 0x1d0),0x100,iVar2);
      fVar4 = FUN_004e6710(0x5ba830);
      *(double *)((int)param_1 + 0x1c8) = (double)fVar4;
      FUN_004d9a70(0,0x572628);
      FUN_004d9b50(0,0);
    }
    *(undefined4 *)((int)param_1 + 0xe0) = 0;
    fVar4 = FUN_004292e0((float *)&DAT_005aefe4);
    *(float *)((int)param_1 + 0x1c4) = (float)fVar4;
    FUN_004292a0(&DAT_005aefe4,DAT_0056c8cc);
    *(undefined4 *)((int)param_1 + 0xec) = DAT_005b8858;
    DAT_005b8858 = 1;
    *(uint *)((int)param_1 + 0x2d0) = *(uint *)((int)param_1 + 0x2d0) & 0xfffffffb;
  }
  return;
}

