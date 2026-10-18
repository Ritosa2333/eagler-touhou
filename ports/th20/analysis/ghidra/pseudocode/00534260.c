/* Entry: 0x00534260; symbol: FUN_00534260; body bytes: 74 */

int FUN_00534260(void)

{
  int iVar1;
  int iVar2;
  
  iVar1 = FUN_00464080(0);
  iVar1 = FUN_0051b8e0(iVar1);
  iVar1 = (iVar1 * 0x4b0) / 100;
  iVar2 = FUN_00464080(0);
  iVar2 = FUN_005342b0(iVar2);
  return (iVar2 * iVar1) / 100 + iVar1;
}

