/* Entry: 0x004e6b80; symbol: FUN_004e6b80; body bytes: 76 */

undefined4 * FUN_004e6b80(void)

{
  undefined4 *puVar1;
  int iVar2;
  
  puVar1 = FUN_004e1ba0();
  iVar2 = FUN_004e5980((int)puVar1);
  if (iVar2 != 0) {
    if (puVar1 != (undefined4 *)0x0) {
      FUN_0041f7c0(puVar1);
    }
    puVar1 = (undefined4 *)0x0;
  }
  return puVar1;
}

