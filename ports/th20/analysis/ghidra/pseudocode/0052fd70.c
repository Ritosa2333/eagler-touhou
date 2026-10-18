/* Entry: 0x0052fd70; symbol: FUN_0052fd70; body bytes: 260 */

undefined4 __fastcall FUN_0052fd70(int param_1)

{
  bool bVar1;
  void *this;
  int iVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  
  bVar1 = FUN_00423590((void *)(param_1 + 0x14),0);
  if (!bVar1) {
    this = (void *)FUN_00464080(0);
    iVar2 = FUN_00464080(0);
    iVar2 = FUN_0051b920(iVar2);
    iVar3 = FUN_0040ff90(param_1 + 0x14);
    iVar4 = FUN_00534260();
    FUN_004be940(this,(iVar2 * iVar3) / iVar4);
    iVar2 = FUN_00460830(0);
    iVar2 = FUN_004ff6b0(iVar2);
    uVar5 = FUN_00530350(iVar2);
    if (uVar5 == 0) {
      FUN_00423540((undefined4 *)(param_1 + 0x24));
    }
    else {
      FUN_00423520((void *)(param_1 + 0x24),0);
    }
    bVar1 = FUN_00423560((void *)(param_1 + 0x24),0x1e);
    if ((bVar1) || (iVar2 = FUN_00472040((void *)(param_1 + 0x24),4), iVar2 == 0)) {
      FUN_00429420((void *)(param_1 + 0x14));
    }
    return 0;
  }
  return 1;
}

