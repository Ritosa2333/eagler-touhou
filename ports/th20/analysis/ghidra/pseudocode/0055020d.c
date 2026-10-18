/* Entry: 0x0055020d; symbol: FUN_0055020d; body bytes: 33 */

uint FUN_0055020d(void)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = FUN_005579c2();
  uVar2 = *(int *)(iVar1 + 0x18) * 0x343fd + 0x269ec3;
  *(uint *)(iVar1 + 0x18) = uVar2;
  return uVar2 >> 0x10 & 0x7fff;
}

