/* Entry: 0x004e59e0; symbol: FUN_004e59e0; body bytes: 492 */

void FUN_004e59e0(void)

{
  bool bVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  undefined3 extraout_var;
  int iVar4;
  float10 fVar5;
  undefined4 local_10;
  undefined4 local_c;
  void *local_8;
  
  FUN_004bce10(0x5ba568);
  if (*(int *)(DAT_005ba828 + 0x108) == 1) {
    FUN_004a0fb0(&DAT_005c4d40,4);
  }
  else {
    *(uint *)(DAT_005ba828 + 0xe8) = *(uint *)(DAT_005ba828 + 0xe8) | 0x10;
    FUN_004e6a90(local_8,3);
    FUN_004e69a0(local_8,3);
    bVar1 = FUN_00488850(0x5ba568);
    if (bVar1) {
      FUN_004e69a0(local_8,5);
    }
    bVar1 = FUN_004bd820(0x5ba568);
    if (!bVar1) {
      bVar1 = FUN_00488850(0x5ba568);
      if (!bVar1) {
        local_c = FUN_00470670();
        puVar2 = FUN_0049e080(&local_10,(uint *)&DAT_0056fe1c,0x57,(undefined4 *)0x0);
        *(undefined4 *)((int)local_8 + 0xcc) = *puVar2;
        FUN_004e6600(DAT_005c0028,*(undefined4 *)((int)local_8 + 0xcc),
                     (int)(DAT_0056cd94 * DAT_005b8818),(int)(DAT_0056cd90 * DAT_005b8818),
                     (int)(DAT_0056cda4 * DAT_005b8818),(int)(DAT_0056d7c0 * DAT_005b8818));
      }
    }
    uVar3 = FUN_004b8250(DAT_005c06a4);
    *(undefined4 *)((int)local_8 + 0x2d4) = uVar3;
    *(undefined4 *)((int)local_8 + 0xe0) = 1;
    fVar5 = FUN_004292e0((float *)&DAT_005aefe4);
    *(float *)((int)local_8 + 0x1c4) = (float)fVar5;
    FUN_004292a0(&DAT_005aefe4,DAT_0056c8cc);
    *(undefined4 *)((int)local_8 + 0xec) = DAT_005b8858;
    DAT_005b8858 = 1;
    bVar1 = FUN_00478160(DAT_005c06a4);
    if (CONCAT31(extraout_var,bVar1) != 0) {
      iVar4 = FUN_004b8070(DAT_005c06a4);
      FUN_004b8b30(iVar4);
    }
    FUN_004b8810();
    *(uint *)((int)local_8 + 0x2d0) = *(uint *)((int)local_8 + 0x2d0) | 4;
  }
  return;
}

