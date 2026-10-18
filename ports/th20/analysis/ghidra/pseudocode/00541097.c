/* Entry: 0x00541097; symbol: ___std_fs_convert_wide_to_narrow@20; body bytes: 179 */

/* Library Function - Single Match
    ___std_fs_convert_wide_to_narrow@20
   
   Library: Visual Studio 2017 Release */

undefined8
___std_fs_convert_wide_to_narrow_20
          (UINT param_1,LPCWSTR param_2,int param_3,LPSTR param_4,int param_5)

{
  int iVar1;
  DWORD DVar2;
  BOOL local_8;
  
  if ((param_1 == 0xfde9) || (param_1 == 0xd698)) {
    iVar1 = WideCharToMultiByte(param_1,0x80,param_2,param_3,param_4,param_5,(LPCSTR)0x0,(LPBOOL)0x0
                               );
  }
  else {
    local_8 = 0;
    iVar1 = WideCharToMultiByte(param_1,0x400,param_2,param_3,param_4,param_5,(LPCSTR)0x0,&local_8);
    if (local_8 != 0) {
      DVar2 = 0x459;
      iVar1 = 0;
      goto LAB_00541143;
    }
  }
  if (iVar1 == 0) {
    DVar2 = GetLastError();
  }
  else {
    DVar2 = 0;
  }
  if (DVar2 == 0x3ec) {
    iVar1 = WideCharToMultiByte(param_1,0,param_2,param_3,param_4,param_5,(LPCSTR)0x0,(LPBOOL)0x0);
    if (iVar1 == 0) {
      DVar2 = GetLastError();
    }
    else {
      DVar2 = 0;
    }
  }
LAB_00541143:
  return CONCAT44(DVar2,iVar1);
}

