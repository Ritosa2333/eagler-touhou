/* Entry: 0x005531ce; symbol: FUN_005531ce; body bytes: 85 */

undefined4 FUN_005531ce(void)

{
  uint *puVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  
  if (DAT_005e55ac != (undefined4 *)0x0) {
    return 0;
  }
  puVar1 = FUN_0055f18e();
  if (puVar1 == (uint *)0x0) {
    FUN_00557ba0((LPVOID)0x0);
    return 0xffffffff;
  }
  puVar2 = FUN_00553308((short *)puVar1);
  if (puVar2 == (undefined4 *)0x0) {
    uVar3 = 0xffffffff;
  }
  else {
    uVar3 = 0;
    DAT_005e55ac = puVar2;
    DAT_005e55b0 = puVar2;
  }
  FUN_00557ba0((LPVOID)0x0);
  FUN_00557ba0(puVar1);
  return uVar3;
}

