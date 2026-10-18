/* Entry: 0x0041cb10; symbol: FUN_0041cb10; body bytes: 332 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0041cb10(void)

{
  undefined4 uVar1;
  DWORD DVar2;
  double in_XMM0_Qa;
  double dVar3;
  LARGE_INTEGER local_14;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  uVar1 = FUN_0040c310(&DAT_005c0240,5);
  FUN_0040b730(&local_c,uVar1);
  if (DAT_005b67d0 == 0 && DAT_005b67d4 == 0) {
    DVar2 = timeGetTime();
    if ((double)(int)DVar2 + *(double *)(&DAT_0056cdd0 + ((int)DVar2 >> 0x1f) * -8) < DAT_005b8838)
    {
      DAT_005b8838 = (double)(int)DVar2 + *(double *)(&DAT_0056cdd0 + ((int)DVar2 >> 0x1f) * -8);
    }
    FUN_0040b900(&local_c);
  }
  else {
    QueryPerformanceCounter(&local_14);
    FUN_00542e00(local_14.s.LowPart - _DAT_005b67d8,
                 (local_14.s.HighPart - _DAT_005b67dc) - (uint)(local_14.s.LowPart < _DAT_005b67d8))
    ;
    dVar3 = in_XMM0_Qa;
    FUN_00542e00(DAT_005b67d0,DAT_005b67d4);
    if (in_XMM0_Qa / dVar3 < DAT_005b8838) {
      DAT_005b8838 = in_XMM0_Qa / dVar3;
    }
    FUN_0040b900(&local_c);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

