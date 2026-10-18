/* Entry: 0x0053b3d0; symbol: FUN_0053b3d0; body bytes: 57 */

undefined4 * FUN_0053b3d0(void)

{
  undefined4 *puVar1;
  undefined4 local_c;
  
  puVar1 = (undefined4 *)operator_new(0x48);
  if (puVar1 == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    local_c = FUN_004a3580(puVar1);
  }
  return local_c;
}

