/* Entry: 0x00535260; symbol: FUN_00535260; body bytes: 163 */

bool __fastcall FUN_00535260(int param_1)

{
  bool bVar1;
  void *this;
  int iVar2;
  int iVar3;
  int iVar4;
  
  bVar1 = FUN_00423590((void *)(param_1 + 0x14),0);
  if (!bVar1) {
    FUN_0044f130();
    FUN_0044fa30();
    this = (void *)FUN_00464080(0);
    iVar2 = FUN_00464080(0);
    iVar2 = FUN_0051b920(iVar2);
    iVar3 = FUN_0040ff90(param_1 + 0x14);
    iVar4 = FUN_00534260();
    FUN_004be940(this,(iVar2 * iVar3) / iVar4);
    FUN_00429420((void *)(param_1 + 0x14));
  }
  return bVar1;
}

