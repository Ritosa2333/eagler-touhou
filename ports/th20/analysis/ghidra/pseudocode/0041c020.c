/* Entry: 0x0041c020; symbol: FUN_0041c020; body bytes: 186 */

void FUN_0041c020(void)

{
  DWORD DVar1;
  _STARTUPINFOW local_464;
  WCHAR local_420 [262];
  WCHAR local_214 [262];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  DAT_005b8890 = CreateMutexW((LPSECURITY_ATTRIBUTES)0x0,1,L"th20 App");
  DVar1 = GetLastError();
  if (DVar1 == 0xb7) {
    FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_0056cd20);
  }
  else {
    local_464.cb = 0x44;
    _memset(&local_464.lpReserved,0,0x40);
    GetModuleFileNameW((HMODULE)0x0,local_214,0x104);
    GetConsoleTitleW(local_420,0x104);
    GetStartupInfoW(&local_464);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

