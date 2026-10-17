/* Entry: 0x004c7f90; symbol: FUN_004c7f90; body bytes: 57 */

undefined4 * FUN_004c7f90(void)

{
  undefined4 *puVar1;
  undefined4 local_c;
  
  puVar1 = (undefined4 *)operator_new(0x3c);
  if (puVar1 == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    local_c = FUN_004c82c0(puVar1);
  }
  return local_c;
}

