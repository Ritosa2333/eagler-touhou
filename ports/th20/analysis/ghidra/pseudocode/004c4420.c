/* Entry: 0x004c4420; symbol: FUN_004c4420; body bytes: 381 */

undefined4 __fastcall FUN_004c4420(int param_1)

{
  uint uVar1;
  void *pvVar2;
  int iVar3;
  
  *(undefined4 *)(param_1 + 0xc24) = 2;
  if (*(int *)(param_1 + 0xc28) == 0xd) {
    uVar1 = FUN_00423ee0(0x5ba4a8);
    if (uVar1 % 8 < 5) {
      pvVar2 = (void *)FUN_00464080(0);
      uVar1 = FUN_00423ee0(0x5ba4a8);
      iVar3 = FUN_004641d0(pvVar2,uVar1 % 4);
      *(int *)(param_1 + 0xc28) = iVar3 / 2 + 9;
    }
    else {
      uVar1 = FUN_00423ee0(0x5ba4a8);
      *(uint *)(param_1 + 0xc28) = uVar1 % 4 + 9;
    }
  }
  switch(*(undefined4 *)(param_1 + 0xc28)) {
  case 9:
    iVar3 = FUN_0051b960();
    pvVar2 = (void *)FUN_00411700(iVar3);
    FUN_00438380(pvVar2,(void *)(param_1 + 0x18),0x25);
    break;
  case 10:
    iVar3 = FUN_0051b960();
    pvVar2 = (void *)FUN_00411700(iVar3);
    FUN_00438380(pvVar2,(void *)(param_1 + 0x18),0x26);
    break;
  case 0xb:
    iVar3 = FUN_0051b960();
    pvVar2 = (void *)FUN_00411700(iVar3);
    FUN_00438380(pvVar2,(void *)(param_1 + 0x18),0x27);
    break;
  case 0xc:
    iVar3 = FUN_0051b960();
    pvVar2 = (void *)FUN_00411700(iVar3);
    FUN_00438380(pvVar2,(void *)(param_1 + 0x18),0x28);
  }
  FUN_0045dd60((void *)(param_1 + 0x18),(undefined4 *)(param_1 + 0xbe4));
  FUN_0044fd00(param_1 + 0x5fc);
  return 0;
}

