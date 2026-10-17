/* Entry: 0x004878b0; symbol: FUN_004878b0; body bytes: 602 */

void __fastcall FUN_004878b0(int param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  int iVar2;
  void *pvVar3;
  uint uVar4;
  uint local_14;
  int local_10;
  int local_c;
  
  if ((*(uint *)(param_1 + 0x78) & 1) != 0) {
    FUN_004883a0(DAT_005c069c);
    FUN_00479040((void *)(param_1 + 0x14));
    FUN_00479040((void *)(param_1 + 0x18));
    FUN_00479040((void *)(param_1 + 0x1c));
    *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) & 0xfffffffe;
    FUN_0044fcd0((undefined4 *)(param_1 + 0x10));
    *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) & 0xffffffdf;
    FUN_00488ab0((int)DAT_005c06a4);
    FUN_0044fcd0((undefined4 *)(param_1 + 0x20));
    if ((*(uint *)(param_1 + 0x78) >> 1 & 1) == 0) {
      FUN_004b90e0(DAT_005c06a4,1,0);
    }
    else {
      FUN_00488550(&DAT_005ba568,*(uint *)(param_1 + 0x7c));
      FUN_004b90e0(DAT_005c06a4,0,*(int *)(param_1 + 0x7c));
      bVar1 = FUN_00488800(DAT_005c60fc);
      if (CONCAT31(extraout_var,bVar1) == 0) {
        bVar1 = FUN_00488850(0x5ba568);
        local_14 = (uint)bVar1;
        iVar2 = FUN_0050fc50(DAT_005c6108);
        pvVar3 = (void *)FUN_00486e10((void *)(iVar2 + 0xb08),*(int *)(param_1 + 0x74));
        FUN_00488510(pvVar3,local_14);
        iVar2 = FUN_00488700((int)DAT_005c6108);
        pvVar3 = (void *)FUN_00486e10((void *)(iVar2 + 0xb08),*(int *)(param_1 + 0x74));
        FUN_00488510(pvVar3,local_14);
        local_c = 0;
        for (local_10 = 0; local_10 < 0x71; local_10 = local_10 + 1) {
          iVar2 = FUN_00488700((int)DAT_005c6108);
          iVar2 = FUN_00486e10((void *)(iVar2 + 0xb08),local_10);
          if (*(int *)(iVar2 + 0xc0) == 0) {
            iVar2 = FUN_00488700((int)DAT_005c6108);
            iVar2 = FUN_00486e10((void *)(iVar2 + 0xb08),local_10);
            if (*(int *)(iVar2 + 0xc4) != 0) goto LAB_00487aaf;
          }
          else {
LAB_00487aaf:
            local_c = local_c + 1;
          }
        }
        if (0x70 < local_c) {
          FUN_0052f900(0x28);
        }
      }
      FUN_00426d70(&DAT_005ba830,0x2e,0);
    }
    uVar4 = FUN_00488880(param_1);
    if (uVar4 != 0) {
      FUN_00426d70(&DAT_005ba830,0x45,0);
    }
  }
  return;
}

