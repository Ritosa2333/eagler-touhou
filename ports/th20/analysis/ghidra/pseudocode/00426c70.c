/* Entry: 0x00426c70; symbol: FUN_00426c70; body bytes: 244 */

void FUN_00426c70(void)

{
  bool bVar1;
  DWORD DVar2;
  BOOL BVar3;
  char **ppcVar4;
  tagMSG local_24;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  bVar1 = false;
  ppcVar4 = (char **)0x1;
  while (!bVar1) {
    DVar2 = MsgWaitForMultipleObjects(1,(HANDLE *)&DAT_005bfffc,0,0xffffffff,0x1cbf);
    if (DAT_005bfff4 == (void *)0x0) {
      bVar1 = true;
    }
    if (DVar2 == 0) {
      if ((DAT_005bfff4 != (void *)0x0) && (*(int *)((int)DAT_005bfff4 + 0x58) != 0)) {
        *(undefined4 *)((int)DAT_005bfff4 + 0xa4) = 1;
        FUN_0045a580(DAT_005bfff4,1);
        *(undefined4 *)((int)DAT_005bfff4 + 0xa4) = 0;
      }
    }
    else if (DVar2 == 1) {
      while (BVar3 = PeekMessageW(&local_24,(HWND)0x0,0,0,1), BVar3 != 0) {
        if (local_24.message == 0x12) {
          bVar1 = true;
        }
      }
    }
  }
  std::_Adl_verify_range<char*,char_const*>((char **)&DAT_0056dffc,ppcVar4);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

