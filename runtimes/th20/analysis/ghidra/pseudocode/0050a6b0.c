/* Entry: 0x0050a6b0; symbol: FUN_0050a6b0; body bytes: 315 */

void __fastcall FUN_0050a6b0(int param_1)

{
  short sVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 *puVar4;
  int *piVar5;
  uint uVar6;
  
  if (*(int *)(param_1 + 0x10) == 0) {
    puVar2 = FUN_00507080();
    iVar3 = FUN_00474d60(0x5ba568);
    puVar4 = (undefined4 *)FUN_00414580((void *)(param_1 + 0x20),iVar3);
    *puVar4 = puVar2;
    iVar3 = FUN_00474d60(0x5ba568);
    piVar5 = (int *)FUN_00414580((void *)(param_1 + 0x20),iVar3);
    piVar5 = (int *)*piVar5;
    iVar3 = FUN_0041ca90(0x5ba4a8);
    piVar5[1] = iVar3;
    FUN_004d9940(&DAT_005ba4c4,piVar5[1]);
    FUN_0049da50((undefined4 *)&DAT_005ba4a8);
    sVar1 = FUN_00474d60(0x5ba568);
    *piVar5 = (int)sVar1;
    uVar6 = FUN_0049c000();
    piVar5[0xa6] = piVar5[0xa6] & 0xfffffffeU | uVar6 & 1;
  }
  else if (*(int *)(param_1 + 0x10) == 1) {
    iVar3 = FUN_00474d60(0x5ba568);
    puVar2 = (undefined4 *)FUN_00486e30((void *)(param_1 + 0xe8),iVar3);
    iVar3 = puVar2[4];
    FUN_00509e10(puVar2);
    FUN_004d9940(&DAT_005ba4a8,*(undefined4 *)(iVar3 + 4));
    FUN_004d9940(&DAT_005ba4c4,*(undefined4 *)(iVar3 + 4));
    FUN_0049da50((undefined4 *)&DAT_005ba4a8);
  }
  return;
}

