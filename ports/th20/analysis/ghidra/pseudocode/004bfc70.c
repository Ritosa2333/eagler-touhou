/* Entry: 0x004bfc70; symbol: FUN_004bfc70; body bytes: 86 */

undefined4 * FUN_004bfc70(void)

{
  undefined4 *puVar1;
  int iVar2;
  
  puVar1 = FUN_004bedf0();
  if (puVar1 == (undefined4 *)0x0) {
    puVar1 = (undefined4 *)0x0;
  }
  else {
    iVar2 = FUN_004bf990((int)puVar1);
    if (iVar2 != 0) {
      if (puVar1 != (undefined4 *)0x0) {
        FUN_0041f7c0(puVar1);
      }
      puVar1 = (undefined4 *)0x0;
    }
  }
  return puVar1;
}

