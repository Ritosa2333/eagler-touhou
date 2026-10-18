/* Entry: 0x0055c974; symbol: FUN_0055c974; body bytes: 117 */

void FUN_0055c974(void)

{
  wchar_t *pwVar1;
  wchar_t *pwVar2;
  wchar_t local_208 [256];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  DAT_005b29ec = 0xffffffff;
  DAT_005b29e0 = 0xffffffff;
  DAT_005e58b0 = 0;
  pwVar1 = FUN_0055c4c9(local_208);
  pwVar2 = pwVar1;
  if (pwVar1 == local_208) {
    pwVar2 = (wchar_t *)0x0;
  }
  if ((pwVar1 == (wchar_t *)0x0) || (*pwVar1 == L'\0')) {
    tzset_from_system_nolock();
  }
  else {
    FUN_0055c587(pwVar1);
  }
  FUN_00557ba0(pwVar2);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

