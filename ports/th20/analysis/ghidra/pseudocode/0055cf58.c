/* Entry: 0x0055cf58; symbol: ___acrt_GetLocaleInfoA; body bytes: 431 */

/* Library Function - Single Match
    ___acrt_GetLocaleInfoA
   
   Library: Visual Studio 2019 Release */

void __cdecl
___acrt_GetLocaleInfoA(int *param_1,int param_2,ushort *param_3,uint param_4,int *param_5)

{
  uint uVar1;
  LPVOID pvVar2;
  int iVar3;
  DWORD DVar4;
  LPWSTR pWVar5;
  void *this;
  ushort *local_8c;
  CHAR local_88 [128];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_8c = param_3;
  *param_5 = 0;
  if (param_2 == 1) {
    uVar1 = FUN_0055ce76(param_1,param_3,param_4,local_88,0x80);
    if (uVar1 != 0) {
      pvVar2 = FUN_00557b40(uVar1,1);
      *param_5 = (int)pvVar2;
      FUN_00557ba0((LPVOID)0x0);
      if ((*param_5 != 0) &&
         (iVar3 = FUN_005651e0((char *)*param_5,uVar1,(int)local_88,uVar1 - 1), iVar3 != 0)) {
                    /* WARNING: Subroutine does not return */
        __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      goto LAB_0055d0ee;
    }
    DVar4 = GetLastError();
    if ((DVar4 != 0x7a) || (uVar1 = FUN_0055ce76(param_1,param_3,param_4,(LPSTR)0x0,0), uVar1 == 0))
    goto LAB_0055d0ee;
    pWVar5 = (LPWSTR)FUN_00557b40(uVar1,1);
    if (pWVar5 != (LPWSTR)0x0) {
      iVar3 = FUN_0055ce76(param_1,local_8c,param_4,(LPSTR)pWVar5,uVar1);
LAB_0055d05d:
      if (iVar3 != 0) {
        *param_5 = (int)pWVar5;
        pWVar5 = (LPWSTR)0x0;
      }
    }
  }
  else {
    if (param_2 != 2) {
      if (param_2 == 0) {
        local_8c = (ushort *)0x0;
        iVar3 = ___acrt_GetLocaleInfoEx_16(param_1,param_3,param_4 | 0x20000000,(LPWSTR)&local_8c,2)
        ;
        if (iVar3 != 0) {
          *(undefined1 *)param_5 = local_8c._0_1_;
        }
      }
      goto LAB_0055d0ee;
    }
    uVar1 = ___acrt_GetLocaleInfoEx_16(param_1,param_3,param_4,(LPWSTR)0x0,0);
    if (uVar1 == 0) goto LAB_0055d0ee;
    this = (void *)0x2;
    pWVar5 = (LPWSTR)FUN_00557b40(uVar1,2);
    if (pWVar5 != (LPWSTR)0x0) {
      iVar3 = ___acrt_GetLocaleInfoEx_16(this,local_8c,param_4,pWVar5,uVar1);
      goto LAB_0055d05d;
    }
  }
  FUN_00557ba0(pWVar5);
LAB_0055d0ee:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

