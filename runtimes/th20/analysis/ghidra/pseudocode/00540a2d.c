/* Entry: 0x00540a2d; symbol: FUN_00540a2d; body bytes: 79 */

int __cdecl FUN_00540a2d(HANDLE param_1,undefined4 param_2,DWORD *param_3)

{
  DWORD DVar1;
  BOOL BVar2;
  int iVar3;
  DWORD local_8;
  
  DVar1 = WaitForSingleObjectEx(param_1,0xffffffff,0);
  if (DVar1 == 0xffffffff) {
LAB_00540a76:
    iVar3 = 4;
  }
  else {
    if (param_3 != (DWORD *)0x0) {
      BVar2 = GetExitCodeThread(param_1,&local_8);
      if (BVar2 == 0) goto LAB_00540a76;
      *param_3 = local_8;
    }
    BVar2 = CloseHandle(param_1);
    iVar3 = (-(uint)(BVar2 != 0) & 0xfffffffc) + 4;
  }
  return iVar3;
}

