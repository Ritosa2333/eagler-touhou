/* Entry: 0x0055ce76; symbol: FUN_0055ce76; body bytes: 224 */

/* WARNING: Function: __alloca_probe_16 replaced with injection: alloca_probe */

void __cdecl FUN_0055ce76(int *param_1,ushort *param_2,LCTYPE param_3,LPSTR param_4,int param_5)

{
  int iVar1;
  void *pvVar2;
  LPWSTR pWVar3;
  LPSTR pCVar4;
  void *this;
  void *this_00;
  LPWSTR pWVar5;
  int local_1c;
  int local_18;
  char local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0054c44c(&local_1c,param_1);
  local_c = *(uint *)(local_18 + 8);
  iVar1 = ___acrt_GetLocaleInfoEx_16(this,param_2,param_3,(LPWSTR)0x0,0);
  if (iVar1 == 0) goto LAB_0055cf32;
  this_00 = (void *)(iVar1 * 2 + 8);
  pvVar2 = (void *)(-(uint)((void *)(iVar1 * 2) < this_00) & (uint)this_00);
  if (pvVar2 == (void *)0x0) {
    pWVar3 = (LPWSTR)0x0;
  }
  else if (pvVar2 < (void *)0x401) {
    pWVar5 = (LPWSTR)&stack0xffffffd8;
    pWVar3 = (LPWSTR)&stack0xffffffd8;
    if (&stack0x00000000 != (undefined1 *)0x28) {
LAB_0055ceef:
      pWVar3 = pWVar5 + 4;
      if ((pWVar3 != (LPWSTR)0x0) &&
         (iVar1 = ___acrt_GetLocaleInfoEx_16(this_00,param_2,param_3,pWVar3,iVar1), iVar1 != 0)) {
        pCVar4 = (LPSTR)0x0;
        if (param_5 != 0) {
          pCVar4 = param_4;
        }
        FUN_0055de4b(local_c,0,pWVar3,-1,pCVar4,param_5,(LPCSTR)0x0,(LPBOOL)0x0);
      }
    }
  }
  else {
    this_00 = pvVar2;
    pWVar3 = (LPWSTR)FUN_005584c0((SIZE_T)pvVar2);
    if (pWVar3 != (LPWSTR)0x0) {
      pWVar3[0] = L'\xdddd';
      pWVar3[1] = L'\0';
      pWVar5 = pWVar3;
      goto LAB_0055ceef;
    }
  }
  FUN_0054273f((int)pWVar3);
LAB_0055cf32:
  if (local_10 != '\0') {
    *(uint *)(local_1c + 0x350) = *(uint *)(local_1c + 0x350) & 0xfffffffd;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

