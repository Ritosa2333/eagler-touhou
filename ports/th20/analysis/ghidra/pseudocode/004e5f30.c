/* Entry: 0x004e5f30; symbol: FUN_004e5f30; body bytes: 359 */

void FUN_004e5f30(void)

{
  bool bVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  undefined3 extraout_var;
  int iVar5;
  float10 fVar6;
  undefined4 local_10;
  undefined4 local_c;
  void *local_8;
  
  uVar2 = FUN_004e6790((int)DAT_005c60fc);
  if (uVar2 == 0) {
    FUN_004e6a90(local_8,1);
    FUN_004e69a0(local_8,1);
    *(uint *)(DAT_005ba828 + 0xe8) = *(uint *)(DAT_005ba828 + 0xe8) | 0x10;
    FUN_004e60c0((int)local_8);
    uVar3 = FUN_004b8250(DAT_005c06a4);
    *(undefined4 *)((int)local_8 + 0x2d4) = uVar3;
    FUN_0044fcd0((undefined4 *)((int)local_8 + 200));
    local_c = *(undefined4 *)((int)local_8 + 0x2d4);
    puVar4 = FUN_0049e080(&local_10,(uint *)"front",0x92,(undefined4 *)0x0);
    *(undefined4 *)((int)local_8 + 200) = *puVar4;
    FUN_00486350((void *)((int)local_8 + 200));
    FUN_004ba440(0x5ba830);
    FUN_004e60a0(&DAT_005ba830);
    fVar6 = FUN_004292e0((float *)&DAT_005aefe4);
    *(float *)((int)local_8 + 0x1c4) = (float)fVar6;
    FUN_004292a0(&DAT_005aefe4,DAT_0056c8cc);
    *(undefined4 *)((int)local_8 + 0xec) = DAT_005b8858;
    DAT_005b8858 = 0;
    bVar1 = FUN_00478160(DAT_005c06a4);
    if (CONCAT31(extraout_var,bVar1) != 0) {
      iVar5 = FUN_004b8070(DAT_005c06a4);
      FUN_004b8b30(iVar5);
    }
    FUN_004b8810();
    FUN_004e6b30(DAT_005c06a4);
    *(uint *)((int)local_8 + 0x2d0) = *(uint *)((int)local_8 + 0x2d0) & 0xfffffffb;
    CFrameWnd::DelayUpdateFrameTitle(DAT_005c60fc);
  }
  return;
}

