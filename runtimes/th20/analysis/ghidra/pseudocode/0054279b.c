/* Entry: 0x0054279b; symbol: FUN_0054279b; body bytes: 66 */

int __cdecl FUN_0054279b(int param_1,uint param_2)

{
  int iVar1;
  int iVar2;
  
  iVar1 = *(int *)(param_1 + 0x3c) + param_1;
  iVar2 = iVar1 + 0x18 + (uint)*(ushort *)(iVar1 + 0x14);
  iVar1 = (uint)*(ushort *)(iVar1 + 6) * 0x28 + iVar2;
  while( true ) {
    if (iVar2 == iVar1) {
      return 0;
    }
    if ((*(uint *)(iVar2 + 0xc) <= param_2) &&
       (param_2 < *(int *)(iVar2 + 8) + *(uint *)(iVar2 + 0xc))) break;
    iVar2 = iVar2 + 0x28;
  }
  return iVar2;
}

