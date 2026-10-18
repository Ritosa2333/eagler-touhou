/* Entry: 0x00543d32; symbol: FUN_00543d32; body bytes: 33 */

void FUN_00543d32(void)

{
  code *pcVar1;
  int iVar2;
  
  iVar2 = FUN_00557210((uint *)0x0,0x10000,0x30000);
  if (iVar2 == 0) {
    return;
  }
  FUN_00543a39();
  pcVar1 = (code *)swi(3);
  (*pcVar1)();
  return;
}

