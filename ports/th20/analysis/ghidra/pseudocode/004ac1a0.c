/* Entry: 0x004ac1a0; symbol: FUN_004ac1a0; body bytes: 86 */

undefined4 * FUN_004ac1a0(void)

{
  undefined4 *puVar1;
  int iVar2;
  
  puVar1 = FUN_004abad0();
  if (puVar1 == (undefined4 *)0x0) {
    puVar1 = (undefined4 *)0x0;
  }
  else {
    iVar2 = FUN_004ac080((int)puVar1);
    if (iVar2 != 0) {
      if (puVar1 != (undefined4 *)0x0) {
        FUN_0041f7c0(puVar1);
      }
      puVar1 = (undefined4 *)0x0;
    }
  }
  return puVar1;
}

