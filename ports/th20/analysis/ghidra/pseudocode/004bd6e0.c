/* Entry: 0x004bd6e0; symbol: FUN_004bd6e0; body bytes: 45 */

void __fastcall FUN_004bd6e0(int param_1)

{
  int iVar1;
  float10 fVar2;
  
  iVar1 = FUN_00488830(DAT_005ba828);
  if (iVar1 == 0) {
    fVar2 = (float10)FUN_0041cb10();
    *(double *)(param_1 + 0x2b0) = (double)fVar2;
  }
  return;
}

