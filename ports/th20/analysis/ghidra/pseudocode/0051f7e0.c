/* Entry: 0x0051f7e0; symbol: FUN_0051f7e0; body bytes: 161 */

undefined4 FUN_0051f7e0(void)

{
  char cVar1;
  int iVar2;
  
  iVar2 = FUN_0051f3c0(DAT_005c6124);
  if (iVar2 == 0) {
    while (cVar1 = FUN_0044d880(DAT_005c0028), cVar1 == '\0') {
      Sleep(1);
    }
    if (DAT_005c4d2c != 0) {
      while ((iVar2 = FUN_0052c280(DAT_005c4d2c), iVar2 < 0xb4 && ((DAT_005c5888 >> 5 & 3) == 0))) {
        Sleep(0x10);
      }
      FUN_0052c1f0();
    }
    FUN_00412d80(*(int *)(DAT_005c6124 + 8));
    DAT_005b8858 = 1;
  }
  else {
    FUN_004a0fb0(&DAT_005c4d40,3);
  }
  return 0;
}

