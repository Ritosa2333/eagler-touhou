/* Entry: 0x004d9ea0; symbol: FUN_004d9ea0; body bytes: 187 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_004d9ea0(void)

{
  char cVar1;
  SIZE_T local_8c;
  wchar_t local_88 [64];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  cVar1 = FUN_0053a6f0(&DAT_005b66c8,(char **)"th20.dat");
  if (cVar1 == '\0') {
    FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_00571974);
  }
  else {
    FID_conflict__swprintf(local_88,(wchar_t *)0x80,"th20_%.4x%c.ver",0x100,0x61);
    DAT_005c5b04 = FUN_00410aa0((undefined1 (*) [16])local_88,&local_8c,0);
    _DAT_005c5b00 = local_8c;
    if (DAT_005c5b04 == 0) {
      FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_00571944);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

