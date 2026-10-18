/* Entry: 0x00538e10; symbol: FUN_00538e10; body bytes: 248 */

void __cdecl FUN_00538e10(undefined1 (*param_1) [16],LPCSTR param_2)

{
  int iVar1;
  WCHAR local_210 [260];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  _memset(local_210,0,0x208);
  MultiByteToWideChar(0x3a4,0,param_2,-1,local_210,0x104);
  iVar1 = FUN_00538f50((ushort *)local_210,0x3a);
  if (iVar1 == 0) {
    GetModuleFileNameW((HMODULE)0x0,(LPWSTR)param_1,0x104);
    iVar1 = FUN_00538f70(param_1,0x5c);
    if (iVar1 == 0) {
      FUN_00548100((short *)param_1,0x104,0x5767d0);
    }
    else {
      *(undefined2 *)(iVar1 + 2) = 0;
    }
    FUN_005517b0((short *)param_1,0x104,(int)local_210);
  }
  else {
    FUN_00548100((short *)param_1,0x104,(int)local_210);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

