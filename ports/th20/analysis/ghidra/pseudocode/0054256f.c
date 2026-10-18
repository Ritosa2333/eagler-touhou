/* Entry: 0x0054256f; symbol: FUN_0054256f; body bytes: 464 */

/* WARNING: Function: __alloca_probe_16 replaced with injection: alloca_probe */

void __cdecl
FUN_0054256f(LPCWSTR param_1,uint param_2,char *param_3,int param_4,LPWSTR param_5,int param_6,
            UINT param_7,int param_8)

{
  int iVar1;
  DWORD dwFlags;
  uint uVar2;
  LPCWSTR lpWideCharStr;
  int iVar3;
  LPCWSTR pWVar4;
  LPCWSTR pWVar5;
  LPCWSTR local_10;
  LPCWSTR local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar3 = param_4;
  if (0 < param_4) {
    iVar1 = FUN_00552590(param_3,param_4);
    iVar3 = iVar1 + 1;
    if (param_4 <= iVar1) {
      iVar3 = iVar1;
    }
  }
  dwFlags = 1;
  if (param_8 != 0) {
    dwFlags = 9;
  }
  iVar1 = MultiByteToWideChar(param_7,dwFlags,param_3,iVar3,(LPWSTR)0x0,0);
  if (iVar1 == 0) goto LAB_0054272d;
  uVar2 = iVar1 * 2 + 8;
  uVar2 = -(uint)((uint)(iVar1 * 2) < uVar2) & uVar2;
  if (uVar2 == 0) {
    local_10 = (LPCWSTR)0x0;
  }
  else {
    if (uVar2 < 0x401) {
      pWVar4 = (LPCWSTR)&stack0xffffffe0;
      lpWideCharStr = (LPCWSTR)&stack0xffffffe0;
      if (&stack0x00000000 != (undefined1 *)0x20) {
LAB_00542608:
        lpWideCharStr = pWVar4 + 4;
      }
    }
    else {
      lpWideCharStr = (LPCWSTR)FUN_0054a550(uVar2);
      if (lpWideCharStr != (LPCWSTR)0x0) {
        lpWideCharStr[0] = L'\xdddd';
        lpWideCharStr[1] = L'\0';
        pWVar4 = lpWideCharStr;
        goto LAB_00542608;
      }
    }
    local_10 = lpWideCharStr;
    if (((lpWideCharStr != (LPCWSTR)0x0) &&
        (iVar3 = MultiByteToWideChar(param_7,1,param_3,iVar3,lpWideCharStr,iVar1), iVar3 != 0)) &&
       (iVar3 = LCMapStringEx(param_1,param_2,lpWideCharStr,iVar1,(LPWSTR)0x0,0,
                              (LPNLSVERSIONINFO)0x0,(LPVOID)0x0,0), iVar3 != 0)) {
      if ((param_2 & 0x400) == 0) {
        uVar2 = iVar3 * 2 + 8;
        uVar2 = -(uint)((uint)(iVar3 * 2) < uVar2) & uVar2;
        if (uVar2 == 0) {
          local_c = (LPCWSTR)0x0;
        }
        else {
          if (uVar2 < 0x401) {
            pWVar5 = (LPCWSTR)&stack0xffffffe0;
            pWVar4 = (LPCWSTR)&stack0xffffffe0;
            if (&stack0x00000000 != (undefined1 *)0x20) {
LAB_005426c0:
              pWVar4 = pWVar5 + 4;
            }
          }
          else {
            pWVar4 = (LPCWSTR)FUN_0054a550(uVar2);
            if (pWVar4 != (LPCWSTR)0x0) {
              pWVar4[0] = L'\xdddd';
              pWVar4[1] = L'\0';
              pWVar5 = pWVar4;
              goto LAB_005426c0;
            }
          }
          local_c = pWVar4;
          if ((pWVar4 != (LPCWSTR)0x0) &&
             (iVar1 = LCMapStringEx(param_1,param_2,lpWideCharStr,iVar1,pWVar4,iVar3,
                                    (LPNLSVERSIONINFO)0x0,(LPVOID)0x0,0), iVar1 != 0)) {
            if (param_6 == 0) {
              param_6 = 0;
              param_5 = (LPWSTR)0x0;
            }
            WideCharToMultiByte(param_7,0,pWVar4,iVar3,(LPSTR)param_5,param_6,(LPCSTR)0x0,
                                (LPBOOL)0x0);
          }
        }
        FUN_00542556((int *)&local_c);
      }
      else if ((param_6 != 0) && (iVar3 <= param_6)) {
        LCMapStringEx(param_1,param_2,lpWideCharStr,iVar1,param_5,param_6,(LPNLSVERSIONINFO)0x0,
                      (LPVOID)0x0,0);
      }
    }
  }
  FUN_00542556((int *)&local_10);
LAB_0054272d:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

