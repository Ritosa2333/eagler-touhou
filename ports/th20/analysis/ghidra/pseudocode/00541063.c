/* Entry: 0x00541063; symbol: ___std_fs_convert_narrow_to_wide@20; body bytes: 52 */

/* Library Function - Single Match
    ___std_fs_convert_narrow_to_wide@20
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined8
___std_fs_convert_narrow_to_wide_20
          (UINT param_1,LPCSTR param_2,int param_3,LPWSTR param_4,int param_5)

{
  int iVar1;
  DWORD DVar2;
  
  iVar1 = MultiByteToWideChar(param_1,8,param_2,param_3,param_4,param_5);
  if (iVar1 == 0) {
    DVar2 = GetLastError();
  }
  else {
    DVar2 = 0;
  }
  return CONCAT44(DVar2,iVar1);
}

