/* Entry: 0x00540753; symbol: FUN_00540753; body bytes: 147 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint __cdecl FUN_00540753(uint param_1)

{
  uint uVar1;
  HMODULE hModule;
  FARPROC pFVar2;
  FARPROC pFVar3;
  FARPROC pFVar4;
  
  while( true ) {
    LOCK();
    uVar1 = DAT_005e4b8c;
    if (param_1 == DAT_005e4b8c) {
      DAT_005e4b8c = 1;
      uVar1 = param_1;
    }
    UNLOCK();
    if (uVar1 == param_1) break;
    param_1 = uVar1;
    if (1 < uVar1) {
      return uVar1;
    }
  }
  hModule = GetModuleHandleW(L"api-ms-win-core-synch-l1-2-0.dll");
  if (hModule == (HMODULE)0x0) {
    DAT_005e4b8c = 2;
    return 2;
  }
  pFVar2 = GetProcAddress(hModule,"WaitOnAddress");
  pFVar3 = GetProcAddress(hModule,"WakeByAddressSingle");
  pFVar4 = GetProcAddress(hModule,"WakeByAddressAll");
  if (pFVar2 == (FARPROC)0x0) {
    DAT_005e4b8c = 2;
    return 2;
  }
  if (pFVar3 == (FARPROC)0x0) {
    DAT_005e4b8c = 2;
    return 2;
  }
  if (pFVar4 == (FARPROC)0x0) {
    DAT_005e4b8c = 2;
    return 2;
  }
  DAT_005e4b80 = pFVar2;
  _DAT_005e4b84 = pFVar3;
  DAT_005e4b88 = pFVar4;
  DAT_005e4b8c = 3;
  return 3;
}

