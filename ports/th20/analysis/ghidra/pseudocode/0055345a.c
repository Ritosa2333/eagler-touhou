/* Entry: 0x0055345a; symbol: FUN_0055345a; body bytes: 129 */

undefined4 FUN_0055345a(void)

{
  LPCSTR pCVar1;
  uint uVar2;
  wchar_t *pwVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 *puVar6;
  
  if (DAT_005e55a8 == (undefined4 *)0x0) {
LAB_005534d0:
    uVar5 = 0xffffffff;
  }
  else {
    pCVar1 = (LPCSTR)*DAT_005e55a8;
    puVar6 = DAT_005e55a8;
    while (pCVar1 != (LPCSTR)0x0) {
      uVar2 = FUN_0055dd91(0,0,pCVar1,-1,(LPWSTR)0x0,0);
      if (uVar2 == 0) goto LAB_005534d0;
      pwVar3 = (wchar_t *)FUN_00557b40(uVar2,2);
      if (pwVar3 == (wchar_t *)0x0) {
        pwVar3 = (wchar_t *)0x0;
LAB_005534ca:
        FUN_00557ba0(pwVar3);
        goto LAB_005534d0;
      }
      iVar4 = FUN_0055dd91(0,0,(LPCSTR)*puVar6,-1,pwVar3,uVar2);
      if (iVar4 == 0) goto LAB_005534ca;
      FUN_0055f596(pwVar3,0);
      FUN_00557ba0((LPVOID)0x0);
      puVar6 = puVar6 + 1;
      pCVar1 = (LPCSTR)*puVar6;
    }
    uVar5 = 0;
  }
  return uVar5;
}

