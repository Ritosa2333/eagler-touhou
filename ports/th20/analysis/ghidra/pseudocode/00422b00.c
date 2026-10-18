/* Entry: 0x00422b00; symbol: FUN_00422b00; body bytes: 94 */

CDocObjectServer * FUN_00422b00(void)

{
  CDocObjectServer *pCVar1;
  CDocObjectServer *pCVar2;
  int iVar3;
  
  pCVar2 = (CDocObjectServer *)FUN_0041f830();
  if (pCVar2 == (CDocObjectServer *)0x0) {
    pCVar2 = (CDocObjectServer *)0x0;
    pCVar1 = DAT_005b8898;
  }
  else {
    iVar3 = FUN_00420990(pCVar2);
    pCVar1 = pCVar2;
    if (iVar3 != 0) {
      if (pCVar2 != (CDocObjectServer *)0x0) {
        FUN_0041f7c0((undefined4 *)pCVar2);
      }
      pCVar2 = (CDocObjectServer *)0x0;
      pCVar1 = DAT_005b8898;
    }
  }
  DAT_005b8898 = pCVar1;
  return pCVar2;
}

