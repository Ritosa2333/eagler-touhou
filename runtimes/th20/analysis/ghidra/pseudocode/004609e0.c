/* Entry: 0x004609e0; symbol: FUN_004609e0; body bytes: 369 */

undefined4 __fastcall FUN_004609e0(int param_1)

{
  byte bVar1;
  void *this;
  bool bVar2;
  undefined3 extraout_var;
  int iVar3;
  undefined4 *puVar4;
  undefined4 uVar5;
  float10 fVar6;
  float fVar7;
  undefined4 local_10;
  
  bVar2 = FUN_00423560((void *)(param_1 + 0x3d0),0x50);
  if (bVar2) {
    bVar2 = FUN_0045d030((int *)(param_1 + 0x3d0));
    if (CONCAT31(extraout_var,bVar2) != 0) {
      iVar3 = FUN_0040ff90(param_1 + 0x3d0);
      if (0x45 < iVar3) {
        for (local_10 = 0; local_10 < 0x50; local_10 = local_10 + 1) {
          iVar3 = FUN_00414580((void *)(param_1 + 0x288),local_10);
          bVar1 = *(byte *)(iVar3 + 3);
          fVar6 = FUN_00429830(0x5ba4c4);
          fVar7 = (float)fVar6 * DAT_0056c8d0 + DAT_0056c8d0;
          iVar3 = FUN_00414580((void *)(param_1 + 0x288),local_10);
          *(char *)(iVar3 + 3) = *(char *)(iVar3 + 3) - (char)(int)((float)bVar1 / fVar7);
        }
      }
      iVar3 = FUN_00460830(0);
      if ((iVar3 != 0) && (*(char *)(param_1 + 0x3f0) != '\0')) {
        this = *(void **)(param_1 + 4);
        iVar3 = FUN_00460830(0);
        puVar4 = (undefined4 *)FUN_00460850(iVar3);
        FUN_0045dd60(this,puVar4);
      }
    }
    FUN_00423540((undefined4 *)(param_1 + 0x3d0));
    uVar5 = 0;
  }
  else {
    uVar5 = 1;
  }
  return uVar5;
}

