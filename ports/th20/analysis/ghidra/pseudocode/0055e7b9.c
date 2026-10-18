/* Entry: 0x0055e7b9; symbol: ___acrt_GetModuleFileNameA; body bytes: 161 */

/* Library Function - Single Match
    ___acrt_GetModuleFileNameA
   
   Library: Visual Studio 2019 Release */

void __cdecl ___acrt_GetModuleFileNameA(HMODULE param_1,undefined4 param_2,undefined4 param_3)

{
  DWORD DVar1;
  uint uVar2;
  undefined4 local_230;
  undefined4 local_22c;
  undefined4 local_228;
  undefined4 local_224;
  undefined4 local_220;
  undefined1 local_21c;
  undefined1 local_215;
  WCHAR local_214 [262];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  DVar1 = GetModuleFileNameW(param_1,local_214,0x105);
  if (DVar1 == 0) {
    DVar1 = GetLastError();
    ___acrt_errno_map_os_error(DVar1);
  }
  else {
    local_220 = 0;
    local_230 = param_2;
    local_22c = param_3;
    local_228 = param_2;
    local_224 = param_3;
    local_21c = 0;
    uVar2 = __acrt_get_utf8_acp_compatibility_codepage();
    FUN_00552660(local_214,(int)&local_230,&local_215,uVar2);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

