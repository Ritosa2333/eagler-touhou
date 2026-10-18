/* Entry: 0x0056513a; symbol: FUN_0056513a; body bytes: 131 */

undefined8 __cdecl
FUN_0056513a(uint param_1,undefined4 param_2,undefined4 param_3,PLARGE_INTEGER param_4,int param_5)

{
  byte *pbVar1;
  LARGE_INTEGER liDistanceToMove;
  HANDLE hFile;
  BOOL BVar2;
  DWORD DVar3;
  DWORD unaff_EDI;
  uint local_c;
  uint local_8;
  
  hFile = (HANDLE)FUN_0055fb8c(param_1);
  if (hFile == (HANDLE)0xffffffff) {
    *(undefined1 *)(param_5 + 0x1c) = 1;
    *(undefined4 *)(param_5 + 0x18) = 9;
  }
  else {
    liDistanceToMove.s.HighPart = (LONG)&local_c;
    liDistanceToMove.s.LowPart = param_3;
    BVar2 = SetFilePointerEx(hFile,liDistanceToMove,param_4,unaff_EDI);
    if (BVar2 == 0) {
      DVar3 = GetLastError();
      FUN_00551688(DVar3,param_5);
    }
    else if ((local_c & local_8) != 0xffffffff) {
      pbVar1 = (byte *)((&DAT_005e5690)[(int)param_1 >> 6] + 0x28 + (param_1 & 0x3f) * 0x38);
      *pbVar1 = *pbVar1 & 0xfd;
      goto LAB_005651b9;
    }
  }
  local_c = 0xffffffff;
  local_8 = 0xffffffff;
LAB_005651b9:
  return CONCAT44(local_8,local_c);
}

