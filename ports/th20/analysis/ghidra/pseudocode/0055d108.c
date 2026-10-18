/* Entry: 0x0055d108; symbol: FUN_0055d108; body bytes: 254 */

/* WARNING: Function: __alloca_probe_16 replaced with injection: alloca_probe */

void __cdecl
FUN_0055d108(int *param_1,DWORD param_2,LPCSTR param_3,int param_4,LPWORD param_5,uint param_6,
            int param_7)

{
  uint uVar1;
  LPCWSTR lpSrcStr;
  int cchSrc;
  LPCWSTR pWVar2;
  int local_20;
  int local_1c;
  char local_14;
  int local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0054c44c(&local_20,param_1);
  if (param_6 == 0) {
    param_6 = *(uint *)(local_1c + 8);
  }
  uVar1 = 1;
  if (param_7 != 0) {
    uVar1 = 9;
  }
  local_10 = FUN_0055dd91(param_6,uVar1,param_3,param_4,(LPWSTR)0x0,0);
  if (local_10 == 0) goto LAB_0055d1e2;
  local_c = local_10 * 2;
  uVar1 = -(uint)(local_c < local_c + 8) & local_c + 8;
  if (uVar1 == 0) {
    lpSrcStr = (LPCWSTR)0x0;
  }
  else if (uVar1 < 0x401) {
    pWVar2 = (LPCWSTR)&stack0xffffffd4;
    lpSrcStr = (LPCWSTR)&stack0xffffffd4;
    if (&stack0x00000000 != (undefined1 *)0x2c) {
LAB_0055d19d:
      lpSrcStr = pWVar2 + 4;
      if (lpSrcStr != (LPCWSTR)0x0) {
        _memset(lpSrcStr,0,local_c);
        cchSrc = FUN_0055dd91(param_6,1,param_3,param_4,lpSrcStr,local_10);
        if (cchSrc != 0) {
          GetStringTypeW(param_2,lpSrcStr,cchSrc,param_5);
        }
      }
    }
  }
  else {
    lpSrcStr = (LPCWSTR)FUN_005584c0(uVar1);
    if (lpSrcStr != (LPCWSTR)0x0) {
      lpSrcStr[0] = L'\xdddd';
      lpSrcStr[1] = L'\0';
      pWVar2 = lpSrcStr;
      goto LAB_0055d19d;
    }
  }
  FUN_0054273f((int)lpSrcStr);
LAB_0055d1e2:
  if (local_14 != '\0') {
    *(uint *)(local_20 + 0x350) = *(uint *)(local_20 + 0x350) & 0xfffffffd;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

