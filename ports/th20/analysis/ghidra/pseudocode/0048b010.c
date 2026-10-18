/* Entry: 0x0048b010; symbol: FUN_0048b010; body bytes: 82 */

undefined4 * FUN_0048b010(void)

{
  undefined4 *puVar1;
  undefined4 *local_c;
  
  puVar1 = (undefined4 *)operator_new(0x1c);
  if (puVar1 == (undefined4 *)0x0) {
    local_c = (undefined4 *)0x0;
  }
  else {
    *puVar1 = 0;
    puVar1[1] = 0;
    puVar1[2] = 0;
    puVar1[3] = 0;
    puVar1[4] = 0;
    puVar1[5] = 0;
    puVar1[6] = 0;
    local_c = FUN_0048b370(puVar1);
  }
  return local_c;
}

