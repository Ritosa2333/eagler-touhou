/* Entry: 0x004ffff0; symbol: FUN_004ffff0; body bytes: 102 */

undefined4 * __cdecl FUN_004ffff0(int param_1)

{
  undefined4 *puVar1;
  int iVar2;
  
  puVar1 = FUN_004f32b0();
  if (puVar1 == (undefined4 *)0x0) {
    puVar1 = (undefined4 *)0x0;
  }
  else {
    iVar2 = FUN_004f9520(param_1);
    if (iVar2 == 0) {
      FUN_004c20d0(param_1);
    }
    else {
      if (puVar1 != (undefined4 *)0x0) {
        FUN_0041f7c0(puVar1);
      }
      puVar1 = (undefined4 *)0x0;
    }
  }
  return puVar1;
}

