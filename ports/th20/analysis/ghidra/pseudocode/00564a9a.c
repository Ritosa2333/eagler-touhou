/* Entry: 0x00564a9a; symbol: FUN_00564a9a; body bytes: 125 */

bool __cdecl FUN_00564a9a(uint param_1,int *param_2)

{
  byte bVar1;
  undefined3 extraout_var;
  BOOL BVar2;
  int iVar3;
  int iVar4;
  DWORD local_8;
  
  bVar1 = FUN_00563dcf(param_1);
  if (CONCAT31(extraout_var,bVar1) != 0) {
    iVar4 = (int)param_1 >> 6;
    iVar3 = (param_1 & 0x3f) * 0x38;
    if (*(char *)((&DAT_005e5690)[iVar4] + 0x28 + iVar3) < '\0') {
      if ((char)param_2[5] == '\0') {
        FUN_0054a360(param_2);
      }
      if ((*(int *)(param_2[3] + 0xa8) != 0) ||
         (*(char *)((&DAT_005e5690)[iVar4] + 0x29 + iVar3) != '\0')) {
        BVar2 = GetConsoleMode(*(HANDLE *)((&DAT_005e5690)[iVar4] + 0x18 + iVar3),&local_8);
        return BVar2 != 0;
      }
    }
  }
  return false;
}

