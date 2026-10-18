/* Entry: 0x0047d580; symbol: FUN_0047d580; body bytes: 119 */

int __cdecl FUN_0047d580(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  
  iVar2 = *(int *)(param_1 + 0x5c8);
  iVar1 = FUN_00485640(iVar2);
  if (-1 < *(int *)(&DAT_005c06ac + iVar1 * 0x158)) {
    iVar1 = FUN_00485640(iVar2);
    iVar2 = FUN_00485720(iVar2);
    param_2 = *(int *)(&DAT_005c06ac + param_2 * 4 + iVar2 * 0x14 + iVar1 * 0x158);
  }
  return param_2;
}

