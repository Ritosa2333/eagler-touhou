/* Entry: 0x00543d53; symbol: FUN_00543d53; body bytes: 29 */

void FUN_00543d53(void)

{
  uint *puVar1;
  
  puVar1 = (uint *)FUN_0041f1c0();
  *puVar1 = *puVar1 | 0x24;
  puVar1[1] = puVar1[1];
  puVar1 = (uint *)FUN_00422b80();
  *puVar1 = *puVar1 | 2;
  puVar1[1] = puVar1[1];
  return;
}

