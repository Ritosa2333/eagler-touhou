/* Entry: 0x00486630; symbol: FUN_00486630; body bytes: 90 */

undefined4 * __cdecl FUN_00486630(int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  
  puVar1 = FUN_0047b5f0();
  if (puVar1 == (undefined4 *)0x0) {
    puVar1 = (undefined4 *)0x0;
  }
  else {
    iVar2 = FUN_00480fa0(param_1);
    if (iVar2 != 0) {
      if (puVar1 != (undefined4 *)0x0) {
        FUN_0041f7c0(puVar1);
      }
      puVar1 = (undefined4 *)0x0;
    }
  }
  return puVar1;
}

