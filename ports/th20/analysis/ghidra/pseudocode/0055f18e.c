/* Entry: 0x0055f18e; symbol: FUN_0055f18e; body bytes: 78 */

uint * FUN_0055f18e(void)

{
  LPWCH pWVar1;
  wchar_t *pwVar2;
  uint *puVar3;
  uint uVar4;
  
  pWVar1 = GetEnvironmentStringsW();
  if (pWVar1 == (LPWCH)0x0) {
    return (uint *)0x0;
  }
  pwVar2 = find_end_of_double_null_terminated_sequence(pWVar1);
  uVar4 = (int)pwVar2 - (int)pWVar1 & 0xfffffffe;
  puVar3 = (uint *)FUN_005584c0(uVar4);
  if (puVar3 != (uint *)0x0) {
    FUN_00543e20(puVar3,(uint *)pWVar1,uVar4);
  }
  FUN_00557ba0((LPVOID)0x0);
  FreeEnvironmentStringsW(pWVar1);
  return puVar3;
}

