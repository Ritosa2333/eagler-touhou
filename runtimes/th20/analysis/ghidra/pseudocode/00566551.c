/* Entry: 0x00566551; symbol: FUN_00566551; body bytes: 160 */

undefined4 __cdecl FUN_00566551(uint param_1,int param_2)

{
  int iVar1;
  int iVar2;
  HANDLE hObject;
  BOOL BVar3;
  undefined4 uVar4;
  DWORD DVar5;
  
  iVar1 = FUN_0055fb8c(param_1);
  if (iVar1 != -1) {
    if (((param_1 == 1) && ((*(byte *)(DAT_005e5690 + 0x98) & 1) != 0)) ||
       ((param_1 == 2 && ((*(byte *)(DAT_005e5690 + 0x60) & 1) != 0)))) {
      iVar1 = FUN_0055fb8c(2);
      iVar2 = FUN_0055fb8c(1);
      if (iVar2 == iVar1) goto LAB_00566567;
    }
    hObject = (HANDLE)FUN_0055fb8c(param_1);
    BVar3 = CloseHandle(hObject);
    if (BVar3 == 0) {
      DVar5 = GetLastError();
      goto LAB_005665b9;
    }
  }
LAB_00566567:
  DVar5 = 0;
LAB_005665b9:
  FUN_0055fafb(param_1);
  *(undefined1 *)((&DAT_005e5690)[(int)param_1 >> 6] + 0x28 + (param_1 & 0x3f) * 0x38) = 0;
  if (DVar5 == 0) {
    uVar4 = 0;
  }
  else {
    FUN_00551688(DVar5,param_2);
    uVar4 = 0xffffffff;
  }
  return uVar4;
}

