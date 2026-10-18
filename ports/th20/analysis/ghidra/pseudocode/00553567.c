/* Entry: 0x00553567; symbol: thunk_FUN_00553174; body bytes: 5 */

undefined4 thunk_FUN_00553174(void)

{
  LPWCH pWVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  
  if (DAT_005e55a8 != (undefined4 *)0x0) {
    return 0;
  }
  ___acrt_initialize_multibyte();
  pWVar1 = FUN_0055f0ee();
  if (pWVar1 == (LPWCH)0x0) {
    FUN_00557ba0((LPVOID)0x0);
    return 0xffffffff;
  }
  puVar2 = FUN_00553223((char *)pWVar1);
  if (puVar2 == (undefined4 *)0x0) {
    uVar3 = 0xffffffff;
  }
  else {
    uVar3 = 0;
    DAT_005e55a8 = puVar2;
    DAT_005e55b4 = puVar2;
  }
  FUN_00557ba0((LPVOID)0x0);
  FUN_00557ba0(pWVar1);
  return uVar3;
}

