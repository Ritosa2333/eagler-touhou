/* Entry: 0x004d9040; symbol: FUN_004d9040; body bytes: 214 */

undefined4 FUN_004d9040(void)

{
  int *piVar1;
  int iVar2;
  
  if (DAT_005c4edc != 0) {
    FUN_0044f3d0(DAT_005c0028,0x1f,0);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0xe4))(piVar1,0xf,0);
    iVar2 = FUN_004ac0c0(0x5c4d40);
    if ((iVar2 == 7) || (iVar2 = FUN_004ac0c0(0x5c4d40), iVar2 == 0xc)) {
      FUN_0044c570(DAT_005c4ef8);
      FUN_0044fa10(DAT_005c4ef8,0xffffffff);
    }
    else {
      FUN_0044c570(DAT_005c4efc);
      FUN_0044fa10(DAT_005c4efc,0xffffffff);
    }
    FUN_004455c0((int)DAT_005c0028);
    piVar1 = (int *)FUN_00412730(0x5c4d40);
    (**(code **)(*piVar1 + 0xe4))(piVar1,0xf,1);
    return 1;
  }
  return 1;
}

