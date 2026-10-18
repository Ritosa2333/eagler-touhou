/* Entry: 0x00540f5f; symbol: FUN_00540f5f; body bytes: 122 */

void FUN_00540f5f(DWORD param_1,int *param_2)

{
  int iVar1;
  int iVar2;
  DWORD DVar3;
  DWORD local_8;
  
  DVar3 = 0;
  local_8 = 0;
  iVar2 = 0;
  do {
    if (DVar3 != 0) break;
    if (iVar2 == 0) {
      local_8 = 0x409;
LAB_00540fac:
      DVar3 = FormatMessageA(0x1300,(LPCVOID)0x0,param_1,local_8,(LPSTR)param_2,0,(va_list *)0x0);
    }
    else {
      if (iVar2 != 1) {
        local_8 = 0;
        goto LAB_00540fac;
      }
      iVar1 = GetLocaleInfoEx(L"!x-sys-default-locale",0x20000001,(LPWSTR)&local_8,2);
      if (iVar1 != 0) goto LAB_00540fac;
    }
    iVar2 = iVar2 + 1;
  } while (iVar2 < 3);
  FUN_00540f3b(*param_2,DVar3);
  return;
}

