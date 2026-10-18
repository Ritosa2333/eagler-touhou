/* Entry: 0x004e5d60; symbol: FUN_004e5d60; body bytes: 440 */

void FUN_004e5d60(void)

{
  bool bVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined3 extraout_var;
  float10 fVar5;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  void *local_8;
  
  FUN_004bce10(0x5ba568);
  FUN_004e6a90(local_8,1);
  *(uint *)(DAT_005ba828 + 0xe8) = *(uint *)(DAT_005ba828 + 0xe8) | 0x10;
  uVar2 = FUN_004b8250(DAT_005c06a4);
  *(undefined4 *)((int)local_8 + 0x2d4) = uVar2;
  FUN_0044fcd0((undefined4 *)((int)local_8 + 200));
  iVar3 = FUN_00488830(DAT_005ba828);
  if (iVar3 == 0) {
    local_10 = *(undefined4 *)((int)local_8 + 0x2d4);
    puVar4 = FUN_0049e080(&local_18,(uint *)"front",0x90,(undefined4 *)0x0);
    *(undefined4 *)((int)local_8 + 200) = *puVar4;
  }
  else {
    local_c = *(undefined4 *)((int)local_8 + 0x2d4);
    puVar4 = FUN_0049e080(&local_14,(uint *)"front",0x91,(undefined4 *)0x0);
    *(undefined4 *)((int)local_8 + 200) = *puVar4;
  }
  FUN_00486350((void *)((int)local_8 + 200));
  FUN_004ba440(0x5ba830);
  FUN_00426d70(&DAT_005ba830,0xe,0);
  bVar1 = FUN_00488850(0x5ba568);
  if (!bVar1) {
    FUN_004e60a0(&DAT_005ba830);
  }
  do {
    iVar3 = FUN_004277f0(&DAT_005ba830);
  } while (iVar3 != 0);
  FUN_004e60c0((int)local_8);
  fVar5 = FUN_004292e0((float *)&DAT_005aefe4);
  *(float *)((int)local_8 + 0x1c4) = (float)fVar5;
  FUN_004292a0(&DAT_005aefe4,DAT_0056c8cc);
  *(undefined4 *)((int)local_8 + 0xec) = DAT_005b8858;
  DAT_005b8858 = 0;
  bVar1 = FUN_00478160(DAT_005c06a4);
  if (CONCAT31(extraout_var,bVar1) != 0) {
    iVar3 = FUN_004b8070(DAT_005c06a4);
    FUN_004b8b30(iVar3);
  }
  FUN_004b8810();
  FUN_004e6b30(DAT_005c06a4);
  *(uint *)((int)local_8 + 0x2d0) = *(uint *)((int)local_8 + 0x2d0) & 0xfffffffb;
  return;
}

