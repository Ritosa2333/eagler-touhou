/* Entry: 0x004c03d0; symbol: FUN_004c03d0; body bytes: 164 */

void __fastcall FUN_004c03d0(char **param_1)

{
  bool bVar1;
  
  FUN_0047a1f0((float *)(param_1 + 0xd));
  param_1[6] = (char *)((float)param_1[6] + (float)param_1[8]);
  FUN_004530c0(param_1 + 9,(float)param_1[10]);
  FUN_004117a0(param_1 + 0x29,0);
  FUN_00429420(param_1 + 0x1f);
  param_1[0x2a] = param_1[0x2a] + -1;
  bVar1 = FUN_00423590(param_1 + 0x1f,0);
  if (bVar1) {
    FUN_004c1f30(param_1);
  }
  return;
}

