/* Entry: 0x00562575; symbol: FUN_00562575; body bytes: 84 */

undefined4 __cdecl FUN_00562575(ushort *param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = 0;
  iVar3 = 0xe3;
  do {
    iVar4 = (iVar3 + iVar5) / 2;
    iVar1 = FUN_00565cd0(param_1,*(ushort **)(&UNK_0059c920 + iVar4 * 8),0x55);
    if (iVar1 == 0) {
      return *(undefined4 *)(&UNK_0059c924 + iVar4 * 8);
    }
    iVar2 = iVar4 + -1;
    if (-1 < iVar1) {
      iVar2 = iVar3;
    }
    if (-1 < iVar1) {
      iVar5 = iVar4 + 1;
    }
    iVar3 = iVar2;
  } while (iVar5 <= iVar2);
  return 0xffffffff;
}

