/* Entry: 0x00534fd0; symbol: FUN_00534fd0; body bytes: 220 */

undefined4 __fastcall FUN_00534fd0(int param_1)

{
  int iVar1;
  void *pvVar2;
  float *pfVar3;
  undefined4 *puVar4;
  undefined4 uVar5;
  
  iVar1 = FUN_00464230(0);
  iVar1 = FUN_004ff6d0(iVar1);
  if (iVar1 == 2) {
    pvVar2 = (void *)FUN_00478e80(0);
    iVar1 = FUN_00460830(0);
    pfVar3 = (float *)FUN_00460850(iVar1);
    FUN_0047cf60(pvVar2,pfVar3,DAT_0056cd94,0,99999,0);
    pvVar2 = (void *)FUN_00478ee0(0);
    iVar1 = FUN_00460830(0);
    puVar4 = (undefined4 *)FUN_00460850(iVar1);
    FUN_004caad0(pvVar2,puVar4,DAT_0056cd94,0,1);
    FUN_00534fa0((void *)(param_1 + 0x14),DAT_0056ec98);
    uVar5 = 1;
  }
  else {
    FUN_0044fcd0((undefined4 *)&DAT_005c6970);
    uVar5 = FUN_00532f20(param_1);
  }
  return uVar5;
}

