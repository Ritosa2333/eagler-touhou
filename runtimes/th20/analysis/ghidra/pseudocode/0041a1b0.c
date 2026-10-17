/* Entry: 0x0041a1b0; symbol: FUN_0041a1b0; body bytes: 76 */

void __cdecl FUN_0041a1b0(HWND param_1)

{
  RAWINPUTDEVICE local_14;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_14.usUsagePage = 1;
  local_14.usUsage = 4;
  local_14.dwFlags = 0x100;
  local_14.hwndTarget = param_1;
  RegisterRawInputDevices(&local_14,1,0xc);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

