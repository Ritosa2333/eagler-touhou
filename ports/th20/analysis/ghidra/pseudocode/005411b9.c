/* Entry: 0x005411b9; symbol: FUN_005411b9; body bytes: 94 */

undefined8 FUN_005411b9(LPCWSTR param_1)

{
  BOOL BVar1;
  DWORD DVar2;
  DWORD local_2c [4];
  uint local_1c;
  uint local_c;
  
  BVar1 = CreateDirectoryW(param_1,(LPSECURITY_ATTRIBUTES)0x0);
  if (BVar1 == 0) {
    DVar2 = GetLastError();
    if (DVar2 == 0xb7) {
      DVar2 = FUN_00541217(param_1,local_2c,3,0xffffffff);
      if ((DVar2 == 0) && ((local_1c >> 4 & 1) == 0)) {
        DVar2 = 0xb7;
      }
    }
    local_c = local_c & 0xffffff00;
  }
  else {
    local_c = CONCAT31(local_c._1_3_,1);
    DVar2 = 0;
  }
  return CONCAT44(DVar2,local_c);
}

