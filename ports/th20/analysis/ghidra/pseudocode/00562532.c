/* Entry: 0x00562532; symbol: FUN_00562532; body bytes: 67 */

int __cdecl FUN_00562532(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  bool bVar5;
  
  iVar3 = 0;
  iVar4 = 0xe3;
  do {
    iVar2 = (iVar4 + iVar3) / 2;
    if (param_1 == *(int *)(&DAT_0059b800 + iVar2 * 8)) {
      return iVar2;
    }
    bVar5 = -1 < param_1 - *(int *)(&DAT_0059b800 + iVar2 * 8);
    iVar1 = iVar2 + -1;
    if (bVar5) {
      iVar1 = iVar4;
    }
    if (bVar5) {
      iVar3 = iVar2 + 1;
    }
    iVar4 = iVar1;
  } while (iVar3 <= iVar1);
  return -1;
}

